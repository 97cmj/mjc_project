#include "DHT.h"
#include "SoftwareSerial.h"

#define DHTPIN 12
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
#define ENA 2
#define IN1 3
#define IN2 4
#define IN3 5
#define IN4 6
#define ENB 7

const int rainPin = A5;
const int gasPin = A4;
const int rightSwitch = A2;
const int leftSwitch = A3;
int gas, rain;
unsigned long pulse = 0;
float ugm3 = 0;
int RX = 11;
int TX = 10;
String value, values, ch, ugm3_value, humi_value, temp_value;
float cp_humi, cp_temp;
String window_status;
int detect_gas, detect_rain, detect_humi, detect_temp;

SoftwareSerial bluetooth(RX, TX);

void setup()
{
  Serial.begin(9600);
  bluetooth.begin(9600);
  dht.begin();
  pinMode(rainPin, INPUT);
  pinMode(gasPin, INPUT);
  pinMode(rightSwitch, INPUT);
  pinMode(leftSwitch, INPUT);
  pinMode(8, INPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);


  //HIGH 안눌림 LOW 눌림 처음 창문 상태를 저장
  if (digitalRead(leftSwitch) == LOW && digitalRead(rightSwitch) == HIGH) { //왼쪽 스위치가 눌려있고 오른쪽 스위치가 안눌려있다 = 창문이 닫혀있다
    window_status = "0"; // 창문은 닫혀있음
  }
  if (digitalRead(rightSwitch) == LOW && digitalRead(leftSwitch) == HIGH) { //오른쪽 스위치가 눌려있고 왼쪽 스위치가 안눌려있다 = 창문이 닫혀있다
    window_status = "1"; //창문은 열려있음
  }
  if (digitalRead(leftSwitch) == HIGH && digitalRead(rightSwitch) == HIGH) { //왼쪽스위치와 오른쪽 스위치가 안눌려있다 = 창문이 정지했다
    window_status = "2"; //창문은 정지상태
  }
  delay(10);
}

void loop()
{
  if (bluetooth.available()) {
    //Serial.write(bluetooth.read());  //블루투스측 내용을 시리얼모니터에 출력
    value = bluetooth.readString(); //채널값을 bluetooth로 통해 전달받음
    value.replace("'", "");
    values = value;
    int one = values.indexOf(',');
    int two = values.indexOf(',', one + 1);
    int three = values.indexOf(',', two + 1);
    int four = values.indexOf(',', three + 1);

    ch = values.substring(0, one);
    ugm3_value = values.substring(one + 1, two);
    ugm3_value.trim();
    humi_value = values.substring(two + 1, three);
    humi_value.trim();
    temp_value = values.substring(three + 1, four);
    temp_value.trim();
    cp_humi = humi_value.toFloat();
    cp_temp = temp_value.toFloat();
  }

  gas = analogRead(gasPin);
  rain = analogRead(rainPin);
  float humi = dht.readHumidity();
  float temp = dht.readTemperature();
 
  


  if ( humi >= cp_humi) { //현재 습도가 설정 습도를 넘으면 
    detect_humi = 1;
  }
  else {
    detect_humi = 0;
  }

  if( temp >= cp_temp) {
    detect_temp = 1;
  }
  else {
    detect_temp = 0;
  }

  if (rain < 1000) {
    detect_rain = 1;
  }
  else {
    detect_rain = 0;
  }

  if (gas < 900) {
    detect_gas = 0;
  }
  if (gas >= 900) {
    detect_gas = 1;
  }

  pulse = pulseIn(8, LOW, 20000);
  ugm3 = pulse2ugm3(pulse); //초미세먼지 지수

  String sendhumi = "";
  sendhumi.concat(humi);
  String sendtemp = "";
  sendtemp.concat(temp);
  String sendrain = "";
  sendrain.concat(rain);
  String sendgas = "";
  sendgas.concat(gas);
  String sendugm3 = "";
  sendugm3.concat(ugm3);

   Serial.println(gas);
  Serial.println(rain);
  Serial.println(humi);
  Serial.println(temp);
  Serial.println(ugm3);

  if (ch == "0") { //채널 0 = 정지
    Stop();
  }
  //window_status 0=닫힘 1=열림 2=정지
  if (ch == "1" && window_status == "0" ) { //열기버튼+닫힘상태
    manualOpen();
  }
  if (ch == "1" && window_status == "2") { //열기버튼+정지상태
    manualOpen();
  }
  if (ch == "2" && window_status == "1") { //닫기버튼+열림상태
    manualClose();
  }
  if (ch == "2" && window_status == "2") { //닫기버튼+정지상태
    manualClose();
  }
  if (ch == "3") { //자동모드 시작
    //if(fire =="1"){}

    if (detect_gas == 1) {//가스감지+닫힘상태
      {
        if (window_status == "2" || window_status == "0") {
          autoOpen();
          Serial.println("가스 감지로 인해 창문 개방");
        }
      }
    }
    else if (detect_rain == 1 && detect_gas == 0) { //빗물감지+열림상태
      if (window_status == "2" || window_status == "1") {
        autoClose();
        Serial.println("빗물 감지로 인해 창문 닫음");
      }
    }
    else if (detect_humi == 1 && detect_rain == 0 && detect_gas == 0) { //현재 습도가 설정 습도를 넘음
      if (window_status == "2" || window_status == "0") {
        autoOpen();
        Serial.println("습도로 인해 창문 개방");
      }
    }
  }
  if (ch == "4") { //수동모드. 기능 없음
  }
  if (Serial.available()) {
    bluetooth.write(Serial.read());  //시리얼 모니터 내용을 블루추스 측에 WRITE
  }
  Serial.println("현재 창문상태:" + window_status); //0닫힘 1열림 2정지
  bluetooth.print(sendhumi + "," + sendtemp + "," + sendgas + "," + sendrain + "," + sendugm3 + "," + window_status);

  delay(1500);
}

float pulse2ugm3(unsigned long pulse) {
  float value = (pulse - 1400) / 14.0;
  if (value > 300) {
    value = 0;
  }
  return value;
}

void manualOpen() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  for (int i = 0; i <= 75; i++) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    delay(2);

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    delay(2);

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    delay(2);

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    delay(2);
  }
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  if (digitalRead(rightSwitch) == LOW) {
    digitalWrite(ENA, LOW);
    digitalWrite(ENB, LOW);
    window_status = "1";
    delay(10);

  }
  if (ch == "0") {
    Stop();
  }
}

void manualClose() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  for (int i = 0; i <= 75; i++) {
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
  }
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  if (digitalRead(leftSwitch) == LOW) {
    digitalWrite(ENA, LOW);
    digitalWrite(ENB, LOW);
    window_status = "0";
    delay(10);
  }
  if (ch == "0") {
    Stop();
  }
}

void Stop() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  window_status = "2";
  delay(10);
}

void autoOpen() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  while (1) {
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

    if (digitalRead(rightSwitch) == LOW) {
      digitalWrite(ENA, LOW);
      digitalWrite(ENB, LOW);
      window_status = "1";
      delay(10);
      break;
    }
  }
}

void autoClose() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  while (1) {
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
    if (digitalRead(leftSwitch) == LOW) {
      digitalWrite(ENA, LOW);
      digitalWrite(ENB, LOW);
      window_status = "0";
      break;
    }
  }
}
