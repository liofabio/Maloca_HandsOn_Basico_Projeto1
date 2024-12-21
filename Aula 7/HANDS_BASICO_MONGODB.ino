#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Definições dos pinos
#define DHTPIN 21       // Pino conectado ao sensor DHT11
#define DHTTYPE DHT22   // Tipo de sensor
#define BUZZER1 25      // Pino do buzzer 1
#define BUZZER2 26      // Pino do buzzer 2
#define POT_PIN 2       // Pino do potenciômetro

// Inicialização do sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Credenciais Wi-Fi
const char* ssid = "SEU_SSID";
const char* password = "SUA_SENHA";

// Configurações do MongoDB Atlas
const char* mongoAPIEndpoint = "SUA_URL_DO_ENDPOINT"; // Exemplo: "https://sua-api.mongodb.net/data"
const char* apiKey = "SUA_API_KEY";

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

  // Inicialização do sensor DHT
  dht.begin();

  // Configuração dos pinos
  pinMode(BUZZER1, OUTPUT);
  pinMode(BUZZER2, OUTPUT);
  digitalWrite(BUZZER1, LOW);
  digitalWrite(BUZZER2, LOW);
}

void enviarDados(float temperature, float humidity) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(mongoAPIEndpoint);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("api-key", apiKey);

    String jsonPayload = "{";
    jsonPayload += "\"temperature\":" + String(temperature) + ",";
    jsonPayload += "\"humidity\":" + String(humidity);
    jsonPayload += "}";

    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      Serial.print("Dados enviados com sucesso: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Erro ao enviar dados: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Wi-Fi desconectado!");
  }
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

  // Verificação das condições para o Buzzer 1 (temperatura)
  if (temperature > 30 || temperature < 24) {
    digitalWrite(BUZZER2, HIGH);
    Serial.println("Alerta! Temperatura fora do intervalo seguro!");
  } else {
    digitalWrite(BUZZER2, LOW);
  }

  // Leitura do valor do potenciômetro (oxigenação simulada)
  int leituraPot = analogRead(POT_PIN);
  float oxigenacao = map(leituraPot, 0, 4095, 70, 100); // Mapeia para uma faixa de 70 a 100

  // Mostra o nível de oxigenação
  Serial.print("Oxigenação: ");
  Serial.print(oxigenacao);
  Serial.println(" %");

  // Verificação das condições para o Buzzer 2 (oxigenação)
  if (oxigenacao < 90) {
    digitalWrite(BUZZER1, HIGH);
    Serial.println("Alerta! Oxigenação abaixo do limite!");
  } else {
    digitalWrite(BUZZER1, LOW);
  }
  
  // Envia dados para o MongoDB Atlas
  enviarDados(temperature, humidity, oxigenacao);
  
  // Aguardar 2 segundos
  delay(2000);
}
