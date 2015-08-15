#include <TinyGPS.h>
#include <SoftwareSerial.h>
#include <SD.h>
SoftwareSerial gprsSerial(2, 3);
SoftwareSerial gps_Serial(6, 5);

static const uint32_t GPSBaud = 9600;

File sdLog;
TinyGPS gps;
void GPS(TinyGPS &gps);

unsigned long age, date, time, chars;
int year;
byte month, day, hour, minute, second, hundredths;
   
void d(int dly){delay(dly);ShowSerialData();}
void ShowSerialData(){while(gprsSerial.available()!=0)Serial.write(gprsSerial.read());}

void setup()
{
  gps_Serial.begin(GPSBaud);
  gprsSerial.begin(19200);
  Serial.begin(19200);
  


  Serial.print("Loading SDCARD>");
  pinMode(10, OUTPUT);
  if (!SD.begin(10)) {
    Serial.println("SDFAILED");
    return;
  }
  Serial.println("SD OK");
  d(1000);
  
}

void loop()
{
    bool newGPSDATA = false;
    unsigned long start = millis();
    
    int IR = pow(3027.4/analogRead(A0), 1.2134);

    Cache(String(IR));
    
    
    gps_Serial.listen();
    while (millis() - start < 1000) {
      if (gps_Serial.available()) {
        char c = gps_Serial.read();//Serial.write(c);
        if (gps.encode(c)) {
          newGPSDATA = true;
          Serial.println("newGPSDATA = TRUE");
        }
      }
    }
    
    if (newGPSDATA) {
       //GPS(gps);
       //Serial.println("-----VOIDING GPS");
      //long lat, lon;
      //float flat, flon;

      //unsigned short sentences, failed; 
      
      gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
      Serial.print("Date: "); Serial.print(static_cast<int>(month)); Serial.print("/"); 
      Serial.print(static_cast<int>(day)); Serial.print("/"); Serial.print(year);
      Serial.print("  Time: "); Serial.print(static_cast<int>(hour)+8); Serial.print(":"); 
      Serial.print(static_cast<int>(minute)); Serial.print(":"); Serial.print(static_cast<int>(second));
      Serial.print("."); Serial.print(static_cast<int>(hundredths));
      
      
      gprsSerial.listen();
      if (gprsSerial.available())
      Serial.write(gprsSerial.read());

      String m = String(static_cast<int>(month));
      String d = String(static_cast<int>(day));
      String y = String(year);
      String h = String(static_cast<int>(hour)+8);
      String mn = String(static_cast<int>(minute));
      String sec = String(static_cast<int>(second));
      String hun = String(static_cast<int>(hundredths));
      
//      String timeStamp = String(m+"/"+d+"/"+y+" - "+h+":"+mn+":"+sec+"."+hun);
      Send("{\"value\":\""+String(IR)+"\", \"time created\":\"" +m+"/"+d+"/"+y+" - "+h+":"+mn+":"+sec+"."+hun+"\"}");
    }
    


}

void Cache(String data){
  sdLog = SD.open("DATA.txt", FILE_WRITE);
  if (sdLog) {
    Serial.print("Writing to DATA.txt...");
    sdLog.println(data);
    sdLog.close();
    Serial.println("done.");
  } else {
    Serial.println("DATA.txt ERR!");
  }
}
    
void Send(String &value)
{
  int num;
  String le;
  String var;
  //var="{\"value\":\""+ value + "\"}";
  num=var.length();
  le=String(num);
  for(int i = 0;i<1;i++){
    gprsSerial.println("AT+CGATT?");
    d(2000);
  }
  ("AT+CSTT=\"web.vmc.net.co\"");d(1000);
  gprsSerial.println("AT+CIPSTART=\"tcp\",\"Xinner-suprstate-820.appspot.com\",\"80\"");d(3000);
  gprsSerial.println("AT+CIPSEND");d(3000);
  gprsSerial.print("POST /api/v1/farm/1001/variable/0001");d(100);
  gprsSerial.println(" HTTP/1.1");d(1000);
  gprsSerial.println("Content-Type: application/json");d(1000);
  gprsSerial.println("Content-Length: "+le);d(100);
  gprsSerial.print("X-Farm-Token: ");d(100);
  gprsSerial.println("\"token\"");d(1000);
  gprsSerial.println("Host: Xinner-suprstate-820.appspot.com");d(1000);
  gprsSerial.println();d(1000);
  gprsSerial.println(value);d(1000);
  gprsSerial.println();d(1000);
  gprsSerial.println((char)26);delay(5000);
  gprsSerial.println();ShowSerialData();
  gprsSerial.println("AT+CIPCLOSE");d(1000);
}

/*void GPS(TinyGPS &gps)
{
  long lat, lon;
  float flat, flon;
  unsigned long age, date, time, chars;
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned short sentences, failed;  
  
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  Serial.print("Date: "); Serial.print(static_cast<int>(month)); Serial.print("/"); 
  Serial.print(static_cast<int>(day)); Serial.print("/"); Serial.print(year);
  Serial.print("  Time: "); Serial.print(static_cast<int>(hour)+8); Serial.print(":"); 
  Serial.print(static_cast<int>(minute)); Serial.print(":"); Serial.print(static_cast<int>(second));
  Serial.print("."); Serial.print(static_cast<int>(hundredths));
  
  
  gprsSerial.listen();
  if (gprsSerial.available())
  Serial.write(gprsSerial.read());
  Send(String(IR)+"RICK");
}*/
