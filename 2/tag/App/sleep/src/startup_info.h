#ifndef __STARTUP_INFO__
#define __STARTUP_INFO__

/*��¼������Ϣ*/
typedef struct startup_info{
  uint32_t poweron_cnt;         //�ϵ���������(������)
  uint32_t powerup_cnt;         //��λ��������(������)
  uint32_t isfist_on;           //�ж��Ƿ�Ϊ�״��ϵ�
}StartUp_Info;

/*��������*/
typedef enum
{
    PowerOn,                    //�ϵ�����                  
    PowerUp                     //��λ����
} STARTUP_TYPE;

void startup_info_process(void);
StartUp_Info *get_CurStartInfo();
void Clearn_StartInfo(void);


#endif


