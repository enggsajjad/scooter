
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

/// BME280 I2C Address 0x77 for Red, 0x76 for Purple (chinese)
#define BME_ADDR  0x76
/// BME280 I2C SDA pin
#define BME_SDA 19//21
/// BME280 I2C SCL pin
#define BME_SCL 18//22


unsigned char bmeAddress;
bool use_bme280;


#define SEALEVELPRESSURE_HPA (1013.25)

bool wireStatus = Wire1.begin(BME_SDA, BME_SCL);
Adafruit_BME680 bme(&Wire1); // I2C


void setup() 
{
  delay(1000);
  Serial.begin(115200);

  Serial.println(F("BME680 test"));

  if (!wireStatus) Serial.print("Wire1 failed to init");

  bmeAddress = BME_ADDR;
  
  if (!bme.begin(bmeAddress, true)) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }
  
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void loop() {
  if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  Serial.print("Temperature = ");
  Serial.print(bme.temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bme.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("Humidity = ");
  Serial.print(bme.humidity);
  Serial.println(" %");

  Serial.print("Gas = ");
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(" KOhms");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.println();
  delay(2000);
}
