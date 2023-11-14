#define BLYNK_TEMPLATE_ID "id"  //ID do projeto
#define BLYNK_TEMPLATE_NAME = "nome"   //Nome do projeto
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>        //Biblioteca para uso do esp8266
#include <BlynkSimpleEsp8266.h>   //Biblioteca para uso do esp8266 no blynk
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>

char auth[] = "token"; //Auth token do projeto
char ssid[] = "wifi"; //Nome do wifi
char pass[] = "senha";       //Senha do wifi

#define sensor A0   //Define pino A0 para sensor de umidade
#define relay 15    //Define pino 15 para relé

Adafruit_BMP085 bmp; //define bmp como um objeto adafruit_bmp085

int value;        //valor de leitura do sensor
int dry = 1024;   //valor do sensor com terra seca
int wet = 0;      //valor do sensor com terra molhada

bool relayActive = false;

int ref = 0;            //valor de referência do sensor
int relayStartTime = 0; //tempo de inicio do relé
const int relayDuration = 30000;  //duração do relé em milissegundos(30 segundos)

BLYNK_WRITE(V9)
{
  ref = param.asInt();  //atualiza o valor de ref com o valor do slider
}

void setup()
{
  Serial.begin(115200);           //inicializa cominição serial
  Blynk.begin(auth, ssid, pass);  //inicializa Blynk
  bmp.begin();
  pinMode(sensor, INPUT);   //define sensor como entrada
  pinMode(relay, OUTPUT);   //define relé como saida
}

void loop()
{
  Blynk.run();  //chama a função Blynk.run()

  if(!bmp.begin()) 
  {
    Serial.println("Não foi possivel encontrar o sensor BMP180.");
    while (1);
  }

  float temp = bmp.readTemperature();   //lê o sensor de temperatura
  value = analogRead(sensor);           //lê o sensor de umidade
  value = map(value, dry, wet, 0, 100); //transforma em porcentagem

  Blynk.virtualWrite(V6, temp);   //envia valor da temperatura para o Blynk
  Blynk.virtualWrite(V7, value);  //envia valor da umidade para o Blynk

  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.println(" °C");

  Serial.print("Umidade: ");
  Serial.print(value);
  Serial.println(" %");

  Serial.print("Limite: ");
  Serial.print(ref);
  Serial.println(" %");
  delay(1000);

  if(!relayActive && value <= ref)  //liga relé se relayActive for falso e valor <= ref
  {
    Serial.println("Ligando relé.");
    digitalWrite(relay, HIGH);
    relayActive = true;
    relayStartTime = millis();
  }
  else if(relayActive && (millis() - relayStartTime >= relayDuration)) //desliga relé se relayActive for verdadeiro e valor > ref
  {
    Serial.println("Desligando relé.");
    digitalWrite(relay, LOW);
    relayActive = false;
  }
}
