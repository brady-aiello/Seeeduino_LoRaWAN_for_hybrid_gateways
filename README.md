# Seeeduino LoRaWAN for hybrid gateways

These examples are meant for connecting a Seeeduino-LoRaWAN (with or without GPS) to a LoRaWAN gateway running the "The Things Network" packet forwarder (the new one using TCP and written in Go, here: https://github.com/TheThingsNetwork/packet_forwarder). 

This project assumes that you are using a "hybrid" LoRaWAN gateway. A full gateway uses a Chirp Spread Spectrum (CSS) modulation on 64 channels, and costs a lot of money. In a hybrid gateway, only a few of the channels are used. Though the Seeeduino LoRaWAN can broadcast in full LoRa style (all 64 channels), most of us don't have enough money for a gateway that can afford to listen on 64 channels. If you're tried sample code from Seeeduino and are wondering why it didn't work reliably, it is because of this Seeeduino transmitting / gateway listening assymetry. 

It has only been tested with the US plan running on a RisingHF + Raspberry Pi gateway, which can listen on 8 channels. I have also included additional frequency plans for other countries, referring both to TTN's gateway configuration files (here: https://github.com/TheThingsNetwork/gateway-conf) as well as the latest AT Command Specification for RisingHF's RHF76-052 chip, which I've uploaded on TTN's forum, here:
https://www.thethingsnetwork.org/forum/uploads/default/original/2X/3/3995d6855260b7c36ff9ae19748aa527701c59a6.pdf

If you live outside the US, and are able to get your Seeeduino to work reliably on The Things Network, please let me know, so we can document that it works, or pull in your working code, to reflect your experiments.
