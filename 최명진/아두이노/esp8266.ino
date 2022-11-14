

//ESP8266 데이터베이스


#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

#define FIREBASE_HOST "fire-detection-768d5-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "hyWURkSzb3xgIFk2oVQOihrBhmM3Wd24bWyWg8jh"
#define WIFI_SSID "SK_WiFiB7BC"
#define WIFI_PASSWORD "chlaudwls1!"                               //Password of your wifi network 



// Declare the Firebase Data object in the global scope
FirebaseData firebaseData;

// Declare global variable to store value
String values, sensor_data;
String ch, window_status;
String cp_humi, cp_temp, cp_rain, cp_gas, return_values;
String humi, temp, dust;
float humi2, temp2, dust2;
float humi3, temp3, dust3;
void setup() {
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);   // connect to firebase
  Firebase.reconnectWiFi(true);
  delay(1000);
}

void loop() {
  bool Sr = false;

  while (Serial.available()) {
    sensor_data = Serial.readString();
    Sr = true;
  }

  if (Sr == true) {
    values = sensor_data;
    int firstCommaIndex = values.indexOf(',');
    int secondCommaIndex = values.indexOf(',', firstCommaIndex + 1);
    //int thirdCommaIndex = values.indexOf(',', secondCommaIndex + 1);
    //int fourthCommaIndex = values.indexOf(',', thirdCommaIndex + 1);
    //get sensors data from values variable by  spliting by commas and put in to variables
    String humi_value = values.substring(0, firstCommaIndex);
    String temp_value = values.substring(firstCommaIndex + 1, secondCommaIndex);
    //String rain_value = values.subString(secondCommaIndex+1, thirdCommaIndex);
    //String gas_value = values.subString(thirdCommaIndex+1, fourthCommaIndex);

    Firebase.setString(firebaseData, "dht/humi", humi_value);
    Firebase.setString(firebaseData, "dht/temp", temp_value);
    //Firebase.setString(firebaseData, "dht/rain", rain_value);

    if (Firebase.getString(firebaseData, "conf/ch"))
    {
      ch = firebaseData.stringData(); // 채널
    }
    if (Firebase.getString(firebaseData, "conf/window_status"))
    {
      window_status = firebaseData.stringData(); // 창문 상태
    }

    if (Firebase.getString(firebaseData, "conf/humi"))
    {
      humi = firebaseData.stringData();
      humi3 = humi.toFloat();
      humi2 = humi_value.toFloat();

      if (humi3 >= humi2) { //설정습도가 현재습도 이상이면
        cp_humi = "1";
      }
      else { //설정습도가 현재습도보다 낮을때
        cp_humi = "0";
      }
    }

    if (Firebase.getString(firebaseData, "conf/temp"))
    {
      temp = firebaseData.stringData();
      temp3 = temp.toFloat();
      temp2 = temp_value.toFloat();

      if (temp3 >= temp2) { //설정온도가 현재온도 이상이면
        cp_temp = "1";
      }
      else { //설정온도가 현재온도보다 낮을때
        cp_temp = "0";
      }
    }


    //    if (Firebase.getString(firebaseData, "conf/dust"))
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


    if (Firebase.getString(firebaseData, "dht/rain"))
    {
      rain = firebaseData.stringData();
      rain2 = firebase.toInt();

      if (rain2 <= 950) { //빗물이 감지되면
        cp_rain = "1";
      }
      else {  //빗물이 감지되지 않으면
        cp_rain = "0";
      }
    }

    if (Firebase.getString(firebaseData, "dht/gas"))
    {
      gas = firebaseData.stringData();
      gas2 = firebase.toInt();

      if (gas2 <= 950) { //가스가 감지되면
        cp_gas = "1";
      }
      else {  //가스가 감지되지 않으면
        cp_gas = "0";
      }
    }



    return_values = (ch + "," + window_status + "," + cp_humi + "," + cp_temp + ","); //채널, 창문상태, 습도비교, 온도비교
    Serial.println(return_values);

  }


  delay(1000);


}
