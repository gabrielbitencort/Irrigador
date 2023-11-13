#define BLYNK_TEMPLATE_ID "TMPL0nWhjDJh"
#define BLYNK_TEMPLATE_NAME = "NdoeMCU"

#include <ESP8266WiFi.h>        //biblioteca para uso do esp8266
#include <BlynkSimpleEsp8266.h> //biblioteca para uso do esp8266 no Blynk
#include <DHT.h>

char auth[] = "w3U5Znsk_02ERjes0jefqCxq0ZoGx8V1"; //auth token do projeto
char ssid[] = "NORT _Fernando"; //nome do wifi
char pass[] = "s1a2r3a4"; //senha do wifi

#define umid A0 //define pino A0 para sensor de umidade
#define relay 15  //define pino 15 para rele

#define DHTPIN 4  //define pino 4 para sensor temperatura
#define DHTTYPE DHT11 //define tipo de sensor de temperatura

BlynkTimer timer; //inicializa o objeto timer do tipo BlinkTimer
DHT dht(DHTPIN, DHTTYPE);

int value;
int dry = 1024;
int wet = 290;

void tempSensor()
{
  float temp = dht.readTemperature();
  if (isnan(temp))
  {
    Serial.println("Falha ao ler dados do sensor DHT11!   ");
    return;
  }
  Blynk.virtualWrite(V6, temp);
}

void umidSensor()
{
  value = analogRead(umid);
  value = map(value, dry, wet, 0, 100);
  Serial.print("Umidity finded: ");
  Serial.print(value);
  Serial.println(" %");

  if(value < 40){
    digitalWrite(relay, HIGH);
  }else {
    digitalWrite(relay, LOW);
    Serial.print("Solo está molhado.  ");
  }
  tempSensor();
  Blynk.virtualWrite(V7, value);
}

void setup()
{
  Serial.begin(115200);  //inicializa comunicação serial
  Blynk.begin(auth, ssid, pass);  //inicializa Blynk
  dht.begin();
  timer.setInterval(1000L, umidSensor);
  pinMode(umid, INPUT);
  pinMode(relay, HIGH);
  pinMode(relay, OUTPUT);
}

void loop()
{
  Blynk.run();  //chama a função Blynk.run()
  timer.run();  //chama a função timer.run()
}
