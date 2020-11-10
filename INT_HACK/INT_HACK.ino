#include <LiquidCrystal_I2C.h>


#include <Wire.h> 
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "HX711.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
HX711 cell(2,0);
String data2;
// Replace with your network credentials
const char* ssid = "ASUS_X00TD"; //Enter Wi-Fi SSID
const char* password =  "1234567890"; //Enter Wi-Fi Password
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)
float val = 0;
float count = 0;
float weight = 0;

String page = "";
String text = "";
double data;
void setup(void) {
  Wire.begin();

  // Turn on the blacklight and print a message.
  lcd.init();
  lcd.setCursor(1, 0);
  lcd.backlight();
  lcd.print("filled : 90%");
  pinMode(A0, INPUT);
  delay(1000);
  Serial.begin(115200);
  WiFi.begin(ssid, password); //begin WiFi connection
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/data.txt", []() {
    text = (String)data +"%";
    server.send(200, "text/html", text);
  });
  server.on("/", []() {
    page = " <style> table,th,td { border : 2px solid red; } </style><h1 style=font-size:300%; ALIGN=CENTER >Real Time Intra Venous Fluid Level</h1> <table ALIGN = CENTER border=1 width=800 height=80  >  <tr> <th style=font-size:200%;> S.No. </th> <th style=font-size:200%;> Room No. </th> <th style=font-size:200%;> Fluid Level </th><th style=font-size:200%;> Alert</th></tr> <tr height= 10><td  ALIGN = CENTER style=font-size:150%;>1 </td><td  ALIGN = CENTER style=font-size:150%;> A2 </td><td  ALIGN = CENTER style=font-size:150%; id=\"data\"> ""</td><td ALIGN = CENTER style=font-size:150%;>" +data2+"</td></tr></table>\r\n";
    page += "<script>\r\n";
    page += "var x = setInterval(function() {loadData(\"data.txt\",updateData)}, 1000);\r\n";
    page += "function loadData(url, callback){\r\n";
    page += "var xhttp = new XMLHttpRequest();\r\n";
    page += "xhttp.onreadystatechange = function(){\r\n";
    page += " if(this.readyState == 4 && this.status == 200){\r\n";
    page += " callback.apply(xhttp);\r\n";
    page += " }\r\n";
    page += "};\r\n";
    page += "xhttp.open(\"GET\", url, true);\r\n";
    page += "xhttp.send();\r\n";
    page += "}\r\n";
    page += "function updateData(){\r\n";
    page += " document.getElementById(\"data\").innerHTML = this.responseText;\r\n";
    page += "}\r\n";
    page += "</script>\r\n";
    server.send(2000, "text/html", page);
  });

  server.begin();
  Serial.println("Web server started!");
}

void loop(void) {
    count = count + 1;
  
  // Use only one of these
  //val = ((count-1)/count) * val    +  (1/count) * cell.read(); // take long term average
  //val = 0.5 * val    +   0.5 * cell.read(); // take recent average
  val = cell.read(); // most recent reading
  
 // Serial.println( val );
  
  weight = -(-(910.0/(7975000.0-8349800.0))*(val-8349800.0));
  //data = weight;
  float filledlevel=(weight/1000.00)*100;
  data= filledlevel;
  
 // lcd.print("90%");
  //delay(1000);
  //lcd.clear();
  if(weight <= 0.0){
    Serial.println("0.0");
  }
  else{
  
  Serial.println(weight);
  }
  if(filledlevel<=10)
  {data2 = "DANGER";}
  else{data2="SAFE";}
  server.handleClient();
}
