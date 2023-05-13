#include <Arduino.h>
#include "WiFi.h"
#include "WiFiUdp.h"
#include <HTTPClient.h>
#include <iostream>
#include <Arduino_GFX_Library.h>
#include <my_gfx.h>

using namespace std;
void Wifi_initial();
String get_ntp();
void sendNTPPacket();
String Get_Weather();
String formatTwoDigit(int number);
String GetURL(String api, String city);

const char *ssid = "wdxm";          // WiFi 名称
const char *password = "woaini998"; // WiFi 密码

// NTP 服务器地址
const char *ntpServer = "pool.ntp.org";
const int ntpPort = 123;

// UDP 客户端对象
WiFiUDP udp;
// 缓存 NTP 数据包的数组
byte packetBuffer[48];
// 时间偏移量
const int timeZone = 8;

String API = "SMF6tu5lHTaAK2GbQ";
String WeatherURL = "";
String CITY = "北京";
String url_xinzhi = "";
String Weather = "";

Arduino_DataBus *bus = new Arduino_HWSPI(22 /* DC */, 5 /* CS */, 18, 23);
Arduino_GFX *gfx = new Arduino_ILI9341(bus, 21 /* RST */);
my_gfx m_gfx(gfx);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  m_gfx.initial_test();
  Wifi_initial();
  WeatherURL = GetURL(API, CITY);
}

void loop()
{
  m_gfx.update_dis(Get_Weather(), get_ntp());
}

String Get_Weather()
{
  String payload = "";

  HTTPClient http;
  http.begin(WeatherURL);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK)
  {
    payload = http.getString();
  }

  http.end();
  return payload;
}

void Wifi_initial()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // 初始化 UDP 客户端
  udp.begin(8888);
}

String GetURL(String api, String city)
{
  url_xinzhi = "https://api.seniverse.com/v3/weather/now.json?key=";
  url_xinzhi += api;
  url_xinzhi += "&location=";
  url_xinzhi += city;
  url_xinzhi += "&language=zh-Hans&unit=c";
  return url_xinzhi;
}

String formatTwoDigit(int number)
{
  // 将数字格式化为两位数的字符串
  if (number < 10)
  {
    return "0" + String(number);
  }
  else
  {
    return String(number);
  }
}

void sendNTPPacket()
{
  // 清空缓存
  memset(packetBuffer, 0, 48);

  // 设置 NTP 数据包头部信息
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;

  // 向 NTP 服务器发送请求
  udp.beginPacket(ntpServer, ntpPort);
  udp.write(packetBuffer, 48);
  udp.endPacket();
}

String get_ntp()
{
  sendNTPPacket();
  // 等待 NTP 响应
  delay(1000);
  if (udp.parsePacket())
  {
    // 读取 NTP 数据包
    udp.read(packetBuffer, 48);

    // 将 NTP 数据包转换为时间戳
    unsigned long secsSince1900 = (unsigned long)packetBuffer[40] << 24;
    secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
    secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
    secsSince1900 |= (unsigned long)packetBuffer[43];

    // 计算时间偏移量
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;
    unsigned long localEpoch = epoch + timeZone * 3600;

    // 将时间戳转换为日期和时间
    struct tm *timeinfo;
    timeinfo = gmtime((time_t *)&localEpoch);

    // 输出日期和时间
    int year = (timeinfo->tm_year + 1900);
    int month = (timeinfo->tm_mon + 1);
    int day = (timeinfo->tm_mday);
    int hour = (timeinfo->tm_hour);
    int minute = (timeinfo->tm_min);
    int second = (timeinfo->tm_sec);

    String dateTime ="更新时间：" +  String(year) + "-" + formatTwoDigit(month) + "-" + formatTwoDigit(day) +
                      " " + formatTwoDigit(hour) + ":" + formatTwoDigit(minute) + ":" + formatTwoDigit(second);

    Serial.println(dateTime);
    return dateTime;
  }
  return "";
}