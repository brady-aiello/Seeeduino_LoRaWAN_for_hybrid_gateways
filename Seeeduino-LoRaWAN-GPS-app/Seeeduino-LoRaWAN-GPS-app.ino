#include <TinyGPS++.h>
#include <TimerTCC0.h>
#include<LoRaWan.h>
#include "keys.h"

#define FREQ_RX_WNDW_SCND_US  923.3
const float US_hybrid_channels[8] = {903.9, 904.1, 904.3, 904.5, 904.7, 904.9, 905.1, 905.3}; //rx 923.3
#define DOWNLINK_DATA_RATE_US DR8
#define UPLINK_DATA_RATE_MAX_US  DR3
#define MAX_EIRP_NDX_US 13
#define UPLINK_DATA_RATE_MIN DR0
#define DEFAULT_RESPONSE_TIMEOUT 5

/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
char buffer[128] = {0};
int sec = 0;

// The TinyGPS++ object
TinyGPSPlus gps;
typedef union {
    float f[2];         // Assigning fVal.f will also populate fVal.bytes;
    unsigned char bytes[8];   // Both fVal.f and fVal.bytes share the same 4 bytes of memory.
} floatArr2Val;

floatArr2Val latlong;

float latitude;
float longitude;

void setHybridForTTN(const float* channels){
    for(int i = 0; i < 8; i++){
        if(channels[i] != 0){
            lora.setChannel(i, channels[i], UPLINK_DATA_RATE_MIN, UPLINK_DATA_RATE_MAX_US);        
        }
    }
}

void displayInfo()
{
    if(latitude){
      SerialUSB.println("Got real lat");
    }
    if(longitude){
      SerialUSB.println("Got real lon");
    }
    SerialUSB.print(F("Location: ")); 
    SerialUSB.print(latlong.f[0], 6);
    //SerialUSB.print(latitude, 6);
    SerialUSB.print(F(","));
    SerialUSB.print(latlong.f[1], 6);
    //SerialUSB.print(longitude, 6);
    SerialUSB.println();
}

void timerIsr(void)  //interrupt routine
{
    sec = (sec + 1) % 6;   
    SerialUSB.println(sec);
    displayInfo();
}

void setupLoRaABP(){  
    lora.init();
    lora.setDeviceDefault();
  
    memset(buffer, 0, 256);
    lora.getVersion(buffer, 256, 1);
    SerialUSB.print(buffer); 
       
    memset(buffer, 0, 256);
    lora.getId(buffer, 256, 1);
    SerialUSB.print(buffer);
    
    lora.setId(DEV_ADDR, DEV_EUI, APP_EUI);
    lora.setKey(NWK_S_KEY, APP_S_KEY, APP_KEY);
    
    lora.setDeciveMode(LWABP);
    lora.setDataRate(DR0, US915HYBRID);
    lora.setPower(MAX_EIRP_NDX_US);
    setHybridForTTN(US_hybrid_channels);
    lora.setReceiceWindowFirst(1);
    //lora.setReceiceWindowFirst(0,  US_hybrid_channels[0]);
    lora.setReceiceWindowSecond(FREQ_RX_WNDW_SCND_US, DOWNLINK_DATA_RATE_US);
}

void setupLoRaOTAA(){
    
    lora.setDeviceReset();
    lora.getVersion(buffer, 256, 1);
    SerialUSB.print(buffer); 
       
    memset(buffer, 0, 256);
    lora.getId(buffer, 256, 1);
    SerialUSB.print(buffer);
    lora.setId(NULL, DEV_EUI, APP_EUI);
    lora.setKey(NULL, NULL, APP_KEY);
    lora.setDeciveMode(LWOTAA);
    lora.setDataRate(DR0, US915HYBRID);
    setHybridForTTN(US_hybrid_channels);
    //lora.setReceiceWindowFirst(1);
    lora.setReceiceWindowFirst(0,  US_hybrid_channels[0]);
    lora.setReceiceWindowSecond(FREQ_RX_WNDW_SCND_US, DOWNLINK_DATA_RATE_US);

    lora.setDutyCycle(false);
    lora.setJoinDutyCycle(true);
    lora.setPower(MAX_EIRP_NDX_US);
    while(!lora.setOTAAJoin(JOIN));
    lora.loraDebug();
}

void setup()
{
    Serial.begin(9600);
    SerialUSB.begin(115200);
    
    memset(buffer, 0, 256);
    setupLoRaABP();
    //setupLoRaOTAA();
  //TimerTcc0.initialize(60000000); 1 Minute
    TimerTcc0.initialize(10000000); //10 seconds
    TimerTcc0.attachInterrupt(timerIsr);
}

void loop()
{
  
//    if (sec == 3){
//      //Serial.write("h"); //Turn on GPS
//    }
    if (sec <= 2 ) {
        while (Serial.available() > 0){
            char currChar = Serial.read();
            SerialUSB.print(currChar);
            gps.encode(currChar);
        }
        latitude  = gps.location.lat();
        longitude = gps.location.lng();
        if((latitude && longitude) && latitude != latlong.f[0]
            && longitude != latlong.f[1]){     
            latlong.f[0] = latitude;
            latlong.f[1] = longitude;
            
            SerialUSB.println("LatLong: ");
            for(int i = 0; i < 8; i++){
                SerialUSB.print(latlong.bytes[i], HEX);
            }
            SerialUSB.println("");
            bool result = lora.transferPacket(latlong.bytes, 8, DEFAULT_RESPONSE_TIMEOUT);
        }
    }
//    else if (sec == 5 ){
//        //Serial.write("$PMTK161,0*28\r\n"); //Put GPS to sleep
//    }
}
