

#include <LiquidCrystal.h>
#include <stdio.h>
//LiquidCrystal lcd(6, 7, 5, 4, 3, 2);
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);

#include <WiFi.h>
#include <HTTPClient.h>

HTTPClient http;

const char *ssid = "iotserver";
const char *password = "iotserver123";

int httpResponseCode;
String servername = "http://projectsfactoryserver.in/storedata.php?name=";
String accountname = "iot1681";
String field1 = "&s1=";
String field2 = "&s2=";
String field3 = "&s3=";
String field4 = "&s4=";
String field5 = "&s5=";
String payload="";

/*
#include "DHTesp.h"
#define DHTpin 21
DHTesp dht;
*/

#include "DHT.h"
#define DHTPIN 19    
#include "DHT.h"
#define DHTTYPE DHT11
//#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

int buzzer  = 21;
int mos     = 5;
int ldr     = 18;

int fan     = 22;
int pump    = 23;

int tempc=0,humc=0;
String mos_string="";
String ldr_string="";
 
unsigned char rcv,count,gchr,gchr1,robos='s';
 

int sti=0;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

/*
#define RXD2 16
#define TXD2 17
*/



int i=0,k=0,lop=0;
int  gps_status=0;
float latitude=0; 
float logitude=0;                       
String Speed="";
String gpsString="";
char *test="$GPRMC";


//int hbtc=0,hbtc1=0,rtrl=0;

unsigned char gv=0,msg1[10],msg2[11];
 float lati=0,longi=0;
unsigned int lati1=0,longi1=0;
unsigned char flat[5],flong[5];
char finallat[10],finallong[10];
String finallat1="";
String finallong1="";

 int ii=0,rchkr=0;
 
void beep()
{
  digitalWrite(buzzer, LOW);delay(2000);digitalWrite(buzzer, HIGH);
}
void okcheck()
{
  unsigned char rcr;
  do{
      rcr = Serial.read();
    }while(rcr != 'K');
}


//int sts1=0,sts2=0,cntlmk=0;

int cntlmk=0;
int stn=0;
int hbv=0;


void iot_send()
{
      lcd.setCursor(15,0);lcd.print("U");
      http.begin(servername + accountname + field1 + String(tempc) + field2 + String(humc) + field3 + mos_string + field4 + ldr_string);
      httpResponseCode = http.GET();
      if(httpResponseCode>0) 
        {
         payload="";
         //Serial.print("HTTP Response code: ");
         //Serial.println(httpResponseCode);
         payload = http.getString();
         //Serial.println(payload);
        }
      else 
        {
         ;
         //Serial.print("Error code: ");
         //Serial.println(httpResponseCode);
        }
      delay(3000); 
      lcd.setCursor(15,0);lcd.print(" ");
}         
         
void setup() 
{
  Serial.begin(9600);//serialEvent();
 // Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  pinMode(ldr, INPUT); pinMode(mos, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(fan, OUTPUT);pinMode(pump, OUTPUT);
  
  digitalWrite(buzzer, HIGH);
  digitalWrite(fan, LOW);digitalWrite(pump, LOW);

  lcd.begin(16, 2);  
  lcd.print("Integrated Env");
  lcd.setCursor(0,1);
  lcd.print("Analysis Plant Growth");
  delay(2500);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) 
       {
           delay(500);
       }
  //Serial.println(WiFi.localIP());
  delay(3000);

  dht.begin();

  lcd.clear(); 
  lcd.print("T:");
  lcd.setCursor(8,0);
  lcd.print("H:");
   
  lcd.setCursor(0,1);
  lcd.print("M:");//2
  lcd.setCursor(8,1);
  lcd.print("l:");//10
 
}

void loop() 
{    
  tempc = dht.readTemperature();
  humc = dht.readHumidity();

  lcd.setCursor(2,0);convertl(tempc);
  lcd.setCursor(10,0);convertl(humc);

  if(tempc > 40 || humc > 85)
    {
      digitalWrite(fan, HIGH);
      beep();
      iot_send();
    } 
  else
    {
      digitalWrite(fan, LOW);   
    }

 mos_string="";
 if(digitalRead(mos) == LOW)
   {
      lcd.setCursor(2,1);lcd.print("Wet");
      mos_string="Wet";
    //  iot_send();
    digitalWrite(pump, LOW);
   }   
 if(digitalRead(mos) == HIGH)
   {
      lcd.setCursor(2,1);lcd.print("Dry");
      mos_string="Dry";
      digitalWrite(pump, HIGH);
   }

 
 ldr_string="";
 if(digitalRead(ldr) == LOW)
   {
      lcd.setCursor(10,1);lcd.print("Light");
      ldr_string="Light";
   }   
 if(digitalRead(ldr) == HIGH)
   {
      lcd.setCursor(10,1);lcd.print("Dark ");
      ldr_string="Dark";
   }
      
 delay(1000);

 cntlmk++;

 if(cntlmk >= 50)
   {cntlmk = 0;
      iot_send();
   }
}




void serialEvent() 
{
  while (Serial.available()) 
        {
         
         char inChar = (char)Serial.read();
           if(inChar == '*')
             {
              gchr = Serial.read();
             }
           if(inChar == '#')
             {
              gchr1 = Serial.read();
             }  
        }
}

void gpsEvent()
{
  gpsString="";
  while(1)
  {
   //while (gps.available()>0)            //Serial incoming data from GPS

    while (Serial2.available() > 0)
   {
    //char inChar = (char)gps.read();
    char inChar = (char)Serial2.read();
     gpsString+= inChar;                    //store incoming data from GPS to temparary string str[]
     i++;
    // Serial.print(inChar);
     if (i < 7)                      
     {
      if(gpsString[i-1] != test[i-1])         //check for right string
      {
        i=0;
        gpsString="";
      }
     }
    if(inChar=='\r')
    {
     if(i>60)
     {
       gps_status=1;
       break;
     }
     else
     {
       i=0;
     }
    }
  }
   if(gps_status)
    break;
  }
}

void get_gps()
{
  lcd.clear();
  lcd.print("Getting GPS Data");
  lcd.setCursor(0,1);
  lcd.print("Please Wait.....");
 
   gps_status=0;
   int x=0;
   while(gps_status==0)
   {
    gpsEvent();
    int str_lenth=i;
    coordinate2dec();
    i=0;x=0;
    str_lenth=0;
   }
}

void coordinate2dec()
{
  String lat_degree="";
    for(i=17;i<=18;i++)         
      lat_degree+=gpsString[i];
      
  String lat_minut="";
     for(i=18;i<=19;i++)         
      lat_minut+=gpsString[i];
     for(i=21;i<=22;i++)         
      lat_minut+=gpsString[i];
      
  String log_degree="";
    for(i=29;i<=31;i++)
      log_degree+=gpsString[i];
  String log_minut="";
    for(i=32;i<=33;i++)
      log_minut+=gpsString[i];
    for(i=35;i<=36;i++)
      log_minut+=gpsString[i];
      
    Speed="";
    for(i=42;i<45;i++)          //extract longitude from string
      Speed+=gpsString[i];
      
     float minut= lat_minut.toFloat();
     minut=minut/60;
     float degree=lat_degree.toFloat();
     latitude=degree+minut;
     
     minut= log_minut.toFloat();
     minut=minut/60;
     degree=log_degree.toFloat();
     logitude=degree+minut;
}

void gps_convert()
{
  if(gps_status)
  {
 // Serial.println(gpsString);

  if(gpsString[0] == '$' && gpsString[1] == 'G' && gpsString[2] == 'P' && gpsString[3] == 'R' && gpsString[4] == 'M' && gpsString[5] == 'C')
    {
     // Serial.println("Don11111111111111111111111111111111111111111111111111111\r\n");      
    //  Serial.write(gpsString[18]);Serial.write(gpsString[19]);Serial.write(gpsString[20]);Serial.write(gpsString[21]);Serial.write(gpsString[22]);
     //lcd.setCursor(0,0);
     for(ii=0;ii<9;ii++)
       {
        //lcd.write(gpsString[19+ii]);    
        msg1[ii] = gpsString[19+ii];
        //Serial.write(msg1[ii]);
       }
       //Serial.println("\r\n");
     //lcd.setCursor(0,1);
      for(ii=0;ii<10;ii++)
       {
        //lcd.write(gpsString[32+ii]);
        msg2[ii] = gpsString[32+ii];    
       // Serial.write(msg2[ii]);
       }

// Serial.println(msg1);
// Serial.println(msg2);

       //lati = (((msg1[2]-48)*100000) +((msg1[3]-48)*10000) + ((msg1[5]-48)*1000) + ((msg1[6]-48)*100) + ((msg1[7]-48)*10) + (msg1[8]-48)); 
       //longi = (((msg2[3]-48)*100000) + ((msg2[4]-48)*10000) + ((msg2[6]-48)*1000) + ((msg2[7]-48)*100) + ((msg2[8]-48)*10) + (msg2[9]-48));

       lati = (((msg1[2]-48)*1000) + ((msg1[3]-48)*100) + ((msg1[5]-48)*10) + (msg1[6]-48)); 
       longi = (((msg2[3]-48)*1000) + ((msg2[4]-48)*100) + ((msg2[6]-48)*10) + (msg2[7]-48));

      // converts(lati);Serial.write("-");
      // converts(longi);Serial.write("\r\n");
       
       lati = (lati/60);  longi = (longi/60);

       lati = (lati*100); longi = (longi*100);
       lati1 = lati;      longi1 = longi;
       
// Serial.write("After ");
  //      converts(lati1);Serial.write("-");
    //   converts(longi1);Serial.write("\r\n");
 

              convlat(lati); convlong(longi);
        finallat[0] = msg1[0];
        finallat[1] = msg1[1];
        finallat[2] = '.';
        finallat[3] = flat[0]; finallat[4] = flat[1];finallat[5] = flat[2];finallat[6] = flat[3];finallat[7] = '\0';


        finallong[0] = msg2[0];
        finallong[1] = msg2[1];
        finallong[2] = msg2[2];
        finallong[3] = '.';
        finallong[4] = flong[0];finallong[5] = flong[1];finallong[6] = flong[2];finallong[7] = flong[3];finallong[8] = '\0'; 

   
    }
  }  
}

 void convlat(unsigned int value)  
      {
             unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;              
      c=c|0x30;
      e=e|0x30; 
    g=g|0x30;              
      h=h|0x30;
   
  // dlcd(a);
//   dlcd(c);dlcd(e); dlcd(g);dlcd(h);//lcddata('A');//lcddata(' ');lcddata(' ');
    
         
               flat[0] = c;
               flat[1] = e;
               flat[2] = g;
             flat[3] = h;


          }

 void convlong(unsigned int value)  
      {
            unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;              
      c=c|0x30;
      e=e|0x30; 
      g=g|0x30;              
      h=h|0x30;
   
  // dlcd(a);
//   dlcd(c);dlcd(e); dlcd(g);dlcd(h);//lcddata('A');//lcddata(' ');lcddata(' ');
    
         
               flong[0] = c;
               flong[1] = e;
               flong[2] = g;
               flong[3] = h;
}

void converts(unsigned int value)
{
  unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;               
      c=c|0x30;
      e=e|0x30; 
      g=g|0x30;              
      h=h|0x30;
    
     
   Serial.write(a);
   Serial.write(c);
   Serial.write(e); 
   Serial.write(g);
   Serial.write(h);
}

void convertl(unsigned int value)
{
  unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;               
      c=c|0x30;
      e=e|0x30; 
      g=g|0x30;              
      h=h|0x30;
    
     
  // lcd.write(a);
  // lcd.write(c);
   lcd.write(e); 
   lcd.write(g);
   lcd.write(h);
}
