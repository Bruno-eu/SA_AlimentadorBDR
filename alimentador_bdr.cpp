#include <Servo.h>                 // Biblioteca para controlar o servo motor
#include <Wire.h>                  // Biblioteca para comunicação I2C
#include <LiquidCrystal_I2C.h>     // Biblioteca para controlar o LCD I2C

// ----------- Definição dos pinos usados -----------
#define TRIG_PIN 8                 // Pino de trigger do sensor ultrassônico
#define ECHO_PIN 7                 // Pino de echo do sensor ultrassônico
#define BUTTON_PIN 2               // Pino do botão (com pull-up interno)
#define BUZZER_PIN 6               // Pino do buzzer
#define LED_R 5                    // Pino do LED vermelho
#define LED_G 4                    // Pino do LED verde
#define LED_B 3                    // Pino do LED azul
#define SERVO_PIN 9                // Pino do servo (Tinkercad não aceita A3)

// ---------- Objetos ----------
LiquidCrystal_I2C lcd(0x27, 16, 2); // Cria display I2C no endereço 0x27 com 16 colunas e 2 linhas
Servo myServo;                      // Objeto para controlar o servo

// ---------- Variáveis de controle do servo ----------
bool servoActivated = false;        // Marca se o servo está ativado
unsigned long servoStartTime = 0;   // Guarda o momento em que o servo começou a girar

void setup() {

  Serial.begin(9600);               // Inicializa comunicação serial a 9600 baud

  // ---------- Configuração dos pinos ----------
  pinMode(TRIG_PIN, OUTPUT);        // Trigger do sensor como saída
  pinMode(ECHO_PIN, INPUT);         // Echo do sensor como entrada
  pinMode(BUTTON_PIN, INPUT_PULLUP);// Botão usando resistor pull-up interno
  pinMode(BUZZER_PIN, OUTPUT);      // Buzzer como saída
  pinMode(LED_R, OUTPUT);           // LED vermelho como saída
  pinMode(LED_G, OUTPUT);           // LED verde como saída
  pinMode(LED_B, OUTPUT);           // LED azul como saída

  myServo.attach(SERVO_PIN);        // Conecta o servo ao pino definido
  delay(200);                       // Tempo para estabilizar o servo
  myServo.write(0);                 // Coloca o servo na posição inicial (0°)
  delay(300);                       // Evita vibração inicial do Tinkercad

  // ---------- Inicialização do LCD ----------
  lcd.init();                       // Inicia o display
  lcd.backlight();                  // Liga a luz de fundo
  lcd.setCursor(0, 0);              // Vai para coluna 0, linha 0
  lcd.print("Dispenser:");          // Escreve texto inicial
}                                    // Fim da função setup()

void loop() {

  // ------------ BOTÃO → GIRA SERVO -------------
  // Ativa o servo somente se: O botão for pressionado (nível LOW) e o servo ainda não estiver ativado
  if (digitalRead(BUTTON_PIN) == LOW && !servoActivated) { // Verifica se botão foi pressionado e servo não está ativo

    servoActivated = true;         // Marca que o servo foi ativado
    servoStartTime = millis();     // Guarda o tempo atual

    myServo.write(180);             // Gira o servo para 180° (abre)
    tone(BUZZER_PIN, 1000, 200);   // Buzzer toca por 200ms
    delay(200);                    // Espera o tempo do buzzer
    noTone(BUZZER_PIN);            // Desliga o buzzer
  }

  // ----------- VOLTA PARA 0 APÓS 5s -----------
  if (servoActivated && millis() - servoStartTime >= 5000) { // Verifica se servo está ativo e passaram 5 segundos

    myServo.write(0);              // Volta o servo para 0°
    servoActivated = false;        // Marca que ele desligou
  }

  // ------------ SENSOR ULTRASSÔNICO -----------
  digitalWrite(TRIG_PIN, LOW);     // Garante pulso baixo
  delayMicroseconds(4);            // Aguarda 4 microssegundos
  digitalWrite(TRIG_PIN, HIGH);    // Envia pulso ultrassônico
  delayMicroseconds(10);           // Aguarda 10 microssegundos para o pulso
  digitalWrite(TRIG_PIN, LOW);     // Desliga o pino TRIG após enviar o pulso

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);   // Mede tempo de retorno do pulso (máx 30ms)
  long distance = duration * 0.034 / 2;             // Converte tempo para distância em cm

  // ---------- Imprime no monitor serial ----------
  Serial.print("Distancia: ");     // Imprime texto "Distancia: " no monitor serial
  Serial.print(distance);          // Imprime o valor da distância no monitor serial
  Serial.println(" cm");           // Imprime " cm" e pula linha no monitor serial
                                    // Linha em branco para organização do código

  // ---------------- LCD ----------------
  lcd.setCursor(0,0);               // Posiciona cursor na primeira coluna, primeira linha
  lcd.print("                ");   // Limpa a linha com espaços em branco
  lcd.setCursor(0,0);               // Reposiciona cursor na primeira coluna, primeira linha

  if (distance == 0) lcd.print("Sem leitura");         // Sensor não detectou retorno
  else if (distance > 30) lcd.print("Dispenser Vazio!"); // Acima de 20 cm → vazio
  else if (distance > 20) lcd.print("Nivel Baixo");       // Entre 15 e 20 → baixo
  else lcd.print("Dispenser Cheio!");                    // Abaixo de 15 → cheio

  // ------------- LED + BUZZER -------------
  if (distance == 0) {              // Se sem leitura
    digitalWrite(LED_R, HIGH);      // Liga o LED vermelho
    digitalWrite(LED_G, LOW);       // Desliga o LED verde
    digitalWrite(LED_B, LOW);       // Desliga o LED azul
    noTone(BUZZER_PIN);            // Buzzer desligado
  }
  else if (distance > 30) {         // Dispenser vazio
    digitalWrite(LED_R, HIGH);      // Liga o LED vermelho
    digitalWrite(LED_G, LOW);       // Desliga o LED verde
    digitalWrite(LED_B, LOW);       // Desliga o LED azul
    tone(BUZZER_PIN, 1500);        // Buzzer contínuo
  }
  else if (distance > 20) {         // Nível baixo
    digitalWrite(LED_R, LOW);      // Desliga o LED vermelho
    digitalWrite(LED_G, LOW);     // Desliga o LED verde 
    digitalWrite(LED_B, HIGH);       // Liga o LED azul
    noTone(BUZZER_PIN);             // Desliga o buzzer
  }                                 
  else {                            // Cheio
    digitalWrite(LED_R, LOW);       // Desliga o LED vermelho
    digitalWrite(LED_G, HIGH);      // Liga o LED verde
    digitalWrite(LED_B, LOW);       // Desliga o LED azul
    noTone(BUZZER_PIN);             // Desliga o buzzer
  }                                 // Fim do bloco else para dispenser cheio

  delay(150);                       // Pequena pausa antes de repetir o loop
}                                    // Fim da função loop()
