#include <Adafruit_NeoPixel.h>//オンボードLEDでも使用している
#include <time.h>
#include <Wire.h>
#include <CRC8.h>//by Rob Tillaart

//ピンアサイン割り当て
#define onboardLED 2//オンボードLED picoは27
#define onboardBTN_PIN 3//オンボードタクトスイッチ picoは39

Adafruit_NeoPixel pixels0(1, onboardLED);
//Adafruit_NeoPixel pixels0 = Adafruit_NeoPixel(3, 10, NEO_GRB + NEO_KHZ800);//いる？
hw_timer_t * timer = NULL;

const int PIN_I2C_SDA = 8;
const int PIN_I2C_SCL = 9;
const byte AHT25_ADDR = 0x38;
const double ERROR_VALUE = 999.0;

CRC8 crc;
double temperature;
double humidity;

uint8_t count_timer_10 = 0;
uint8_t count_timer_10_one = 0;
uint8_t count_timer_100 = 0;
uint8_t count_timer_100_one = 0;
uint8_t count_timer_500 = 0;
uint8_t count_timer_500_one = 0;
uint16_t count_timer_1000 = 0;
uint16_t count_timer_1000_one = 0;
uint16_t count_timer_60000 = 0;
uint16_t count_timer_60000_one = 0;
uint16_t count_timer_10m = 0;
uint16_t count_timer_10m_one = 0;
uint16_t count_timer_LCD_one = 0;
uint16_t count_onbordLed_one = 0;
uint16_t count_flicker_100 = 0;
uint16_t count_flicker_500 = 0;
uint16_t count_flicker_1000 = 0;
uint16_t count_flicker_60000 = 0;
uint16_t count_beep_100 = 0;
uint8_t onboardBTN = 0;
uint8_t onboardBTN_re = 0;
uint8_t onboardBTN_one = 0;

uint32_t colors[] = {
    pixels0.Color(10, 0, 0), //0 Red
    pixels0.Color(0, 10, 0), //1 Green
    pixels0.Color(0, 0, 30), //2 Blue
    pixels0.Color(10, 0, 10), //3 Pink
    pixels0.Color(10, 10, 0), //4 Yellow
    pixels0.Color(10, 3, 0),//5 Orange
    pixels0.Color(0, 10, 10), //6 Light blue
    pixels0.Color(10, 10, 10), //6 White
    pixels0.Color(10, 0, 0)};//7 Red
//const uint8_t COLORS_LEN = (uint8_t)(sizeof(colors) / sizeof(colors[0]));

void onTimer(){
  count_timer_10_one = 1;
  if(count_timer_100==9){count_timer_100 = 0;
    count_timer_100_one = 1;
    count_timer_LCD_one = 1;
    count_onbordLed_one = 1;
    if(count_flicker_100 == 0){  count_flicker_100 = 1;
    }else{                        count_flicker_100 = 0;}

    if(count_beep_100 == 0){}else{count_beep_100--;}//ビープ音長さ

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

    if(count_timer_60000==59){  count_timer_60000 = 0;
      count_timer_60000_one = 1;
      if(count_timer_10m == 9){ count_timer_10m = 0;
        count_timer_10m_one = 1;
      }else{  count_timer_10m++;}
    }else{  count_timer_60000++;}

    if(count_flicker_1000 == 0){  count_flicker_1000 = 1;
    }else{                        count_flicker_1000 = 0;}
  }else{                    count_timer_1000++;      }
}

void setup(void) {
  pixels0.begin();// initialize the pixel オンボードLED
  Serial.begin(115200);
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  //Wire.setWireTimeout(1000); // タイムアウトを1000msに設定
  // #if defined(WIRE_HAS_TIMEOUT)
  //   Wire.setWireTimeout(1000 /* us */, true /* reset_on_timeout */);
  // #endif  

  //割り込み処理の初期化
  timer = timerBegin(0, 80, true);// タイマ作成
  timerAttachInterrupt(timer, &onTimer, true); //タイマ割り込みサービス・ルーチン onTimer を登録
  timerAlarmWrite(timer, 10000, true);// 割り込みタイミングの設定 10000=10msec
  timerAlarmEnable(timer);// タイマ有効化

  //入出力設定
  //pinModeの設定値:INPUT_PULLUP,INPUT,OUTPUT
  pinMode(onboardBTN_PIN, INPUT_PULLUP);//オンボードタクトスイッチ

  //温湿度計センサー初期化
  initAht25();

  Serial.printf("Start.\n");
}

void loop() {

  io_prosess();//IO処理

  updateAht25();

  if(count_timer_1000_one != 0){count_timer_1000_one = 0;
    printSensorValues();
  }

  if(onboardBTN_one!=0){onboardBTN_one = 0;
    Serial.printf("Button OK.\n");  
  }

  //delay(3000);
}

void io_prosess(){
  //入出力処理

  //オンボードスイッチ
  onboardBTN = digitalRead(onboardBTN_PIN);
  if(onboardBTN != onboardBTN_re && onboardBTN == 0){
    onboardBTN_one = 1;
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

void initAht25(void) {
  delay(100);
  Wire.beginTransmission(AHT25_ADDR);
  Wire.write(0x71);
  Wire.endTransmission();
  delay(10);

  crc.setPolynome(0x31);
  crc.setStartXOR(0xFF);
}

void updateAht25(void) {
  byte buf[7];
  uint32_t humidity_raw;
  uint32_t temperature_raw;
  byte state;

  Wire.beginTransmission(AHT25_ADDR);
  Wire.write(0xAC);
  Wire.write(0x33);
  Wire.write(0x00);
  Wire.endTransmission();
  do {
    delay(80);
    Wire.requestFrom(AHT25_ADDR, 7);
    if (Wire.available() >= 7) {
      for(int i=0; i<7; i++) {
        buf[i] = Wire.read();
      }
    }
  } while((buf[0] & 0x80) != 0);

  crc.restart();
  crc.add(buf, 6);

  if (buf[6] == crc.getCRC()) {
    state = buf[0];
    humidity_raw = ((uint32_t)buf[1] << 12)|((uint32_t)buf[2] << 4)|(((uint32_t)buf[3] >> 4) & 0x0F);
    temperature_raw = (((uint32_t)buf[3] & 0x0F) << 16)|((uint32_t)buf[4] << 8)|((uint32_t)buf[5]);

    humidity = humidity_raw / 1048576.0 * 100;
    temperature = temperature_raw / 1048576.0 * 200 - 50;
  } else {
    // error
    humidity = ERROR_VALUE;
    temperature = ERROR_VALUE;
  }
}

void printSensorValues() {//温湿度をシリアル出力
  if (temperature == ERROR_VALUE || humidity == ERROR_VALUE) return;

  Serial.print("temperature: ");
  Serial.println(temperature);
  Serial.print("humidity: ");
  Serial.println(humidity);
}
