/*************************************************************************
   BME680 Wireling Tutorial:
   Print the temperature(C), altitude, pressure(hPa), and humidity(%)
   readings the BME680 sensor is capable of reading

   Hardware by: TinyCircuits
   BME680 Library by: Adafruit
   Code by: Laveréna Wienclaw for TinyCircuits

   Initiated: Mon. 11/29/2017
   Updated: Tue. 07/31/2019
 ************************************************************************/

// This library is used for communication with I2C devices
#include <Wire.h>
#include <Wireling.h>

// These libraries are used to interface with the BME680 Sensor
#include "Adafruit_Sensor.h"
#include "Adafruit_BME680.h"

// Global Sensor Variables
#define SEALEVELPRESSURE_HPA (1013.25) // used to find approximate altitude 
Adafruit_BME680 bme; // I2C

// Used to control how often sensor values are updated in the main loop()
unsigned long delayTime = 1000;
const bool displayAlt = false; // if set to true, altitude will be displayed on the TinyScreen+ instead of gas 

// Make Serial Monitor compatible for all TinyCircuits processors
#if defined(ARDUINO_ARCH_AVR)
  #define SerialMonitorInterface Serial
#elif defined(ARDUINO_ARCH_SAMD)
  #define SerialMonitorInterface SerialUSB
#endif

void setup() {
  SerialMonitorInterface.begin(9600); // Bandwidth for our communication
  // Print to Serial Monitor
  // You can pass flash-memory based strings to SerialMonitorInterface.print() by wrapping them with F().
  // This means you're using flash memory instead of RAM to print stuff
  SerialMonitorInterface.println(F("BME680 test"));

  Wire.begin();
  Wireling.begin();
  Wireling.selectPort(0); // The adapter board has 4 different ports (0-3),

  // If the bme sensor is not found, throw statement and stop program
  // If you end up here, check to make sure your value in selectPort() is correct!
  if (!bme.begin(0x76)) {
    SerialMonitorInterface.println("Could not find a valid BME680 sensor, check wiring!"); // Printed to Serial Monitor
    while (1); // loop forever, because the rest of the program means nothing without the sensor
  }
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  SerialMonitorInterface.println("-- Default Test --");
  SerialMonitorInterface.println();
}

// Forever looping the following logic
void loop() {
  if (! bme.performReading()) {
    SerialMonitorInterface.println("Failed to perform reading :(");
    return;
  }
  printValues(); // Print to serial monitor
  delay(delayTime); // How often values are updated and printed
}

// This function prints out the values from the sensor to the Serial Monitor
void printValues() {
  SerialMonitorInterface.print("Temperature = ");
  SerialMonitorInterface.print(bme.readTemperature());
  SerialMonitorInterface.println(" *C");

  SerialMonitorInterface.print("Pressure = ");
  SerialMonitorInterface.print(bme.readPressure() / 100.0F);
  SerialMonitorInterface.println(" hPa");

  SerialMonitorInterface.print("Approx. Altitude = ");
  SerialMonitorInterface.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  SerialMonitorInterface.println(" m");

  SerialMonitorInterface.print("Humidity = ");
  SerialMonitorInterface.print(bme.readHumidity());
  SerialMonitorInterface.println(" %");

  SerialMonitorInterface.print("Gas = ");
  SerialMonitorInterface.print(bme.gas_resistance / 1000.0);
  SerialMonitorInterface.println(" KOhms");

  SerialMonitorInterface.println();
}
