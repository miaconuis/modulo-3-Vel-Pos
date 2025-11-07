#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>
#include "driver/ledc.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pines
#define SERVO_PIN 23
#define POT_POS_PIN 34
#define POT_VEL_PIN 32
#define BTN_PIN 26
#define MOTOR_PWM 25
#define MOTOR_DIR 33
#define ENCODER_PIN 27

Servo myservo;

// Variables
int setAngle = 90;
int currentAngle = 90;
bool moveEnabled = false;
bool lastButtonState = HIGH;

const int PULSOS_POR_REV = 20;
volatile unsigned long pulsosEncoder = 0;
unsigned long lastRPMTime = 0;
float rpm = 0;
unsigned long lastPotTime = 0;
unsigned long lastOLEDTime = 0;

// PWM Config (ESP-IDF API)
#define PWM_CHANNEL    LEDC_CHANNEL_0
#define PWM_TIMER      LEDC_TIMER_0
#define PWM_FREQ       1000
#define PWM_RESOLUTION LEDC_TIMER_8_BIT // 8-bit => 0..255

void IRAM_ATTR contarPulsos() { pulsosEncoder++; }

void setup() {
  Serial.begin(115200);

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("No se encontró pantalla OLED!");
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 25);
  display.println("Inicializando...");
  display.display();
  delay(800);

  // Servo
  myservo.attach(SERVO_PIN);
  myservo.write(currentAngle);

  // Motor DIR pin (siempre HIGH en tu configuración actual)
  pinMode(MOTOR_DIR, OUTPUT);
  digitalWrite(MOTOR_DIR, HIGH);

  // Configurar PWM con API LEDC
  ledc_timer_config_t timer_conf = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_8_BIT,
    .timer_num = PWM_TIMER,
    .freq_hz = PWM_FREQ,
    .clk_cfg = LEDC_AUTO_CLK
  };
  ledc_timer_config(&timer_conf);

  ledc_channel_config_t ch_conf = {
    .gpio_num = MOTOR_PWM,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = PWM_CHANNEL,
    .timer_sel = PWM_TIMER,
    .duty = 0,
    .hpoint = 0
  };
  ledc_channel_config(&ch_conf);

  // Encoder
  pinMode(ENCODER_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), contarPulsos, RISING);

  // Botón
  pinMode(BTN_PIN, INPUT_PULLUP);

  display.clearDisplay();
  display.setCursor(30, 25);
  display.println("Sistema Listo!");
  display.display();
  delay(600);
}

void loop() {
  unsigned long now = millis();
  static int potPosValue = 0;
  static int potVelValue = 0;

  // Leer potenciómetros cada 30 ms
  if (now - lastPotTime >= 30) {
    lastPotTime = now;
    potPosValue = analogRead(POT_POS_PIN); // 0..4095
    potVelValue = analogRead(POT_VEL_PIN); // 0..4095
  }

  // Servo control (setpoint en tiempo real, movimiento sólo con botón)
  bool buttonState = digitalRead(BTN_PIN);
  setAngle = map(potPosValue, 0, 4095, 180, 0);
  if (lastButtonState == HIGH && buttonState == LOW) moveEnabled = true;
  lastButtonState = buttonState;
  if (moveEnabled) {
    myservo.write(setAngle);
    currentAngle = setAngle;
    moveEnabled = false;
  }

  // Motor control:
  // pwmValue = valor que mostramos en pantalla (0..255) — intuitivo para el usuario
  int pwmValue = map(potVelValue, 0, 4095, 255, 0);

  // Debido a que tu driver interpreta el PWM invertido (duty alto -> frena),
  // invertimos el valor al escribirlo al hardware:
  int pwmOut = 255 - pwmValue; // <-- esto hace que la pantalla y la acción correspondan

  // Escribir PWM al canal LEDC (uso API IDF)
  ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL, pwmOut);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL);

  // Calcular RPM cada 200 ms
  if (now - lastRPMTime >= 200) {
    unsigned long pulses = pulsosEncoder;
    pulsosEncoder = 0;
    float deltaT = (now - lastRPMTime) / 1000.0;
    rpm = (pulses / (float)PULSOS_POR_REV) * (60.0 / deltaT);
    lastRPMTime = now;
  }

  // Visualización en OLED (cada 200 ms)
  if (now - lastOLEDTime >= 200) {
    lastOLEDTime = now;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.println("MIACON - MONITOR");
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

    // Mostrar setpoint y servo
    display.setCursor(0, 18);
    display.print("Setpoint:");
    display.setCursor(75, 18);
    display.printf("%3d°", setAngle);

    display.setCursor(0, 30);
    display.print("Servo:");
    display.setCursor(75, 30);
    display.printf("%3d°", currentAngle);

    // Mostrar PWM y RPM. IMPORTANTE: mostramos pwmValue (lo que el usuario espera),
    // pero al hardware le dimos pwmOut (inverso).
    display.setCursor(0, 42);
    display.print("PWM:");
    display.setCursor(75, 42);
    display.printf("%3d", pwmValue);

    display.setCursor(0, 54);
    display.print("RPM:");
    display.setCursor(75, 54);
    display.printf("%5.1f", rpm);

    display.display();
  }
}
