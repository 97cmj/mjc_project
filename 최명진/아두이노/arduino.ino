#include <SoftwareSerial.h>
#include "DHT.h"
#define DHTPIN 6 
#define DHTTYPE DHT22
int speaker = 2;
int Raindrops_pin = A0;
int RX=7;
int TX=8;
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial bluetooth(RX, TX);
#define LIMIT_TEMP_MAX_VALUE 30 //더위가 감지되었는지 판단할 기준 온도
#define LIMIT_TEMP_LIM_VALUE 10 //추위가 감지되었는지 판단할 기준 온도
#define LIMIT_GAS_VALUE 300
#define LIMIT_RAIN_VALUE 300
int8_t humi, temp;
int rain_value = 0;
int gas_value = 0;

int window_control = 0; // 0: 수동 1:자동
int window_status = 0; // 0:정지, 1:열림, 2:닫힘
int prev_window_status = 0; //창문 마지막 상태 저장 0:알수없음 1:열림 2:닫힘
int prev_limit_1_status; //리미트 스위치1 상태 저장
int prev_limit_2_status; //리미트 스위치2 상태 저장
 
const int BUFFER_SIZE = 50;
char buf[BUFFER_SIZE];

void setup(){
  Serial.begin(9600);
  bluetooth.begin(9600); //블루투스 시작
  dht.begin(); //온도센서 시작
  pinMode(A1, INPUT);
  window_status = 0;

  pinMode(speaker, OUTPUT);
}
 
void loop(){
  sendSensor_Value();
  getSensor_Value();

  while (bluetooth.available()){
      if(bluetooth.available())
      {
        int ch = bluetooth.readBytes(buf,BUFFER_SIZE);
        if(buf[0] =='0') //창문 정지
        {
          Serial.println("channel=0");
          window_status = 0; //현재 창문 상태
          window_control = 0; //창문 수동모드
          prev_window_status = 0;

        }
        if(buf[0] =='1') //창문 열기
        {
          Serial.println("channel=1");
          window_status = 1; //창문 열림
          window_control =0; //창문 수동모드
          prev_window_status = 0;
        }
        if(buf[0] =='2') //창문 닫기
        {
          Serial.println("channel=2");
          window_status = 2; //창문 닫힘
          window_control = 0; //창문 수동모드
          prev_window_status = 0;
        }
        if(buf[0]=='3') //창문 자동모드
        {
          Serial.println("channel=3");
          window_control = 1;
          prev_window_status = 0;
        }
        if(buf[0]=='4') //창문 수동모드
        {
          Serial.println("channel=4");
          window_control = 0;
          prev_window_status = 0;
        }
      }
    }
  if(window_control ==1) //자동창문모드인경우
  {
    if( (gas_value >= LIMIT_GAS_VALUE)) //현재 가스값이 한계가스값을 넘었을때
    {
      if(prev_window_status !=1) //이전 창문 상태가 열림이 아닐때
      {
        if (window_status !=1) //창문이 안열린경우
        {
          window_status =1; //창문 열림
        }
      }
    }
    else if((rain_value < LIMIT_RAIN_VALUE)) //현재 빗물수치가 한계 감지값보다 작아졌을때(작을수록 비가 온다는 뜻임)
    {
      if(prev_window_status !=2) //이전 창문 상태가 닫힘이 아닐때
      {
        if(window_status !=2) // 지금 창문이 안닫힌경우
        {
          window_status=2; //창문 닫힘
        }
      }
    }
    else if((temp >= LIMIT_TEMP_MAX_VALUE)) // 현재 온도가 설정 최고 온도보다 높을 경우
    {
      if(prev_window_status !=1) //이전 창문 상태가 열림이 아닐때
      {
        if(window_status !=1) //창문이 안열린경우
        {
          window_status =1; // 창문 열림
        }
      }
    }
    else
    {
      window_status =0; //모든 경우가 아닐때 창문 상태는 정지
    }
  }
  if(buf[1] =='1')
  {
    digitalWrite(speaker,HIGH);
  }
  if(buf[1] =='2')
  {
    digitalWrite(speaker,LOW);

  }

}

void sensor_open() {
  Serial.println("window open");
  //step(step);

}

void sendSensor_Value(){ 
  rain_value = analogRead(A0);
  gas_value = analogRead(A1);
  humi = (int8_t)dht.readHumidity();
  temp = (int8_t)dht.readTemperature();

  /*Serial.println("##############################");
  Serial.println("########SENSOR VALUE##########");
  Serial.println("##############################");
  Serial.print("TEMPERATURE : ");
  Serial.print(temp);
  Serial.println("°C");
  Serial.print("HUMIDITY : ");
  Serial.print(humi);
  Serial.println("%");
  Serial.print("RAIN :");
  Serial.print(rain_value);
  if(rain_value < LIMIT_RAIN_VALUE) {
    Serial.println("(DETECTED)");
  }
  else
  {
    Serial.println("(NOT DETECTED)");
  }
  Serial.print("GAS :");
  Serial.print(gas_value);
  if ( gas_value > LIMIT_GAS_VALUE)
  {
    Serial.println("(DETECTED)");
  }
  else
  {
    Serial.println("(NOT DETECTED)");
  }*/

  String sendhumi ="";
  sendhumi.concat(humi);
  String sendtemp ="";
  sendtemp.concat(temp);
  String sendrain ="";
  sendrain.concat(rain_value);
  String sendgas = "";
  sendgas.concat(gas_value);

  bluetooth.print(sendhumi + "," + sendtemp + "," + sendgas + "," + sendrain);

  delay(3000);
}

void getSensor_Value(){
  }
