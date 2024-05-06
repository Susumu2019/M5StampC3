#include <Adafruit_NeoPixel.h>//オンボードLEDでも使用している
#include <time.h>
#include <Wire.h>
#include "ADS1X15.h"//RobTillaart/ADS1X15 ライブラリ
#include <WebServer.h>
#include <NTPClient.h>//NTPで使用
#include <WiFiUdp.h>//NTPで使用

ADS1115 ADS(0x48);
WebServer server(80);
WiFiUDP ntpUDP;//NTPサーバー関係
NTPClient timeClient(ntpUDP, "ntp.nict.jp", 9*3600); // NICTのNTPサーバーを使用し、日本標準時(UTC+9)を指定

hw_timer_t * timer = NULL;

#define onboardLED 2//オンボードLED picoは27
#define onboardBTN_PIN 3//オンボードタクトスイッチ picoは39

#define Pin_G4 4//
#define Pin_G5 5//
#define Pin_G6 6//
#define Pin_G7 7//
#define Pin_G8 8//
#define Pin_G10 10//
#define Pin_G1 1//
#define Pin_G0 0//
#define Pin_G21 21//
#define Pin_G20 20//
#define Pin_G9 9//
#define Pin_G18 18//
#define Pin_G19 19//

Adafruit_NeoPixel pixels0(1, onboardLED);
//Adafruit_NeoPixel pixels0 = Adafruit_NeoPixel(3, 10, NEO_GRB + NEO_KHZ800);//いる？

int8_t count_timer_10 = 0;
int8_t count_timer_10_one = 0;
int8_t count_timer_100 = 0;
int8_t count_timer_100_one = 0;
int8_t count_timer_500 = 0;
int8_t count_timer_500_one = 0;
int16_t count_timer_1000 = 0;
int16_t count_timer_1000_one = 0;
int16_t count_timer_LCD_one = 0;
int16_t count_onbordLed_one = 0;
int16_t count_flicker_100 = 0;
int16_t count_flicker_500 = 0;
int16_t count_flicker_1000 = 0;
int16_t count_beep_100 = 0;
uint8_t onboardBTN = 0;
uint8_t onboardBTN_re = 0;
uint8_t checkON_count = 0;
const char* ssid = "aterm-0dfd8a-g";// WiFi情報
const char* pass = "9e6b4a5f07432";// WiFi情報
int date_year; // 年
int date_month;// 月
int date_day;  // 日
int date_hour; // 時
int date_minute;// 分
int date_second;// 秒
int8_t parameter = 0;

//湿度、温度取得
float humi;//湿度
float temp;//温度
float an0;//
float an1;//
float an2;//
float an3;//

uint32_t colors[] = {
    pixels0.Color(10, 0, 0), //0 Red
    pixels0.Color(0, 10, 0), //1 Green
    pixels0.Color(0, 0, 30), //2 Blue
    pixels0.Color(10, 0, 10), //3 Pink
    pixels0.Color(10, 10, 0), //4 Yellow
    pixels0.Color(10, 3, 0),//5 Orange
    pixels0.Color(0, 10, 10), //6 Light blue
    pixels0.Color(10, 10, 10)};//7 White
const uint8_t COLORS_LEN = (uint8_t)(sizeof(colors) / sizeof(colors[0]));

void onTimer(){
  count_timer_10_one = 1;
  if(count_timer_100==9){count_timer_100 = 0;
    count_timer_100_one = 1;
    count_timer_LCD_one = 1;
    count_onbordLed_one = 1;
    if(count_flicker_100 == 0){  count_flicker_100 = 1;
    }else{                        count_flicker_100 = 0;}

    if(count_beep_100 == 0){}else{count_beep_100--;}//ビープ音長さ

    if(checkON_count == 0){}else{checkON_count--;}//Check表示時間  
  }else{                  count_timer_100++;  }

  if(count_timer_500==49){count_timer_500 = 0;
    count_timer_500_one = 1;
    //Serial.printf("millis(500)= %u ms\n", millis());//デバッグ用

    if(count_flicker_500 == 0){  count_flicker_500 = 1;
    }else{                        count_flicker_500 = 0;}
  }else{                    count_timer_500++;      }

  if(count_timer_1000==99){count_timer_1000 = 0;
    count_timer_1000_one = 1;
    //Serial.printf("millis(1000)= %u ms\n", millis());//デバッグ用

    if(count_flicker_1000 == 0){  count_flicker_1000 = 1;
    }else{                        count_flicker_1000 = 0;}
  }else{                    count_timer_1000++;      }
}

void setup() {
  pixels0.begin();  // initialize the pixel オンボードLED
  Serial.begin(115200);
  Wire.begin();//SDA=8,SCL=9(デフォルト)
  ADS.begin();

  //割り込み処理の初期化
  timer = timerBegin(0, 80, true);// タイマ作成
  timerAttachInterrupt(timer, &onTimer, true); //タイマ割り込みサービス・ルーチン onTimer を登録
  timerAlarmWrite(timer, 10000, true);// 割り込みタイミングの設定 10000=10msec
  timerAlarmEnable(timer);// タイマ有効化

  //入出力設定
  //pinModeの設定値:INPUT_PULLUP,INPUT,OUTPUT
  pinMode(onboardBTN_PIN, INPUT_PULLUP);//オンボードタクトスイッチ
  // pinMode(0,INPUT);//
  // pinMode(1,INPUT);//
  // pinMode(4,INPUT);//
  //pinMode(Pin_G6, INPUT_PULLUP);//

  //初期値設定
  //

  // WiFiのアクセスポイントに接続
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
  }
  // ESP32のIPアドレスを出力
  Serial.println("WiFi Connected.");
  Serial.print("IP = ");
  Serial.println(WiFi.localIP());
  //Serial.printf("parameter=%d\r\n",parameter);

  // NTPサーバーに接続
  timeClient.begin();
  timeClient.update();// 時刻の更新
  date_update();
  Serial.printf("%d/%d/%d %d:%d:%d\r\n",date_year,date_month,date_day,date_hour,date_minute,date_second);

  // 処理するアドレスを定義
  server.on("/", handleRoot);
  server.on("/set", handleSET);
  server.onNotFound(handleNotFound);
  server.begin();// Webサーバーを起動


  //Serial.printf("Start.\n");

}

void loop() {
  
  io_prosess();

  server.handleClient();

  if(count_timer_1000_one!=0){count_timer_1000_one = 0;

    // getLocalTime(&timeInfo);
    // Serial.printf("%02u/%02u/%02u\r\n", timeInfo.tm_year - 100, timeInfo.tm_mon + 1, timeInfo.tm_mday);
    // Serial.printf("%02u:%02u:%02u\r\n", timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);

    //湿度、温度取得
    byte rdptr[20];
    readAM2321(rdptr,8);
    humi=(float)(rdptr[2]*256+rdptr[3])/10.0;
    temp=(float)(rdptr[4]*256+rdptr[5])/10.0;

    //ADS1115アナログ入力
    ADS.setGain(0);
    float ADS_f = ADS.toVoltage(1);  // voltage factor
    an0 = ADS.readADC(0)*ADS_f;//
    an1 = ADS.readADC(1)*ADS_f;//
    an2 = ADS.readADC(2)*ADS_f;//
    an3 = ADS.readADC(3)*ADS_f;//

    //デバッグ用
    Serial.printf("humi=%2.1f, temp=%2.1f\r\n",humi,temp);//湿度、温度取得
    Serial.printf("an0,1,2,3=%1.2f,%1.2f,%1.2f,%1.2f\r\n",an0,an1,an2,an3);  
    //Serial.printf("PIN_analog4=%ld\n",PIN_analog4);//PIN_analog5
    //Serial.printf("BTN=%d,%d,%d %d\n",onBTN,checkBTN,offBTN,relay_on);
    //通常運転時
    // for(uint8_t tmp1=0;tmp1<42;++tmp1){
    // }

  }

}

void io_prosess(){
  //入出力処理

  //オンボードスイッチ
  onboardBTN = digitalRead(onboardBTN_PIN);
  if(onboardBTN != onboardBTN_re && onboardBTN == 0){
    Serial.printf("onboardBTN on..\n");  
  }
  onboardBTN_re = onboardBTN;

  if(count_onbordLed_one!=0){count_onbordLed_one == 0;
    //オンボードLEDを制御
    if(count_flicker_1000 == 0){
      pixels0.setPixelColor(0, colors[1]);
    }else{
      pixels0.clear();
    }
    pixels0.show();
  }

  //アナログ入力
  //int an0 = analogRead(0);

  //デジタル入力
  //onBTN = !digitalRead(Pin_G6);//無線リモコンからのON信号
  //checkBTN = !digitalRead(Pin_G7);//無線リモコンからのCheck信号

}

void readAM2321(byte *rdptr, byte length ) {
  int i;
  byte  deviceaddress=0x5C;
  //step1
  Wire.beginTransmission(deviceaddress);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(1);
  //step2
  Wire.beginTransmission(deviceaddress);
  Wire.write( 0x03);
  Wire.write(0x00);
  Wire.write(0x04);
  Wire.endTransmission();
  delay(2);
  //step3
  Wire.requestFrom(deviceaddress,length);
  delayMicroseconds(60);
  if(length <= Wire.available())
  {
    for(i=0;i<length;i++)
    {
      rdptr[i] = Wire.read();
    }
  }
}

void date_update(){
    // Unix時間を取得
    unsigned long unixTime = timeClient.getEpochTime();
    // Unix時間をローカル時間に変換
    struct tm *timeinfo;
    timeinfo = localtime((const time_t *)&unixTime);
    // 日時要素の取得
    date_year = timeinfo->tm_year + 1900; // 年
    date_month = timeinfo->tm_mon + 1;     // 月
    date_day = timeinfo->tm_mday;          // 日
    date_hour = timeinfo->tm_hour;         // 時
    date_minute = timeinfo->tm_min;        // 分
    date_second = timeinfo->tm_sec;        // 秒
}

// ルート画面
void handleRoot(void){
    String html;
    char buffer[20]; // 文字列を格納するためのバッファ

    date_update();//日時を更新

    // HTMLを組み立てる
    html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
    html += "<title>parameterを1/0する</title></head><body>";
    html += "<p>リンクをクリックするとparameterが1/0します</p>";
    html += "<a href='/set?p=1'>1</a><br>";
    html += "<a href='/set?p=0'>0</a>";
    html += "<form action='set' method='get'>";
    html += "   <select name='p'>";
    html += "     <option value='10'>10</option>";
    html += "     <option value='11'>11</option>";
    html += "   </select>";
    html += "<input type='submit' value='Submit'>";
    html += "</form>";
    sprintf(buffer, "%d/%d/%d %d:%d:%d\r\n",date_year,date_month,date_day,date_hour,date_minute,date_second);
    html += buffer;
    html += "</body></html>";

    // HTMLを出力する
    server.send(200, "text/html", html);
}

// 
void handleSET(void){
    String html;

    // 「/led?s=○」のパラメータが指定されているかどうかを確認
    if (server.hasArg("p"))
    {

        html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
        html += "<title>parameterを1/0する</title></head><body>";

        // 「○」の値に応じて、LEDをON/OFFする
        if (server.arg("p").equals("1")){
            parameter = 1;
            //digitalWrite(4, HIGH);

            html += "<p>parameterを1にしました</p>";
        }else if (server.arg("p").equals("0")){
            parameter = 0;
            html += "<p>parameterを0にしました</p>";
        }else if (server.arg("p").equals("10")){
            parameter = 10;
            html += "<p>parameterを10にしました</p>";
        }else if (server.arg("p").equals("11")){
            parameter = 11;
            html += "<p>parameterを11にしました</p>";

        }else{
            html += "パラメータが正しく指定されていません";

        }

        html += "<a href='#' onclick='history.back(); return false;'>back</a>";
        html += "</body></html>";

        Serial.printf("parameter=%d\r\n",parameter);
    }

    // 変数msgの文字列を送信する
    server.send(200, "text/html", html);
    //server.send(200, "text/plain; charset=utf-8", msg);
}

// 存在しないアドレスが指定された時の処理
void handleNotFound(void){
    server.send(404, "text/plain", "Not Found.");
}
