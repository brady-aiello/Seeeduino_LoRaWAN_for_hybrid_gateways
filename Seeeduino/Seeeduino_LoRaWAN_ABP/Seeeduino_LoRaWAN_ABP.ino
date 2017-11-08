#include "LoRaWan.h"
#include "keys.h"

#define US_FREQ_RX_WNDW_SCND 923.3
//#define EU_FREQ_RX_WNDW_SCND 869.525
//#define AU_FREQ_RX_WNDW_SCND 923.3
//#define CN_FREQ_RX_WNDW_SCND 505.3
//#define KR_FREQ_RX_WNDW_SCND 923.3
//#define IN_FREQ_RX_WNDW_SCND 866.55
//#define AS1_FREQ_RX_WNDW_SCND 923.3
//#define AS2_FREQ_RX_WNDW_SCND 923.3

const float US_hybrid_channels[8] = {903.9, 904.1, 904.3, 904.5, 904.7, 904.9, 905.1, 905.3}; //rx 923.3
//const float EU_hybrid_channels[8] = {868.1, 868.3, 868.5, 867.1, 867.3, 867.5, 867.7, 867.9}; //rx 869.525
//const float AU_hybrid_channels[8] = {916.8, 917.0, 917.2, 917.4, 917.6, 917.8, 918.0, 918.2}; //rx 923.3
//const float CN_hybrid_channels[8] = {487.1, 487.3, 487.5, 487.7, 486.3, 486.5, 486.7, 486.9}; //rx 505.3 
//const float KR_hybrid_channels[8] = {922.1, 922.3, 922.5, 922.7, 922.9, 923.1, 923.3, 0};     //rx 921.9 
//
//const float IN_hybrid_channels[8] = {865.0625, 865.4025, 865.9850, 0, 0, 0, 0, 0};            //rx 866.55
//
//const float AS1_hybrid_channels[8] = {923.2, 923.4, 922.2, 922.4, 922.6, 922.8, 923.0, 922.1}; //rx 923.2
//const float AS2_hybrid_channels[8] = {923.2, 923.4, 923.6, 923.8, 924.0, 924.2, 924.4, 924.6}; //rx 923.2

//United States Receive Window Data Rate = DR8
#define US_RX_DR DR8
//#define EU_RX_DR DR8
//#define AU_RX_DR DR8
//#define CN_RX_DR DR0
//#define KR_RX_DR DR0
//#define IN_RX_DR DR2
//#define AS1_RX_DR DR2
//#define AS2_RX_DR DR2

#define US_MAX_EIRP_NDX 13
//#define EU_MAX_EIRP_NDX 2
//#define AU_MAX_EIRP_NDX 13
//#define CN_MAX_EIRP_NDX 7
//#define KR_MAX_EIRP_NDX 4
//#define IN_MAX_EIRP_NDX 13
//#define AS1_MAX_EIRP_NDX 5
//#define AS2_MAX_EIRP_NDX 5

unsigned char frame_counter = 1;
char buffer[256];


void setup(void)
{
    SerialUSB.begin(115200);
    while(!SerialUSB);     
    
    lora.init();
    lora.setDeviceDefault();
  
    memset(buffer, 0, 256);
    lora.getVersion(buffer, 256, 1);
    SerialUSB.print(buffer); 
       
    memset(buffer, 0, 256);
    lora.getId(buffer, 256, 1);
    SerialUSB.print(buffer);

    //void setId(char *DevAddr, char *DevEUI, char *AppEUI);
    //void setKey(char *NwkSKey, char *AppSKey, char *AppKey);    

    lora.setId(DEV_ADDR, DEV_EUI, APP_EUI);
    lora.setKey(NWK_S_KEY, APP_S_KEY, APP_KEY);
    
    lora.setDeciveMode(LWABP);
    lora.setDataRate(DR0, US915HYBRID);
    lora.setPower(US_MAX_EIRP_NDX);
    
    //for(uint8_t i = 8; i < 72; i ++)lora.setChannel(i, 0);
    setHybridForTTN(US_hybrid_channels);
    
    lora.setReceiceWindowFirst(1);
    lora.setReceiceWindowSecond(923.3, US_RX_DR);

    //lora.setPower(14);
    //lora.setPower(US_MAX_EIRP_NDX);
}

void setHybridForTTN(const float* channels){
    
    for(int i = 0; i < 8; i++){
        // DR0 is the min data rate
        // US_RX_DR = DR3 is the max data rate for the US
        if(channels[i] != 0){
          lora.setChannel(i, channels[0], DR0, DR3);
        }
    }
}
void loop(void)
{
    bool result = lora.transferPacket(&frame_counter, 1, 5);
    
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
