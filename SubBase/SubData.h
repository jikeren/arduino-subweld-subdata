#ifndef __SUBDATA_H__
#define __SUBDATA_H__
#include <Arduino.h>
#include <EEPROM.h>
/*
  EEPROM 一个扇区为4096字节
  共251个扇区
  申请无扇区参数时为251
*/

//#define SUB_DUBEG

#define SUB_EEPROM_START_ADD  200     //SUB数据存储的起始地址
#define SUB_EEPROM_INFO_LEN   1				//SUB基础数据存储的长度
#define SUB_EEPROM_STATE_LEN  1				//SUB状态数据存储的长度
#define SUB_EEPROM_WAVE_LEN   1				//SUB波形数据存储的长度
#define SUB_EEPROM_WIREP_LEN  7				//SUB焊丝和能耗数据存储的长度
#define SUB_EEPROM_QALRAM_LEN 7				//SUB质量预警数据存储的长度

#define SUB_DATA_MAX_ADD   4010
//最大最小地址定义
#define SUB_EEPROM_INFO_MIN   SUB_EEPROM_START_ADD
#define SUB_EEPROM_INFO_MAX   SUB_EEPROM_INFO_MIN+SUB_EEPROM_INFO_LEN

#define SUB_EEPROM_STATE_MIN   SUB_EEPROM_INFO_MAX
#define SUB_EEPROM_STATE_MAX   SUB_EEPROM_STATE_MIN+SUB_EEPROM_STATE_LEN

#define SUB_EEPROM_WAVE_MIN   SUB_EEPROM_STATE_MAX
#define SUB_EEPROM_WAVE_MAX   SUB_EEPROM_WAVE_MIN+SUB_EEPROM_WAVE_LEN

#define SUB_EEPROM_WIREP_MIN   SUB_EEPROM_WAVE_MAX
#define SUB_EEPROM_WIREP_MAX   SUB_EEPROM_WIREP_MIN+SUB_EEPROM_WIREP_LEN

#define SUB_EEPROM_QALRAM_MIN   SUB_EEPROM_WIREP_MAX
#define SUB_EEPROM_QALRAM_MAX   SUB_EEPROM_QALRAM_MIN+SUB_EEPROM_QALRAM_LEN


//基础数据结构体
/*
struct SubBaseInfoT {
  String sn;
  String name;
  String model;
  String mDate;
  String uDate;
  String userId;
  String code;
  String qRcode;
};
*/

struct SubBaseInfoT {
  char sn[16];
  char name[16];
  char model[16];
  char mDate[16];
  char uDate[16];
  char userId[16];
  char code[16];
  char qRcode[32];
};


//数据头结构体
struct SubDateHandHT {
  uint8_t  	putSector, getSector;  //
  uint32_t  putNums, putPlace;
  uint32_t	getNums, getPlace;
};
//基础数据存储
#define SUB_HEAD_BASE_ADD   0
#define SUB_HEAD_CONST_LEN  sizeof(SubDateHandHT)   //数据头结构体长度

#define SUB_HEAD_STATE		0*SUB_HEAD_CONST_LEN +SUB_HEAD_BASE_ADD     //状态数据头存储号
#define SUB_HEAD_WAVE		  1*SUB_HEAD_CONST_LEN +SUB_HEAD_BASE_ADD       //波形数据头存储号
#define SUB_HEAD_WIREP		2*SUB_HEAD_CONST_LEN +SUB_HEAD_BASE_ADD       //焊丝数据头存储号
#define SUB_HEAD_QALRAM		3*SUB_HEAD_CONST_LEN +SUB_HEAD_BASE_ADD       //报警数据头存储号
#define SUB_HEAD_BASEINFO	4*SUB_HEAD_CONST_LEN +SUB_HEAD_BASE_ADD      //基础数据头存储号

#define SUB_HEAD_MAX_ADD   1000


//数据结构体
struct SubDateT {
  unsigned short  int  Val1;
  unsigned short  int  Val2;
  unsigned        int  startTime;
  unsigned short  int  userTime;
};
////焊丝和能耗结构体
//struct SubWirePowerT {
//  unsigned short  int  wireVal;
//  unsigned short  int  powerVal;
//  unsigned        int  startTime;
//  unsigned short  int  userTime;
//};
////预警和状态结构体
//struct SubQalarmStateT {
//  unsigned short  int  Val;
//  unsigned        int  startTime;
//  unsigned short  int  ValOrUserTime;
//};
class SubDataClass {
  public:

    SubDataClass(void) {}       //各子类接口:构造函数
    ~SubDataClass(void) {}      //各子类接口:析构造函数


    bool getData(void);     //取出数据
    void putData(void);           //写入数据
    bool getData(SubDateT *sp);     //取出数据
    void putData(SubDateT *sp);           //写入数据
	


    void init(void);

    void commit(void);            //提交数据到EEPROM

    String objInfo(void);

    void putBaseInfo(SubBaseInfoT *p);		//写头信息
    void getBaseInfo(SubBaseInfoT *p);		//读头信息

  protected:
  

    SubDateT getDatabuf;             //当前数据
    SubDateT putDatabuf;
    //    uint32_t  _curSector;       //同_headData.getSector   _headData.putSector
    uint32_t  _maxSector, _minSector; //各子类接口:最大扇区和最小扇区
	uint32_t  maxPlace;
    unsigned short int _headDataADD;  //各子类接口:头地址
    SubDateHandHT _headData;          //各子类接口:头数据
    String  _name;                    //各子类接口:名称
    EEPROMClass *_putROM;
    EEPROMClass *_getROM;
    uint32_t getAdd(void);
    uint32_t putAdd(void);
    void commitHead(void);
    void deletebase(void);
	void getHead(void);		
    void putHead(void);		  

};


class SubWirePowerClass : public SubDataClass {
  public:
    unsigned short  int *wireVal;
    unsigned short  int *powerVal;
    unsigned        int *startTime;
    unsigned short  int *userTime;


    SubWirePowerClass(void);
    ~SubWirePowerClass(void) {
      deletebase();
    }

  protected:
};

class SubQalarmClass : public SubDataClass {
  public:

    SubQalarmClass(void);
    ~SubQalarmClass(void) {
      deletebase();
    }

  protected:
    unsigned short  int *alarmVal;
};

class SubStateClass : public SubDataClass {
  public:
    SubStateClass(void);
    ~SubStateClass(void) {
      deletebase();
    }

  protected:
    unsigned short  int *stateVal;
};


class SubWaveClass : public SubDataClass {
  public:

    SubWaveClass(void);
    ~SubWaveClass(void) {
      deletebase();
    }

  protected:
    unsigned short  int *currVal;
    unsigned short  int *voltVal;
};
#endif
