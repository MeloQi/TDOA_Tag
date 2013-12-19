
#ifndef __APP_ADC_H__
#define __APP_ADC_H__


typedef enum 
{
    NLOW = 0,                                	//�͵���
    LOW = 1                                		//�ǵ͵���  
} Bat_Stat;

typedef struct Battery
{
    Bat_Stat    stat;
    uint32_t      v;
} Bat;

typedef struct Battery_V_Conf
{
    uint32_t      key;
    uint32_t    value;
} Bat_V_T;

#define KEY_VALUE_END   0xFFFFFFFF                //��ֵ�Խ�����־

void Battery_V_Process();
void init_battery_show(void);
void battery_show_init(void);
#endif