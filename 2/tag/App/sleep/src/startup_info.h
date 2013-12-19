#ifndef __STARTUP_INFO__
#define __STARTUP_INFO__

/*记录启动信息*/
typedef struct startup_info{
  uint32_t poweron_cnt;         //上电启动次数(冷启动)
  uint32_t powerup_cnt;         //复位启动次数(热启动)
  uint32_t isfist_on;           //判断是否为首次上电
}StartUp_Info;

/*启动类型*/
typedef enum
{
    PowerOn,                    //上电启动                  
    PowerUp                     //复位启动
} STARTUP_TYPE;

void startup_info_process(void);
StartUp_Info *get_CurStartInfo();
void Clearn_StartInfo(void);


#endif


