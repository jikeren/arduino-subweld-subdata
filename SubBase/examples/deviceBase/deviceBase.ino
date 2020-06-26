
#include <ESP8266WiFi.h>
#include <SubData.h>
#include <time.h>

void timeProc()
{
  static int num = 0;
  num++;
  time_t now = time(nullptr);
  Serial.print("now time:");
  Serial.println(num);
}

void subDataProc()
{
  static int timr = 0;
  uint8_t * upoint = 0;
  int num = 100;
  SubWirePowerClass sp;
  SubBaseInfoT base;
  SubBaseInfoT rbase;
  timeProc();
//  sprintf(base.sn, "%s", "jsadoaqkjhjkhjkllkjhkfdoe");
//  sprintf(base.name, "%s", "sub welder");
//  sprintf(base.model, "%s", "mz-1000i");
//  sp.putBaseInfo(&base);
  sp.getBaseInfo(&rbase);
  Serial.println(rbase.sn);
  Serial.println(rbase.model);

  Serial.println();
  return;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("start!");
}

void loop() {
  subDataProc();
  delay(2000);
}
