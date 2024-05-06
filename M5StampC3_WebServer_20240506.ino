#include <WiFi.h>
#include <WebServer.h>
#include <NTPClient.h>//NTPで使用
#include <WiFiUdp.h>//NTPで使用

WebServer server(80);

// WiFi情報
const char* ssid = "aterm-0dfd8a-g";
const char* pass = "9e6b4a5f07432";

int8_t parameter = 0;

//NTPサーバー関係
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp.nict.jp", 9*3600); // NICTのNTPサーバーを使用し、日本標準時(UTC+9)を指定

// 初期画面
void handleRoot(void){
    String html;

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
    html += "</body></html>";


    // HTMLを出力する
    server.send(200, "text/html", html);
}

// LEDのON/OFF
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

// 初期化
void setup()
{
    // シリアルポートの初期化
    Serial.begin(115200);

    // WiFiのアクセスポイントに接続
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    // ESP32のIPアドレスを出力
    Serial.println("WiFi Connected.");
    Serial.print("IP = ");
    Serial.println(WiFi.localIP());
    Serial.printf("parameter=%d\r\n",parameter);

    // Groveセンサーの初期化
    //pinMode(4, OUTPUT);

    // NTPサーバーに接続
    timeClient.begin();
    // 時刻の更新
    timeClient.update();
    // 時刻の取得
    Serial.println(timeClient.getFormattedTime());    

    // 処理するアドレスを定義
    server.on("/", handleRoot);
    server.on("/set", handleSET);
    server.onNotFound(handleNotFound);
    // Webサーバーを起動
    server.begin();
}

// 処理ループ
void loop()
{
    server.handleClient();
}
