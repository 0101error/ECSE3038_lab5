#include <Arduino.h>       // Core Arduino functions like setup() and loop()
#include <WiFi.h>          // For Wi-Fi connection
#include <HTTPClient.h>    // For making HTTP requests to the API
#include <ArduinoJson.h>   // For parsing JSON data
#include <LiquidCrystal_I2C.h> // For the I2C LCD
#include "env.h"           // For our secret variables (SSID, PASS, API_KEY)

// Define LCD parameters - you might need to adjust the I2C address
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 columns, 2 rows. Check your LCD's address!

String apiKeyHeader = "api-key"; // The name of the API key header
String messageEndpoint = "/message"; // The endpoint to request the message from
String serverPath = "https://" + String(ENDPOINT) + messageEndpoint; // Full API URL

void setup() {
    Serial.begin(115200); // Start serial communication for debugging

    // Initialize LCD
    lcd.init();
    lcd.backlight();
    lcd.print("Connecting WiFi...");

    // Connect to Wi-Fi
    if (IS_WOKWI) {
        WiFi.begin(SSID, PASS, CHANNEL); // For Wokwi, use channel
    } else {
        WiFi.begin(SSID, PASS);          // For real ESP32, no channel needed
    }

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    lcd.clear();
    lcd.print("WiFi Connected!");
    delay(2000); // Wait for 2 seconds to show "WiFi Connected!" before fetching message
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) { // Only do this if Wi-Fi is connected
        HTTPClient http;                // Declare an HTTPClient object
        http.begin(serverPath.c_str()); // Specify the URL to request
        http.addHeader(apiKeyHeader.c_str(), API_KEY); // Add the API key header

        Serial.print("Making API request to: ");
        Serial.println(serverPath);

        int httpResponseCode = http.GET(); // Send the GET request

        if (httpResponseCode > 0) { //Check for a successful response
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);

            if (httpResponseCode == HTTP_CODE_OK) { // HTTP_CODE_OK is 200
                String payload = http.getString(); // Get the response payload (the JSON data)
                Serial.println("Payload received:\n<<");
                Serial.println(payload);
                Serial.println(">>");

                DynamicJsonDocument doc(1024); // Create a JSON document
                DeserializationError error = deserializeJson(doc, payload); // Parse the JSON

                if (error) { // If JSON parsing failed
                    Serial.print("JSON Deserialization failed: ");
                    Serial.println(error.c_str());
                    lcd.clear();
                    lcd.print("JSON Error");
                } else { // JSON parsing successful
                    String line1 = doc["line_1"]; // Extract "line_1" value
                    String line2 = doc["line_2"]; // Extract "line_2" value

                    Serial.print("Line 1: "); Serial.println(line1);
                    Serial.print("Line 2: "); Serial.println(line2);

                    lcd.clear();             // Clear the LCD
                    lcd.setCursor(0, 0);    // Set cursor to first line, first column
                    lcd.print(line1);        // Print line 1
                    lcd.setCursor(0, 1);    // Set cursor to second line, first column
                    lcd.print(line2);        // Print line 2
                }
            } else { // HTTP response was not OK (not 200)
                Serial.println("HTTP request failed");
                lcd.clear();
                lcd.print("API Error:");
                lcd.setCursor(0, 1);
                lcd.print(httpResponseCode); // Show the error code on LCD
            }
        } else { // HTTP response code was less than 0 (connection error)
            Serial.println("HTTP request failed - connection error");
            lcd.clear();
            lcd.print("Conn. Error");
        }
        http.end(); // Close the HTTP connection
    } else { // Wi-Fi is not connected
        Serial.println("WiFi Disconnected");
        lcd.clear();
        lcd.print("WiFi Disconnected");
    }
    delay(10000); // Wait for 10 seconds before making the next API request
}
