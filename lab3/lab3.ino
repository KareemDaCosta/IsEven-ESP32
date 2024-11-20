/**************************************************************************
HTTP Request Example
Fetch a random color palette from colormind.io and draw the colors on the display

To fetch a new color, press either button on the LILYGO (GPIO 0 or 35)     
**************************************************************************/
#include "TFT_eSPI.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

// TODO: replace with your own SSID & Password
const char* ssid = "Columbia University";
const char* password = "";

#define BUTTON_LEFT 0
#define BUTTON_RIGHT 35

volatile bool leftButtonPressed = false;
volatile bool rightButtonPressed = false;

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  
  // setup our display
  tft.init();
  tft.setRotation(1);  // 1 = landscape, 2 = portrait
  tft.fillScreen(TFT_BLACK);

  Serial.print("display dimensions: ");
  Serial.print(tft.width());
  Serial.print(" x ");
  Serial.println(tft.height());

  // connect to wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // Send the HTTP POST request
  if (WiFi.status() == WL_CONNECTED) {
    fetchColors();
  } else {
    Serial.println("WiFi not connected");
  }

  // setup our buttons
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_LEFT), pressedLeftButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_RIGHT), pressedRightButton, FALLING);
}

void fetchColors() {
    HTTPClient http;

    int number = random(10000);
    String url = "https://api.isevenapi.xyz/api/iseven/" + String(number);
    http.begin(url);

    // Set the content type to JSON
    http.addHeader("Content-Type", "application/json");
    
    // Send the request
    int httpResponseCode = http.GET();

    // Check the response
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(response);
      JSONVar responseJSON = JSON.parse(response);

      if (JSON.typeof(responseJSON) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }

      Serial.println(responseJSON);
      String output_str_1 = String(number) + " is";
      String output_str_2;
      if(JSON.stringify(responseJSON["iseven"]) == "false") {
        output_str_2 = "not even!";
      }
      else {
        output_str_2 = "even!" ;
      }

      tft.fillScreen(TFT_BLACK);
      tft.setTextSize(3);
      tft.setTextColor(TFT_BLUE, TFT_BLACK);
      tft.drawString(output_str_1, 0, 0);
      tft.drawString(output_str_2, 0, 50);
    } else {
      Serial.println("Error on sending GET request");
    }

    // Free resources
    http.end();
}

void pressedLeftButton() {
  leftButtonPressed = true;
}

void pressedRightButton() {
  rightButtonPressed = true;
}

void loop() {
  // fetch colors when either button is pressed
  if (leftButtonPressed) {
    fetchColors();
    leftButtonPressed = false;
  }
  if (rightButtonPressed) {
    fetchColors();
    rightButtonPressed = false;
  }
}
