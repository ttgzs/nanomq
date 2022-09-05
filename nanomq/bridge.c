#include "include/bridge.h"
#include "nng/mqtt/mqtt_client.h"
#include "nng/mqtt/mqtt_quic.h"
#include "nng/nng.h"
#include "nng/protocol/mqtt/mqtt.h"
#include "nng/supplemental/nanolib/log.h"
#include "nng/supplemental/util/platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/nanomq.h"

static nng_thread *hybridger_thr;

static void
fatal(const char *func, int rv)
{
	fprintf(stderr, "%s: %s\n", func, nng_strerror(rv));
}

nng_msg *
bridge_publish_msg(const char *topic, uint8_t *payload, uint32_t len, bool dup,
    uint8_t qos, bool retain, property *props)
{
	int rv;

	// create a PUBLISH message
	nng_msg *pubmsg;
	nng_mqtt_msg_alloc(&pubmsg, 0);
	nng_mqtt_msg_set_packet_type(pubmsg, NNG_MQTT_PUBLISH);
	nng_mqtt_msg_set_publish_dup(pubmsg, dup);
	nng_mqtt_msg_set_publish_qos(pubmsg, qos);
	nng_mqtt_msg_set_publish_retain(pubmsg, retain);
	nng_mqtt_msg_set_publish_payload(pubmsg, payload, len);
	nng_mqtt_msg_set_publish_topic(pubmsg, topic);
	if (props) {
		nng_mqtt_msg_set_publish_property(pubmsg, props);
	}
	log_debug("publish to '%s'", topic);

	return pubmsg;
}

static void
sub_callback(void *arg)
{
	nng_mqtt_client *client = (nng_mqtt_client *) arg;
	nng_aio *        aio    = client->sub_aio;
	nng_msg *        msg    = nng_aio_get_msg(aio);
	uint32_t         count;
	uint8_t *        code;
	if (msg) {
		code = nng_mqtt_msg_get_suback_return_codes(msg, &count);
		log_debug("suback %d \n", *(code));
	}
	log_debug("bridge: subscribe result %d \n", nng_aio_result(aio));
	nng_msg_free(msg);
	// nng_mqtt_client_free(client, true);
}

// Disconnect message callback function
static void
disconnect_cb(nng_pipe p, nng_pipe_ev ev, void *arg)
{
	int reason = 0;
	// get connect reason
	nng_pipe_get_int(p, NNG_OPT_MQTT_DISCONNECT_REASON, &reason);
	// property *prop;
	// nng_pipe_get_ptr(p, NNG_OPT_MQTT_DISCONNECT_PROPERTY, &prop);
	log_warn("bridge client disconnected! RC [%d] \n", reason);

	nng_cv *cv = arg;
	nng_cv_wake(cv);
}

// Connack message callback function
static void
bridge_connect_cb(nng_pipe p, nng_pipe_ev ev, void *arg)
{
	// Connected succeed
	bridge_param *param  = arg;
	int           reason = 0;
	// get connect reason
	nng_pipe_get_int(p, NNG_OPT_MQTT_CONNECT_REASON, &reason);
	// get property for MQTT V5
	// property *prop;
	// nng_pipe_get_ptr(p, NNG_OPT_MQTT_CONNECT_PROPERTY, &prop);
	log_info("bridge client connected! RC [%d] \n", reason);

	/* MQTT V5 SUBSCRIBE */
	if (reason == 0 && param->config->sub_count > 0) {
		nng_mqtt_topic_qos *topic_qos =
		    nng_mqtt_topic_qos_array_create(param->config->sub_count);
		for (size_t i = 0; i < param->config->sub_count; i++) {
			nng_mqtt_topic_qos_array_set(topic_qos, i,
			    param->config->sub_list[i].topic,
			    param->config->sub_list[i].qos);
		}
		nng_mqtt_client *client = param->client;
		// Property?
		nng_mqtt_subscribe_async(
		    client, topic_qos, param->config->sub_count, NULL);
		nng_mqtt_topic_qos_array_free(
		    topic_qos, param->config->sub_count);
	}
}

static int
bridge_tcp_client(bridge_param *bridge_arg, nng_cv *cv)
{
	int           rv;
	nng_dialer    dialer;
	nng_socket *  sock;
	conf_bridge_node *node;

	if (node->proto_ver == MQTT_PROTOCOL_VERSION_v5) {
		if ((rv = nng_mqttv5_client_open(sock)) != 0) {
			fatal("nng_mqttv5_client_open", rv);
			return rv;
		}
	} else {
		if ((rv = nng_mqtt_client_open(sock)) != 0) {
			fatal("nng_mqtt_client_open", rv);
			return rv;
		}
	}
	nng_socket_set(*sock, NANO_CONF, node, sizeof(conf_bridge_node));

	if ((rv = nng_dialer_create(&dialer, *sock, node->address))) {
		fatal("nng_dialer_create", rv);
		return rv;
	}

	// create a CONNECT message
	/* CONNECT */
	nng_msg *connmsg;
	nng_mqtt_msg_alloc(&connmsg, 0);
	nng_mqtt_msg_set_packet_type(connmsg, NNG_MQTT_CONNECT);
	nng_mqtt_msg_set_connect_keep_alive(connmsg, node->keepalive);
	nng_mqtt_msg_set_connect_proto_version(connmsg, node->proto_ver);
	nng_mqtt_msg_set_connect_clean_session(connmsg, node->clean_start);
	if (node->clientid) {
		nng_mqtt_msg_set_connect_client_id(connmsg, node->clientid);
	}
	if (node->username) {
		nng_mqtt_msg_set_connect_user_name(connmsg, node->username);
	}
	if (node->password) {
		nng_mqtt_msg_set_connect_password(connmsg, node->password);
	}

	bridge_arg         = (bridge_param *) nng_alloc(sizeof(bridge_param));
	bridge_arg->config = node;
	bridge_arg->sock   = sock;
	bridge_arg->client = nng_mqtt_client_alloc(*sock, sub_callback, true);

	node->sock         = (void *) sock;

	nng_dialer_set_ptr(dialer, NNG_OPT_MQTT_CONNMSG, connmsg);
	nng_mqtt_set_connect_cb(*sock, bridge_connect_cb, bridge_arg);
	nng_mqtt_set_disconnect_cb(*sock, disconnect_cb, cv);

	nng_dialer_start(dialer, NNG_FLAG_NONBLOCK);

	return 0;
}

#if defined(SUPP_QUIC)

// Disconnect message callback function
static int
quic_disconnect_cb(void *rmsg, void *arg)
{
	int reason = 0;
	if (!rmsg)
		return 0;
	// get connect reason
	reason = nng_mqtt_msg_get_connack_return_code(rmsg);
	// property *prop;
	// nng_pipe_get_ptr(p, NNG_OPT_MQTT_DISCONNECT_PROPERTY, &prop);
	log_warn("quic bridge client disconnected! RC [%d] \n", reason);
	nng_msg_free(rmsg);

	nng_cv *cv = arg;
	nng_cv_wake(cv);

	return 0;
}


// Connack message callback function
static int
bridge_quic_connect_cb(void *rmsg, void *arg)
{
	// Connected succeed
	bridge_param *param  = arg;
	nng_msg *msg = rmsg;
	int           reason = 0;
	// get connect reason
	reason = nng_mqtt_msg_get_connack_return_code(msg);
	// get property for MQTT V5
	// property *prop;
	// nng_pipe_get_ptr(p, NNG_OPT_MQTT_CONNECT_PROPERTY, &prop);
	log_debug("quic bridge client connected! RC [%d] \n", reason);
	nng_msg_free(msg);

	/* MQTT V5 SUBSCRIBE */
	if (reason == 0 && param->config->sub_count > 0) {
		nng_mqtt_topic_qos *topic_qos =
		    nng_mqtt_topic_qos_array_create(param->config->sub_count);
		for (size_t i = 0; i < param->config->sub_count; i++) {
			nng_mqtt_topic_qos_array_set(topic_qos, i,
			    param->config->sub_list[i].topic,
			    param->config->sub_list[i].qos);
		}

		nng_mqtt_client *client = param->client;
		// TODO support MQTT V5
		nng_mqtt_subscribe_async(
		    client, topic_qos, param->config->sub_count, NULL);
		nng_mqtt_topic_qos_array_free(
		    topic_qos, param->config->sub_count);
	}
	return 0;
}


static int
bridge_quic_client(bridge_param *bridge_arg, nng_cv *cv)
{
	int           rv;
	nng_dialer    dialer;
	log_info("Quic bridge service start.\n");

	nng_socket *sock = bridge_arg->sock;
	conf_bridge_node* node = bridge_arg->config;
	node->sock         = (void *) sock;

	if ((rv = nng_mqtt_quic_client_open(sock, node->address)) != 0) {
		fatal("nng_mqtt_quic_client_open", rv);
		return rv;
	}

	// TODO mqtt v5 protocol
	nng_socket_set(*sock, NANO_CONF, node, sizeof(conf_bridge_node));

	bridge_arg         = (bridge_param *) nng_alloc(sizeof(bridge_param));
	bridge_arg->config = node;
	bridge_arg->sock   = sock;
	bridge_arg->client = nng_mqtt_client_alloc(*sock, sub_callback, true);

	node->sock         = (void *) sock;

	if (0 != nng_mqtt_quic_set_connect_cb(sock, bridge_quic_connect_cb, (void *)bridge_arg) ||
	    0 != nng_mqtt_quic_set_disconnect_cb(sock, quic_disconnect_cb, (void *)cv)) {
	    //0 != nng_mqtt_quic_set_msg_recv_cb(sock, msg_recv_cb, (void *)arg) ||
	    //0 != nng_mqtt_quic_set_msg_send_cb(sock, msg_send_cb, (void *)arg)) {
		log_debug("error in quic client cb set.");
		return -1;
	}

	// create a CONNECT message
	/* CONNECT */
	nng_msg *connmsg;
	nng_mqtt_msg_alloc(&connmsg, 0);
	nng_mqtt_msg_set_packet_type(connmsg, NNG_MQTT_CONNECT);
	nng_mqtt_msg_set_connect_keep_alive(connmsg, node->keepalive);
	nng_mqtt_msg_set_connect_proto_version(connmsg, node->proto_ver);
	nng_mqtt_msg_set_connect_clean_session(connmsg, node->clean_start);
	if (node->clientid) {
		nng_mqtt_msg_set_connect_client_id(connmsg, node->clientid);
	}
	if (node->username) {
		nng_mqtt_msg_set_connect_user_name(connmsg, node->username);
	}
	if (node->password) {
		nng_mqtt_msg_set_connect_password(connmsg, node->password);
	}

	nng_sendmsg(*sock, connmsg, NNG_FLAG_NONBLOCK);

	return 0;
}

#endif

static void
hybridger_cb(void *arg)
{
	const char *quic_scheme = "mqtt-quic";
	const char *tcp_scheme  = "mqtt-tcp";

	bridge_param *bridge_arg = arg;
	conf_bridge_node *node;

	int rv = nng_mtx_alloc(&bridge_arg->switch_mtx);
	if (rv != 0) {
		fatal("nng_mtx_alloc", rv);
		return;
	}
	rv = nng_cv_alloc(&bridge_arg->switch_cv, bridge_arg->switch_mtx);
	if (rv != 0) {
		fatal("nng_cv_alloc", rv);
		return;
	}

	for (;;) {
		// Get next bridge node
		node = bridge_arg->config;
		log_warn("Bridge has switched to %s", node->address);

		if (0 == strncmp(node->address, tcp_scheme, 8)) {
			bridge_tcp_client(bridge_arg, bridge_arg->switch_cv);
#if defined(SUPP_QUIC)
		} else if (0 == strncmp(node->address, quic_scheme, 9)) {
			bridge_quic_client(bridge_arg, bridge_arg->switch_cv);
#endif
		} else {
			log_error("Unsupported bridge protocol.");
		}
		if (bridge_arg->exec_cv)
			nng_cv_wake1(bridge_arg->exec_cv);
		nng_cv_wait(bridge_arg->switch_cv);
	}

	nng_cv_free(bridge_arg->switch_cv);
}

int
bridge_client(nng_socket *sock, conf *config, conf_bridge_node *node)
{
	bridge_param *bridge_arg;
	bridge_arg = nng_alloc(sizeof(bridge_param));
	bridge_arg->config = node;
	bridge_arg->sock   = sock;
	bridge_arg->conf   = config;

	int rv = nng_mtx_alloc(&bridge_arg->exec_mtx);
	if (rv != 0) {
		fatal("nng_mtx_alloc", rv);
		return rv;
	}
	rv = nng_cv_alloc(&bridge_arg->exec_cv, bridge_arg->exec_mtx);
	if (rv != 0) {
		fatal("nng_cv_alloc", rv);
		return rv;
	}

	rv = nng_thread_create(&hybridger_thr, hybridger_cb, (void *)bridge_arg);
	if (rv != 0) {
		fatal("nng_thread_create", rv);
		return rv;
	}

	nng_cv_wait(bridge_arg->exec_cv);
	nng_cv_free(bridge_arg->exec_cv);
	bridge_arg->exec_cv = NULL;

	return rv;
}

