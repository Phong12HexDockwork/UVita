#include <Wire.h>
#include <SparkFunTSL2561.h>
#include <ML8511.h>
#include <SoftwareSerial.h>

// Create an SFE_TSL2561 object
SFE_TSL2561 light;

// Define the pins for the ML8511 sensor
#define ANALOGPIN     A0
#define ENABLEPIN     A1

// Create an instance of the ML8511 sensor
ML8511 uvSensor(ANALOGPIN, ENABLEPIN);
SoftwareSerial Bluetooth(2, 3);

void setup() {
  Serial.begin(9600);
  Bluetooth.begin(9600);

  // Initialize the TSL2561 sensor
  Serial.println("Initializing TSL2561...");
  
  if (light.begin()) {
    Serial.println("TSL2561 Initialized successfully");
  } else {
    Serial.println("Error initializing TSL2561");
    while (1); // halt execution
  }

  // Set default timing
  boolean gain = 0; // Gain: low (1X)
  unsigned char time = 2; // Integration time: 402 ms
  unsigned int ms; // Variable to hold integration time in ms

  light.setTiming(gain, time, ms); // Set timing with a reference to ms
  light.setPowerUp(); // Power up the sensor
  Serial.println("TSL2561 powered up.");

  Serial.println("UV UltraViolet ML8511 Initialized");
}

void loop() {
  // Wait for the integration time
  delay(402); // Delay for the integration time set in setup

  unsigned int data0, data1;
  
  // Retrieve the data from the TSL2561
  if (light.getData(data0, data1)) {
    Serial.print("Data0: ");
    Serial.print(data0);
    Serial.print(" | Data1: ");
    Serial.print(data1);

    double lux; // Resulting lux value
    boolean good = light.getLux(0, 402, data0, data1, lux); // Calculate lux

    // Send light level to Bluetooth
    Bluetooth.print(lux);
    
    // Print lux to Serial
    Serial.print(" Calculated Lux: ");
    Serial.println(lux);

    // Check if the light level is greater than 2 lux
    if (data0 > 10) {
      // Manually enable the ML8511 sensor
      uvSensor.enable();      
      // Read UV level from ML8511
      float UV = uvSensor.getUV(); // Default mode, no low power here
      uvSensor.disable();
      // Print both light and UV values on the same line
      Bluetooth.print(",");
      Bluetooth.print(UV, 4);
      

      // Print UV level to Serial
      Serial.print("UV Level: ");
      Serial.print(UV, 4);

      // Manually disable the ML8511 sensor
      Serial.println(" ML8511 sensor disabled. ");
    } else {
      // Print only light level when UV sensor is not enabled
      Bluetooth.print(" /t ");
      Bluetooth.print("N/A");
      Serial.println("UV measurement skipped (light < 2 lux).");
    }
    Bluetooth.println(); // Move to the next line after printing both values
  } else {
    // Handle communication errors
    byte error = light.getError();
    Serial.print("I2C error: ");
    Serial.println(error);
  }
  
  delay(1000); // Delay between measurements
}
