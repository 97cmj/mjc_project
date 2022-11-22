#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include "DHT.h"
#define DHTPIN A0
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
#define FIREBASE_HOST "fire-detection-768d5-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "hyWURkSzb3xgIFk2oVQOihrBhmM3Wd24bWyWg8jh"
#define WIFI_SSID "SK_WiFiB7BC"
#define WIFI_PASSWORD "chlaudwls1!"                               //Password of your wifi network 
FirebaseData firebaseData;
#define ENA D0      //ENA
#define IN1 D1 //IN1
#define IN2 D2 //IN2
#define IN3 D6 //IN3
#define IN4 D7 //IN4
#define ENB D8      //ENB
const int gasPin = A0;
int rainPin;
int gas, rain, dust;
int detect_gas, detect_rain , detect_temp, detect_humi, detect_dust;

//변수 선언하는 곳
String values, sensor_data;
String window_status = "0";
String fire, humi, temp;
String set_humi, set_temp;
float com_humi, com_temp;
String ch;
void setup() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  dht.begin();
  Serial.begin(9600);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  delay(10);

  //pinMode(D7, INPUT);
  //pinMode(D8, INPUT);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);   // connect to firebase
  Firebase.reconnectWiFi(true);
}

void loop() {

  gas = analogRead(gasPin);
  rain = analogRead(rainPin);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  String h_ = "";
  String t_ = "";
  h_.concat(h);
  t_.concat(t);

  //  int right_switch = digitalRead(D7);
  //  int left_switch = digitalRead(D8);
  //
  //  if (right_switch == LOW) { //오른쪽 스위치가 눌려있으면
  //    window_status = "1"; //창문은 열려있다
  //  }
  //  else if (left_switch == LOW) {
  //    왼쪽 스위치가 눌려있으면
  //    window_status = "0"; //창문은 닫혀있다.
  //  }
  //  else if (right_switch == HIGH && left_switch == HIGH) { //왼쪽,오른쪽 스위치가 다 안눌려있다
  //    window_status = "2"; //창문은 정지되어있다.
  //  }

  if (rain < 1000) {
    Serial.println("빗물 감지됨");
    detect_rain = 1;
  }
  else {
    detect_rain = 0;
  }




  Firebase.setString(firebaseData, "dht/humi", h_);
  Firebase.setString(firebaseData, "dht/temp", t_);
  Serial.println("현재 온도: " + t_ + "," + "현재 습도: " + h_);
  if (Firebase.getString(firebaseData, "conf/ch"))
  {
    ch = firebaseData.stringData(); // 채널
  }
  //if (Firebase.getString(firebaseData, "conf/window_status"))
  //{
  //  window_status = firebaseData.stringData(); // 창문 상태
  //}
  if (Firebase.getString(firebaseData, "conf/humi"))
  {
    set_humi = firebaseData.stringData(); // 설정습도
  }
  if (Firebase.getString(firebaseData, "conf/temp"))
  {
    set_temp = firebaseData.stringData(); // 설정온도
  }

  //가스값을 감지하는 코드
  if (gas < 900) {
    detect_gas = 0;
  }
  else if (gas >= 900) {
    detect_gas = 1;
  }

  //온도,습도값을 비교하는 코드
  com_temp = set_temp.toFloat();
  com_humi = set_humi.toFloat();
  if (com_humi > h) {
    detect_humi = 1;
  }
  else {
    detect_humi = 0;
  }

  if (com_temp > t) {
    detect_temp = 1;
  }
  else {
    detect_temp = 0;
  }








  if (ch == "0") { //정지모드
    Stop();
    window_status = "2";
  }

  if (ch == "1") { //수동으로 열기모드
    if (window_status == "0") { //창문이 닫혀있다면
      Open();
      window_status = "1";
    }
    if (window_status == "1") { //창문이 열려있다면
      Serial.println("아무것도 하지않음");
    }
    if (window_status == "2") { //창문이 정지해있다면
      Open();
      window_status = "1";
    }
  }

  if (ch == "2") { //수동으로 닫기모드
    if (window_status == "0") { //창문이 닫혀있다면
      Serial.println("아무것도 하지않음");
    }
    if (window_status == "1") { //창문이 열려있다면
      Close();
      window_status = "0";
    }
    if (window_status == "2") { //창문이 정지해있다면
      Close();
      window_status = "0";
    }
  }


  if (ch == "3") { //자동모드
    if (fire == "1") {
      Serial.println("가스가 감지됨");
    }
    else if (detect_gas == 1 && window_status == "0") { //가스가 감지되고 창문이 닫혀있다면
      Open();
    }
    else if (detect_rain == 1 && window_status == "1") { //빗물이 감지되고 창문이 열려있다면
      Close();
    }
    else if (detect_dust == 1 && window_status == "1") { //실내 미세먼지 수치가 초과되고 창문이 열려있다면
      Close();
    }
    else if (detect_temp == 1 && window_status == "0") { //실내 온도 수치가 초과되고 창문이 닫혀있다면 ex)설정 30도 현재온도 25도면 열림
      Open();
    }
    else if (detect_humi == 1 && window_status == "0") {
      Open();
    }
  }

  if ( ch == "4") {
    Serial.println("수동모드 상태입니다");
  }








  //    if (Firebase.getString(firebaseData, "conf / dust"))
  //    {
  //      dust = firebaseData.stringData();
  //      dust3 = dust.toFloat();
  //      dust2 = dust_value.toFloat();
  //
  //      if (dust3 >= dust2) { //설정미세먼지가 현재미세먼지 이상이면
  //        cp_dust = "1";
  //      }
  //      else { //설정미세먼지가 현재미세먼지보다 낮으면
  //        cp_dust = "0";
  //      }
  //    }


  //    if (Firebase.getString(firebaseData, "dht / rain"))
  //    {
  //      rain = firebaseData.stringData();
  //      rain2 = firebase.toInt();
  //
  //      if (rain2 <= 950) { //빗물이 감지되면
  //        cp_rain = "1";
  //      }
  //      else {  //빗물이 감지되지 않으면
  //        cp_rain = "0";
  //      }
  //    }

  //    if (Firebase.getString(firebaseData, "dht / gas"))
  //    {
  //      gas = firebaseData.stringData();
  //      gas2 = firebase.toInt();
  //
  //      if (gas2 <= 950) { //가스가 감지되면
  //        cp_gas = "1";
  //      }
  //      else {  //가스가 감지되지 않으면
  //        cp_gas = "0";
  //      }
  //    }
  delay(1000);
}



void Open() {
  for (int i = 0; i <= 310; i++) {
    digitalWrite(ENA, HIGH);
    digitalWrite(ENB, HIGH);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    delay(5);

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    delay(5);

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    delay(5);

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    delay(5);
    if (ch == "0") {
      digitalWrite(ENA, LOW);
      digitalWrite(ENB, LOW);
      delay(10);
    }
  }
}


void Close() {
  for (int i = 0; i <= 310; i++) {
    digitalWrite(ENA, HIGH);
    digitalWrite(ENB, HIGH);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    delay(2);

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    delay(2);

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    delay(2);

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    delay(2);
    if (ch == "0") {
      digitalWrite(ENA, LOW);
      digitalWrite(ENB, LOW);
      delay(10);
    }
  }
}

void Stop() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  delay(10);
}
