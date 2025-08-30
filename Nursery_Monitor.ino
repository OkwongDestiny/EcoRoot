#include <WiFi.h>//

#include <HTTPClient.h>//

#include <Adafruit_GFX.h>

#include <Adafruit_SSD1306.h>

#include <Wire.h>

#include <DHT.h>

#define TYPE DHT11

#define SCREEN_WIDTH 128

#define SCREEN_HEIGHT 64

#define OLED_RESET -1

const char* ssid = ""; // YOUR SSID GOES HERE

const char* password = ""; // your password 

String apiKey = ""; //put yours here

String apiKey2 = "";//put yours here

String phoneNumber = ""; //put yours

const char* server = "http://api.thingspeak.com/update";

int dhtPin = 15;

int lightPin = 32;

int waterPin = 33;

int lightLevel;

int waterLevel;

float tempC;

float humidity;

unsigned long lastWhatsAppMessageTime = 0; 

unsigned long messageCooldown = 180000; // 5 minutes in milliseconds

DHT HT(dhtPin, TYPE);

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


String getWaterLevel(int value) {

if (value < 100) return "CRITICAL";

else if (value < 800) return "LOW";

else if (value < 1800) return "MID";

else return "HIGH";

}

String getLightLevel(int value) {

if (value < 100) return "CRITICAL";

else if (value < 1000) return "LOW";

else if (value < 3000) return "MID";

else return "HIGH";

}

int getValueFromString(String level) {

if (level.equalsIgnoreCase("low")) return 2;

else if (level.equalsIgnoreCase("mid")) return 3;

else if (level.equalsIgnoreCase("high")) return 4;

else if (level.equalsIgnoreCase("critical")) return 1;

else return 0; // default for invalid input

}


void showMessage(String msg, int textSize = 1, int x = 0, int y = 0) {

oled.clearDisplay();

oled.setTextSize(textSize);

oled.setTextColor(SSD1306_WHITE);

oled.setCursor(x, y);

oled.println(msg);

oled.display();


}



void sendWhatsAppMessage(String msg) {

if (WiFi.status() == WL_CONNECTED) {

HTTPClient http;

msg.replace(" ", "%20"); // Encode spaces for URL

String url = "http://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&text=" + msg + "&apikey=" + apiKey2;


http.begin(url);

int httpCode = http.GET();


if (httpCode > 0) {

Serial.println("Message sent successfully!");

showMessage("Message sent");

oled.setCursor(0, 10);

oled.print("successfully");

oled.display();

} else {

Serial.println("Error sending message.");

showMessage("Error sending");

oled.setCursor(0, 10);

oled.print("message");

oled.display();

}

http.end();

} else {

Serial.println("WiFi not connected.");

}

}


void sendCallAlert(String message) {

if (WiFi.status() == WL_CONNECTED) {

HTTPClient http;

String encodedMsg = message;

encodedMsg.replace(" ", "+"); // URL encode spaces

String url = "http://api.callmebot.com/call.php?phone=" + phoneNumber + "&text=" + encodedMsg + "&apikey=" + apiKey2;

http.begin(url);

int httpCode = http.GET();

if (httpCode > 0) {

Serial.println("Call alert sent successfully!");

} else {

Serial.println("Error sending call alert.");

}

http.end();

} else {

Serial.println("WiFi not connected.");

}

}




void setup() {


// put your setup code here, to run once:

Serial.begin(115200);

Serial.println("Hello, ESP32!");

HT.begin();

pinMode(dhtPin, INPUT);

pinMode(waterPin, INPUT);

pinMode(lightPin, INPUT);

if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3c)) {

Serial.println("Oled not found");

for (;;);

}


showMessage("INITIALISED");

delay(2000);

oled.clearDisplay();

oled.display();

delay(1000);

WiFi.begin("Wokwi-GUEST", "", 6); //WiFi.begin(ssid, password); //

Serial.print("Connecting to WiFi");


while (WiFi.status() != WL_CONNECTED) {

delay(1000);

showMessage("CONNECTING TO WIFI...");

delay(1000);

showMessage("");

Serial.print(".");


}


Serial.println("\nConnected!");

showMessage("CONNECTED TO WIFI");

delay(3000);

}



void loop() {

tempC = HT.readTemperature();

humidity = HT.readHumidity();

lightLevel = analogRead(lightPin);

waterLevel = analogRead(waterPin);

String waterRange = getWaterLevel(waterLevel);

String lightRange = getLightLevel(lightLevel);

int waterRangeVal = getValueFromString(waterRange);

int lightRangeVal = getValueFromString(lightRange);

Serial.println(tempC);

Serial.println(humidity);

Serial.println(lightLevel);

Serial.println(waterLevel);

Serial.println(waterRange);

Serial.println(lightRange);

Serial.println(waterRangeVal);

Serial.println(lightRangeVal);

showMessage("Temp Celsius:");

oled.setCursor(0, 10);

oled.print(tempC);

oled.setCursor(0, 20);

oled.print("Humidity %:");

oled.setCursor(0, 30);

oled.print(humidity);

oled.display();

delay(3000);

showMessage("Water Level");

oled.setCursor(0, 10);

oled.print(waterRange);

oled.setCursor(0, 20);

oled.print("Light Level");

oled.setCursor(0, 30);

oled.print(lightRange);

oled.display();

delay(3000);

showMessage("");



if ((millis() - lastWhatsAppMessageTime >= messageCooldown) || (lastWhatsAppMessageTime == 0) ) { 

if (waterRange.equalsIgnoreCase("critical")) {

// sendCallAlert("Water level critical in nursery");

sendWhatsAppMessage("Water level critical in nursery");

lastWhatsAppMessageTime = millis();

}

if (lightRange.equalsIgnoreCase("critical")) {

//sendCallAlert("Light level critical in nursery");

sendWhatsAppMessage("Light level is critical in nursery");

lastWhatsAppMessageTime = millis();

}

} 

Serial.println(lastWhatsAppMessageTime);

if (WiFi.status() == WL_CONNECTED) {


HTTPClient http;



// Construct URL with data

String url = server;

url += "?api_key=" + apiKey;

url += "&field1=" + String(tempC);

url += "&field2=" + String(humidity);

url += "&field3=" + String(lightRangeVal);

url += "&field4=" + String(waterRangeVal);

http.begin(url);

int httpCode = http.GET();



if (httpCode > 0) {

Serial.println("Data sent to ThingSpeak!");

showMessage("Data sent to");

oled.setCursor(0, 10);

oled.print("ThingSpeak");

oled.display();

} else {

Serial.println("Error sending data");

showMessage("Error sending data");


}

http.end();


}

delay(15000);

}




