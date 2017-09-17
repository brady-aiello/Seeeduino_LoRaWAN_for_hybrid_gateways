#include "LoRaWan.h"
#include "keys.h"

//unsigned char data[10] = "Hey World";
  unsigned char data[10] = "From Poly";
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

    lora.setKey(NWK_S_KEY, APP_S_KEY, APP_KEY);
    
    lora.setDeciveMode(LWABP);
    lora.setDataRate(DR3, US915);
    lora.setAdaptiveDataRate(true);

    for(uint8_t i = 0; i < 72; i ++)lora.setChannel(i, 0);
    
    lora.setChannel(0, 903.9, DR0, DR3);
    lora.setChannel(1, 904.1, DR0, DR3);
    lora.setChannel(2, 904.3, DR0, DR3);
    lora.setChannel(3, 904.5, DR0, DR3);
    lora.setChannel(4, 904.7, DR0, DR3);
    lora.setChannel(5, 904.9, DR0, DR3);
    lora.setChannel(6, 905.1, DR0, DR3);
    lora.setChannel(7, 905.3, DR0, DR3);

    //Turning this on gets "DR error"
    //lora.setChannel(8, 904.6, DR4, DR4);
    
    lora.setReceiceWindowFirst(0);
    lora.setReceiceWindowSecond(923.3, DR8);
    
    lora.setPower(20);
}

void loop(void)
{
    bool result = false;
    
    //result = lora.transferPacket("Hello", 5, 10);
    result = lora.transferPacket(data, 10, 5);
    
    if(result)
    {
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
