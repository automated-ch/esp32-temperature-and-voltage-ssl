#include <OneWire.h>
#include<DallasTemperature.h>
#include <sstream>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <esp_wifi.h>

using namespace std;

#define uS_TO_S_FACTOR 1000000LL             /* Conversion factor for micro seconds to seconds */
#define SENSOR_SLEEP_TIME_SEC  1800LL        /* Time ESP32 will go to sleep (in seconds), */
                                             /*it is suffixed with LL  to allow large values*/

#define ONE_WIRE_BUS 15 // DS18B20 on arduino pin4 corresponds to GPI12 on physical board
#define RED_LED_PIN 13
const uint8_t vbatPin = 36;

#define SENSOR_TYPE "temperature"
#define SENSOR_UNIT "celsius"


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
float temp;

WiFiClientSecure client;

// use a local server
bool isLocal = false;

const char* ssid     = "<Your WIFI SSID>";     // your network SSID (name of wifi network)
const char* password = "<Your WLAN password>"; // your network password

const char*  INET_SERVER = "<target remote server url>";  // production server URL
const char*  LOCAL_SERVER = "<local or dev server url>";  // dlocal or dev server URL
const char*  UUID_CLIENT = "c65898bc-8a98-473a-ad04-4ed87ab3f884";  // a unique uuid string


/*
 *Note: Remote server certificate validation 
 *      is ONLY used for target remote server 
 *      and NOT for LOCAL or DEV
 * 
 * your target remote servers root cert make sure to keep the linebreaks
 * and the linebreaks (\n) at the end of each line  \n
 * 
 * Below is an example for sites which use a letsencrypt certificate
 * 
 * Further reading: https://letsencrypt.org/ and
 *                  https://letsencrypt.org/certificates/
 * 
 */
const char* test_root_ca = \
     "-----BEGIN CERTIFICATE-----\n" \
     "MIIEkjCCA3qgAwIBAgIQCgFBQgAAAVOFc2oLheynCDANBgkqhkiG9w0BAQsFADA/\n" \
     "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
     "DkRTVCBSb290IENBIFgzMB4XDTE2MDMxNzE2NDA0NloXDTIxMDMxNzE2NDA0Nlow\n" \
     "SjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxIzAhBgNVBAMT\n" \
     "GkxldCdzIEVuY3J5cHQgQXV0aG9yaXR5IFgzMIIBIjANBgkqhkiG9w0BAQEFAAOC\n" \
     "AQ8AMIIBCgKCAQEAnNMM8FrlLke3cl03g7NoYzDq1zUmGSXhvb418XCSL7e4S0EF\n" \
     "q6meNQhY7LEqxGiHC6PjdeTm86dicbp5gWAf15Gan/PQeGdxyGkOlZHP/uaZ6WA8\n" \
     "SMx+yk13EiSdRxta67nsHjcAHJyse6cF6s5K671B5TaYucv9bTyWaN8jKkKQDIZ0\n" \
     "Z8h/pZq4UmEUEz9l6YKHy9v6Dlb2honzhT+Xhq+w3Brvaw2VFn3EK6BlspkENnWA\n" \
     "a6xK8xuQSXgvopZPKiAlKQTGdMDQMc2PMTiVFrqoM7hD8bEfwzB/onkxEz0tNvjj\n" \
     "/PIzark5McWvxI0NHWQWM6r6hCm21AvA2H3DkwIDAQABo4IBfTCCAXkwEgYDVR0T\n" \
     "AQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAYYwfwYIKwYBBQUHAQEEczBxMDIG\n" \
     "CCsGAQUFBzABhiZodHRwOi8vaXNyZy50cnVzdGlkLm9jc3AuaWRlbnRydXN0LmNv\n" \
     "bTA7BggrBgEFBQcwAoYvaHR0cDovL2FwcHMuaWRlbnRydXN0LmNvbS9yb290cy9k\n" \
     "c3Ryb290Y2F4My5wN2MwHwYDVR0jBBgwFoAUxKexpHsscfrb4UuQdf/EFWCFiRAw\n" \
     "VAYDVR0gBE0wSzAIBgZngQwBAgEwPwYLKwYBBAGC3xMBAQEwMDAuBggrBgEFBQcC\n" \
     "ARYiaHR0cDovL2Nwcy5yb290LXgxLmxldHNlbmNyeXB0Lm9yZzA8BgNVHR8ENTAz\n" \
     "MDGgL6AthitodHRwOi8vY3JsLmlkZW50cnVzdC5jb20vRFNUUk9PVENBWDNDUkwu\n" \
     "Y3JsMB0GA1UdDgQWBBSoSmpjBH3duubRObemRWXv86jsoTANBgkqhkiG9w0BAQsF\n" \
     "AAOCAQEA3TPXEfNjWDjdGBX7CVW+dla5cEilaUcne8IkCJLxWh9KEik3JHRRHGJo\n" \
     "uM2VcGfl96S8TihRzZvoroed6ti6WqEBmtzw3Wodatg+VyOeph4EYpr/1wXKtx8/\n" \
     "wApIvJSwtmVi4MFU5aMqrSDE6ea73Mj2tcMyo5jMd6jmeWUHK8so/joWUoHOUgwu\n" \
     "X4Po1QYz+3dszkDqMp4fklxBwXRsW10KXzPMTZ+sOPAveyxindmjkW8lGy+QsRlG\n" \
     "PfZ+G6Z6h7mjem0Y+iWlkYcV4PIWL1iwBi8saCbGS5jN2p8M+X+Q7UNKEkROb3N6\n" \
     "KOqkqm57TH2H3eDJAkSnh6/DNFu0Qg==\n" \
     "-----END CERTIFICATE-----\n";


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1500); //Take some time to open up the Serial Monitor

  //Print the wakeup reason for ESP32
  printWakeupReason();

  /*
    First we configure the wake up source
    We set our ESP32 to wake up every x seconds
  */
  esp_sleep_enable_timer_wakeup(SENSOR_SLEEP_TIME_SEC * uS_TO_S_FACTOR);
  

  // put your main code here, to run repeatedly:
  temperatura();
    
  if(prepareWifi(3)){
    sendData(temp);
  }
  
  prepareSleep();
  esp_deep_sleep_start();

}

void loop() {
  
}

void temperatura()
{
  //init temperature sensor
  DS18B20.begin();
  
  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, HIGH);
  
  // execute temperature meassurement
  DS18B20.requestTemperatures();
  temp = DS18B20.getTempCByIndex(0);
  Serial.print("current temperature: ");
  Serial.println(temp);
  
  digitalWrite(RED_LED_PIN, LOW);
  
}

/*
  Method to print the reason by which ESP32
  has been awaken from sleep
*/
void printWakeupReason() {
  esp_sleep_wakeup_cause_t wakeupReason;

  wakeupReason = esp_sleep_get_wakeup_cause();
  
  switch (wakeupReason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}

void prepareSleep(){
  
  //esp_bluedroid_disable();
  //esp_bt_controller_disable();
  esp_wifi_stop();
  Serial.println("Going to sleep now");
}

/*************************************
 * 
 * connect to the WIFI, if a connection
 * attempt fails function will retry to connect after 2000 milliseconds
 * 
 * @return true 
 * 
 ************************************/
boolean prepareWifi(int maxRetries){
  WiFi.begin(ssid, password);
  int retries=0;

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("-");
    // wait 1 second for re-trying
    delay(5000);
    retries = retries +1;
    if(retries > maxRetries){
      flashy();
      flashy();
      flashy();
      flashy();
      return false;
    }
    return true;
  }

  Serial.print("Connected to ");
  Serial.println(ssid);
}

/****************************************
 * 
 * Send data to server
 * 
 * data can be send to a remote server 
 * or to a local machine for testing
 * 
 * @param data - this var should hold only the sensor data 
 * 
 * *************************************/
void sendData(float data){
  
  analogSetAttenuation(ADC_0db);
  
  float VBAT;
  int adcReadval = analogRead(vbatPin);
  // Battery Voltage
  VBAT = (4.2 / 4095.0)*(float)(adcReadval); // LiPo battery voltage in volts
  
  
  if (! isLocal) {
    Serial.print("LIVE mode");
    client.setCACert(test_root_ca);
    
    Serial.print("LIVE mode");
    Serial.println("\nStarting connection to server...");
    if (!client.connect(INET_SERVER, 443)) {              // change this if your server uses a different port for https
      Serial.println("Connection failed!");
      Serial.println(INET_SERVER);
      flashy();
      flashy();
      flashy();
      client.stop();
    }
    else {
      client.stop();
      blink();
      blink();
      Serial.println("Connected to server!");
      Serial.println(INET_SERVER);

      // prepare sensor data
      std::stringstream sensorDataStream;

      sensorDataStream << "sensor_mac=" << WiFi.macAddress().c_str();
      sensorDataStream << "&sensor_connection_type=WIFI"; 
      sensorDataStream << "&sensor_type=" << SENSOR_TYPE; 
      sensorDataStream << "&sensor_unit=" << SENSOR_UNIT; 
      sensorDataStream << "&sensor_sleep_time_sec=" << SENSOR_SLEEP_TIME_SEC;
      sensorDataStream << "&sensor_ip=" << WiFi.localIP().toString().c_str();
      sensorDataStream << "&sensor_data=" << data;
      sensorDataStream << "&sensor_voltage=" << VBAT;
        
      // Make a HTTP request:
      HTTPClient http;
      http.begin("https://<target remote server url>/sensor/indata");     // change this to your needs
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      http.addHeader("X-Auth", UUID_CLIENT);
      http.POST(sensorDataStream.str().c_str()); // sensor data
      //http.writeToStream(&Serial);
      http.end();

      client.stop();
    }
  } else {
    Serial.print("LOCAL mode");

    Serial.println("\nStarting connection to server...");
    if (!client.connect(LOCAL_SERVER, 8181)) {              // chnage this if your local server uses a different port for https
      Serial.println("Connection failed!");
      Serial.println(LOCAL_SERVER);
      flashy();
      flashy();
      flashy();
    }
    else {
      blink();
      blink();
      blink();
      Serial.println("Connected to server!");
      Serial.println(LOCAL_SERVER);

      // prepare sensor data
      std::stringstream sensorDataStream;

      sensorDataStream << "sensor_mac=" << WiFi.macAddress().c_str();
      sensorDataStream << "&sensor_connection_type=WIFI"; 
      sensorDataStream << "&sensor_type=" << SENSOR_TYPE; 
      sensorDataStream << "&sensor_unit=" << SENSOR_UNIT; 
      sensorDataStream << "&sensor_sleep_time_sec=" << SENSOR_SLEEP_TIME_SEC;
      sensorDataStream << "&sensor_ip=" << WiFi.localIP().toString().c_str();
      sensorDataStream << "&sensor_data=" << data;
      sensorDataStream << "&sensor_voltage=" << VBAT;

      Serial.print("POSTDATA: ");
      Serial.println(sensorDataStream.str().c_str());

      // Make a HTTP request:
      HTTPClient http;
      http.begin("https://<target remote server url>/sensor/indata");      // change this to your needs
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      http.addHeader("X-Auth", UUID_CLIENT);
      http.POST(sensorDataStream.str().c_str()); // sensor data
      http.writeToStream(&Serial);
      http.end();
    }
  }
}

void blink() {
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);                       // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(500);
}

void flashy() {
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(100);
}


