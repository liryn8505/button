# Arduino 双按键 HTTP 触发器

通过两个物理按键，分别向两台电脑发送 HTTP POST 请求，触发果茶接单接口。

## 硬件清单
- Arduino Leonardo + W5500 以太网扩展板 × 1
- 瞬时轻触按键 × 2
- 杜邦线若干
- 网线 × 1

## 接线说明
两个按键均使用内置上拉模式，无需额外电阻：

| 按键 | 开发板引脚 | 另一端 |
|------|-----------|--------|
| 按键1（电脑A） | D2 | GND |
| 按键2（电脑B） | D3 | GND |

## 功能说明
- 按下按键1 → 向第1台电脑发送 `POST /order-manager/start`
- 按下按键2 → 向第2台电脑发送 `POST /order-manager/start`
- 内置 50ms 消抖，按一次只触发一次
- 空请求体，符合原果茶接单协议

## 环境准备
### 软件
- Arduino IDE
- 安装 Ethernet 库（库管理器搜索 `Ethernet` 安装）

### 网络
- 开发板和两台电脑在**同一个局域网**内
- 两台电脑均运行 HTTP 服务，监听对应端口（默认 3266）

## 配置修改
打开 `button.ino`，修改以下参数：    

### 1. 开发板网络配置
```cpp
IPAddress localIP(192, 168, 6, 201);  // 板子静态IP，同网段不冲突即可
IPAddress gateway(192, 168, 6, 1);   // 网关地址
IPAddress subnet(255, 255, 255, 0);   // 子网掩码

###2.  两台目标电脑配置

// 第1台电脑（D2按键触发）
IPAddress serverIP1(192, 168, 6, 179);
const int serverPort1 = 3266;

// 第2台电脑（D3按键触发）
IPAddress serverIP2(192, 168, 6, 202);
const int serverPort2 = 3266;


### 3. 接口路径（默认不用改）
String pathKey1 = "/order-manager/start";
String pathKey2 = "/order-manager/start";





测试方法
1.自测（单电脑）
电脑运行 HTTP 测试服务，监听 0.0.0.0:3266
两个按键路径都设为同一个地址
用杜邦线碰 D2/D3 到 GND，观察服务端是否收到请求
返回 {"code":100000,"message":"成功"} 即为正常
2.联调（双电脑）
两台电脑分别启动 HTTP 服务
配置好 serverIP1 和 serverIP2
按下对应按键，两台电脑分别收到请求
3.串口输出说明:
板子IP: 192.168.6.201
D2 → 电脑1 | D3 → 电脑2
--------------------------------------
POST -> /order-manager/start
HTTP/1.1 200 OK
...
{"code":100000,"message":"成功"}
--- 请求完成 ---





