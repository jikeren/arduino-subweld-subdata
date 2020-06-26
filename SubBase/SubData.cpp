#include "SubData.h"

EEPROMClass subBROM(SUB_EEPROM_INFO_MIN);

void subBROMBegin()
{ static char save = 0;
  if (save) return;
  save = 1;
  subBROM.begin(SUB_HEAD_MAX_ADD);
}

EEPROMClass *newEEPROM(EEPROMClass *src, uint32_t _add)
{ if (src)
  { (*src).end();
    delete src;
  }
  src = new EEPROMClass(_add);
  (*src).begin(SUB_DATA_MAX_ADD);
  return src;
}

/*

  static int writeStringToEEPROM(int addrOffset, const String &strToWrite)
  {
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
  return addrOffset + 1 + len;
  }

  static int readStringFromEEPROM(int addrOffset, String *strToRead)
  {
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0';
   strToRead = String(data);
  //  Serial.println("data pr:%d",(int)data);
  Serial.println((long) data, DEC);
  return addrOffset + 1 + newStrLen;
  }
*/


/*
   取数据头信息：从EEPROM基础信息中获取数据头信息
   操作基础信息扇区中的不同对象的地址
*/
void SubDataClass::getHead(void) {
  char commit = 0;
  uint32_t nums = (SUB_DATA_MAX_ADD / sizeof(SubDateT)) - 1;
  subBROMBegin();
  subBROM.get(_headDataADD, _headData);
  if ((_headData.putSector >= _maxSector) || (_headData.putSector < _minSector))   commit = 1;
  if ((_headData.getSector >= _maxSector) || (_headData.getSector < _minSector))   commit = 1;
  if ( _headData.putPlace > nums) commit = 1;
  if ( _headData.getPlace > nums) commit = 1;
  if (commit) {
    init();
    commitHead();
  }
  if (_putROM == 0)   _putROM = newEEPROM (_putROM, _headData.putSector);
  if (_getROM == 0)   _getROM = newEEPROM (_getROM, _headData.getSector);
  maxPlace = nums*(_maxSector - _minSector);
}


void SubDataClass::init(void) {
  _headData.getNums = 0;
  _headData.getPlace = 0;
  _headData.getSector = _minSector;

  _headData.putNums = 0;
  _headData.putPlace = 0;
  _headData.putSector = _minSector;
}

/*
   写数据头信息：从EEPROM基础信息中写入数据头信息
   操作基础信息扇区中的不同对象的地址
*/
void SubDataClass::putHead(void) {
  subBROM.put(_headDataADD, _headData);
}



String SubDataClass::objInfo(void) {
  String str = "\n";
  str += _name + " info:";
  str += "\n\tbaseSector = " + String(SUB_EEPROM_INFO_MIN);
  str += "\tbasePlace = " + String(_headDataADD);
  str += "\n\tminSector = " + String(_minSector);
  str += "\tmaxSector = " + String(_maxSector - 1);

  str += "\n\treadSector = " + String(_headData.getSector);
  str += "\treadPlace = " + String(_headData.getPlace);
  str += "\treadNums = " + String(_headData.getNums);

  str += "\n\twriteSector = " + String(_headData.putSector);
  str += "\twritePlace = " + String(_headData.putPlace);
  str += "\twriteNums = " + String(_headData.putNums);

  str += "\n\twritePoint = " + String((int)_putROM);
  str += "\treadPoint = " + String((int)_getROM );
  str += "\n";

  return str;
}

/*
   提交数据头信息：从EEPROM基础信息中提交数据头信息
   操作基础信息扇区中的不同对象的地址
*/
void SubDataClass::commit(void) {

  (*_putROM).commit();
  if (_headData.getSector == _headData.putSector) _getROM = newEEPROM (_getROM, _headData.getSector);
#ifdef SUB_DUBEG
  Serial.printf("sub %s put eeprom commit ok!", _name.c_str());
#endif
  commitHead();
}
void SubDataClass::commitHead(void) {
  putHead();
  subBROM.commit();
#ifdef SUB_DUBEG
  Serial.println("sub base eeprom commit ok!");
#endif
}

bool SubDataClass::getData(void) {
  if (  _headData.getNums >=  _headData.putNums)  return false;
  (*_getROM).get(getAdd(), getDatabuf);
  return true;
}

void SubDataClass::putData(void) {
  (*_putROM).put(putAdd(), putDatabuf);
}

bool SubDataClass::getData(SubDateT *sp) {

  if (  _headData.getNums >=  _headData.putNums)  return false;
  (*_getROM).get(getAdd(), *sp);
  return true;
}

void SubDataClass::putData(SubDateT *sp) {

  (*_putROM).put(putAdd(), *sp);
}


uint32_t SubDataClass::getAdd(void) {

  uint32_t nums = (SUB_DATA_MAX_ADD / sizeof(SubDateT)) - 1;                        //一个扇区存储的结构体最大数量
  uint32_t address = _headData.getPlace;
  _headData.getPlace++;
  if (_headData.getPlace > nums) {
    _headData.getPlace = 0;
    if ((++_headData.getSector) >= _maxSector)_headData.getSector = _minSector;
    if (((_maxSector - _minSector) != 1) || (_getROM == 0))
      _getROM = newEEPROM (_getROM, _headData.getSector);
  }
#ifdef SUB_DUBEG
  Serial.print("***data read Sector= ");
  Serial.print(_headData.getSector);
  Serial.print("\t read Place= ");
  Serial.print(_headData.getPlace);
  Serial.print("\t read total num = ");
  Serial.println(_headData.getNums);
#endif
  _headData.getNums++;
  return address * sizeof(SubDateT);
}

/*

*/
uint32_t SubDataClass::putAdd(void) {
  uint32_t nums = (SUB_DATA_MAX_ADD / sizeof(SubDateT)) - 1;                        //一个扇区存储的结构体最大数量
  uint32_t address = _headData.putPlace;
  _headData.putPlace++;
  if (_headData.putPlace > nums) {
    commit();
    _headData.putPlace = 0;
    // _headData.putSector++;
    if ((++_headData.putSector) >= _maxSector)_headData.putSector = _minSector;
    if (((_maxSector - _minSector) != 1) || (_putROM == 0))
      _putROM = newEEPROM (_putROM, _headData.putSector);
  }
#ifdef SUB_DUBEG
  Serial.print("***data write Sector= ");
  Serial.print(_headData.putSector);
  Serial.print("\t write Place= ");
  Serial.println(_headData.putPlace);
#endif

//防止重复传输

  if((_headData.getNums + maxPlace) >= _headData.putNums) 
  {
	_headData.getNums =  _headData.putNums - maxPlace;
  }
  _headData.putNums++;
  return address * sizeof(SubDateT);
}

void SubDataClass::putBaseInfo(SubBaseInfoT *p) {
  subBROM.put(SUB_HEAD_BASEINFO, *p);

}



void SubDataClass::getBaseInfo(SubBaseInfoT *p) {
  int num =  sizeof(SubBaseInfoT);
  subBROM.get(SUB_HEAD_BASEINFO, *p);
#ifdef SUB_DUBEG
  Serial.print("sub base info len:");
  Serial.println(num);
#endif
}




/*
父类的释放函数
*/
void SubDataClass::deletebase(void) {
  commit();
  if (_putROM)  {
    (*_putROM).end();
    delete _putROM;
  }
  if (_getROM)  {
    (*_getROM).end();
    delete _getROM;
  }
#ifdef SUB_DUBEG
  Serial.printf("%s delete ok!\n", _name.c_str());
#endif
}


//子类
SubWirePowerClass::SubWirePowerClass() {
  _minSector = SUB_EEPROM_WIREP_MIN;
  _maxSector = SUB_EEPROM_WIREP_MAX;
  _headDataADD = SUB_HEAD_WIREP ;
  _putROM = 0;
  _getROM = 0;
  _name = "wire power";
  getHead();
}
SubStateClass::SubStateClass() {
  _minSector = SUB_EEPROM_STATE_MIN;
  _maxSector = SUB_EEPROM_STATE_MAX;
  _headDataADD = SUB_HEAD_STATE ;
  _putROM = 0;
  _getROM = 0;
  _name = "state ";
  getHead();
}


SubQalarmClass::SubQalarmClass() {
  _minSector = SUB_EEPROM_QALRAM_MIN;
  _maxSector = SUB_EEPROM_QALRAM_MAX;
  _headDataADD = SUB_HEAD_QALRAM ;
  _putROM = 0;
  _getROM = 0;
  _name = "QALRAM ";
  getHead();
}


SubWaveClass::SubWaveClass() {
  _minSector = SUB_EEPROM_WAVE_MIN;
  _maxSector = SUB_EEPROM_WAVE_MAX;
  _headDataADD = SUB_HEAD_WAVE ;
  _putROM = 0;
  _getROM = 0;
  _name = "Wave ";
  getHead();
}
