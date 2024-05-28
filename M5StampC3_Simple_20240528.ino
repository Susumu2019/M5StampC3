#include <Adafruit_NeoPixel.h>//オンボードLEDでも使用している
#define onboardLED 2//オンボードLED picoは27
#define onboardBTN_PIN 3//オンボードタクトスイッチ picoは39

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
uint8_t checkON_count = 0;
uint8_t onboardBTN = 0;
uint8_t onboardBTN_re = 0;

hw_timer_t * timer = NULL;
Adafruit_NeoPixel pixels0(1, onboardLED);

uint32_t colors[] = {
    pixels0.Color(10, 0, 0), //0 Red
    pixels0.Color(0, 10, 0), //1 Green
    pixels0.Color(0, 0, 30), //2 Blue
    pixels0.Color(10, 0, 10), //3 Pink
    pixels0.Color(10, 10, 0), //4 Yellow
    pixels0.Color(10, 3, 0),//5 Orange
    pixels0.Color(0, 10, 10), //6 Light blue
    pixels0.Color(10, 10, 10), //7 White
    pixels0.Color(255, 255, 255)};//8 FULL

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
    pinMode(onboardLED, OUTPUT);
    pinMode(onboardBTN_PIN, INPUT_PULLUP);

    //割り込み処理の初期化
    timer = timerBegin(0, 80, true);// タイマ作成
    timerAttachInterrupt(timer, &onTimer, true); //タイマ割り込みサービス・ルーチン onTimer を登録
    timerAlarmWrite(timer, 10000, true);// 割り込みタイミングの設定 10000=10msec
    timerAlarmEnable(timer);// タイマ有効化

    Serial.print("Start\n");
}

void loop() {
    if (digitalRead(onboardBTN_PIN) != true) {
        Serial.print("ON\n");
        if(count_flicker_500!=0){
            pixels0.setPixelColor(0, colors[4]);
        }else{
            pixels0.setPixelColor(0, colors[5]);
        }
    }else{
        if(count_flicker_500!=0){
            pixels0.setPixelColor(0, colors[1]);
        }else{
            pixels0.setPixelColor(0, colors[7]);
        }
    }
    pixels0.show();
}
