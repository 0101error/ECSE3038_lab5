#include <Arduino.h>       
#include <WiFi.h>          
#include <HTTPClient.h>   
#include <ArduinoJson.h>  
#include <LiquidCrystal_I2C.h> 
#include "env.h"          


LiquidCrystal_I2C lcd(0x27, 16, 2); 

String apiKeyHeader = "api-key";
String messageEndpoint = "/message"; 
String serverPath = "https://" + String(ENDPOINT) + messageEndpoint;

void setup() {
    Serial.begin(115200); 

   
    lcd.init();
    lcd.backlight();
    lcd.print("Connecting WiFi...");

    
    if (IS_WOKWI) {
        WiFi.begin(SSID, PASS, CHANNEL); 
    } else {
        WiFi.begin(SSID, PASS);          
    }

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    lcd.clear();
    lcd.print("WiFi Connected!");
    delay(2000); 
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) { 
        HTTPClient http;               
        http.begin(serverPath.c_str()); 
        http.addHeader(apiKeyHeader.c_str(), API_KEY); 

        Serial.print("Making API request to: ");
        Serial.println(serverPath);

        int httpResponseCode = http.GET(); 

        if (httpResponseCode > 0) { 
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);

            if (httpResponseCode == HTTP_CODE_OK) { 
                String payload = http.getString(); 
                Serial.println("Payload received:\n<<");
                Serial.println(payload);
                Serial.println(">>");

                DynamicJsonDocument doc(1024); 
                DeserializationError error = deserializeJson(doc, payload); 

                if (error) { 
                    Serial.print("JSON Deserialization failed: ");
                    Serial.println(error.c_str());
                    lcd.clear();
                    lcd.print("JSON Error");
                } else { 
                    String line1 = doc["line_1"]; 
                    String line2 = doc["line_2"]; 

                    Serial.print("Line 1: "); Serial.println(line1);
                    Serial.print("Line 2: "); Serial.println(line2);

                    lcd.clear();            
                    lcd.setCursor(0, 0);    
                    lcd.print(line1);       
                    lcd.setCursor(0, 1);    
                    lcd.print(line2);       
                }
            } else { 
                Serial.println("HTTP request failed");
                lcd.clear();
                lcd.print("API Error:");
                lcd.setCursor(0, 1);
                lcd.print(httpResponseCode);
            }
        } else { 
            Serial.println("HTTP request failed - connection error");
            lcd.clear();
            lcd.print("Conn. Error");
        }
        http.end(); 
    } else { 
        Serial.println("WiFi Disconnected");
        lcd.clear();
        lcd.print("WiFi Disconnected");
    }
    delay(10000); 
}
