#include <SPI.h>
#include <Wire.h>

// =========== Preferences ===========
#define DIST_MAX 150
#define TRIGER_DISTANCE 70
#define WAIT_INTERVAL 10000

#define MAX_ANGEL 140
#define MIN_ANGEL 0
// =========== Preferences ===========

//------- Pins -------
#define DHTPIN 2
#define DHTTYPE DHT11
#define OLED_RESET 4
#define SERVO 3

#define TRIG 4
#define ECHO 5

#define TRIG2 6
#define ECHO2 7
//------- Pins -------

// Display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(OLED_RESET);
//

// RTC module
#include <iarduino_RTC.h>
iarduino_RTC time(RTC_DS1307);
//

// DHT module
#include "DHT.h"
DHT dht(DHTPIN, DHTTYPE);
//


// Servo
#include "Servo.h"
Servo servo;
//

// Sonars
#include <NewPing.h>
NewPing sonarLeft(TRIG, ECHO, DIST_MAX);
NewPing sonarRight(TRIG2, ECHO2, DIST_MAX);
//


class Sweeper
{
    Servo servo; // сервопривод
    int pos; // текущее положение серво
    int increment; // увеличиваем перемещение на каждом шаге

    int updateInterval_display = 10000; // промежуток времени между обновлениями
    unsigned long lastUpdate_display; // последнее обновление положения

    int range1;
    int range2;
    bool MODE = false;
    bool FIRST_RUN = true;

  public:
    Sweeper(int interval)
    {
      //updateInterval = interval;
      increment = 2;
    }

    void Attach(int pin)
    {
      servo.attach(pin);
      servo.write((MAX_ANGEL - MIN_ANGEL) / 2);
    }

    void Detach()
    {
      servo.detach();
    }

    void timerInterrupt() {

      if (((millis() - lastUpdate_display) > updateInterval_display) || (FIRST_RUN)) // время обновлять
      { 
        FIRST_RUN = false;

        PrintSensorsInfo();

        lastUpdate_display = millis();
      }
    }

    void Update()
    {

      range1 = sonarLeft.ping_cm();
      range2 = sonarRight.ping_cm();

      if (((range1 < TRIGER_DISTANCE) || (range2 < TRIGER_DISTANCE)) && (range1 != 0) && (range2 != 0))
      {

        digitalWrite(8, 1);
        digitalWrite(9, 0);

      }
      else
      {

        digitalWrite(8, 0);
        digitalWrite(9, 0);

      }

    }

    void PrintSensorsInfo()
    {
      display.setTextSize(1);
      display.setTextColor(WHITE);

      display.setCursor(0, 0);
      display.println(time.gettime("H:i      d-m-Y"));

      display.drawLine(0, 8, display.width() - 1, 8, WHITE);

      display.setCursor(0, 10);
      display.print("Temperature  ");
      display.print(String(int(dht.readTemperature())));
      display.print(" ");
      display.print(char(0176));
      display.print("C");


      display.setCursor(0, 20);
      display.print("Humidity     ");
      display.print(String(int(dht.readHumidity())));
      display.println(" %");
      display.display();

      display.clearDisplay();
    }
};

Sweeper sweeper(15);

void setup()
{
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  time.begin();
  dht.begin();

  pinMode(8, OUTPUT); // Мотор
  pinMode(9, OUTPUT);
  digitalWrite(8, 0);
  digitalWrite(9, 0);

  sweeper.Attach(SERVO);
  sweeper.Detach();

  loadingScreen();
}



//------------ Main Loop ------------
void loop()
{
  sweeper.Update();
  sweeper.timerInterrupt();
}
//------------ Main Loop ------------


// LOADING SCREEN
void loadingScreen () 
{
  display.setTextSize(0);
  display.setTextColor(WHITE);

  display.setCursor(30, 0);
  display.println("Created by:");
  display.setCursor(0, 10);
  display.println("...");

  display.display();
  delay(10000);
  display.clearDisplay();
}
