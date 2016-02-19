/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * R. Derstadt wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Rob Derstadt
 * ----------------------------------------------------------------------------
 */

#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <Wire.h>
#include <SoftwareSerial.h>
#include <string.h>

#include <SparkFunMPL3115A2.h>
#include <Adafruit_FONA.h>

#define CELL_NUMBER "" // The cell number to notify (XXXXXXXXXX)
#define ALTITUDE_THRESHOLD 5.0 // Feet above launch site altitude
#define NUM_UPDATE_SMS 5 // Number of SMS messages to send upon landing
#define UPDATE_INTERVAL 120 // Interval between SMS messages

#define FONA_RX 8
#define FONA_TX 9
#define FONA_RST 4
#define ONBOARD_LED 13

typedef struct GPS {

  Adafruit_FONA* fona;
  float latitude;
  float longitude;
  float altitude; // Meters
  float UTC; // UTC time as yyymmddHHMMSS.ms
  long ttff; // Time to first fix (seconds)
  int satelites; // Number of visible satelites
  float speed; // Knots
  float course; // Heading in degrees

  // Read and parse GPS data
  inline void update() {

    char buffer[80];
    char* token;

    if (!fona) return;

    fona->getGPS(0, buffer, 80);

    if (strtok(buffer, ",") != NULL) {

      token = strtok(NULL, ",");
      longitude = atof(token) / 100.0;

      token = strtok(NULL, ",");
      latitude = atof(token) / 100.0;

      token = strtok(NULL, ",");
      altitude = atof(token);

      token = strtok(NULL, ",");
      UTC = atof(token);

      token = strtok(NULL, ",");
      ttff = atol(token);

      token = strtok(NULL, ",");
      satelites = atoi(token);

      token = strtok(NULL, ",");
      speed = atof(token);

      token = strtok(NULL, ",");
      course = atof(token);
    }
  }

  char* googleURL() {
    char sLat[13];
    char sLon[13];
    char* url = (char*) malloc(sizeof(char) * 50);
    dtostrf(longitude, 10, 8, sLon);
    dtostrf(latitude, 11, 8, sLat);
    sprintf(url, "comgooglemaps://q=%s,%s", sLat, sLon);
    return url;
  }

} GPS;

MPL3115A2 mpl3115A2;
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
GPS gps = {.fona = &fona};
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;
float launchSiteAltitude, maxAltitude;

void sendReadySMS() {
  char* googleUrl = gps.googleURL();
  char* message = (char*) malloc(sizeof(char) * 140);
  sprintf(message, "Ready for launch. Site altitude is %d ft. %d visible GPS satelites. Current location: %s", (int) launchSiteAltitude, gps.satelites, googleUrl);
  fona.sendSMS(CELL_NUMBER, message);
  free(googleUrl);
  free(message);
}

void sendLocationSMS() {
  char* googleUrl = gps.googleURL();
  char* message = (char*) malloc(sizeof(char) * 140);
  sprintf(message, "I've landed. My max altitude (AGL) was %d ft. Come find me at %s", (int) (maxAltitude - launchSiteAltitude), googleUrl);
  fona.sendSMS(CELL_NUMBER, message);
  free(googleUrl);
  free(message);
}

void setup() {

  Wire.begin();

  fonaSerial->begin(4800);
  if (!fona.begin(*fonaSerial)) {
    digitalWrite(ONBOARD_LED, HIGH);
  } else {
    fona.enableGPS(true);
  }

  mpl3115A2.begin();
  mpl3115A2.setModeAltimeter();
  mpl3115A2.setOversampleRate(7); // Requires at least 512ms between readings
  mpl3115A2.enableEventFlags();
}

void loop() {

  bool hasLaunched = 0;
  launchSiteAltitude = maxAltitude = 0;

  // Ensure RSSI is above 5 in order to send message
  while (fona.getRSSI() <= 5) {
    delay(500);
  }

  // Wait for GPS Lock (Fix)
  while (fona.GPSstatus() <= 1) {
    delay(500);
  }

  // Get average launch site altitude
  launchSiteAltitude = mpl3115A2.readAltitudeFt();
  for (int i = 0; i < 10; i++) {
    launchSiteAltitude = (launchSiteAltitude +  mpl3115A2.readAltitudeFt()) / 2.0;
    delay(600); // Required between readings
  }

  gps.update();

  sendReadySMS();

  float triggerAltitude = ALTITUDE_THRESHOLD + launchSiteAltitude;

  // Wait for launch
  while (true) {

    delay(515);

    float currentAltitude = mpl3115A2.readAltitudeFt();

    if (currentAltitude > maxAltitude) {
      maxAltitude = currentAltitude;
    }

    if (currentAltitude > triggerAltitude) {
      hasLaunched = true;
    }

    // we are about to land
    if (currentAltitude < triggerAltitude && hasLaunched) {
      // Wait about a minute to land
      delay(6000);
      break;
    }

  }

  // Send updates with location
  for (int i = 0; i < NUM_UPDATE_SMS; i++) {
    gps.update();
    sendLocationSMS();
    delay(UPDATE_INTERVAL * 1000.0);
  }

  // Done
  while (1) {
    delay(1000);
  }
}


