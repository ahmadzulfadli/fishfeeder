#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif9pt7b.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h> 
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int last_second;

//------------------------------
const int onhourpakan=8;
const int onminpakan=40;

const int offhourpakan=8;
const int offminpakan=41;

const int onhourpakan1=12;
const int onminpakan1=40;

const int offhourpakan1=12;
const int offminpakan1=41;

const int onhourpakan2=17;
const int onminpakan2=40;

const int offhourpakan2=17;
const int offminpakan2=41;

//------------------------------

const char* ssid = "Redmi";
const char* password = "badbaoys15";

//----------------------------------------------------------------------------
int ENA = 13; //D7
int IN1 = 0; //D1
int IN2 = 2; //D2

int led = 15; //D8

//----------------------------------------------------------------------------
WiFiUDP ntpUDP;
const long utcOffsetInSeconds = 25200;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds );

//----------------------------------------------------------------------------
#define BOT_TOKEN "5817518692:AAEUEnppQya0YwFu6bdVGzvZTThRDa1ox_k"
#define CHAT_ID "1324445586"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

//Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
//----------------------------------------------------------------------------

void setup() {

  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting.");

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(led, OUTPUT);  
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected");
  timeClient.begin();
  timeClient.update();

  // Add root certificate for api.telegram.org
  configTime(0, 0, "pool.ntp.org");
  secured_client.setTrustAnchors(&cert);
  bot.sendMessage(CHAT_ID, "Sistem Berjalan", "");

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
 
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 1);
  display.println("Senin 27/9/2021");
  
  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.setCursor(0, 20);
  display.println("13:20");
  
  display.display();
}


void loop() {
  timeClient.update();
  //Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());

  //----------------------------------------------------------------------------
  unsigned long rawTime = timeClient.getEpochTime() + 25200;

  if (rawTime != last_second){
    time_t t = rawTime;
  
    int jam = timeClient.getHours();
    String jamStr = jam < 10 ? "0" + String(jam) : String(jam);
    int menit = timeClient.getMinutes();
    String menitStr = menit < 10 ? "0" + String(menit) : String(menit);
    int detik = timeClient.getSeconds();
    String detikStr = detik < 10 ? "0" + String(detik) : String(detik);
      
    String hari;
    switch (weekday(t)){
      case 1 :
        hari = "Minggu";
        break;
      case 2 :
        hari = "Senin";
        break;
      case 3 :
        hari = "Selasa";
        break;
      case 4 :
        hari = "Rabu";
        break;
      case 5 :
        hari = "Kamis";
        break;
      case 6 :
        hari = "Jumat";
        break;
      case 7 :
        hari = "Sabtu";
        break;
      }
    
    int tgl = day(t);
    String tglStr = tgl < 10 ? "0" + String(tgl) : String(tgl);
    int bln = month(t);
    String blnStr = bln < 10 ? "0" + String(bln) : String(bln);
    int thn = year(t);
    String thnStr = String(thn);
    
    String tanggal = hari + " " + tglStr + "/" + blnStr + "/" + thnStr;
    String waktu = jamStr + ":" + menitStr + ":" + detikStr;
  
    Serial.println(tanggal);
    Serial.println(waktu);
    display.clearDisplay();
   
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 1);
    display.println(tanggal);
    
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 20);
    display.println(waktu);
    
    display.display();
  }
  //----------------------------------------------------------------------------
  
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
  
  //----------------------------------------------------------------------------
  if(timeClient.getHours()==onhourpakan && timeClient.getMinutes()==onminpakan)
  {
    if(timeClient.getHours()==onhourpakan && timeClient.getMinutes()==onminpakan && timeClient.getSeconds()==0){
      bot.sendMessage(CHAT_ID, "Pakan Ikan Sedang Diberikan", "");
      Serial.println("Pakan Ikan Sedang Diberikan");
    }
    makan();
    
  }else if(timeClient.getHours()==offhourpakan && timeClient.getMinutes()==offminpakan)
  {
    if(timeClient.getHours()==offhourpakan && timeClient.getMinutes()==offminpakan && timeClient.getSeconds()==0){
      Serial.println("Pakan Ikan Selesai Diberikan");
      bot.sendMessage(CHAT_ID, "Pakan Ikan Selesai Diberikan", "");
    }
    stop_makan();
  }

  //----------------------------------------------------------------------------
  if(timeClient.getHours()==onhourpakan1 && timeClient.getMinutes()==onminpakan1)
  {
    if(timeClient.getHours()==onhourpakan1 && timeClient.getMinutes()==onminpakan1 && timeClient.getSeconds()==0){
      bot.sendMessage(CHAT_ID, "Pakan Ikan Sedang Diberikan", "");
      Serial.println("Pakan Ikan Sedang Diberikan");
    }
    makan();
    
  }else if(timeClient.getHours()==offhourpakan1 && timeClient.getMinutes()==offminpakan1)
  {
    if(timeClient.getHours()==offhourpakan1 && timeClient.getMinutes()==offminpakan1 && timeClient.getSeconds()==0){
      Serial.println("Pakan Ikan Selesai Diberikan");
      bot.sendMessage(CHAT_ID, "Pakan Ikan Selesai Diberikan", "");
    }
    stop_makan();
  }

  //----------------------------------------------------------------------------
  if(timeClient.getHours()==onhourpakan2 && timeClient.getMinutes()==onminpakan2)
  {
    if(timeClient.getHours()==onhourpakan2 && timeClient.getMinutes()==onminpakan2 && timeClient.getSeconds()==0){
      bot.sendMessage(CHAT_ID, "Pakan Ikan Sedang Diberikan", "");
      Serial.println("Pakan Ikan Sedang Diberikan");
    }
    makan();
    
  }else if(timeClient.getHours()==offhourpakan2 && timeClient.getMinutes()==offminpakan2)
  {
    if(timeClient.getHours()==offhourpakan2 && timeClient.getMinutes()==offminpakan2 && timeClient.getSeconds()==0){
      Serial.println("Pakan Ikan Selesai Diberikan");
      bot.sendMessage(CHAT_ID, "Pakan Ikan Selesai Diberikan", "");
    }
    stop_makan();
  }
  digitalWrite(led, HIGH);
  delay(1000);
  digitalWrite(led, LOW);
}


void makan(){
  for(int i = 0; i <= 15; i++){
    analogWrite(ENA, 255);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    delay(1000);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    delay(1000);
  }
}

void stop_makan(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

void handleNewMessages(int numNewMessages) {
  Serial.print("Handle New Messages: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);
    
    String from_name = bot.messages[i].from_name;
    if (text == "/start") {
      String welcome = "Welcome , " + from_name + "\n";
      welcome += "Panduan Penggunaan \n";
      welcome += "/pakan_on : Memberi pakan \n";
      welcome += "/pakan_off : Pakan selesai diberikan \n";
      bot.sendMessage(CHAT_ID, welcome, "");
    }
    
    if (text == "/pakan_on") {
      bot.sendMessage(CHAT_ID, "Pakan Ikan Sedang Diberikan", "");
      Serial.println("Pakan Ikan Sedang Diberikan");
      makan();
      stop_makan();
      Serial.println("Pakan Ikan Selesai Diberikan");
      bot.sendMessage(CHAT_ID, "Pakan Ikan Selesai Diberikan", "");
    }
    
    if (text == "/pakan_off") {
      Serial.println("Pakan Ikan Selesai Diberikan");
      bot.sendMessage(CHAT_ID, "Pakan Ikan Selesai Diberikan", "");
      stop_makan();
    }     
  }
}
