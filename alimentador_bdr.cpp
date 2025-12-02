#include <Servo.h>                 // Biblioteca para controlar o servo motor
#include <Wire.h>                  // Biblioteca para comunicação I2C
#include <LiquidCrystal_I2C.h>     // Biblioteca para controlar o LCD via I2C

// ---------------------- DEFINIÇÃO DOS PINOS ----------------------
#define TRIG_PIN 8                 // Define o pino 8 como pino de trigger do sensor ultrassônico
#define ECHO_PIN 7                 // Define o pino 7 como pino de echo do sensor ultrassônico
#define BUTTON_PIN 2               // Define o pino 2 como pino do botão (com pull-up interno)
#define BUZZER_PIN 6               // Define o pino 6 como pino do buzzer
#define LED_R 5                    // Define o pino 5 como pino do LED vermelho (RGB)
#define LED_G 4                    // Define o pino 4 como pino do LED verde (RGB)
#define LED_B 3                    // Define o pino 3 como pino do LED azul (RGB)
#define SERVO_PIN 9                // Define o pino 9 como pino do servo (Tinkercad não aceita A3)

// ---------------------- OBJETOS DO SISTEMA ----------------------
LiquidCrystal_I2C lcd(0x27, 16, 2); // Cria objeto LCD com endereço I2C 0x27, 16 colunas e 2 linhas
Servo myServo;                      // Cria objeto para controlar o servo motor

// Variáveis de controle para a ativação do servo
bool servoActivated = false;        // Variável que controla se o servo foi ativado
unsigned long servoStartTime = 0;   // Armazena o tempo de início da ativação do servo

void setup() {
  Serial.begin(9600);               // Inicializa a comunicação serial a 9600 bauds para monitoramento

  // ---------------------- CONFIGURAÇÃO DOS PINOS ----------------------
  pinMode(TRIG_PIN, OUTPUT);        // Configura o pino TRIG como saída
  pinMode(ECHO_PIN, INPUT);         // Configura o pino ECHO como entrada
  pinMode(BUTTON_PIN, INPUT_PULLUP);// Configura o pino do botão com resistor pull-up interno
  pinMode(BUZZER_PIN, OUTPUT);      // Configura o buzzer como saída
  pinMode(LED_R, OUTPUT);           // Configura o LED vermelho como saída
  pinMode(LED_G, OUTPUT);           // Configura o LED verde como saída
  pinMode(LED_B, OUTPUT);           // Configura o LED azul como saída

  myServo.attach(SERVO_PIN);        // Anexa o servo no pino definido
  delay(200);                       // Aguarda o servo estabilizar
  myServo.write(0);                 // Move o servo para a posição inicial (0 graus)
  delay(300);                       // Atraso extra para evitar vibração inicial no Tinkercad

  // ---------------------- CONFIGURAÇÃO DO LCD ----------------------
  lcd.init();                       // Inicializa o display LCD
  lcd.backlight();                  // Liga a luz de fundo do LCD
  lcd.setCursor(0, 0);              // Posiciona o cursor na primeira coluna e primeira linha
  lcd.print("Dispenser:");          // Imprime texto inicial no display
}

void loop() {

  // ---------------- BOTÃO → GIRA O SERVO --------------------
  if (digitalRead(BUTTON_PIN) == LOW && !servoActivated) { // Se o botão foi pressionado e o servo está desligado
    servoActivated = true;         // Marca o servo como ativado
    servoStartTime = millis();     // Armazena o tempo atual em milissegundos

    myServo.write(90);             // Gira o servo para 90 graus
    tone(BUZZER_PIN, 1000, 200);   // Toca o buzzer a 1000Hz por 200ms
    delay(200);                    // Aguarda 200ms
    noTone(BUZZER_PIN);            // Desliga o buzzer
  }

  // --------- RETORNO DO SERVO PARA 0° APÓS 5 SEGUNDOS ---------
  if (servoActivated && millis() - servoStartTime >= 5000) { // Verifica se passaram 5 segundos
    myServo.write(0);              // Retorna o servo para 0 graus
    servoActivated = false;        // Desativa o servo
  }

  // --------------------- SENSOR ULTRASSÔNICO ---------------------
  digitalWrite(TRIG_PIN, LOW);     // Garante sinal baixo no TRIG
  delayMicroseconds(4);            // Aguarda 4 microssegundos
  digitalWrite(TRIG_PIN, HIGH);    // Envia pulso ultrassônico
  delayMicroseconds(10);           // Mantém o pulso por 10 microssegundos
  digitalWrite(TRIG_PIN, LOW);     // Finaliza o pulso

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); 
                                   // Mede o tempo de retorno do pulso (timeout de 30ms)
  long distance = duration * 0.034 / 2; 
                                   // Converte o tempo em distância (cm)

  // ------------------ SAÍDA NO MONITOR SERIAL ------------------
  Serial.print("Distancia: ");     // Imprime texto fixo no monitor serial
  Serial.print(distance);          // Imprime o valor da distância
  Serial.println(" cm");           // Finaliza a linha com "cm"

  // ---------------------- LCD ----------------------
  lcd.setCursor(0,0);              // Vai para a primeira linha
  lcd.print("                ");   // Limpa a linha com espaços
  lcd.setCursor(0,0);              // Volta para o início da linha

  if (distance == 0) lcd.print("Sem leitura");          // Quando não há retorno do sensor
  else if (distance > 20) lcd.print("Dispenser Vazio!"); // Distância maior que 20cm → vazio
  else if (distance > 15) lcd.print("Nivel Baixo");       // Entre 15 e 20cm → baixo
  else lcd.print("Dispenser Cheio!");                    // Abaixo de 15cm → cheio

  // ----------------- LED + BUZZER -----------------
  if (distance == 0) {              // Caso sem leitura do sensor
      digitalWrite(LED_R, HIGH);    // Liga LED vermelho
      digitalWrite(LED_G, LOW);     // Desliga LED verde
      digitalWrite(LED_B, LOW);     // Desliga LED azul
      noTone(BUZZER_PIN);           // Desliga o buzzer
  }
  else if (distance > 20) {         // Caso dispenser vazio
      digitalWrite(LED_R, HIGH);    // Liga LED vermelho
      digitalWrite(LED_G, LOW);     // Desliga LED verde
      digitalWrite(LED_B, LOW);     // Desliga LED azul
      tone(BUZZER_PIN, 1500);       // Liga buzzer continuamente a 1500Hz
  }
  else if (distance > 15) {         // Caso nível baixo
      digitalWrite(LED_R, HIGH);    // Liga LED vermelho
      digitalWrite(LED_G, HIGH);    // Liga LED verde (forma cor amarela)
      digitalWrite(LED_B, LOW);     // Desliga LED azul
      noTone(BUZZER_PIN);           // Buzzer desligado
  }
  else {                            // Caso dispenser cheio
      digitalWrite(LED_R, LOW);     // Desliga LED vermelho
      digitalWrite(LED_G, HIGH);    // Liga LED verde
      digitalWrite(LED_B, LOW);     // Desliga LED azul
      noTone(BUZZER_PIN);           // Buzzer desligado
  }

  delay(150);                       // Pequena pausa antes da próxima repetição
}
