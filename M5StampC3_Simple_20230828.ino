#include <WiFi.h>
#include <EMailSender.h>
#include <Adafruit_NeoPixel.h>
#define LED_PIN 2
#define COLOR_REPEAT 2
const char* ssid     = "";
const char* password = "";

WiFiServer server(80);
// create a pixel strand with 1 pixel on PIN_NEOPIXEL
Adafruit_NeoPixel pixels(1, LED_PIN);

//system
String ver = "0.9 20230816";

// Time
char ntpServer[] = "ntp.jst.mfeed.ad.jp";
const long gmtOffset_sec = 9 * 3600;
const int  daylightOffset_sec = 0;
struct tm timeinfo;
String dateStr;
String timeStr;
int8_t count_timer_10 = 0;
int8_t count_timer_10_one = 0;
int8_t count_timer_100 = 0;
int8_t count_timer_100_one = 0;
int8_t flicker_100 = 0;
int16_t count_timer_1000 = 0;
int16_t count_timer_1000_one = 0;
int16_t count_timer_LCD_one = 0;
int8_t count_timer_record_60000 = 0;//1秒単位でカウントアップ
int8_t count_timer_record_one = 0;
int16_t count_flicker_1000 = 0;
int16_t count_beep_100 = 0;
float millis_re = 0;
hw_timer_t * timer = NULL;

//e-mail
char myMailAdr[100]  = "myexperiment2020@gmail.com";
char myMailPass[100] = "cvmrglhxpboetpgb";
char toMailAdr[100]  = "myexperiment2020@gmail.com";//"zeatec.hirai@gmail.com";
EMailSender emailSend(myMailAdr, myMailPass);
//EMailSender emailSend("smtp.account@gmail.com", "mpdvpfpofvyaygfj");
//EMailSender(myMailAdr,myMailPass,myMailAdr,"smtp.account@gmail.com",25);
//EMailSender(const char* email_login, const char* email_password, const char* email_from, const char* smtp_server, uint16_t smtp_port);
int8_t sendmail_number = 0;//メールの送信回数

//SW
//※長押し処理を入れるとややこしくなるので要注意
//長押しをやめたときに0を入れてもすぐに1に戻ってしまい、0にするタイミングが難しい。
int8_t btnG3_on = 0;
// int8_t btnB_on = 0;
// int8_t btnC_on = 0;
int8_t btnG3_one = 0;
// //int8_t btnA_L_one = 0;
// int8_t btnB_one = 0;
// //int8_t btnB_L_one = 0;
// int8_t btnC_one = 0;
// //int8_t btnC_L_one = 0;

//LED
//sk6812 G2
//uint8_t color = 0, count = 0;
uint32_t colors[] = {pixels.Color(125, 0, 0), pixels.Color(0, 125, 0), pixels.Color(0, 0, 125), pixels.Color(125, 125, 125)};
const uint8_t COLORS_LEN = (uint8_t)(sizeof(colors) / sizeof(colors[0]));


//SPIのデフォルトピン番号
// SCK	4
// MISO	5
// MOSI	6
// SS	7

void setup(){
    Serial.begin(115200);
    pixels.begin();  // initialize the pixel

    pinMode(5, OUTPUT);      // set the LED pin mode

    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

  pixels.setPixelColor(0, colors[1]);
  pixels.show();    

}

void loop(){


 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(5, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(5, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
