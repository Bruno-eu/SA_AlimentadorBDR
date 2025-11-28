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
#define SERVO_PIN A3   // Funciona em Arduino físico


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


  // -------------------------------------------------------
  //   BOTÃO → GIRA O SERVO 90 GRAUS
  // -------------------------------------------------------
  if (digitalRead(BUTTON_PIN) == LOW && !servoActivated) {
    servoActivated = true;
    servoStartTime = millis();


    myServo.write(90);     // GIRA 90°


    tone(BUZZER_PIN, 1000, 200);
    delay(200);
    noTone(BUZZER_PIN);
  }


  // Após 5 segundos → volta para 0°
  if (servoActivated && millis() - servoStartTime >= 5000) {
    myServo.write(0);      // Volta para 0°
    servoActivated = false;
  }




  // -------------------------------------------------------
  //          LEITURA DO SENSOR ULTRASSÔNICO
  // -------------------------------------------------------
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);


  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  long distance = duration * 0.034 / 2;




  // -------------------------------------------------------
  //               EXIBIÇÃO NO LCD
  // -------------------------------------------------------
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,0);


  if (distance == 0) lcd.print("Sem leitura");
  else if (distance > 20) lcd.print("Dispenser Vazio!");
  else if (distance > 15) lcd.print("Nivel Baixo");
  else lcd.print("Dispenser Cheio!");




  // -------------------------------------------------------
  //             LED + BUZZER (ALERTAS)
  // -------------------------------------------------------
  if (distance == 0) {
      digitalWrite(LED_R, HIGH);
      digitalWrite(LED_G, LOW);
      digitalWrite(LED_B, LOW);
      noTone(BUZZER_PIN);
  }
  else if (distance > 20) {
      digitalWrite(LED_R, HIGH);
      digitalWrite(LED_G, LOW);
      digitalWrite(LED_B, LOW);
      tone(BUZZER_PIN, 1500);
  }
  else if (distance > 15) {
      digitalWrite(LED_R, HIGH);
      digitalWrite(LED_G, HIGH);
      digitalWrite(LED_B, LOW);
      noTone(BUZZER_PIN);
  }
  else {
      digitalWrite(LED_R, LOW);
      digitalWrite(LED_G, HIGH);
      digitalWrite(LED_B, LOW);
      noTone(BUZZER_PIN);
  }


  delay(200);
}





