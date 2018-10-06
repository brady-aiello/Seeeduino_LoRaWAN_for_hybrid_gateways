//#include "LoRaWan.h"
#include<LoRaWan.h>
#include "keys.h"

#define FREQ_RX_WNDW_SCND_US  923.3
/*#define FREQ_RX_WNDW_SCND_EU  869.525
#define FREQ_RX_WNDW_SCND_AU  923.3
#define FREQ_RX_WNDW_SCND_CN  505.3
#define FREQ_RX_WNDW_SCND_KR  923.3
#define FREQ_RX_WNDW_SCND_IN  866.55
#define FREQ_RX_WNDW_SCND_AS1 923.3
#define FREQ_RX_WNDW_SCND_AS2 923.3*/

const float US_hybrid_channels[8] = {903.9, 904.1, 904.3, 904.5, 904.7, 904.9, 905.1, 905.3}; //rx 923.3
/*const float EU_hybrid_channels[8] = {868.1, 868.3, 868.5, 867.1, 867.3, 867.5, 867.7, 867.9}; //rx 869.525
const float AU_hybrid_channels[8] = {916.8, 917.0, 917.2, 917.4, 917.6, 917.8, 918.0, 918.2}; //rx 923.3
const float CN_hybrid_channels[8] = {487.1, 487.3, 487.5, 487.7, 486.3, 486.5, 486.7, 486.9}; //rx 505.3 
const float KR_hybrid_channels[8] = {922.1, 922.3, 922.5, 922.7, 922.9, 923.1, 923.3, 0};     //rx 921.9 
const float IN_hybrid_channels[8] = {865.0625, 865.4025, 865.9850, 0, 0, 0, 0, 0};            //rx 866.55
const float AS1_hybrid_channels[8] = {923.2, 923.4, 922.2, 922.4, 922.6, 922.8, 923.0, 922.1}; //rx 923.2
const float AS2_hybrid_channels[8] = {923.2, 923.4, 923.6, 923.8, 924.0, 924.2, 924.4, 924.6}; //rx 923.2*/

//United States Receive Window Data Rate = DR8
#define DOWNLINK_DATA_RATE_US DR8
/*#define DOWNLINK_DATA_RATE_EU DR8
#define DOWNLINK_DATA_RATE_AU DR8
#define DOWNLINK_DATA_RATE_CN DR0
#define DOWNLINK_DATA_RATE_KR DR0
#define DOWNLINK_DATA_RATE_IN DR2
#define DOWNLINK_DATA_RATE_AS1 DR2
#define DOWNLINK_DATA_RATE_AS2 DR2*/

//United States max data rate for uplink channels = DR3
#define UPLINK_DATA_RATE_MAX_US  DR3
/*#define UPLINK_DATA_RATE_MAX_EU  DR5
#define UPLINK_DATA_RATE_MAX_AU  DR3
#define UPLINK_DATA_RATE_MAX_CN  DR5
#define UPLINK_DATA_RATE_MAX_KR  DR5
#define UPLINK_DATA_RATE_MAX_IN  DR5
#define UPLINK_DATA_RATE_MAX_AS1 DR5
#define UPLINK_DATA_RATE_MAX_AS2 DR5*/

#define MAX_EIRP_NDX_US 13
/*#define MAX_EIRP_NDX_EU  2
#define MAX_EIRP_NDX_AU 13
#define MAX_EIRP_NDX_CN  7
#define MAX_EIRP_NDX_KR  4
#define MAX_EIRP_NDX_IN 13
#define MAX_EIRP_NDX AS1 5
#define MAX_EIRP_NDX_AS2 5*/

//The min uplink data rate for all countries / plans is DR0
#define UPLINK_DATA_RATE_MIN DR0

#define DEFAULT_RESPONSE_TIMEOUT 5
unsigned char frame_counter = 1;
int loopcount = 0;

char buffer[256];


void setup(void)
{
    SerialUSB.begin(115200);
    while(!SerialUSB);     
    
    lora.init();
    lora.setDeviceReset();
  
    memset(buffer, 0, 256);
    lora.getVersion(buffer, 256, 1);
    SerialUSB.print(buffer); 
       
    memset(buffer, 0, 256);
    lora.getId(buffer, 256, 1);
    SerialUSB.print(buffer);

    lora.setId(NULL, DEV_EUI, APP_EUI);
    lora.setKey(NULL, NULL, APP_KEY);
    
    lora.setDeciveMode(LWOTAA);
    lora.setDataRate(DR0, US915HYBRID);
    //lora.setAdaptiveDataRate(true); 
    setHybridForTTN(US_hybrid_channels);
    
    lora.setReceiceWindowFirst(0,  US_hybrid_channels[0]);
    lora.setReceiceWindowSecond(FREQ_RX_WNDW_SCND_US, DOWNLINK_DATA_RATE_US);

    lora.setDutyCycle(false);
    lora.setJoinDutyCycle(false);
    lora.setPower(MAX_EIRP_NDX_US);

//        SerialUSB.print("Starting OTTA Join.\n");
//    loopcount = 0;
//    while(true) {
//        loopcount++;
//        if (lora.setOTAAJoin(JOIN))
//            break;
//    }
//    SerialUSB.print("Took ");
//    SerialUSB.print(loopcount);
//    SerialUSB.println(" tries to join.");
    while(!lora.setOTAAJoin(JOIN));
    //lora.loraDebug();
}

void setHybridForTTN(const float* channels){
    for(int i = 0; i < 8; i++){
        if(channels[i] != 0){
            lora.setChannel(i, channels[i], UPLINK_DATA_RATE_MIN, UPLINK_DATA_RATE_MAX_US);
         
        }
    }
}

void loop(void)
{
    while(!lora.setOTAAJoin(JOIN));
    bool result = lora.transferPacket(&frame_counter, 1, DEFAULT_RESPONSE_TIMEOUT);

    if(result)
    {
        delay(50);
        frame_counter++;
        short length;
        short rssi;
        
        memset(buffer, 0, 256);
        length = lora.receivePacket(buffer, 256, &rssi);
        
        if(length)
        {
            SerialUSB.print("Length is: ");
            SerialUSB.println(length);
            SerialUSB.print("RSSI is: ");
            SerialUSB.println(rssi);
            SerialUSB.print("Data is: ");
            for(unsigned char i = 0; i < length; i ++)
            {
                SerialUSB.print("0x");
                SerialUSB.print(buffer[i], HEX);
                SerialUSB.print(" ");
            }
            SerialUSB.println();
        }
    }
  
    lora.loraDebug();
    delay(1000);
}
