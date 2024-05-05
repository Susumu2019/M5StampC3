#include <Adafruit_NeoPixel.h>//オンボードLEDでも使用している
#include <time.h>
#include <Wire.h>
#include "ADS1X15.h"//RobTillaart/ADS1X15 ライブラリ
// #include "AM232X.h"
// AM2315 sensor(&Wire1);
ADS1115 ADS(0x48);

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

//湿度、温度取得
float humi;//=(float)(rdptr[2]*256+rdptr[3])/10.0;
float temp;//=(float)(rdptr[4]*256+rdptr[5])/10.0;
float an0;// = ADS.readADC(0);//
float an1;// = ADS.readADC(1);//
float an2;// = ADS.readADC(2);//
float an3;// = ADS.readADC(3);//

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

  // while (sensor.begin() == false){//AM2321用
  //   // Serial.print(millis());
  //   // Serial.println("\tCould not connect to sensor.");
  //   // delay(1000);
  // }

  //初期値設定
  //

  //Serial.printf("Start.\n");

}

void loop() {
  
  io_prosess();


  if(count_timer_1000_one!=0){count_timer_1000_one = 0;

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

  // if (millis() - sensor.lastRead() >= 2000){
  //   //  READ DATA
  //   uint32_t start = micros();
  //   int status = sensor.read();
  //   uint32_t stop = micros();
  //   switch (status)
  //   {
  //   case AM2315_OK:
  //     Serial.print("OK,\t");
  //     break;
  //   case AM2315_ERROR_CHECKSUM:
  //     Serial.print("Checksum error,\t");
  //     break;
  //   case AM2315_ERROR_CONNECT:
  //     Serial.print("Connect error,\t");
  //     break;
  //   case AM2315_MISSING_BYTES:
  //     Serial.print("Bytes error,\t");
  //     break;
  //   default:
  //     Serial.print("error <");
  //     Serial.print(status);
  //     Serial.print(">,\t");
  //     break;
  //   }
  //   //  DISPLAY DATA, sensor has only one decimal.
  //   Serial.print("AM2315, \t");
  //   Serial.print(sensor.getHumidity(), 1);
  //   Serial.print(",\t");
  //   Serial.print(sensor.getTemperature(), 1);
  //   Serial.print(",\t");
  //   Serial.print(stop - start);
  //   Serial.print("\n");
  // }

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

void readAM2321(byte *rdptr, byte length ) 
{
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
