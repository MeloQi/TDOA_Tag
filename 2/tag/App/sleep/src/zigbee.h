#ifndef __ZIGBEE_H__
#define __ZIGBEE_H__

uint8_t array_compare(uint8_t *buffer1,uint8_t *buffer2,uint8_t length);
void Zigbee_Receive(void);
void do_zigbee(void);
void zigbee_rcv_init(void);
uint8_t zigbee_is_rcved(void);
void zigbee_rcv_process(void);
void Zigbee_Transmit(void);

#endif