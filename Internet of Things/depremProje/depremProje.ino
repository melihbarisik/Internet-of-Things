#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define  BLYNK_PRINT Serial
#include  <BlynkSimpleEsp8266.h>

/************************* WiFi Access Point *********************************/
#define WLAN_SSID ""
#define WLAN_PASS ""

char  auth[]  = "_WDT0tmApfD932JwbOsYMix5PCY1AJjG"; //Token key 
char  ssid[]  = ""; //Wifi  adı 
char  pass[]  = ""; //Wifi  şifresi

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME "reviu"
#define AIO_KEY "aio_OgWk78hs2MQYTEoPO2HqnYYiqDbs" 

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/
Adafruit_MQTT_Publish feed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Titresim Degeri");
Adafruit_MQTT_Publish feedS = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Sarsinti Siddeti");
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/onoff");
/*************************** Sketch Code ************************************/
void MQTT_connect();
uint32_t x = 0;

void setup()
{

 Serial.begin(115200);
 pinMode(D0, INPUT);
 pinMode(D1, OUTPUT);
 Blynk.begin(auth, ssid, pass);

 Serial.println();
 Serial.println("Deprem Sensörü Başlatılıyor...");
 Serial.println();

 WiFi.begin(WLAN_SSID, WLAN_PASS);
 while (WiFi.status() != WL_CONNECTED)
 {
 delay(500);
 }
}
int ilkGirisKontrol = 0;
float baslangicZamani = 0 ;
float bitisZamani;
int toplamSure = 0;
int aradakiFark;
int buzzerSayar=0;
bool kontrolBuzzer=false;

void loop()
{
 
  Blynk.run();
  MQTT_connect();
  int measurement;

 if(digitalRead(D2) == HIGH)
 {
   measurement = calculateTiltTime(); 
   while(measurement > 0)
  {
   toplamSure = toplamSure + measurement;
   measurement = calculateTiltTime(); 

   if(toplamSure > 20000)   
     {
      startTheBuzzer();
      kontrolBuzzer = true;
     }
       
  }
  bitisZamani = millis();
  aradakiFark = bitisZamani-baslangicZamani;
  buzzerSayar = buzzerSayar + aradakiFark;
  aradakiFark = aradakiFark-2200;
  baslangicZamani = bitisZamani;
  
  
  
  if(aradakiFark < 500 )
  {
     feed.publish(0); 
     delay(1500);
  }
  else if(aradakiFark > 500 && ilkGirisKontrol !=0)
  {
    feed.publish(aradakiFark);
     delay(1200);
  }
  ilkGirisKontrol = 1;
  if(toplamSure>30000)
  {
  feedS.publish(toplamSure);
  delay(1200);
  }
  if(buzzerSayar > 10000 && kontrolBuzzer == true)
  {
     stopTheBuzzer(); 
     buzzerSayar=0;
     kontrolBuzzer = false;
  }
   toplamSure=0;
 }
 
}

void MQTT_connect()
{
 int8_t ret;
 
 // Stop if already connected.
 if (mqtt.connected())
 {
 return;
 }
 Serial.print("Buluta Bağlanılıyor... ");
 uint8_t retries = 3;
 while ((ret = mqtt.connect()) != 0)
 { // connect will return 0 for connected
 Serial.println(mqtt.connectErrorString(ret));
 Serial.println("Bulut Bağlantısı Koptu Buluta Tekrar Bağlanılıyor...");
 mqtt.disconnect();
 delay(5000); // wait 5 seconds
 retries--;
 if (retries == 0)
 {
 while (1) ;
 }
 }
 Serial.println("Buluta Bağlandı.");
}

int calculateTiltTime()
{
  int measurement = pulseIn(D0,HIGH);
  return measurement;
}

void startTheBuzzer()
{    
   
     Blynk.notify("DEPREM OLUYOR !!");
     digitalWrite(D1,HIGH);
}

void stopTheBuzzer()
{    
   digitalWrite(D1,LOW);
}
