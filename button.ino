#include <Ethernet.h>

// ---------------- 按键引脚 ----------------
#define KEY1_PIN 2    // D2 按键
#define KEY2_PIN 3    // D3 按键
const unsigned long debounceDelay = 50;  // 消抖50ms

// 按键1状态
int key1State;
int lastKey1State = HIGH;
unsigned long lastDebounce1 = 0;

// 按键2状态
int key2State;
int lastKey2State = HIGH;
unsigned long lastDebounce2 = 0;

// ---------------- W5500 网络配置 ----------------
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress localIP(192, 168, 6, 201);    // 板子静态IP
IPAddress gateway(192, 168, 6, 1);     // 网关
IPAddress subnet(255, 255, 255, 0);     // 子网掩码
EthernetClient client;

// ---------------- 目标服务器（电脑） ----------------
//IPAddress serverIP(192, 168, 124, 161);   // 电脑的局域网IP【自测

// ===== 联调版（两台电脑IP）=====
IPAddress serverIP1(192, 168, 6, 179); 
IPAddress serverIP2(192, 168, 6, 202);  


const int serverPort = 3266;   // 端口
String pathKey1 = "/order-manager/start";  // D2 触发路径
String pathKey2 = "/order-manager/start";  // D3 触发路径

// ===== IP 转字符串辅助函数 =====
String ipToStr(IPAddress ip) {
  String s = "";
  for (int i = 0; i < 4; i++) {
    s += String(ip[i]);
    if (i < 3) s += ".";
  }
  return s;
}

// ---------------- HTTP POST 请求函数 ----------------
void sendHttpPost(IPAddress ip, int port, String path) {
  if (client.connect(ip, port)) {
    String body = "";  // 空请求体

    String ipStr = ipToStr(ip); // 自定义函数转字符串
    String req = "POST " + path + " HTTP/1.1\r\n";
    req += "Host: " + ipStr + ":" + String(port) + "\r\n";
    req += "Content-Type: application/x-www-form-urlencoded\r\n";
    req += "Content-Length: " + String(body.length()) + "\r\n";
    req += "Connection: close\r\n\r\n";
    req += body;
    
    client.print(req);
    Serial.print("POST -> ");
    Serial.println(path);

    // 读取服务器响应（调试用）
    unsigned long timeout = millis() + 2000;
    while (client.connected() && millis() < timeout) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
      }
    }
    client.stop();
    Serial.println("\n--- 请求完成 ---\n");
  } else {
    Serial.print("连接失败: ");
    Serial.println(ipToStr(ip)); 
  }
}

void setup() {
  Serial.begin(115200);
  //while (!Serial) { ; }// 等待串口监视器连接
  pinMode(KEY1_PIN, INPUT_PULLUP);
  pinMode(KEY2_PIN, INPUT_PULLUP);

  Ethernet.begin(mac, localIP, gateway, gateway, subnet);
  Serial.print("板子IP: ");
  Serial.println(Ethernet.localIP());
  Serial.println("D2/D3: POST /order-manager/start");
  Serial.println("--------------------------------------");
}

void loop() {
  unsigned long currentTime = millis();

  // ====== D2 按键 ======
  int read1 = digitalRead(KEY1_PIN);
  if (read1 != lastKey1State) lastDebounce1 = currentTime;
  if ((currentTime - lastDebounce1) > debounceDelay) {
    if (read1 != key1State) {
      key1State = read1;
      if (key1State == LOW) {
        sendHttpPost(serverIP1, serverPort, pathKey1);//serverIP1
      }
    }
  }
  lastKey1State = read1;

  // ====== D3 按键 ======
  int read2 = digitalRead(KEY2_PIN);
  if (read2 != lastKey2State) lastDebounce2 = currentTime;
  if ((currentTime - lastDebounce2) > debounceDelay) {
    if (read2 != key2State) {
      key2State = read2;
      if (key2State == LOW) {
        sendHttpPost(serverIP2, serverPort, pathKey2);//serverIP2
      }
    }
  }
  lastKey2State = read2;
}