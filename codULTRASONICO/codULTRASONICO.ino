#include <ESP8266WiFi.h>
#include "Ultrasonic.h"
#include <BlynkSimpleEsp8266.h>

#define BLYNK_GREEN     "#23C48E"
#define BLYNK_RED       "#D3435C"
#define azul 2
#define verde 0
#define vermelho 16

//Parâmetros de conexão da rede WiFi
//const char* ssid = "SSID_da_Rede";
//const char* password = "Senha_da_Rede";

const char* ssid = "iTiN";
const char* password = "fernandoclothingco";
//Token de conexão do Blynkk
char auth[] = "8820bf40ce4b45bfa9bd0ede65e9fd23";
//Declaração da variável Cliente.
WiFiClient client;

//Endereço do Host e Porta.
//const char* host = "Endereço_do_Host";
//const uint16_t port = "Numero_da_Porta";

const char* host = "192.168.43.235";
const uint16_t port = 80;

//Inicialização do tipo WidgetLed no pino Virtual V1 (Blynk).
WidgetLED vaga(V1);

//Inicializando Variável de tempo do Blynk.
BlynkTimer timer;

void setup() {
  
//Realizando configuração de pinagem.

  pinMode(azul, OUTPUT);
  pinMode(verde, OUTPUT);
  pinMode(vermelho, OUTPUT);
  digitalWrite(azul, LOW);
  Serial.begin(115200);

//Iniciando conexão WiFi.

  Serial.println();
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Conectado!");
  
//Conexão Blynk

  Blynk.begin(auth, ssid, password);
  
//Ligando Luz virtual em Blynk.

  vaga.on();
}

//Função binária sobre a distancia do veículo.
int sensorU() {
//Inicializa a variável do tipo Ultrasonic.
  Ultrasonic ultrasonic(4, 5);
  Serial.print("Distância: ");
//Pegando o valor e convertendo para Centimetro.
  float distancia = ultrasonic.Ranging(CM);
  Serial.print(distancia);
  Serial.println("cm");
  if (distancia < 10) {
    return 0;
  } else {
    return 1;
  }
}

//Função para configurar a cor Vermelha no Led RGB.
void vermelhoFuncao() {
  digitalWrite(verde, LOW);
  digitalWrite(vermelho, HIGH);
}
//Função para configurar a cor Verde no Led RGB.
void verdeFuncao() {
  digitalWrite(verde, HIGH);
  digitalWrite(vermelho, LOW);
}

void loop()
{
  Blynk.run();
  timer.run();
  if (sensorU() == 0) {
    vermelhoFuncao();
//    Configura a cor do Led virtal no Blynk.
    Blynk.setProperty(V1, "color", BLYNK_RED);
  }
  else {
    verdeFuncao();
//    Configura a cor do Led virtal no Blynk.
    Blynk.setProperty(V1, "color", BLYNK_GREEN);
  }
  delay(1000);
}
