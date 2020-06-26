#ifndef __SUBWELD_H__
#define __SUBWELD_H__

#include <Arduino.h>

typedef enum _SUBSTATE_T
{
  SUB_LOST = 0, //离线
  SUB_IDLE,     //空闲
  SUB_WORK,     //焊接中
  SUB_WARN      //报警
};

class SubWelder {
  private:
//    char  readBuf[32];
//    char  len;
//    int   volt;
//    int   curr;
//    _SUBSTATE_T state;

  public:
    SubWelder() ;
    SubWelder(float t) ;  //u
    ~SubWelder();
    int readVolt();
    int readCurr();
    _SUBSTATE_T readstate();
};



#endif
