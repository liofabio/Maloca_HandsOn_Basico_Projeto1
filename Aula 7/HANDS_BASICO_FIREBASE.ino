#include <DHT.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

// Definições dos pinos
#define DHTPIN 21       // Pino conectado ao sensor DHT11
#define DHTTYPE DHT11   // Tipo de sensor
#define BUZZER1 25      // Pino do buzzer 1
#define BUZZER2 26      // Pino do buzzer 2
#define POT_PIN 2       // Pino do potenciômetro

// Inicialização do sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Credenciais Wi-Fi
const char* ssid = "SEU_SSID";
const char* password = "SUA_SENHA";

// Configurações do Firebase
#define FIREBASE_HOST "SEU_FIREBASE_HOST"  // Exemplo: "seu-projeto.firebaseio.com"
#define FIREBASE_AUTH "SUA_CHAVE_API"
FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);

  // Verificação da placa selecionada
  #if !defined(ARDUINO_ESP32_DEV)
    #error "Placa ESP32 não selecionada. Por favor, selecione a placa correta em Ferramentas > Placa > ESP32 Dev Module."
  #endif

  // Inicialização do Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConectado ao Wi-Fi");

  // Inicialização do Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  // Inicialização do sensor DHT
  dht.begin();

  // Configuração dos pinos
  pinMode(BUZZER1, OUTPUT);
  pinMode(BUZZER2, OUTPUT);
  digitalWrite(BUZZER1, LOW);
  digitalWrite(BUZZER2, LOW);
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Verificação de erro na leitura
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Falha ao ler do sensor DHT!");
    return;
  }

  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" *C");
  Serial.print("Umidade: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Envia dados para o Firebase
  if (Firebase.pushFloat(firebaseData, "/sensor/temperature", temperature)) {
    Serial.println("Temperatura enviada ao Firebase.");
  } else {
    Serial.println("Erro ao enviar temperatura: " + firebaseData.errorReason());
  }

  if (Firebase.pushFloat(firebaseData, "/sensor/humidity", humidity)) {
    Serial.println("Umidade enviada ao Firebase.");
  } else {
    Serial.println("Erro ao enviar umidade: " + firebaseData.errorReason());
  }

  // Recupera dados do Firebase (opcional)
  if (Firebase.getFloat(firebaseData, "/sensor/temperature")) {
    float tempFirebase = firebaseData.floatData();
    Serial.print("Temperatura do Firebase: ");
    Serial.println(tempFirebase);

    if (tempFirebase > 30.0) { // Exemplo de alerta
      digitalWrite(BUZZER1, HIGH);
    } else {
      digitalWrite(BUZZER1, LOW);
    }
  } else {
    Serial.println("Erro ao obter temperatura: " + firebaseData.errorReason());
  }

  if (Firebase.getFloat(firebaseData, "/sensor/humidity")) {
    float humFirebase = firebaseData.floatData();
    Serial.print("Umidade do Firebase: ");
    Serial.println(humFirebase);

    if (humFirebase < 40.0) { // Exemplo de alerta
      digitalWrite(BUZZER2, HIGH);
    } else {
      digitalWrite(BUZZER2, LOW);
    }
  } else {
    Serial.println("Erro ao obter umidade: " + firebaseData.errorReason());
  }

  delay(2000); // Intervalo entre leituras
}
