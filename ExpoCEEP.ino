#define BLYNK_TEMPLATE_ID "TMPL0nWhjDJh"
#define BLYNK_TEMPLATE_NAME = "NdoeMCU"

#include <ESP8266WiFi.h>        //biblioteca para uso do esp8266
#include <BlynkSimpleEsp8266.h> //biblioteca para uso do esp8266 no Blynk
#include <user_interface.h>     //biblioteca para uso do temporizador
#include <DHT.h>                //biblioteca para uso do DHT11

char auth[] = "w3U5Znsk_02ERjes0jefqCxq0ZoGx8V1"; //auth token do projeto
char ssid[] = "NORT _Fernando"; //nome do wifi
char pass[] = "s1a2r3a4"; //senha do wifi

#define umid A0   //define pino A0 para sensor de umidade
#define relay 15  //define pino 15 para rele

#define DHTPIN 4      //define pino 4 para sensor temperatura
#define DHTTYPE DHT11 //define tipo de sensor de temperatura

BlynkTimer timer; //inicializa o objeto timer do tipo BlinkTimer
DHT dht(DHTPIN, DHTTYPE); 

const int threshold = 40; //valor de referência do sensor
int value;                //valor de leitura do sensor
int dry = 1024;           //valor do sensor com terra seca
int wet = 290;            //valor do sensor com terra molhada
int mili = 60000;         //valor de 1 minuto em milissegundos

os_timer_t sensorTimer;
os_timer_t relayTimer;

bool relayActive = false;

void checkSensor(void *arg)
{
  value = analogRead(umid);
  value = map(value, dry, wet, 0, 100);
  Serial.println(value);
  if(!relayActive && value <= threshold)
  {
    digitalWrite(relay, HIGH);
    relayActive = true;
    os_timer_t(&relayTimer, (1*mili), false);
  }
}

void turnOffRelay(void *arg)
{
  digitalWrite(relay, LOW);
  relayActive = false;
}

void blynktempSensor()
{
  float temp = dht.readTemperature();
  if (isnan(temp))
  {
    Serial.println("Falha ao ler dados do sensor DHT11!   ");
    return;
  }
  Blynk.virtualWrite(V6, temp);
}

void blynkumidSensor()
{
  value = analogRead(umid);
  value = map(value, dry, wet, 0, 100);
  // Serial.print("Umidity finded: ");
  // Serial.print(value);
  // Serial.println(" %");

  // if(value < 40){
  //   digitalWrite(relay, HIGH);
  // }else {
  //   digitalWrite(relay, LOW);
  //   Serial.print("Solo está molhado.  ");
  // }
  blynktempSensor();
  Blynk.virtualWrite(V7, value);
}

void setup()
{
  Serial.begin(115200);           //inicializa comunicação serial
  Blynk.begin(auth, ssid, pass);  //inicializa Blynk
  dht.begin();                    //inicializa o sensor DHT11
  timer.setInterval(1000L, blynkumidSensor);
  pinMode(umid, INPUT);
  pinMode(relay, HIGH);
  pinMode(relay, OUTPUT);

  os_timer_setfn(&sensorTimer, checkSensor, NULL);
  os_timer_arm(&sensorTimer, (2*mili), true);
  os_timer_setfn(&relayTimer, turnOffRelay, NULL);
}

void loop()
{
  Blynk.run();  //chama a função Blynk.run()
  timer.run();  //chama a função timer.run()
}
