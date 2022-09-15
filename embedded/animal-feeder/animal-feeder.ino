//Bibliotecas necessarias
#include <Ethernet.h>
#include <PubSubClient.h>

//Configuracoes para rede
byte mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xE1 };
IPAddress ip(198,41,30,241);
IPAddress server(172,25,5,181);

//Declaracao das libs
EthernetClient ethClient;
PubSubClient client(ethClient);

//Definicao dos pinos utilizados
int pinLED = 7;
int pinLDR = A0;

//Funcao para receber mensagem
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived:");
  Serial.println(topic);
  String s = "";
  for (int i=0;i<length;i++) {
    //Serial.print((char)payload[i]);
    s += ((char)payload[i]);
  }
  
  //Verifica conteudo
  Serial.println(s);
  if(s.equals("desligar")) {
    digitalWrite(pinLED,LOW);
  }
  else if(s.equals("ligar")) {
    digitalWrite(pinLED,HIGH);
  }
}

// A funcao Setup eh executada na inicializacao do Arduino
void setup() {
  Serial.begin(9600);
  Serial.println("Setup.");
  //Inicializacao do MQTT
  client.setServer(server, 1883);
  //Definindo callback
  client.setCallback(callback);
  //Inicializa rede
  if (Ethernet.begin(mac) == 0) { //Tenta por DHCP
    Serial.println("DHCP falhou.");
    // Configura por IP
    Ethernet.begin(mac, ip);
  }
  // inicializa pino 8 (digital) como saida
  pinMode(pinLED, OUTPUT);
  //inicializa pino A0 como entrada
  pinMode(pinLDR, INPUT);
  //Mandando debug pela serial
  //Inicializa serial a 9600 bps
  Serial.println("Arduino inicializado.");
}

void reconnect() {
  // Loop ate conectar
  while (!client.connected()) {
    Serial.print("Tentando conexao mqtt...");
    if (client.connect("arduinoClient1")) {
      Serial.println("Conectado ao MQTT.");
      //Subscrevendo em um topico
      client.subscribe("/sihs/aluno1/listener");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentado novamente em 5s.");
      delay(5000);
    }
  }
}

void loop() {
  //Conecta MQTT
  if (!client.connected()) {
  reconnect(); //Caso caia a conexao
  }
  client.loop(); //Deixa MQTT no loop
  //Faz uma leitura do sensor de luminosidade
  // int lum = analogRead(pinLDR);
  // Debug da leitura
  // Serial.println(lum);
  //Publica topico de luminosidade
  //Cria um buffer para enviar
  char buffer[4];
  //Copia leitura de lum no buffer
  sprintf(buffer,"%d", lum);
  //Envia buffer
  client.publish("/sihs/aluno1/lum", buffer);
  delay(1000); // espera 1 segundo
}
