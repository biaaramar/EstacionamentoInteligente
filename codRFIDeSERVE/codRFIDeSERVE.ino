#include <Servo.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>

#define D3 0
#define RST_PIN    D2    
#define SS_PIN     D8 

//Inicializando RFID.
MFRC522 mfrc522(SS_PIN, D2);
//Parâmetros de conexão da rede WiFi
//const char* ssid = "SSID_da_Rede";
//const char* password = "Senha_da_Rede";

const char* ssid = "iTiN";
const char* password = "fernandoclothingco";
//Token de conexão do Blynkk
char auth[] = "8820bf40ce4b45bfa9bd0ede65e9fd23";
Servo s; // Variável Servo
//Declaração da variável Cliente.
WiFiClient client;

//Endereço do Host e Porta.
//const char* host = "Endereço_do_Host";
//const uint16_t port = "Numero_da_Porta";

const char* host = "192.168.43.235";
const uint16_t port = 80;

//Inicialização do tipo WidgetTerminal no pino Virtual V3 (Blynk).
WidgetTerminal sistema(V3);

void setup() {
//Configura o pino D3 para o servo.
  s.attach(D3);

  s.write(0); // Inicia motor posição zero
  Serial.begin(115200);

//Inicia RFID
  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522

  //Iniciando conexão WiFi.
  Serial.println();
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  sistema.clear();

  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado!");
  //Conexão Blynk
  Blynk.begin(auth, ssid, password);
}

//Função que Levanta e Desce a catraca.
void acionaCatraca() {
  Serial.println("Subindo catraca!");
  for (pos = 0; pos < 87; pos++) { //PARA "pos" IGUAL A 0, ENQUANTO "pos" MENOR QUE 180, INCREMENTA "pos"
    s.write(pos); //ESCREVE O VALOR DA POSIÇÃO QUE O SERVO DEVE GIRAR
    delay(20); //INTERVALO DE 15 MILISSEGUNDOS
  }
  delay(4000); //INTERVALO DE 1 SEGUNDO
  Serial.println("Descendo catraca!");
  for (pos = 87; pos >= 0; pos--) { //PARA "pos" IGUAL A 180, ENQUANTO "pos" MAIOR OU IGUAL QUE 0, DECREMENTA "pos"
    s.write(pos); //ESCREVE O VALOR DA POSIÇÃO QUE O SERVO DEVE GIRAR
    delay(20); //INTERVALO DE 15 MILISSEGUNDOS
  }
}

//Função binária de requisição com o servidor que retorna 1 se o usuário estiver apto a entrar no estacionamento, e retorna zero se não.
int requisicao(String rfid) {
//Array para armazenar os dados do usuário
  String user[5];




  //Conectando Client.
  if (!client.connect(host, port)) {
    Serial.println("Impossível se conectar ao servidor.");
    delay(5000);
    return 0;
  }
  //  Serial.println("Sucesso!");
  Serial.println("Enviando requisições ao servidor...");
  Serial.println();
  if (client.connected()) {
    String url = "/iot/?";
    url += "rfid=";
    url += rfid;
    //    Serial.println();
    //    Serial.print("Url = 192.168.1.11:80");
    //    Serial.println(url);
    //    Serial.println();
    client.print(String("GET ") + url + "\r\n");
  }

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 20000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(60000);
      return 0;
    }
  }


  while (client.available()) {
    String result = client.readStringUntil('\r');
    //    Serial.println("Resultado: ");
    //    Serial.println(result);
    //    Serial.println();
    if (result.indexOf("Usuário não cadastrado!") >= 0) {
      Serial.println("Usuário não cadastrado!");
      sistema.println("Usuário não cadastrado!");
      Serial.println();
      sistema.println();
      Blynk.notify("Usuário não cadastrado!");
      return 0;
    } else {
      if (result.indexOf("Crédito Insuficiente. Compre mais créditos diretamente no App!") >= 0) {
        Serial.println("Crédito Insuficiente.");
        sistema.println("Crédito Insuficiente.");
        Serial.println();
        sistema.println();
        Blynk.notify("Crédito Insuficiente");
        return 0;
      } else {
        if (result.indexOf("Usuário Indisponível") >= 0) {
          Serial.println("Usuário Indisponível!");
          sistema.println("Usuário Indisponível!");
          Serial.println();
          sistema.println();
          Blynk.notify("Usuário Indisponível!");
          return 0;
        } else {
          //      int aux_sel = result.indexOf('*');
          int selec1 = 0;
          int selec2;
          for (int i = 0; i < 5; i++) {
            selec1 = result.indexOf('*', selec1);
            selec2 = result.indexOf('*', selec1 + 1);
            //        aux_sel = selec2;
            String subQuery;
            if (i == 0) {
              selec1 = result.indexOf('*');
              subQuery = result.substring(0, selec1);
            } else {
              subQuery = result.substring(selec1 + 1, selec2);
              selec1 = selec2;
            }
            //        Serial.println("Query: ");
            //        Serial.println(subQuery);
            //        Serial.println();
            user[i] = subQuery;
          }

          Serial.println("==================================");
          Serial.println();
          Serial.println("Usuário encontrado!");
          Serial.print("Rfid: ");
          Serial.println(user[0]);
          Serial.print("Nome: ");
          Serial.println(user[1]);
          Serial.print("CPF: ");
          Serial.println(user[2]);
          Serial.print("Crédito: ");
          Serial.println(user[3]);
          Serial.print("Última Passagem: ");
          Serial.println(user[4]);
          Serial.println();
          Serial.println("==================================");
          Serial.println();
          sistema.clear();
          sistema.println("=================");
          sistema.println();
          sistema.println("Usuário encontrado!");
          sistema.print("Rfid: ");
          sistema.println(user[0]);
          sistema.print("Nome: ");
          sistema.println(user[1]);
          sistema.print("CPF: ");
          sistema.println(user[2]);
          sistema.print("Crédito: ");
          sistema.println(user[3]);
          sistema.print("Última Passagem: ");
          sistema.println(user[4]);
          sistema.println();
          sistema.println("=================");
          sistema.println();
          Blynk.notify("Sucesso!");

        }
        // Close the connection
        Serial.println();
        Serial.println("Finalizando conexão");
        client.stop();

      }
      return 1;
    }
  }
}

void loop() {

  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Seleciona o cartao RFID
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  
  Blynk.run();
  Serial.print("Conectando-se ao servidor: ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

   String rfid_cap= "";
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
//     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
//     Serial.print(mfrc522.uid.uidByte[i], HEX);
     rfid_cap.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
     rfid_cap.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  rfid_cap.toUpperCase();

  int ctrl = requisicao(rfid_cap);
  if (ctrl == 1) {
    acionaCatraca();
  } else {
    Serial.println("Negado!");
  }
  //    delay(3000);
  //    requisicao("111112");
  //    delay(3000);
  //    requisicao("111113");
  //    delay(10000); // execute once every 5 minutes, don't flood remote service
}
