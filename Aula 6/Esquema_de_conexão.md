## Simulação do circuito no Wokwi
Link para simulação: https://wokwi.com/projects/417261695668471809

### Imagem da conexão e saída:
![Buzzer temperatura e oxigênio - Wokwi ESP32, STM32, Arduino Simulator - Google Chrome 15_12_2024 00_05_38](https://github.com/user-attachments/assets/52ada3a8-76fa-4055-b1a6-42941b184736)


### Código inicial:
```c
#include <WiFi.h>
#include <HTTPClient.h>
#include <UrlEncode.h>
#include <DHT.h>

#define DHTPIN 14     // Pino conectado ao DHT22
#define DHTTYPE DHT22 

#define BUZZER_PIN 25  // Pino conectado ao Buzzer
#define POT_PIN 2

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  dht.begin();

  pinMode(BUZZER_PIN, OUTPUT);
  
  Serial.println("Iniciando sensor DHT22 e Buzzer...");
}

void loop() {
  // Aguarda um intervalo de tempo entre as leituras
  delay(2000);

  // Lê a temperatura e a umidade
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Celsius

  // Verifica se as leituras falharam
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Falha ao ler o sensor DHT");
    return;
  }

  int potValue = analogRead(POT_PIN);
  // Exibe os valores no Serial Monitor
  Serial.print("Umidade: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" *C");
  Serial.print("Valor do Potenciômetro: \n");
  Serial.println(potValue);

  // Lógica para acionar o buzzer se a temperatura for maior que 30°C
  if (temperature > 39) {
    digitalWrite(BUZZER_PIN, HIGH);  // Ativa o buzzer
    Serial.println("Temperatura alta! Buzzer ativado!");
  } else if (temperature  < 35) {
    digitalWrite(BUZZER_PIN, HIGH);  // Ativa o buzzer
    Serial.println("Temperatura baixa! Buzzer ativado!");
  } else {
    digitalWrite(BUZZER_PIN, LOW);   // Desativa o buzzer
    Serial.println("Temperatura normal. Buzzer desativado.");
  }

  if (potValue < 90) {
    digitalWrite(BUZZER_PIN, HIGH);  // Ativa o buzzer
    Serial.println("Oxigenação baixa! Buzzer ativado!");
  } else {
    digitalWrite(BUZZER_PIN, LOW);   // Desativa o buzzer
    Serial.println("Oxigenação normal. Buzzer desativado.");
  }
  int buzzerIntensity = map(potValue, 0, 1023, 0, 255);  
  analogWrite(BUZZER_PIN, buzzerIntensity);
}
```
