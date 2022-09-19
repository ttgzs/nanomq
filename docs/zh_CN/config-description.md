# 配置说明

## 简介

NanoMQ 的配置文件通常以 .conf 作为后缀名，你可以在 etc 目录找到这些配置文件，主要配置文件包括：

| 配置文件                | 说明                |
| ----------------------- | ------------------- |
| etc/nanomq.conf         | NanoMQ 配置文件     |
| etc/nanomq_gateway.conf | NanoMQ 网关配置文件 |

## 参数说明

### nanomq.conf

#### 基本配置参数

| 参数名                  | 数据类型    | 参数说明                                                  |
| --------------------- | ------- | ------------------------------------------------------------ |
| url              | String  | 监听url。                                                    |
| num_taskq_thread | Integer | 任务线程数。 |
| max_taskq_thread | Integer | 最大任务线程数。 |
| parallel |Long  | 并行数。 |
| property_size |Integer  | 最大属性长度。 |
| msq_len | Integer | 队列长度。 |
| qos_duration | Integer | QOS消息定时间隔时间。 |
| allow_anonymous | Boolean | 允许匿名登录。 |
| tls.enable | Boolean | 启动TLS监听（*默认false*）。 |
| tls.url |String  | TLS监听URL。 |
| tls.key | String | TLS私钥数据。 |
| tls.keypass | String | TLS私钥密码。 |
| tls.cert |String  | TLS Cert证书数据。 |
| tls.cacert | String | TLS CA证书数据。|
| tls.verify_peer | Boolean | 验证客户端证书。 |
| tls.fail_if_no_peer_cert | Boolean | 拒绝无证书连接，与*tls.verify_peer*配合使用。 |
| websocket.enable | Boolean | 启动websocket监听（*默认true*）。 |
| websocket.url | String  | Websocket监听URL。 |
| websocket.tls_url |  String | TLS over Websocket监听URL。 |
| http_server.enable| Boolean | 启动Http服务监听（*默认false*)。 |
| http_server.port | Integer | Http服务端监听端口。 |
| http_server.username | String | 访问Http服务用户名。 |
| http_server.password | String | 访问Http服务密码。 |
| http_server.auth_type | Enum | Http鉴权方式。（*默认 basic*） |
| http_server.jwt.public.keyfile | String |*JWT* 公钥文件. |
| http_server.jwt.private.keyfile | String |*JWT* 私钥文件. |
| log.to | Array[Enum] |日志输出类型数组，使用逗号`,`分隔多种类型<br>支持*文件，控制台，Syslog输出*，对应参数: <br>*file, console, syslog* |
| log.level | Enum |日志等级：trace, debug, info, warn, error, fatal |
| log.dir | String |日志文件存储路径 (输出文件时生效) |
| log.file | String |日志文件名(输出文件时生效) |
| log.rotation.size | Integer | 每个日志文件的最大占用空间; <br>支持单位: `KB | MB | GB`;<br> 默认: `10MB` |
| log.rotation.count | Integer | 轮换的最大日志文件数; <br>默认: `5` |

#### 标准MQTT桥接配置参数

| 参数名                  | 数据类型    | 参数说明                                                  |
| --------------------- | ------- | ------------------------------------------------------------ |
| bridge.mqtt.bridge_mode | Boolean | 启动桥接功能（*默认`false`不启用*）。 |
| bridge.mqtt.address | String | 桥接目标broker地址URL。 |
| bridge.mqtt.proto_ver | String | 桥接客户端MQTT版本（3｜4｜5）。 |
| bridge.mqtt.clientid | String | 桥接客户端ID（*默认NULL为自动生成随机ID*）。 |
| bridge.mqtt.keepalive | Integer | 保活间隔时间。 |
| bridge.mqtt.clean_start | Boolean | 清除会话。 |
| bridge.mqtt.parallel | Long | 桥接客户端并发数。 |
| bridge.mqtt.username | String | 登录用户名。 |
| bridge.mqtt.password | String | 登录密码。 |
| bridge.mqtt.forwards | Array[String] | 转发Topic数组, 使用逗号`,`分隔多个`Topic`。 |
| bridge.mqtt.subscription.1.topic | String | 第1个订阅`Topic`。                               |
| bridge.mqtt.subscription.1.qos | Integer | 第1个订阅`Qos`。                       |
| bridge.mqtt.subscription.2.topic | String        | 第2个（*以此类推*）订阅`Topic`。             |
| bridge.mqtt.subscription.2.qos   | Integer       | 第2个（*以此类推*）订阅`Qos`。 |

#### Aws IoT Core MQTT桥接配置参数

| 参数名                           | 数据类型      | 参数说明                                     |
| -------------------------------- | ------------- | -------------------------------------------- |
| bridge.mqtt.bridge_mode          | Boolean       | 启动桥接功能（*默认`false`不启用*）。        |
| bridge.mqtt.host                 | String        | AWS IoT Core服务地址。                       |
| bridge.mqtt.port                 | Integer       | AWS IoT Core MQTT端口。                      |
| bridge.mqtt.clientid             | String        | 桥接客户端ID（*默认NULL为自动生成随机ID*）。 |
| bridge.mqtt.keepalive            | Integer       | 保活间隔时间。                               |
| bridge.mqtt.clean_start          | Boolean       | 清除会话。                                   |
| bridge.mqtt.parallel             | Long          | 桥接客户端并发数。                           |
| bridge.mqtt.username             | String        | 登录用户名。                                 |
| bridge.mqtt.password             | String        | 登录密码。                                   |
| bridge.mqtt.forwards             | Array[String] | 转发Topic数组, 使用逗号`,`分隔多个`Topic`。  |
| bridge.mqtt.subscription.1.topic | String        | 第1个订阅`Topic`。                           |
| bridge.mqtt.subscription.1.qos   | Integer       | 第1个订阅`Qos`。                             |
| bridge.mqtt.subscription.2.topic | String        | 第2个（*以此类推*）订阅`Topic`。             |
| bridge.mqtt.subscription.2.qos   | Integer       | 第2个（*以此类推*）订阅`Qos`。               |

#### 用户登陆验证配置

| 参数名          | 数据类型 | 参数说明                        |
| --------------- | -------- | ------------------------------- |
| auth.1.login    | String   | 第1个登录用户名。               |
| auth.1.password | String   | 第1个登录密码。                 |
| auth.2.login    | String   | 第2个（*以此类推*）登录用户名。 |
| auth.2.password | String   | 第2个（*以此类推*）登录密码。   |

#### WebHook配置

| 参数名                                    | 数据类型 | 参数说明                                                    |
| ---------------------------------------- | ------- | ------------------------------------------------------------ |
| web.hook.enable                          | Boolean | 启动WebHook (默认: `false`)                                  |
| web.hook.url                             | String  | *Webhook URL*                                                |
| web.hook.headers.\<Any\>                 | String  | *HTTP Headers*<br>*Example:*<br>*1. web.hook.headers.content-type=application/json*<br> *2. web.hook.headers.accept=\** |
| web.hook.body.encoding_of_payload_field  | Enum    | *Payload编码方式*<br>Options: <br>plain \| base64 \| base62  |
| web.hook.ssl.cacertfile                  | String  | *TLS CA证书文件*.                                            |
| web.hook.ssl.certfile                    | String  | *TLS Cert证书文件*                                           |
| web.hook.ssl.keyfile                     | String  | *TLS 私钥文件.*                                              |
| web.hook.ssl.verify                      | Boolean | *验证客户端证书。*  (默认: `false`).                         |
| web.hook.ssl.server_name_indication      | Boolean | *验证服务端名*  (默认: `false`).                             |
| web.hook.pool_size                       | Integer | *连接池大小 （默认: 32）*.                                   |
| web.hook.rule.client.connack.\<No\>      | String  | 示例: <br>*web.hook.rule.client.connack.1={"action": "on_client_connack"}* |
| web.hook.rule.client.disconnected.\<No\> | String  | *示例: <br/>web.hook.rule.client.disconnected.1={"action": "on_client_disconnected"}* |
| web.hook.rule.message.publish.\<No\>     | String  | 示例: <br/>*web.hook.rule.message.publish.1={"action": "on_message_publish"}* <br>*web.hook.rule.message.publish.1={"action": "on_message_publish", "topic": "topic/1/2"}* <br>*web.hook.rule.message.publish.2 = {"action": "on_message_publish", "topic": "foo/#"}* |

#### HTTP身份验证配置

| 参数名                              | 数据类型 | 参数说明                                                     | 默认                                                         |
| ----------------------------------- | -------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| auth.http.enable                    | Boolean  | 启动HTTP认证                                                 | `false`                                                      |
| auth.http.auth_req.url              | String   | 认证请求的目标 URL。                                         | `http://127.0.0.1:80/mqtt/auth`                              |
| auth.http.auth_req.method           | Enum     | 认证请求的请求方法。<br>(`POST`  , `GET`)                    | `POST`                                                       |
| auth.http.auth_req.headers.\<Any\>  | String   | 指定 HTTP 请求头部中的数据。`<Key>` 指定 HTTP 请求头部中的字段名，此配置项的值为相应的字段值。`<Key>` 可以是标准的 HTTP 请求头部字段，也可以自定义的字段，可以配置多个不同的请求头部字段。<br> | `auth.http.auth_req.headers.content-type = application/x-www-form-urlencoded` <br/>`auth.http.auth_req.headers.accept = */*` |
| auth.http.auth_req.params           | String   | 指定认证请求中携带的数据。<br>以 `,` 分隔的 `k=v` 键值对，`v` 可以是固定内容，也可以是占位符。<br> 使用 **GET** 方法时 `auth.http.auth_req.params` 的值将被转换为以 `&` 分隔的 `k=v` 键值对以查询字符串参数的形式发送。<br>使用 **POST** 方法时 `auth.http.auth_req.params` 的值将被转换为以 `&` 分隔的 `k=v` 键值对以 Request Body 的形式发送。所有的占位符都会被运行时数据所替换，可用的占位符如下：<br>`%u: 用户名`<br>`%c: MQTT Client ID`<br>`%a: 客户端的网络 IP 地址`<br>`%r: 客户端使用的协议，可以是：mqtt, mqtt-sn, coap, lwm2m 以及 stomp`<br>`%P: 密码`<br>`%p: 客户端连接的服务端端口`<br>`%C: 客户端证书中的 Common Name`<br>`%d: 客户端证书中的 Subject` | `clientid=%c,username=%u,password=%P`                        |
| auth.http.super_req.url             | String   | 指定超级用户认证请求的目标 URL。                             | `http://127.0.0.1:80/mqtt/superuser`                         |
| auth.http.super_req.method          | String   | 指定超级用户认证请求的请求方法。<br>(`POST`  , `GET`)        | `POST`                                                       |
| auth.http.super_req.headers.\<Any\> | String   | 指定 HTTP 请求头部中的数据。`<Key>` 指定 HTTP 请求头部中的字段名，此配置项的值为相应的字段值。`<Key>` 可以是标准的 HTTP 请求头部字段，也可以自定义的字段，可以配置多个不同的请求头部字段。 | `auth.http.super_req.headers.content-type = application/x-www-form-urlencoded`<br/>`auth.http.super_req.headers.accept = */*` |
| auth.http.super_req.params          | String   | 指定超级用户认证请求中携带的数据。<br>使用 **GET** 方法时 `auth.http.super_req.params` 的值将被转换为以 `&` 分隔的 `k=v` 键值对以查询字符串参数的形式发送。<br>使用 **POST** 方法时 `auth.http.super_req.params` 的值将被转换为以 `&` 分隔的 `k=v` 键值对以 Request Body 的形式发送。所有的占位符都会被运行时数据所替换，可用的占位符同 `auth.http.auth_req.params`。 | `clientid=%c,username=%u`                                    |
| auth.http.acl_req.url               | String   | 指定 ACL 验证请求的目标 URL。                                | `http://127.0.0.1:8991/mqtt/acl`                             |
| auth.http.acl_req.method            | String   | 指定 ACL 验证请求的请求方法。(`POST`  , `GET`)               | `POST`                                                       |
| auth.http.acl_req.headers.\<Any\>   | String   | 指定 HTTP 请求头部中的数据。`<Key>` 指定 HTTP 请求头部中的字段名，此配置项的值为相应的字段值。`<Key>` 可以是标准的 HTTP 请求头部字段，也可以自定义的字段，可以配置多个不同的请求头部字段。 | `auth.http.super_req.headers.content-type = application/x-www-form-urlencoded`<br/>`auth.http.super_req.headers.accept = */*` |
| auth.http.acl_req.params            | String   | 指定 ACL 验证请求中携带的数据。以 `,` 分隔的 `k=v` 键值对，`v` 可以是固定内容，也可以是占位符。<br/> 使用 **GET** 方法时 `auth.http.acl_req.params` 的值将被转换为以 `&` 分隔的 `k=v` 键值对以查询字符串参数的形式发送。<br/>使用 **POST** 方法时 `auth.http.acl_req.params` 的值将被转换为以 `&` 分隔的 `k=v` 键值对以 Request Body 的形式发送。所有的占位符都会被运行时数据所替换，可用的占位符如下：<br/>`%A: 需要验证的权限，1 表示订阅，2 表示发布`<br>`%u: 用户名`<br/>`%c: MQTT Client ID`<br/>`%a: 客户端的网络 IP 地址`<br/>`%r: 客户端使用的协议，可以是：mqtt, mqtt-sn, coap, lwm2m 以及 stomp`<br/>`%m: 挂载点`<br>`%t: 主题` | `access=%A,username=%u,clientid=%c,ipaddr=%a,topic=%t,mountpoint=%m` |
| auth.http.timeout                   | Integer  | HTTP 请求超时时间。任何等价于 `0s` 的设定值都表示永不超时。  | `5s`                                                         |
| auth.http.connect_timeout           | Integer  | HTTP 请求的连接超时时间。任何等价于 `0s` 的设定值都表示永不超时。 | `5s`                                                         |
| auth.http.ssl.cacertfile            | String   | CA 证书文件路径。                                            | `etc/certs/ca.pem`                                           |
| auth.http.ssl.certfile              | String   | 客户端证书文件路径。                                         | `etc/certs/client-cert.pem`                                  |
| auth.http.ssl.keyfile               | String   | 客户端私钥文件路径。                                         | `etc/certs/client.key.pem`                                   |


#### 规则引擎配置

| 参数名                         | 数据类型 | 参数说明                                                                           |
| ------------------------------| ------- | -------------------------------------------------------------------------------- |
| rule_option                   | Enum    | 规则引擎开关, 当使用规则引擎进行持久化，必须设置该选项为 ON。                              |
| rule_option.sqlite            | Enum    | 规则引擎插件开关 (enable/disable)                                                   |
| rule_option.sqlite.conf.path  | String  | 规则引擎选项 sqlite3 配置文件路径                                                    |
| rule_option.repub             | Enum    | 规则引擎 repub 选项 (enable/disable)                                               |
| rule_option.repub.conf.path   | String  | 规则引擎 repub 配置文件路径                                                         |
| rule_option.mysql             | Enum    | 规则引擎 mysql 选项 (enable/disable)                                               |
| rule_option.mysql.conf.path   | String  | 规则引擎选项 mysql 配置文件路径                                                      |

#### SQLITE规则配置

| 参数名                         | 数据类型 | 参数说明                                                                           |
| ------------------------------| ------- | -------------------------------------------------------------------------------- |
| rule.sqlite.path              | String  | 规则引擎 SQLite3 数据库路径, 默认是 /tmp/rule_engine.db                              |
| rule.sqlite.%d.table          | String  | 规则引擎 SQLite3 数据库表名, '%d' 是占位符                                            |
| rule.event.publish.%d.sql     | String  | 规则引擎 sql 语句, '%d' 是占位符                                                     |

#### MYSQL规则配置

| 参数名                         | 数据类型 | 参数说明                                                                           |
| ------------------------------| ------- | -------------------------------------------------------------------------------- |
| rule.mysql.path              | String  | 规则引擎 mysql 数据库名字, 默认是 mysql_rule_db                                      |
| rule.mysql.%d.table           | String  | 规则引擎 mysql 数据库表名字, '%d' 是占位符                                            |
| rule.mysql.%d.host            | String  | 规则引擎 mysql 数据库主机名 '%d' 是占位符                                             |
| rule.mysql.%d.username        | String  | 规则引擎 mysql 数据库用户名, '%d' 是占位符                                            |
| rule.mysql.%d.password        | String  | 规则引擎 mysql 数据库密码, '%d' 是占位符                                              |
| rule.event.publish.%d.sql     | String  | 规则引擎 sql 语句, '%d' 是占位符                                                     |


#### Repub规则配置

| 参数名                         | 数据类型 | 参数说明                                                                           |
| ------------------------------| ------- | -------------------------------------------------------------------------------- |
| rule.repub.%d.address         | String  | 规则引擎重新发布地址 (mqtt-tcp://host:port), '%d' 是占位符                            |
| rule.repub.%d.topic           | String  | 规则引擎重新发布主题, '%d' 是占位符                                                   |
| rule.repub.%d.username        | String  | 规则引擎重新发布用户名, '%d' 是占位符                                                 |
| rule.repub.%d.password        | String  | 规则引擎重新发布密码, '%d' 是占位符                                                   |
| rule.repub.%d.proto_ver       | Integer | 规则引擎重新发布协议版本, 默认是 4, '%d' 是占位符                                       |
| rule.repub.%d.clientid        | String  | 规则引擎重新发布客户端标识符, '%d' 是占位符                                             |
| rule.repub.%d.keepalive       | Integer | 规则引擎重新发布保活时间, 默认值是 60, '%d' 是占位符                                     |
| rule.repub.%d.clean_start     | Boolean | 规则引擎重新发布 clean_start 标志, 默认是 true，'%d' 是占位符                           |
| rule.event.publish.%d.sql     | String  | 规则引擎 sql 语句, '%d' 是占位符                                                     |


### nanomq_gateway.conf

| 参数名                             | 数据类型  | 参数说明                                                      |
| --------------------------------- | ------- | ------------------------------------------------------------ |
| gateway.address                   | String  | 远端 Broker 地址。                                             |
| gateway.proto_ver                 | String  | MQTT 客户端版本（3｜4｜5)。                                     |
| gateway.clientid                  | String  | MQTT 客户端标识符。                                             |
| gateway.keepalive                 | Integer | 保活间隔时间。                                                  |
| gateway.clean_start               | Boolean | 清除会话标志。                                                  |
| gateway.parallel                  | Long    | 并行的 mqtt 客户端数量。                                         |
| gateway.username                  | String  | 登陆的用户名。                                                  |
| gateway.password                  | String  | 登陆的密码。                                                    |
| gateway.forward                   | String  | 转发的主题。                                                    |
| gateway.mqtt.subscription.topic   | String  | 订阅的 Mqtt 主题。                                              |
| gateway.mqtt.subscription.qos     | Integer | 订阅的 Mqtt 服务级别。                                           |
| gateway.zmq.sub.address           | String  | 远端的 ZMQ 服务订阅地址。                                         |
| gateway.zmq.pub.address           | String  | 远端的 ZMQ 服务发布地址。                                         |
| gateway.zmq.sub_prefix            | String  | 远端的 ZMQ 服务订阅前缀。                                         |
| gateway.zmq.pub_prefix            | String  | 远端的 ZMQ 服务发布前缀。                                         |
