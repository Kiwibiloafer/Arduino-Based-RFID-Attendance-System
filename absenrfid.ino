#include <SPI.h>
#include <MFRC522.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

//change to your ssid and password
const char* ssid = ""; 
const char* password = "";

const char* host = "192.168.121.55";

#define LED_PIN 15
#define BTN_PIN 5

#define SDA_PIN 2
#define RST_PIN 0

MFRC522 mfrc522(SDA_PIN, RST_PIN);


void setup() {
  Serial.begin(9600);

  WiFi.hostname("NodeMCU");
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    Serial.println(".");
    delay(500);
  }
  Serial.println("WiFi Connected");
  Serial.println("IP Address = ");
  Serial.println(WiFi.localIP());

  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, OUTPUT);

  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("dekatkan Kartu Anda Ke RFID Reader");
  Serial.println();
}

void loop() {
  if(digitalRead(BTN_PIN) == 1)
  {
    digitalWrite(LED_PIN, HIGH);
    while(digitalRead(BTN_PIN) == 1);
      String getData, Link;
      WiFiClient client;
      HTTPClient http;
      Link = "http://" + String(host) + "/Bahan%20absensi%20rfid/ubahmode.php";

      http.begin(client, Link);

      int httpCode = http.GET();
      String payload = http.getString();

      Serial.println(payload);
      http.end();
  }
  digitalWrite(LED_PIN, LOW);
  
  if(! mfrc522.PICC_IsNewCardPresent())
    return ;
  
  if(! mfrc522.PICC_ReadCardSerial())
    return ;
  
  String IDTAG = "";
  for(byte i=0; i<mfrc522.uid.size; i++){
    IDTAG += mfrc522.uid.uidByte[i];
  }
  digitalWrite(LED_PIN, HIGH);

  WiFiClient client;
  const int httpPort = 80;
  if(!client.connect(host, httpPort)){
    Serial.println("Connection failed");
    return ; 
  }
  String Link;
  HTTPClient http;
  Link = "http://" + String(host) + "/Bahan%20absensi%20rfid/kirimkartu.php?nokartu=" + IDTAG;
  http.begin(client, Link);
  int httpCode = http.GET();
  String payload = http.getString();
  Serial.println(payload);
  http.end();
  delay(3000);
}
