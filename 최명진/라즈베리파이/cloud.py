import time
import pyrebase
from bluetooth import *

socket = BluetoothSocket( RFCOMM )
socket.connect(("98:DA:60:04:FB:02", 1))
print("bluetooth connected!")

config = {
  "apiKey": "hyWURkSzb3xgIFk2oVQOihrBhmM3Wd24bWyWg8jh",
  "authDomain": "fire-detection-768d5",
  "databaseURL": "https://fire-detection-768d5-default-rtdb.firebaseio.com/",
  "storageBucket": "project-id.appspot.com"
}

firebase = pyrebase.initialize_app(config)
db = firebase.database()

print("Send Data to Firebase Using Raspberry Pi")
print("----------------------------------------")
print()
db.child("dht").remove()  #전의 db기록을 삭제한다
while True:

    data = socket.recv(1024)
    #print("Message : {}".format(data))
    humi = data[0:5]
    temp = data[6:11]
    heat = data[12:17]
    gas = data[18:20]
    rain = data[21:25]
    humi_=humi.decode()
    temp_=temp.decode()
    heat_=heat.decode()
    gas_=gas.decode()
    rain_=rain.decode()
    print(humi_, temp_, heat_, gas_, rain_)
    datas = {
    "humi": humi_,
    "temp": temp_,
    "heat": heat_,
    "gas" : gas_,
    "rain": rain_
    }

    db.child("dht").update(datas) #데이터베이스를 수정하지않고 업데이트
    #db.child("dht").child("1-set").set(datas) data를 기록
    #db.child("dht").child("2-push").push(datas) push(데이터를 아래로 수시로 기록)

    time.sleep(2)

    if(data=="q"):
        print("Quit")
        break

socket.close()
