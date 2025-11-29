#include <Servo.h>  // Biblioteca para controlar o servo motor
#include <Wire.h>  // Biblioteca para comunicação I2C
#include <LiquidCrystal_I2C.h>  // Biblioteca para controlar o LCD via I2C

#define TRIG_PIN 8  // Define o pino 8 como pino de trigger do sensor ultrassônico
#define ECHO_PIN 7  // Define o pino 7 como pino de echo do sensor ultrassônico
#define BUTTON_PIN 2  // Define o pino 2 como pino do botão
#define BUZZER_PIN 6  // Define o pino 6 como pino do buzzer
#define LED_R 5  // Define o pino 5 como pino do LED vermelho (RGB)
#define LED_G 4  // Define o pino 4 como pino do LED verde (RGB)
#define LED_B 3  // Define o pino 3 como pino do LED azul (RGB)
#define SERVO_PIN A3  // Define o pino analógico A3 como pino do servo motor

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Cria objeto LCD com endereço I2C 0x27, 16 colunas e 2 linhas
Servo myServo;  // Cria objeto para controlar o servo motor

bool servoActivated = false;  // Variável para controlar se o servo foi ativado
unsigned long servoStartTime = 0;  // Variável para armazenar o tempo de início da ativação do servo

void setup() {
  Serial.begin(9600);  // Inicializa a comunicação serial com velocidade de 9600 bauds para monitoramento

  pinMode(TRIG_PIN, OUTPUT);  // Configura o pino TRIG como saída
  pinMode(ECHO_PIN, INPUT);  // Configura o pino ECHO como entrada
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Configura o pino do botão como entrada com pull-up interno
  pinMode(BUZZER_PIN, OUTPUT);  // Configura o pino do buzzer como saída
  pinMode(LED_R, OUTPUT);  // Configura o pino do LED vermelho como saída
  pinMode(LED_G, OUTPUT);  // Configura o pino do LED verde como saída
  pinMode(LED_B, OUTPUT);  // Configura o pino do LED azul como saída

  myServo.attach(SERVO_PIN);  // Anexa o servo ao pino definido
  myServo.write(0);  // Posiciona o servo na posição inicial (0 graus)

  Wire.begin();  // Inicializa a comunicação I2C
  lcd.init();  // Inicializa o display LCD
  lcd.backlight();  // Liga a luz de fundo do LCD
  lcd.setCursor(0,0);  // Posiciona o cursor na primeira coluna, primeira linha
  lcd.print("Dispenser:");  // Imprime "Dispenser:" no LCD
}

void loop() {

  // --------------- BOTÃO → GIRA O SERVO --------------------
  if (digitalRead(BUTTON_PIN) == LOW && !servoActivated) {  // Verifica se o botão foi pressionado e o servo não está ativo
    servoActivated = true;  // Marca o servo como ativado
    servoStartTime = millis();  // Armazena o tempo atual em milissegundos

    myServo.write(90);  // Move o servo para 90 graus
    tone(BUZZER_PIN, 1000, 200);  // Toca o buzzer na frequência 1000Hz por 200ms
    delay(200);  // Aguarda 200 milissegundos
    noTone(BUZZER_PIN);  // Desliga o buzzer
  }

  if (servoActivated && millis() - servoStartTime >= 5000) {  // Verifica se o servo está ativo e se passaram 5 segundos
    myServo.write(0);  // Retorna o servo para a posição inicial (0 graus)
    servoActivated = false;  // Marca o servo como desativado
  }

  // ----------------- SENSOR ULTRASSÔNICO -------------------
  digitalWrite(TRIG_PIN, LOW);  // Desliga o pino TRIG
  delayMicroseconds(5);  // Aguarda 5 microssegundos
  digitalWrite(TRIG_PIN, HIGH);  // Liga o pino TRIG para enviar pulso ultrassônico
  delayMicroseconds(10);  // Aguarda 10 microssegundos
  digitalWrite(TRIG_PIN, LOW);  // Desliga o pino TRIG

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // Lê o tempo de retorno do pulso no pino ECHO (timeout de 30ms)
  long distance = duration * 0.034 / 2;  // Calcula a distância em centímetros (velocidade do som = 0.034 cm/μs)

  // <<< ---------- MOSTRANDO NO MONITOR SERIAL ----------- >>>
  Serial.print("Distancia: ");  // Imprime "Distancia: " no monitor serial
  Serial.print(distance);  // Imprime o valor da distância no monitor serial
  Serial.println(" cm");  // Imprime " cm" e pula para a próxima linha no monitor serial

  // ---------------------- LCD -----------------------------
  lcd.setCursor(0,0);  // Posiciona o cursor na primeira coluna, primeira linha
  lcd.print("                ");  // Limpa a linha do LCD com espaços em branco
  lcd.setCursor(0,0);  // Posiciona o cursor novamente na primeira coluna, primeira linha

  if (distance == 0) lcd.print("Sem leitura");  // Se a distância for 0, mostra "Sem leitura" no LCD
  else if (distance > 20) lcd.print("Dispenser Vazio!");  // Se a distância for maior que 20cm, mostra "Dispenser Vazio!" no LCD
  else if (distance > 15) lcd.print("Nivel Baixo");  // Se a distância for maior que 15cm, mostra "Nivel Baixo" no LCD
  else lcd.print("Dispenser Cheio!");  // Caso contrário, mostra "Dispenser Cheio!" no LCD

  // ---------------- LED + BUZZER --------------------------
  if (distance == 0) {  // Se a distância for 0 (sem leitura)
      digitalWrite(LED_R, HIGH);  // Liga o LED vermelho
      digitalWrite(LED_G, LOW);  // Desliga o LED verde
      digitalWrite(LED_B, LOW);  // Desliga o LED azul
      noTone(BUZZER_PIN);  // Desliga o buzzer
  }
  else if (distance > 20) {  // Se a distância for maior que 20cm (dispenser vazio)
      digitalWrite(LED_R, HIGH);  // Liga o LED vermelho
      digitalWrite(LED_G, LOW);  // Desliga o LED verde
      digitalWrite(LED_B, LOW);  // Desliga o LED azul
      tone(BUZZER_PIN, 1500);  // Liga o buzzer na frequência 1500Hz continuamente
  }
  else if (distance > 15) {  // Se a distância for maior que 15cm (nível baixo)
      digitalWrite(LED_R, HIGH);  // Liga o LED vermelho
      digitalWrite(LED_G, HIGH);  // Liga o LED verde (forma amarelo com o vermelho)
      digitalWrite(LED_B, LOW);  // Desliga o LED azul
      noTone(BUZZER_PIN);  // Desliga o buzzer
  }
  else {  // Se a distância for menor ou igual a 15cm (dispenser cheio)
      digitalWrite(LED_R, LOW);  // Desliga o LED vermelho
      digitalWrite(LED_G, HIGH);  // Liga o LED verde
      digitalWrite(LED_B, LOW);  // Desliga o LED azul
      noTone(BUZZER_PIN);  // Desliga o buzzer
  }

  delay(200);  // Aguarda 200 milissegundos antes de repetir o loop
}
