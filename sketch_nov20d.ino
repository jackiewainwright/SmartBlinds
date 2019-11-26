#include <printf.h>
#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24.h>
#include <dht.h>
dht DHT;
#define DHT11_PIN 5






RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00011";
     float  UNO_Data[8];

void setup() {
 Serial.begin(9600);
 radio.begin();
  radio.openReadingPipe(0,address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}
void loop() {
  if ( radio.available()){ 
     radio.read(&UNO_Data, sizeof(UNO_Data));
        Serial.print("transmitted data: \n"); 
   Serial.print(UNO_Data[0]); Serial.print(" "); Serial.print(UNO_Data[1]); Serial.print(" ");
   Serial.print(UNO_Data[2]); Serial.print(" "); Serial.print(UNO_Data[3]); Serial.print(" ");
   Serial.print(UNO_Data[4]); Serial.print(" "); Serial.print(UNO_Data[5]); Serial.print(" ");
   Serial.print(UNO_Data[6]); Serial.print(" "); Serial.print(UNO_Data[7]); Serial.print(" ");
   Serial.print("\n");
  
   delay(2000);
  }

}
