#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <FastLED.h>

#define NUM_LEDS 1
#define BRIGHTNESS 10 //max brightness is 10

#define DATA_PIN D3
#define CLOCK_PIN 13

CRGB leds[NUM_LEDS]; //needed crgb to get the colors working correctly

const char* ssid = "ssid"; 
const char* password = "password";
char len;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

void setup() {
  //FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);

  Serial.begin(115200); 

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  
  while(WiFi.status() != WL_CONNECTED) {
    leds[0] = CRGB::Black;
    FastLED.show();  
    delay(500);
    Serial.print(".");
    leds[0] = CRGB::Yellow; //tried to blink yellow while wifi connects but doesn't work right.  light does blink though
    FastLED.show();
  }
  
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {

  // Send an HTTP POST request depending on timerDelay
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      String serverPath = "plex sessions call url"; //here you will paste the session call that checks if users are watching, we will grab this from the plex dashboard page
      
      http.begin(client, serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
       if (httpResponseCode==200) {
 
         String payload = http.getString();
         Serial.println(payload.length());
         
         if (payload.length() > 100){ //to lazy to parse xml so i check if its a string longer than 100 characters to display the green in use light
          leds[0] = CRGB::Black;
          FastLED.show();
          leds[0] = CRGB::Green;
          FastLED.show();   
         }
         else if(payload.length() < 100){ //if less than 100 characters display blue light
          leds[0] = CRGB::Black;
          FastLED.show();
          leds[0] = CRGB::Blue;
          FastLED.show();        
         }
     
       }
       else {
         Serial.println("ERROR"); //if there is an error or plex is down red will be displayed
         leds[0] = CRGB::Black;
         FastLED.show();
         leds[0] = CRGB::Red;
         FastLED.show();
       }
      
       
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected"); //if wifi is disconnected red will also be displayed
      leds[0] = CRGB::Black;
      FastLED.show();
      leds[0] = CRGB::Red;
      FastLED.show();
    }
    lastTime = millis();
  }
}
