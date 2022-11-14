#include "DHT.h"
#define DHTPIN 13
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
String value;
int limit_1_status, limit_2_status;


void setup() {
  dht.begin();
  Serial.begin(9600);

}


void loop() {
  while ( Serial.available()) {
    value = Serial.readString(); //value에서 채널, 창문 상태, 비교값(1이 감지되거나 설정온도 이상, 0은 감지되지 않거나 설정온도이하)을 받아옴
  }
  if (ch == "0") { //정지
    //모터를 정지시킨다
  }
  if (ch == "1") { //열기
    //정회전 시킨다
  }
  if (ch == "2") { //닫기
    //역회전 시킨다
  }
  if (ch == "3") { //자동모드
    if (gas == "1" && window_status == "닫힘") { //가스가 발생했고 창문 상태가 닫혀있다.
      open();

    }
  }
  limit_switch 오른쪽이 눌리고 limit_switch 왼쪽이 안눌려있다 = 안열린 상태
      limit_Switch 왼쪽이 눌리고 limit_switch 오른쪽이 안눌림 = 열린상태
          float h = dht.readHumidity();
  float t = dht.readTemperature();
  String h_ = "";
  String t_ = "";
  h_.concat(h);
  t_.concat(t);
  // get sensors data and put in to values variables as a string.
  values = (h_ + "," + t_ + ",");
  Serial.flush();
  Serial.println(values);
  delay(2000);
}

void open() {
  if(a==high)

}
