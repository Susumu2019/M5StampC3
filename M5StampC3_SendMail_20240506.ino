#include <Adafruit_NeoPixel.h>//オンボードLEDでも使用している
#include <time.h>
#include <WiFi.h>                          //WiFiライブラリ
#include <ESP_Mail_Client.h>               //メール送信ライブラリ
#define WIFI_SSID "aterm-0dfd8a-g"         //WiFiのSSID
#define WIFI_PASSWORD "9e6b4a5f07432"       //SSIDに合った暗号キー
#define SMTP_server "smtp.gmail.com"       //googleのSMTPアドレス
#define SMTP_Port 465                      //SMTPポート
#define sender_name "M5StampC3"                //送信者の名前
#define sender_email "myexperiment2020@gmail.com"    //送信者のGMailアドレス。アカウント名としても使う
#define sender_password "aattyozmhzwsjury"    //メールパスワード。　Googleのアプリ送信許可を得たもの。2段階認証の手続きで新たにもらえる。
#define Recipient_email "zeatec.hirai@gmail.com"    //送信先メールアドレス
#define Recipient_name ""                  //送信先名。空欄でよい。
#define onboardLED 2//オンボードLED picoは27
#define onboardBTN_PIN 3//オンボードタクトスイッチ picoは39

Adafruit_NeoPixel pixels0(1, onboardLED);
//Adafruit_NeoPixel pixels0(1, onboardLED, NEO_GRB + NEO_KHZ800);
SMTPSession smtp;  //電子メール送信に使用するSMTPセッションオブジェクトを作成する。
hw_timer_t * timer = NULL;

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
uint8_t onboardBTN_one = 0;
uint8_t checkON_count = 0;
uint8_t sendmail_status = 0;

uint32_t colors[] = {
    pixels0.Color(10, 0, 0), //0 Red
    pixels0.Color(0, 10, 0), //1 Green
    pixels0.Color(0, 0, 30), //2 Blue
    pixels0.Color(10, 0, 10), //3 Pink
    pixels0.Color(10, 10, 0), //4 Yellow
    pixels0.Color(10, 3, 0),//5 Orange
    pixels0.Color(0, 10, 10), //6 Light blue
    pixels0.Color(10, 10, 10),//7 White
    pixels0.Color(30, 0, 0)};//8 Red
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

void setup(){
  pixels0.begin();  // initialize the pixel オンボードLED
  //pixels.setBrightness(100);
  Serial.begin(115200);

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

  Serial.println();
  Serial.print("Connecting...");
   
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  sendmail_status = 10;
}

void loop(){

  io_prosess();

  if(onboardBTN_one==2){onboardBTN_one = 1;
    //pixels0.setPixelColor(0, colors[4]);//White
    sendmail_status = 20;
  }else if(onboardBTN_one==1){onboardBTN_one = 0;

    //メール送信
    send_mail();

    sendmail_status = 10;
  }

}

void send_mail(){
  //メール設定
  smtp.debug(1);  //デバッグモードを指定。0=行わない 1=基本デバッグを行う

  ESP_Mail_Session session;
  session.server.host_name = SMTP_server ;
  session.server.port = SMTP_Port;
  session.login.email = sender_email;
  session.login.password = sender_password;
  session.login.user_domain = "";

  //メッセージクラスの宣言
  char *subject = "メール送信テスト 202405062231";//メールの件名
  SMTP_Message message;
  message.sender.name = sender_name;
  message.sender.email = sender_email;
  message.subject = subject;
  message.addRecipient(Recipient_name,Recipient_email);

  //テキストメッセージの作成
  String textMsg = "このメッセージが見れたらＯＫ。";//ここにメールの本文を書きます
  message.text.content = textMsg.c_str();
  message.text.charSet = "utf-8";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  //メール送信
  if (!smtp.connect(&session)){
    return;
  }

  if (!MailClient.sendMail(&smtp, &message)){
    Serial.println("Error sending Email, " + smtp.errorReason());
  }
}

void io_prosess(){
  //入出力処理

  //オンボードスイッチ
  onboardBTN = digitalRead(onboardBTN_PIN);
  if(onboardBTN != onboardBTN_re && onboardBTN == 0){
    onboardBTN_one = 2;
    Serial.printf("onboardBTN on..\n");  
  }
  onboardBTN_re = onboardBTN;

  if(count_onbordLed_one!=0){count_onbordLed_one == 0;
    //オンボードLEDを制御
    if(sendmail_status == 10){
      if(count_flicker_1000 == 0){
        pixels0.setPixelColor(0, colors[1]);//Green
      }else{
        pixels0.clear();
      }
    }else if(sendmail_status == 20){
      pixels0.setPixelColor(0, colors[2]);//
    }    
    pixels0.show();
  }

  //アナログ入力
  //int an0 = analogRead(0);

  //デジタル入力
  //onBTN = !digitalRead(Pin_G6);//無線リモコンからのON信号
  //checkBTN = !digitalRead(Pin_G7);//無線リモコンからのCheck信号

}