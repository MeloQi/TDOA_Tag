#ifndef __SYS_TIME_H__
#define	__SYS_TIME_H__

typedef struct my_event_trigger_time	//ĳģ���¼�����ʱ��ڵ�
{	uint32_t	s;							//
 	uint16_t 	ms;							//
} MyEvent_Time;

											//ʵ��������ʼ��һ��ʱ���¼�ʵ��
#define CREATE_DELY_OBJ(obj_name,t_s,t_ms)	\
			static MyEvent_Time obj_name = {\
											.s = t_s,\
											.ms = t_ms,\
											};

void set_mydely(MyEvent_Time *ev_t,uint32_t dely_s,uint16_t dely_ms);
int32_t mydely_come(MyEvent_Time *ev_t);

uint32_t get_systime();
void set_systime(uint32_t t);
uint16_t get_systime_ms(void);
void set_systime_ms(uint16_t t);
#endif