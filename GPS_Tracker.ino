#define BLYNK_PRINT Serial

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

static const int RXPin = 5, TXPin = 4;
static const uint32_t GPSBaud = 9600; //if Baud rate 9600 didn't work in your case then use 4800

TinyGPSPlus gps; // The TinyGPS++ object
WidgetMap myMap(V0);  // V0 for virtual pin of Map Widget

SoftwareSerial ss(RXPin, TXPin);  // The serial connection to the GPS device

BlynkTimer timer;

float spd;       //Variable  to store the speed
String bearing;  //Variable to store orientation or direction of GPS

char auth[] = "3AeN8al6t19PMffdMa_W900X3UViz_ty";              //Your Project authentication key
char ssid[] = "Mamas";                                       // Name of your network (HotSpot or Router name)
char pass[] = "mamasadi";                                      // Corresponding Password

unsigned int move_index = 1;       // fixed location for now

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

    if (!Blynk.connected())
    {
        Blynk.virtualWrite(V4, "Missing Connection");
    }
}

void displayInfo()
{ 
    if (gps.location.isValid() ) 
    {
      float latitude = (gps.location.lat());     //Storing the Lat. and Lon. 
      float longitude = (gps.location.lng()); 
      
      Serial.print("LAT:  ");
      Serial.println(latitude, 6);  // float to x decimal places
      Serial.print("LONG: ");
      Serial.println(longitude, 6);
      Blynk.virtualWrite(V1, String(latitude, 6));   
      Blynk.virtualWrite(V2, String(longitude, 6));  
      myMap.location(move_index, latitude, longitude, "Your Vehicle Location");
      spd = gps.speed.knots();               //get speed
      Blynk.virtualWrite(V3, spd);
         
      Blynk.virtualWrite(V4, "Connected");
  
      bearing = TinyGPSPlus::cardinal(gps.course.value()); // get the direction
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
