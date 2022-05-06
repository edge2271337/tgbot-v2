//импорт библиотек
#include <ESP8266WiFi.h>
#include <SimplePortal.h>
#include <FastBot.h>
//инициализация переменных сети
char* ssid;
char* password;
//инициализация бота
#define token "HERE_BOT_TOKEN"  // Токен бота
FastBot bot(token);
#define logtoken "HERE_LOGGER_TOKEN"
FastBot logger(logtoken);
//менюха бота
const char* menu = "Случайный анекдот \n О боте";
String loggerChatID = "HERE_YOUR_CHAR_ID"; // chat id пользователя, который получает сообщения от logger
bool attachFlag = false;
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
  String message="";
  
  if (msg.text == "Случайный анекдот") {
    message = aneks[random(0, 15)];
    bot.sendMessage(message, msg.chatID);
  }
  if (msg.text == "/start" or msg.text == "О боте") {
    bot.showMenu(menu, msg.chatID);
    message = "Здравствуй, " + msg.username + ".\nЭто бот для случайных лучших анекдотов.\nСоздатель:@NeverForever";
    bot.sendMessage(message, msg.chatID);
  }
  logger.sendMessage("Сообщение от юзера "+msg.username+":\n"+msg.text+"\nОтвет бота:\n"+message,loggerChatID);
}
