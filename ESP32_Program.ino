#include "WiFi.h"
#include <HTTPClient.h>

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const char* ssid ="Linksys23436";
const char* password="44qqx8hbxd";

RF24 radio(5,4); //CE , CSN
const byte address[6] = "10101";

float esp32_data[4];
String payload;
void setup()


{
    delay(1000);
    WiFi.mode(WIFI_OFF);    
    Serial.begin(115200);
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid,password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to Wifi....");
    }
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    //WiFi.mode(WIFI_STA);
    //WiFi.disconnect();
    //delay(100);
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Setup done");

    radio.begin();
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MIN);
    radio.startListening();
}

void loop()
{
    Serial.println("scan start");

    if ((WiFi.status() == WL_CONNECTED)) {
      HTTPClient http;
      http.begin("http://18.219.209.163:3000/schedule/getdata");
      //http.begin("http://18.219.209.163:3000/light");
     
      int httpCode = http.GET();
      if (httpCode > 0) {
        payload = http.getString();
        //Serial.println(httpCode);
        Serial.println(payload);
      }
      else {
        Serial.println("Error  on HTTP request");
      }
      http.end();
    }
    
    // Wait a bit before scanning again

    if (payload == "\"{temperature, DOWN}\""){
      esp32_data[0] = 1; esp32_data[1] = 0; esp32_data[2] = 0; esp32_data[3] = 1;
    }
    else if (payload == "\"{temperature, UP}\""){
      esp32_data[0] = 1; esp32_data[1] = 0; esp32_data[2] = 0; esp32_data[3] = 0;
    }
    else if (payload == "\"{light, DOWN}\""){
      esp32_data[0] = 0; esp32_data[1] = 1; esp32_data[2] = 0; esp32_data[3] = 1;
    }
    else if (payload == "\"{light, UP}\""){
      esp32_data[0] = 0; esp32_data[1] = 1; esp32_data[2] = 0; esp32_data[3] = 0;
    }
    else if (payload == "\"{schedule, DOWN}\""){
      esp32_data[0] = 0; esp32_data[1] = 0; esp32_data[2] = 1; esp32_data[3] = 1;
    }
    else if (payload == "\"{schedule, UP}\""){
      esp32_data[0] = 0; esp32_data[1] = 0; esp32_data[2] = 1; esp32_data[3] = 0;
    }
    
    Serial.print(esp32_data[0]); Serial.print(" ");
    Serial.print(esp32_data[1]); Serial.print(" ");
    Serial.print(esp32_data[2]); Serial.print(" ");
    Serial.print(esp32_data[3]); Serial.print("\n");

    radio.stopListening();
    radio.write(&esp32_data, sizeof(esp32_data));

    Serial.println("");
    
    delay(5000);
}
