#define BLYNK_PRINT Serial
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

static const int RXPin = 5, TXPin = 4;  // RXpin ada di GPIO5, TXpin ada di GPIO4
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps; // The TinyGPS++ object
WidgetMap myMap(V0);  // V0 for virtual pin of Map Widget
SoftwareSerial ss(RXPin, TXPin);  // The serial connection to the GPS device
BlynkTimer timer;

float spd;       // variabel untuk menyimpan nilai speed
String bearing;  // variabel untuk menyimpan nilai arah/direction GPS
unsigned int move_index = 1;       // fixed location for now

char auth[] = "3AeN8al6t19PMffdMa_W900X3UViz_ty";              //Your Project authentication key
char ssid[] = "Mamas";                                       // Name of your network (HotSpot or Router name)
char pass[] = "mamasadi";                                      // Corresponding Password

void setup()
{
    Serial.begin(115200);
    Serial.println();
    ss.begin(GPSBaud);
    Blynk.begin(auth, ssid, pass);
    timer.setInterval(1000L, checkGPS); // every 1s check if GPS is connected, only really needs to be done once
}

void errorMessage(String message)
{
     Serial.println(message);
     Blynk.virtualWrite(V1, "-----");
     Blynk.virtualWrite(V2, "-----");
     Blynk.virtualWrite(V3, "-----");
     Blynk.virtualWrite(V5, "-----");
}

void checkGPS()
{
    if (gps.charsProcessed() < 10)
    {
       errorMessage("No GPS detected: check wiring.");
       Blynk.virtualWrite(V4, "GPS ERROR");  // Value Display widget  on V4 if GPS not detected
    }
}

void displayInfo()
{ 
    if (gps.location.isValid() ) 
    {
      float latitude = (gps.location.lat());     //Storing the Lat. and Lon. 
      float longitude = (gps.location.lng()); 
      spd = gps.speed.knots();   // get speed value
      bearing = TinyGPSPlus::cardinal(gps.course.value()); // get the direction
      myMap.location(move_index, latitude, longitude, "Your Vehicle Location");
      
      Serial.print("LAT:  ");
      Serial.println(latitude, 6);  // float to x decimal places
      Serial.print("LONG: ");
      Serial.println(longitude, 6);
      
      Blynk.virtualWrite(V1, String(latitude, 6));   
      Blynk.virtualWrite(V2, String(longitude, 6));  
      Blynk.virtualWrite(V3, spd);
      Blynk.virtualWrite(V4, "Connected");
      Blynk.virtualWrite(V5, bearing);               
    }
    else
    {
      errorMessage("NEO-6M GPS Module connecting to satellite");
      Blynk.virtualWrite(V4, "Connecting"); delay(500);
      Blynk.virtualWrite(V4, "Connecting ."); delay(500);
      Blynk.virtualWrite(V4, "Connecting . ."); delay(500);
      Blynk.virtualWrite(V4, "Connecting . . ."); delay(500);
      Blynk.virtualWrite(V4, "Connecting . . . ."); delay(1500);
    }
     
    Serial.println();
}

void loop()
{
    while (ss.available() > 0) 
    {
        // sketch displays information every time a new sentence is correctly encoded.
        if (gps.encode(ss.read()))
            displayInfo();
    }

    Blynk.run();
    timer.run();
}
