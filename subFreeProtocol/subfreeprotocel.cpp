#include "subfreeprotocol.h"
#include <Ticker.h>

static Ticker subTicker;
static int actVolt = 0;
static int actCurr = 0;
static int mainCurr = 0;
static _SUBSTATE_T state;
static int timeout = 0;
//SubWelder subWelder;
static void subRun(float t);

SubWelder::SubWelder() {
  state = SUB_LOST;
  subRun(0.003);
}

SubWelder::SubWelder(float t) {
  state = SUB_LOST;
  subRun(t);
}

SubWelder::~SubWelder() {
  subTicker.detach();
}

int SubWelder::readVolt() {
  return actVolt;
}

int SubWelder::readCurr() {
  return actCurr;
}

_SUBSTATE_T SubWelder::readstate() {
  return state;
}
/*
  电源参数写到小车数据结构
  buf[0] = 2;            //READ_POWER
  buf[1] = 7;            //参数个数
  buf[2]<<8  | buf[3]  ; //设定电流
  buf[4]<<8  | buf[5]  ; //实际电压
  buf[6]<<8  | buf[7]  ; //实际电流
  buf[8].7               //高为降特性，低为平特性
  buf[8].6               //高为启动，低为停止
  buf[8].5-0             //(buf[2]+...+buf[7])&0x3f较验
  ex:02 07 00 00 00 11 00 11 22
*/
static char readPowerProc(char * pr) {
  char ctemp = 0;
  if ((pr[0] != 2) || (pr[1] != 7))      return -1;
  for (int i = 0; i < 6; i++)
    ctemp = ctemp + pr[i + 2];
  if ((ctemp & 0x3f) != (pr[8] & 0x3f))  return -1;
  //  mainCurr = (pr[2]<<8)|pr[];
  actVolt = (pr[4] << 8) | pr[5];
  actCurr = (pr[6] << 8) | pr[7];
  if (!(pr[8] & 0x40)) {
    actVolt = 0;
    actCurr = 0;
  }
  return 0;
}

/*
 * 1.超时500次为离线
 * 2.只读电源写参数的数据
*/
static void readSubData()
{
  static int cNum = 0;
  static char buf[32];
  int num ;
  timeout ++;
  if ((cNum == Serial.available()) && Serial.available())
  { memset(buf, 0, 32);
    num = Serial.readBytes(buf, cNum);
    if (num == 9) readPowerProc(buf);
    Serial.println();
    timeout = 0;
  } else
    cNum = Serial.available();
  if (cNum > 20)
  { Serial.readBytes(buf, cNum);
  }
  if(timeout >500) {
    state = SUB_LOST;
    timeout = 0;
    }
}
/*
 * 定时任务
 * 输入参数：定时时间
*/

static void subRun(float t) {
  if (t)
    subTicker.attach(t, readSubData);
  else
    subTicker.detach();
}
