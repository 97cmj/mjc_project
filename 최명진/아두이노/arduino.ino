#include <SoftwareSerial.h>
#include "DHT.h"
#define DHTPIN 6 
#define DHTTYPE DHT22
int Raindrops_pin = A0;
int RX=7;
int TX=8;
DHT dht(DHTPIN, DHTTYPE);

SoftwareSerial bluetooth(RX, TX);
 
void setup(){
  Serial.begin(9600);
  bluetooth.begin(9600); //블루투스 시작
  dht.begin(); //온도센서 시작
  pinMode(A1, INPUT);
}
 
void loop(){
  delay(8000); //10초마다 센서 데이터 보냄
  
  Serial.print("Rain:");
  Serial.println(analogRead(A0 ));
  int rain = analogRead(A0);
  int gas = analogRead(A1);
  
  if(gas < 900) {
    Serial.print("gas:");
    Serial.print(gas);
    Serial.print(" OK");
  }else if(gas>=900){
    Serial.print("gas:");
    Serial.print(gas);
    Serial.println(" WARNING");
  }

  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
    // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  String humi ="";
  humi.concat(h);
  String temp ="";
  temp.concat(t);
  String heat = "";
  heat.concat(hic);
  String ges = "";
  ges.concat(gas);
  String rein ="";
  rein.concat(rain);
  Serial.print(humi+ "," + temp+ "," +heat);
  

  bluetooth.print(humi + "," + temp + "," + heat + "," + ges + "," + rein);
  bluetooth.print(gas);
  bluetooth.print(rain);

  //블루투스 통신
  if (bluetooth.available()) {
    Serial.write(bluetooth.read());
  }
  if (Serial.available()) {
    bluetooth.write(Serial.read());
  }
}
