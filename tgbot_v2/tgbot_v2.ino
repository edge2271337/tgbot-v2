//импорт библиотек
#include <ESP8266WiFi.h>
#include <SimplePortal.h>
#include <FastBot.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
//инициализация переменных сети
char* ssid;
char* password;
//инициализация бота
#define token "HERE_BOT_TOKEN"  // Токен бота
FastBot bot(token);
#define logtoken "HERE_LOGGER_TOKEN"
FastBot logger(logtoken);
//менюха бота
const char* menu = "Случайный анекдот \t Случайная карта OSU \n О боте";
String loggerChatID = "HERE_YOUR_CHATID";
bool attachFlag = false;
uint32_t rndMap; //объявление переменной рандом мапы
//api chimu.moe
String serverName = "https://api.chimu.moe/v1/map/";
String host = "https://api.chimu.moe";
//анеки
const char* aneks[] = {"https://telegra.ph/bilet-do-harkova-05-03",
                       "https://telegra.ph/gus-05-03",
                       "https://telegra.ph/zabor-05-03",
                       "https://telegra.ph/zozo-05-03",
                       "https://telegra.ph/Izya-chasy-05-03",
                       "https://telegra.ph/klyukalo-05-03",
                       "https://telegra.ph/lyubov-obman-05-03",
                       "https://telegra.ph/Piterburg-05-03",
                       "https://telegra.ph/petuh-05-03",
                       "https://telegra.ph/pole-chudes-05-03",
                       "https://telegra.ph/polovinu-napisal-05-03",
                       "https://telegra.ph/s-mamoj-chto-to-05-03",
                       "https://telegra.ph/sapogi-05-03",
                       "https://telegra.ph/tovarishch-praporshchik-05-03",
                       "https://telegra.ph/shapka-razgovarivaet-05-03"
                      };


void setup() {

  Serial.begin(9600);
  startPortal();


  // portal status: 0 error, 1 connect, 2 ap, 3 local, 4 exit, 5 timeout

}

void loop() {

  connectWiFi();
  if (!attachFlag) {
    bot.attach(newMsg);
    attachFlag = true;
  }
  bot.tick();
  logger.tick();
}

void connectWiFi() {
  if (portalTick()) {
    Serial.println( portalStatus());
    if (portalStatus() == SP_SUBMIT) {
      ssid = portalCfg.SSID;
      Serial.println(ssid);
      password = portalCfg.pass;
      Serial.println(password);
      portalStop();
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
    }
  }
}

void startPortal() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(3000);
  Serial.println("portal running...");
  portalStart();
  Serial.println("portal runned.");
}

class Timer {
  public:
    Timer(uint32_t nprd = 0) {
      setPeriod(nprd);
    }
    void setPeriod(uint32_t nprd) {
      prd = nprd;
    }
    bool ready() {
      if (prd && millis() - tmr >= prd) return tmr = millis();
      return false;
    }
  private:
    uint32_t tmr = 0, prd = 0;
};

void newMsg(FB_msg& msg) {
  String message = "";

  if (msg.text == "Случайный анекдот") {
    message = aneks[random(0, 15)];
    bot.sendMessage(message, msg.chatID);
  }
  if (msg.text == "/start" or msg.text == "О боте") {
    //bot.closeMenu(msg.chatID);
    bot.showMenu(menu, msg.chatID);
    message = "Здравствуй, " + msg.username + ".\nЭто бот для случайных лучших анекдотов.\nСоздатель:@NeverForever";
    bot.sendMessage(message, msg.chatID);
  }
  if (msg.text == "Случайная карта OSU") {
    WiFiClientSecure client;
    HTTPClient http;
    client.setInsecure();
    client.connect(host, 443);
    int httpResponseCode = 404;
    while (httpResponseCode > 0 and httpResponseCode == 404) {
      rndMap = random(0, 1000000);
      String serverPath = serverName + rndMap;

      // Your Domain name with URL path or IP address with path
      http.begin(client, serverPath.c_str());

      // Send HTTP GET request
      httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        StaticJsonDocument<768> doc;

        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }

        int BeatmapId = doc["BeatmapId"]; // 53
        int ParentSetId = doc["ParentSetId"]; // 3
        const String DiffName = doc["DiffName"]; // "-Crusin-"
        const char* FileMD5 = doc["FileMD5"]; // "1d23c37a2fda439be752ae2bca06c0cd"
        int Mode = doc["Mode"]; // 0
        int BPM = doc["BPM"]; // 172
        int AR = doc["AR"]; // 4
        int OD = doc["OD"]; // 4
        int CS = doc["CS"]; // 5
        int HP = doc["HP"]; // 3
        int TotalLength = doc["TotalLength"]; // 83
        int HitLength = doc["HitLength"]; // 77
        long Playcount = doc["Playcount"]; // 115056
        long Passcount = doc["Passcount"]; // 53367
        int MaxCombo = doc["MaxCombo"]; // 124
        float DifficultyRating = doc["DifficultyRating"]; // 2.18294
        const String OsuFile = doc["OsuFile"]; // "Ni-Ni - 1,2,3,4, 007 [Wipeout Series] (MCXD) [-Crusin-].osu"
        const String DownloadPath = doc["DownloadPath"]; // "/d/3"
        if (OsuFile != "null") {
          message = "Карта: " + OsuFile + "\nBPM: " + BPM + "\nСложность: " + DifficultyRating + "\nСкачать: " + serverName + BeatmapId + DownloadPath;
          bot.sendMessage(message, msg.chatID);
        }
        http.end();
      }
    }
  }
  logger.sendMessage("Сообщение от юзера " + msg.username + ":\n" + msg.text + "\nОтвет бота:\n" + message, loggerChatID);
}
