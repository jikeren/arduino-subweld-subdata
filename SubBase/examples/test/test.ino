#include "ESP8266WiFi.h"
#include "SubData.h"
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
  int num = 10;
  SubWirePowerClass sp;
//  sp.init();
  SubDateT buf;
  timeProc();
  Serial.println(sp.objInfo());
  for (int i = 0; i < num; i++)
  { delay(10);
    buf.Val1 = timr++;
    sp.putData(&buf);
    Serial.printf("%d\t", buf.Val1);
  }
  Serial.println();
  for (int i = 0; i < num; i++)
  { delay(10);
    if (sp.getData(&buf))
      Serial.printf("%d\t", buf.Val1);
  }
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
