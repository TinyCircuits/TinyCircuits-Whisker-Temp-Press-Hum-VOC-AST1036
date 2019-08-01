/*************************************************************************
   BME680 WhiskerBoard Tutorial:
   Print the temperature(C), altitude, pressure(hPa), and humidity(%)
   readings the BME680 sensor is capable of reading

   Hardware by: TinyCircuits
   BME680 Library by: Adafruit
   Code by: Laverena Wienclaw and Zachary Lee for TinyCircuits

   Initiated: Mon. 11/29/2017
   Updated: Tue. 07/31/2019
 ************************************************************************/

// This library is used for communication with I2C devices, such as the BME680 board
#include <Wire.h>

// This library is used to print sensor values to a TinyScreen
#include <TinyScreen.h>

// These libraries are used to interface with the BME680 Sensor
#include "Adafruit_Sensor.h"
#include "Adafruit_BME680.h"

// Global Sensor Variables
#define SEALEVELPRESSURE_HPA (1013.25) // used to find approximate altitude 
Adafruit_BME680 bme; // I2C

// TinyScreen Global Variables
TinyScreen display = TinyScreen(TinyScreenPlus);
int background = TS_8b_Black; // sets the background color to black

// Used to control how often sensor values are updated in the main loop()
unsigned long delayTime = 1000;

// The power pin for our board, used for digitally writing to output
const int powerPin = 4;
const bool displayAlt = false; // if set to true, altitude will be displayed on the TinyScreen+ instead of gas due to shortage of room. Will not impact serial output.
void setup() {
  SerialUSB.begin(9600); // Bandwidth for our communication
  // Print to Serial Monitor
  // You can pass flash-memory based strings to SerialUSB.print() by wrapping them with F().
  // This means you're using flash memory instead of RAM to print stuff
  while (!SerialUSB);
  SerialUSB.println(F("BME680 test"));

  // We want to see Digital Output from the sensor
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);

  Wire.begin();
  selectPort(0); // The adapter board has 4 different ports (0-3),
  // make sure your software matches the setup!

  // This is the setup used to initialize the TinyScreen's appearance
  display.begin();
  display.setBrightness(15);
  display.setFlip(true);
  display.setFont(thinPixel7_10ptFontInfo);
  display.fontColor(TS_8b_White, background);

  // Set the cursor to the following coordinates before it prints "BME680 Test"
  display.setCursor(0, 0);
  display.print("BME680 Test");

  // If the bme sensor is not found, throw statement and stop program
  // If you end up here, check to make sure your value in selectPort() is correct!
  if (!bme.begin(0x76)) {
    display.setCursor(12, 12);
    display.print("No Sensor!");  // Printed to TinyScreen
    SerialUSB.println("Could not find a valid BME680 sensor, check wiring!"); // Printed to Serial Monitor
    while (1); // loop forever, because the rest of the program means nothing without the sensor
  }
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  SerialUSB.println("-- Default Test --");
  SerialUSB.println();
}

// Forever looping the following logic
void loop() {
  if (! bme.performReading()) {
    SerialUSB.println("Failed to perform reading :(");
    return;
  }
  printValues(); // Print to serial monitor
  printScreen(); // Print to TinyScreen
  delay(delayTime); // How often values are updated and printed
}

// This function prints out the values from the sensor to the Serial Monitor
void printValues() {
  SerialUSB.print("Temperature = ");
  SerialUSB.print(bme.readTemperature());
  SerialUSB.println(" *C");

  SerialUSB.print("Pressure = ");
  SerialUSB.print(bme.readPressure() / 100.0F);
  SerialUSB.println(" hPa");

  SerialUSB.print("Approx. Altitude = ");
  SerialUSB.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  SerialUSB.println(" m");

  SerialUSB.print("Humidity = ");
  SerialUSB.print(bme.readHumidity());
  SerialUSB.println(" %");

  SerialUSB.print("Gas = ");
  SerialUSB.print(bme.gas_resistance / 1000.0);
  SerialUSB.println(" KOhms");

  SerialUSB.println();
}

// This function prints out the values from the sensor to a TinyScreen screen
void printScreen() {
  display.setCursor(0, 12);
  display.print("Temp: ");
  display.print(bme.readTemperature());
  display.println(" C");

  display.setCursor(0, 24);
  display.print("Pres: ");
  display.print(bme.readPressure() / 100.0F);
  display.println(" hPa");

  if (displayAlt) {
    display.setCursor(0, 36);
    display.print("Alt: ");
    display.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    display.println(" m");
  }
  else {
    display.setCursor(0, 36);
    display.print("Gas: ");
    display.print(bme.gas_resistance / 1000.0);
    display.println(" kOhms");
  }

  display.setCursor(0, 48);
  display.print("Hum: ");
  display.print(bme.readHumidity());
  display.println(" %");

  display.println();
}

// **This function is necessary for all Whisker boards attached through an Adapter board**
// Selects the correct address of the port being used in the Adapter board
void selectPort(int port) {
  Wire.beginTransmission(0x70); //I2C
  Wire.write(0x04 + port);
  Wire.endTransmission();
}
