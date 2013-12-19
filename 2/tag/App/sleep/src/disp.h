#ifndef		__DISP_H__
#define 	__DISP_H__

/*按键相关*/
typedef enum {
	NO = 0 ,
	LONG_PRESS ,
	CLICK_PRESS,
	CLICK,
	LONG_PRESS_RELEASE,
	LONG_PRESS_LISTEN
	
} Button_Stat;

typedef struct button {
	Button_Stat stat;						//按键状态
	uint32_t hold_time;						//按键持续按下时间 ms
} Button;



/*显示相关*/
#define RED_LED  	0x01
#define	GREEN_LED  	0x02
#define	FM  		0x04
#define	MOTOR  		0x08


typedef struct disp{
	uint8_t stat;						//显示状态
	void (*operation)(uint8_t opt);		                //显示操作函数
	void (*disable)(void);				        //禁止显示
	void (*enable)(void);				        //使能并恢复显示
}Disp;

#define RedLedOff() 	do{GPIO_PCOUT &=~PC2; disp.stat &= (~RED_LED);}while(0)  	  //红灯PC2
#define RedLedOn()  	do{GPIO_PCOUT |= PC2; disp.stat |= RED_LED;}while(0)

#define GreenLedOff()   do{GPIO_PCOUT &=~PC3;disp.stat &= (~GREEN_LED);}while(0) 	  //绿灯PC3
#define GreenLedOn()    do{GPIO_PCOUT |= PC3;disp.stat |= GREEN_LED;}while(0)

#define FMQOn()        	do{GPIO_PBOUT |= PB6;disp.stat |= FM;}while(0)  		  //蜂鸣器PB6
#define FMQOff()       	do{GPIO_PBOUT &=~PB6;disp.stat &= (~FM);}while(0)

#define MOTOROn()      	do{GPIO_PCOUT |= PC1;disp.stat |= MOTOR;}while(0)  		  //马达PC1
#define MOTOROff()     	do{GPIO_PCOUT &=~PC1;disp.stat &= (~MOTOR);}while(0)


void button_poll(void);
uint8_t listen_button(Button *button,Button_Stat stat);
void button_init(void);
void button_poll(void);
void disp_init(void);
void LEDBlink();
void RingForSuccess();
#endif

