#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define TRIG_PIN 8
#define ECHO_PIN 7
#define BUTTON_PIN 2
#define BUZZER_PIN 6
#define LED_R 5
#define LED_G 4
#define LED_B 3
#define SERVO_PIN A3

LiquidCrystal_I2C lcd(0x27, 16, 2);  
Servo myServo;

bool servoActivated = false;
unsigned long servoStartTime = 0;

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(0);
 
  Wire.begin();
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Dispenser:");
}

void loop() {

  if (digitalRead(BUTTON_PIN) == LOW && !servoActivated) {
    servoActivated = true;
    servoStartTime = millis();
    myServo.write(90);
    tone(BUZZER_PIN, 1000, 300);
    delay(300);
    noTone(BUZZER_PIN);
  }


  if (servoActivated && millis() - servoStartTime >= 5000) {
    myServo.write(0);
    servoActivated = false;
  }

  // Leitura do sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  long distance = duration * 0.034 / 2;

  // Atualiza LCD
  lcd.setCursor(0,0);
  lcd.print("                ");  
  lcd.setCursor(0,0);

  if (distance == 0) {
    lcd.print("Sem leitura");
  }
  else if (distance > 20) {
    lcd.print("Dispenser Vazio!");
  }
  else if (distance > 15) {
    lcd.print("Nivel Baixo");
  }
  else {
    lcd.print("Dispenser Cheio!");
  }

  if (distance == 0) {
      // Sem leitura → VERMELHO
      digitalWrite(LED_R, LOW);
      digitalWrite(LED_G, HIGH);
      digitalWrite(LED_B, HIGH);
      noTone(BUZZER_PIN);
  }
  else if (distance > 20) {
      // VAZIO → VERMELHO + SOM
      digitalWrite(LED_R, LOW);   // vermelho acende
      digitalWrite(LED_G, HIGH);  // verde apaga
      digitalWrite(LED_B, HIGH);  // azul apaga
      tone(BUZZER_PIN, 1500);
  }
  else if (distance > 15) {
      // NÍVEL BAIXO → AMARELO (R+G)
      digitalWrite(LED_R, LOW);   // vermelho aceso
      digitalWrite(LED_G, LOW);   // verde aceso
      digitalWrite(LED_B, HIGH);  // azul apagado
      noTone(BUZZER_PIN);
  }
  else {
      // CHEIO → VERDE
      digitalWrite(LED_R, HIGH);  // vermelho apagado
      digitalWrite(LED_G, LOW);   // verde aceso
      digitalWrite(LED_B, HIGH);  // azul apagado
      noTone(BUZZER_PIN);
  }

  delay(200);
}



