#include <ESP8266HTTPClient.h>



#include <Wire.h>
#include "MutichannelGasSensor.h"
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#include "Seeed_HM330X.h"
#include <NDIRZ16.h>
#include <SoftwareSerial.h>
#include "ESP8266WiFi.h"
#include <WiFiClient.h>


#include "fwsc.h"
const char* ssid     = "NIAQ";// replace subscribe with your WiFi SSID(Name)
const char* password = "indoorairquality";//replace with Your Wifi Password name
//const char* host = "35.239.124.91";

#include "graphql-esp.h"

const char*  graphqlHost = "api.naqm.ml"; // // Change this
const char* graphqlPath = "/v1/graphql"; // Change this
GraphqlEsp gq; // GraphQL Websocket handler




#define DHTTYPE DHT11   // DHT 11

#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE);
SoftwareSerial mySerial(D6,D5);
NDIRZ16 mySensor = NDIRZ16(&mySerial);

#ifdef  ARDUINO_SAMD_VARIANT_COMPLIANCE
  #define SERIAL SerialUSB
#else
  #define SERIAL Serial
#endif


HM330X sensor;
u8 buf[30];





SoftwareSerial mySerial22(D4,D7);


void setup()
{
  Serial.begin(115200);  // start serial for output
  delay(3000);

   mySerial22.begin(9600);
  
  
  dht.begin();
   Wire.begin(D2, D1);
//Wire.begin(I2C_SDA, I2C_SCL)
 
  
  gas.begin(0x04);//the default I2C address of the slave is 0x04
  gas.powerOn();
  mySerial.begin(9600);
  
 SERIAL.println("Serial start");
    if(sensor.init())
    {
        SERIAL.println("HM330X init failed!!!");
        while(1);
    }
    Wire.begin(D2, D1);


       Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, password);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");


      
      Serial.printf_P(PSTR("Connecting to GraphQL server\n"));
 
  gq.connect(graphqlHost, 443, graphqlPath);
   
 delay(2000);

 
}


u16 value1 =0;
u16 value2 =0;
u16 value3 =0;
void loop()
{
   
 //float c;
 float nh3;
 float co;
 float no2;
 float ch4;
 int co2;
 int h;
 int t;

     h = dht.readHumidity();
     t = dht.readTemperature();         

if(h != 2147483647){
     
    Serial.print("Humidity =");
    Serial.print(h);
    Serial.print("%,");
    Serial.print("Temp =");
    Serial.print(t); 
    Serial.print("C,");


     nh3 = gas.measure_NH3();
  Serial.print("NH3 =");
  if (nh3 >= 0) Serial.print(nh3);
  else Serial.print("invalid");
  Serial.print(" ppm,");

  co = gas.measure_CO();
  Serial.print("CO = ");
  if (co >= 0) Serial.print(co);
  else Serial.print("invalid");
  Serial.print(" ppm,");

  no2 = gas.measure_NO2();
  Serial.print("NO2 = ");
  if (no2 >= 0) Serial.print(no2);
  else Serial.print("invalid");
  Serial.print(" ppm,");

;

  ch4 = gas.measure_CH4();
  Serial.print("CH4 = ");
  if (ch4 >= 0) Serial.print(ch4);
  else Serial.print("invalid");
  Serial.print(" ppm,");


 if (mySensor.measure()) {
        Serial.print("CO2 = ");
        co2=mySensor.ppm;
        Serial.print(co2);
        Serial.print("ppm ,");
    }
  
if(sensor.read_sensor_value(buf,29))
    {
        SERIAL.println("HM330X read result failed!!!");
    }


    value1 = (u16)buf[5*2]<<8|buf[5*2+1];
    value2 = (u16)buf[6*2]<<8|buf[6*2+1];
    value3 = (u16)buf[7*2]<<8|buf[7*2+1];
        Serial.print("PM1 = ");
        Serial.print(value1);
        Serial.print("ug/m3,");

        Serial.print("PM2.5 = ");
        Serial.print(value2);
        Serial.print("ug/m3,");

        Serial.print("PM10 = ");
        Serial.print(value3);
        Serial.println("ug/m3,");




    
 
 Serial.println("...");
 Serial.println("..");




String datatowrite = String(t)+"&"+String(value2)+"&"+String(nh3)+"&"+String(co)+"&"+String(h)+"&"+String(co2)+"&"+String(no2)+"&"+String(ch4)+"*";
 // Serial.println(datatowrite);
  int len = datatowrite.length();
  //
  char array1[len+1];
  datatowrite.toCharArray(array1,len+1);
delay(100);
//



 
  mySerial22.write(array1);
delay(1000);



//senddata();
//HTTPClient http;
Serial.println("Sending Data");

gq.loop();


   // gq.mutation("insert_naqm_AirReading_one(object: {ch4: 1.15, co: 2.25, dust: 55, humidity: 19, latitude: 0, longitude: 0, no2: 0.95, node_id: 1, so2: 750, temperature: 21, nh3: 2.50}){ id }");

//String mutationstring = String("insert_naqm_AirReading_one(object: {ch4:")+String(1.15)+String(", dust:")+String(55)+String(", humidity:")+String(19)+String(", latitude:")+String(0)+String(", longitude:")+String(0)+String(", no2:")+String(0.95)+String(", node_id:")+String(26ccd8ec-269d-42ad-ba90-3f71f0e072ff)+String(", so2:")+String(750)+String(", temperature:")+String(21)+String(", nh3:")+String(2.15)+String("}){ id }");


String mutationstring = String("insert_naqm_AirReading_one(object: {ch4:")+String(ch4)+String(", co:")+String(co)+String(", dust:")+String(value2)+String(", humidity:")+String(h)+String(", latitude:")+String(0)+String(", longitude:")+String(0)+String(", no2:")+String(no2)+String(", node_id:")+String(2)+String(", so2:")+String(co2)+String(", temperature:")+String(t)+String(", nh3:")+String(nh3)+String("}){ id }");

int lenn = mutationstring.length();

  char array11[lenn+1];
  mutationstring.toCharArray(array11,lenn+1);
 
 gq.mutation(array11);





// String httpString=String("{\"nh3\":")+String(nh3)+","+String("\"co\":")+String(co)+","+String("\"no2\":")+String(no2)+","+String("\"so2\":")+String(co2)+","+String("\"dust\":")+String(value2)+","+String("\"ch4\":")+String(ch4)+","+String("\"temperature\":")+String(t)+","+String("\"humidity\":")+String(h)+String(",\"latitude\":0,\"longitude\":0,\"node_id\":2}");
 //String httpString=String("http://35.239.124.91:4322/store?204&")+String(nh3)+"&"+String(co)+"&"+String(no2)+"&"+String(ch4)+"&"+String(co2)+"&"+String(value2)+"&"+String(t)+"&"+String(h)+String("&*****&*****");
//Serial.println(httpString);
//http.begin("http://api.naqm.ml:8001/storage/");
//http.addHeader("Content-Type", "application/json");  
//int httpCode = http.POST(httpString);
//String payload = http.getString();
// Serial.print("HTTP GET Code: ");
//  Serial.println(httpCode);
//   Serial.print("Payload: ");
//  Serial.println(payload);
//
//
//  if(httpCode>0){
//    if(httpCode == 200){
//      Serial.println("Data sent");
//      
//      delay(250);
//    } else{
//      Serial.println("Server side error");
//      
//      delay(250);
//    }
//  } else{
//    Serial.println("HTTP Client Error");
//    
//    delay(250);
//  }
//  http.end();


    
  delay(30000);
}

else {
  Serial.println("Temperature value is  incorrect");
  }
  
}
