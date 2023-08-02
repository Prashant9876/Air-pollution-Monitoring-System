const int MQ135_PIN = A1;
#include <dht11.h>
#define DHT11PIN 4
dht11 DHT11;

#define measurePin A0 //Connect dust sensor to Arduino A0 pin
int samplingTime = 280; // time required to sample signal coming out   of the sensor
int deltaTime = 40; // 
int sleepTime = 9680;
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;
void  setup()
{
  Serial.begin(9600);
 
}

void loop()
{
  int chk = DHT11.read(DHT11PIN);
  Serial.print("Humidity (%): ");
  Serial.println((float)DHT11.humidity, 2);
  Serial.print("Temperature  (C): ");
  Serial.println((float)DHT11.temperature, 2);
  //--------------------------------------------------------------------------------------------//
  delayMicroseconds(samplingTime);
  voMeasured = analogRead(measurePin); // read the dust value
  delayMicroseconds(deltaTime);
  
  delayMicroseconds(sleepTime);
  // 0 - 5V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (5.0 / 1024.0);
  dustDensity = 170 * calcVoltage - 0.1;
  Serial.print("Dust Density :-");
  Serial.println(dustDensity); // unit: ug/m3

//-------------------------------------------------------------------------------------------------------
  int sensorValue = analogRead(MQ135_PIN);
  float ppm = getPPM(sensorValue);
  int aqi = calculateAQI(ppm);
  Serial.print("PPM: ");
  Serial.print(ppm);
  Serial.print(", AQI: ");
  Serial.println(aqi);

  delay(2000);

}

float getPPM(int sensorValue) {
  // Replace these values with the calibration values for your specific MQ135 sensor
  float resistanceCleanAir = 10000; // Resistance in clean air (Rs/R0)
  float slope = 2.3; // Sensitivity slope of the MQ135 sensor

  // Calculate the resistance ratio (Rs/R0)
  float resistanceRatio = float(sensorValue) / 1023.0;

  // Calculate the resistance of the sensor (Rs) in kOhms
  float resistanceSensor = (1.0 - resistanceRatio) / resistanceRatio * resistanceCleanAir;

  // Calculate the ppm value based on the sensitivity slope and sensor resistance
  float ppm = pow(10, ((log10(resistanceSensor) - log10(resistanceCleanAir)) / slope));
  
  return ppm;
}

int calculateAQI(float ppm) {
  // AQI calculation based on the EPA (United States Environmental Protection Agency) formula
  // Modify this formula according to your local AQI standards if necessary

  if (ppm < 0) ppm = 0;
  if (ppm > 2000) ppm = 2000;

  int AQI;
  if (ppm <= 50) {
    AQI = map(ppm, 0, 50, 0, 50);
  } else if (ppm <= 100) {
    AQI = map(ppm, 51, 100, 51, 100);
  } else if (ppm <= 150) {
    AQI = map(ppm, 101, 150, 101, 150);
  } else if (ppm <= 200) {
    AQI = map(ppm, 151, 200, 151, 200);
  } else if (ppm <= 300) {
    AQI = map(ppm, 201, 300, 201, 300);
  } else if (ppm <= 500) {
    AQI = map(ppm, 301, 500, 301, 500);
  } else {
    AQI = 500; // Clamp AQI value to 500 if ppm exceeds 500
  }

  return AQI;
}