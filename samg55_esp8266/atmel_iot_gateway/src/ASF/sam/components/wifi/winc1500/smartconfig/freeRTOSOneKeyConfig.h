/*
 * sniffer.h
 *
 * Created: 9/6/2015 11:53:13 AM
 *  Author: water.zhou
 */ 


#ifndef SNIFFER_H_
#define SNIFFER_H_

void wlan_smart_config_start(uint8_t* uSsid, uint8_t* uPassword, uint8_t* uSecurityType);


void wlan_smart_config_stop();

#endif /* SNIFFER_H_ */