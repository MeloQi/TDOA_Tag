#ifndef __COMMU_H__
#define __COMMU_H__

#define CALL_LEN 9							//读卡分站发给标识卡命令长度
#define HELP_RANG_DELY	3000				//求助状态下测距时间间隔	ms

typedef enum {
	NO_CALL = 0,
	SUCCESS_CALL = 1,
	ACK_CALL = 2
	
	
} Call_Card_Stat;

typedef struct call_card_info{
	Call_Card_Stat stat;
}Call_Card;

void call_card_init(void);
void com_ack(void);
MyBoolT parse_rev_com(uint8_t src,uint8_t data_len);
MyBoolT arry_commp(uint8_t *a,uint8_t *b,uint8_t len);
void help_init(void);
void communication(void);
#endif