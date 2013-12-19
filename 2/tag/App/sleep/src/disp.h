#ifndef		__DISP_H__
#define 	__DISP_H__

/*�������*/
typedef enum {
	NO = 0 ,
	LONG_PRESS ,
	CLICK_PRESS,
	CLICK,
	LONG_PRESS_RELEASE,
	LONG_PRESS_LISTEN
	
} Button_Stat;

typedef struct button {
	Button_Stat stat;						//����״̬
	uint32_t hold_time;						//������������ʱ�� ms
} Button;



/*��ʾ���*/
#define RED_LED  	0x01
#define	GREEN_LED  	0x02
#define	FM  		0x04
#define	MOTOR  		0x08


typedef struct disp{
	uint8_t stat;						//��ʾ״̬
	void (*operation)(uint8_t opt);		                //��ʾ��������
	void (*disable)(void);				        //��ֹ��ʾ
	void (*enable)(void);				        //ʹ�ܲ��ָ���ʾ
}Disp;

#define RedLedOff() 	do{GPIO_PCOUT &=~PC2; disp.stat &= (~RED_LED);}while(0)  	  //���PC2
#define RedLedOn()  	do{GPIO_PCOUT |= PC2; disp.stat |= RED_LED;}while(0)

#define GreenLedOff()   do{GPIO_PCOUT &=~PC3;disp.stat &= (~GREEN_LED);}while(0) 	  //�̵�PC3
#define GreenLedOn()    do{GPIO_PCOUT |= PC3;disp.stat |= GREEN_LED;}while(0)

#define FMQOn()        	do{GPIO_PBOUT |= PB6;disp.stat |= FM;}while(0)  		  //������PB6
#define FMQOff()       	do{GPIO_PBOUT &=~PB6;disp.stat &= (~FM);}while(0)

#define MOTOROn()      	do{GPIO_PCOUT |= PC1;disp.stat |= MOTOR;}while(0)  		  //���PC1
#define MOTOROff()     	do{GPIO_PCOUT &=~PC1;disp.stat &= (~MOTOR);}while(0)


void button_poll(void);
uint8_t listen_button(Button *button,Button_Stat stat);
void button_init(void);
void button_poll(void);
void disp_init(void);
void LEDBlink();
void RingForSuccess();
#endif

