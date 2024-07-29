#include <Wire.h>
#include <FirebaseESP32.h>
#include <Adafruit_BMP280.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include<time.h>


#define BMP_SCK (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define CS (10)

// Replace these with your WiFi and Firebase credentials
#define WIFI_SSID "angrybird"
#define WIFI_PASSWORD "musiclove"
#define FIREBASE_HOST "esp32-firebase-demo-791ad-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "AIzaSyCrMRlowDO5V0GidSbrJaRbytXPeyHNrxQ"

Adafruit_BMP280 bmp280; // I2C Interface
Adafruit_MPU6050 mpu;
// GPS settings
HardwareSerial gpsSerial(1); // Use Serial1 for GPS
TinyGPSPlus gps;
 

FirebaseData fbdo;
FirebaseConfig config;
FirebaseAuth firebaseAuth;
unsigned long sendDataPrevMillis = 0;
bool signupok = false;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17); // GPS module on Serial1 with TX=16, RX=17

  Serial.println(F("GPS Test Started")); 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("Connected!");

  // Assign the database URL and authentication token to the FirebaseConfig object
  config.database_url = FIREBASE_HOST;
  config.api_key = FIREBASE_AUTH;

  if (Firebase.signUp(&config, &firebaseAuth, "", "")) {
    Serial.println("Sign up successful");
    signupok = true;
  } else {
    Serial.printf("Sign up failed: %s\n", config.signer.signupError.message.c_str());
  }

  // Assign the callback function for the long running token generation task
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
  Firebase.begin(&config, &firebaseAuth); 
  Firebase.reconnectWiFi(true);
  Serial.println("Firebase initialized successfully!");

  unsigned status1 = bmp280.begin(0x76);
  unsigned status2 = mpu.begin(0x69);
  Serial.println(F("BMP280 test"));
  if (!status1) {
    Serial.println("Device error! Check wiring");
  } else {
    Serial.println("BMP280 Ready for sensing");
  }

  if (!status2) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  bmp280.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                     Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                     Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                     Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                     Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
  }
  
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
      Serial.println("+- 250 deg/s");
      break;
    case MPU6050_RANGE_500_DEG:
      Serial.println("+- 500 deg/s");
      break;
    case MPU6050_RANGE_1000_DEG:
      Serial.println("+- 1000 deg/s");
      break;
    case MPU6050_RANGE_2000_DEG:
      Serial.println("+- 2000 deg/s");
      break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
  }
  // Configure time to fetch NTP
  configTime(19800, 0, "pool.ntp.org", "time.nist.gov");

  // Wait for time to be set
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("Time is set!");
 delay(100);
}                  

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }
  
  float latitude=0.0;
  float longitude=0.0;
  
  float temperature = bmp280.readTemperature();
  float pressure = bmp280.readPressure() / 100;
  float Altitude = bmp280.readAltitude(1013.25);

  Serial.print(F("Temperature  = "));
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print(F("Pressure = "));
  Serial.print(pressure);  //displaying the Pressure in hPa, you can change the unit
  Serial.println("  hPa");

  Serial.print(F("Approx altitude = "));
  Serial.print(Altitude);  //The "963.98" is the pressure(hPa) at sea level in day in your region
  Serial.println("  m");                    //If you don't know it, modify it until you get your current altitude

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  float accx = a.acceleration.x;
  float accy = a.acceleration.y;
  float accz = a.acceleration.z;

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(accx);
  Serial.print(", Y: ");
  Serial.print(accy);
  Serial.print(", Z: ");
  Serial.print(accz);
  Serial.println(" m/s^2");

  float rotx = g.gyro.x;
  float roty = g.gyro.y;
  float rotz = g.gyro.z;  

  Serial.print("Rotation X: ");
  Serial.print(rotx);
  Serial.print(", Y: ");
  Serial.print(roty);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(rotz);

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");

  Serial.println("");
  delay(500);

  if (gps.location.isUpdated()) {

    latitude = gps.location.lat();
    longitude = gps.location.lng();
    Serial.print("Latitude: ");
    Serial.println(latitude, 6);
    Serial.print("Longitude: ");
    Serial.println(longitude, 6);
  } else {
    Serial.println(F("GPS data not updated"));
  }

  if (Firebase.ready() && signupok && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
// Get current time
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    // Print IST time for debugging
    Serial.printf("Current IST Time: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    // Get current timestamp
    // Add 5 hours and 30 minutes for IST


// Convert to Unix timestamp
unsigned long timestamp = mktime(&timeinfo);

    // Write sensor data to Firebase with timestamp
    String path = "devices/device2/sensor_data/" + String(timestamp);
    
    // Write the temperature data to the database
    if (Firebase.setInt(fbdo, path + "/temperature", temperature)) {
      Serial.println("Temperature data sent successfully");
    } else {
      Serial.println("Failed to send temperature data");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    // Write the pressure data to the database
    if (Firebase.setFloat(fbdo, path + "/pressure", pressure)) {
      Serial.println("Pressure data sent successfully");
    } else {
      Serial.println("Failed to send pressure data");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Write the altitude data to the database
    if (Firebase.setFloat(fbdo, path + "/altitude", Altitude)) {
      Serial.println("Altitude data sent successfully");
    } else {
      Serial.println("Failed to send altitude data");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Send acceleration data
    if (Firebase.setFloat(fbdo, path + "/gyro/acceleration/x", accx)&&
    Firebase.setFloat(fbdo, path + "/gyro/acceleration/y", accy)&&
    Firebase.setFloat(fbdo, path + "/gyro/acceleration/z", accz)) {
      Serial.println("Acceleration data sent successfully");
    } else {
      Serial.println("Failed to send acceleration data");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    // Send rotation data
    if ( Firebase.setFloat(fbdo, path + "/gyro/rotation/x", rotx)&&
    Firebase.setFloat(fbdo, path + "/gyro/rotation/y", roty)&&
    Firebase.setFloat(fbdo, path + "/gyro/rotation/z", rotz)) {
      Serial.println("Rotation data sent successfully");
    } else {
      Serial.println("Failed to send rotation data");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Send GPS data
    if( Firebase.setFloat(fbdo, path + "/gps/latitude", latitude)&&
    Firebase.setFloat(fbdo, path + "/gps/longitude", longitude)){
      Serial.println("GPS data sent successfully");
    }else {
      Serial.println("Failed to send GPS data");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    delay(60000);
  }
}