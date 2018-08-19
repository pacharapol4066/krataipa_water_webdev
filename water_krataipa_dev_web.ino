#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <LiquidCrystal_I2C.h>

//------------------------------------------
//SETUP RELAY
#define RELAY0_PIN D3
#define RELAY1_PIN D4
#define RELAY2_PIN D5
#define RELAY3_PIN D6
#define RELAY4_PIN D7
#define RELAY5_PIN D8

//------------------------------------------
ESP8266WebServer server(80);
LiquidCrystal_I2C lcd(0x3F, 16, 2);

#define WIFI_SSID "krataipa"
#define WIFI_PASSWORD "wildrabbit"
#define HOST_NAME "KRATAIPA001"

int relaySolinoidState[6] = {0,0,0,0,0,0};

void setup() {
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  
  //Setup pin mode
  pinMode(RELAY0_PIN, OUTPUT);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);
  pinMode(RELAY5_PIN, OUTPUT);
  SW_all_off(); //Turn off all relays

  //Setup WIFI 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.hostname( HOST_NAME );
  lcd.setCursor(0, 0);
  lcd.print("Init WIFI");

  //Wait for WIFI connection
  while( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    lcd.print(".");
  }
  lcd.setCursor(0, 0);
  lcd.print(WiFi.localIP());

  //Setup HTTP server
  server.on("/", handleRoot);
  server.on("/sw", handleSW);
  server.onNotFound(handleNotFound);
  server.begin();
  lcd.setCursor(0, 1);
  lcd.print("HTTP server started");
  
}
 
void loop() {
  server.handleClient();
}

void SW_all_off(){
  digitalWrite(RELAY0_PIN, LOW);
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);
  digitalWrite(RELAY3_PIN, LOW);
  digitalWrite(RELAY4_PIN, LOW);
  digitalWrite(RELAY5_PIN, LOW);
}

int checkPumpOpen(){
  if(relaySolinoidState[0] == 1 || relaySolinoidState[1] == 1 || relaySolinoidState[2] == 1 || relaySolinoidState[3] == 1 || relaySolinoidState[4] == 1)
      return 1;
  else
      return 0;
}

int checkValveClose(){
  if(relaySolinoidState[0] == 0 && relaySolinoidState[1] == 0 && relaySolinoidState[2] == 0 && relaySolinoidState[3] == 0 && relaySolinoidState[4] == 0 && relaySolinoidState[5] == 1)
      return 1;
  else
      return 0;
}

void SW(byte num, bool sw, int flage){
  switch( num ){
    case 0: digitalWrite(RELAY0_PIN, sw); relaySolinoidState[0] = flage; break;
    case 1: digitalWrite(RELAY1_PIN, sw); relaySolinoidState[1] = flage; break;
    case 2: digitalWrite(RELAY2_PIN, sw); relaySolinoidState[2] = flage; break;
    case 3: digitalWrite(RELAY3_PIN, sw); relaySolinoidState[3] = flage; break;
    case 4: digitalWrite(RELAY4_PIN, sw); relaySolinoidState[4] = flage; break;
    case 5: digitalWrite(RELAY5_PIN, sw); relaySolinoidState[5] = flage; break;
  }

  if(checkValveClose() == 1)
  {
    digitalWrite(RELAY5_PIN, sw); 
    relaySolinoidState[5] = flage;
  }
}

void handleRoot(){
  String html = "<!DOCTYPE html>\r\n";
  html += "<html>";
  html += "<head>";
  html += "<meta charset='UTF-8'>\r\n";
  html += "<style>.button {background-color: #195B6A; border: none; color: white; padding: 16px 40px;text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}";
  html += ".button2 {background-color: #77878A;}</style>";
  html += "</head>";
  html += "<title>ESP8266</title>";
  html += "<body>";

  html += "ESP8266 - ";
  html += HOST_NAME;
  html += "</br>";

  if(digitalRead(RELAY0_PIN)){
    html += "<a href='sw?sw=on1'><button class='button'>ON</button></a>";
  }else{
    html += "<a href='sw?sw=off1'><button class='button  button2'>OFF</button></a>";
  }
  if(digitalRead(RELAY1_PIN)){
    html += "<a href='sw?sw=on2'><button class='button'>ON</button></a>";
  }else{
    html += "<a href='sw?sw=off2'><button class='button  button2'>OFF</button></a>";
  }
  if(digitalRead(RELAY2_PIN)){
    html += "<a href='sw?sw=on3'><button class='button'>ON</button></a>";
  }else{
    html += "<a href='sw?sw=off3'><button class='button  button2'>OFF</button></a>";
  }
  if(digitalRead(RELAY3_PIN)){
    html += "<a href='sw?sw=on4'><button class='button'>ON</button></a>";
  }else{
    html += "<a href='sw?sw=off4'><button class='button  button2'>OFF</button></a>";
  }
  if(digitalRead(RELAY4_PIN)){
    html += "<a href='sw?sw=on5'><button class='button'>ON</button></a>";
  }else{
    html += "<a href='sw?sw=off5'><button class='button  button2'>OFF</button></a>";
  }
  if(digitalRead(RELAY5_PIN)){
    html += "<a href='sw?sw=on6'><button class='button'>ON</button></a>";
  }else{
    html += "<a href='sw?sw=off6'><button class='button  button2'>OFF</button></a>";
  }
  
  html += "</body>";
  html += "</html>\r\n";
  server.send(200, "text/html", html);
}

void handleSW(){

  int returnValveStat = checkValveClose();
  int returnPumpStat = checkPumpOpen();
  
  if (server.arg("sw")== "off1"){
    SW(0, HIGH, 1);
  }
  else if (server.arg("sw")== "on1"){
    SW(0, LOW, 0);
  }
  else if (server.arg("sw")== "off2"){
    SW(1, HIGH, 1);
  }
  else if (server.arg("sw")== "on2"){
    SW(1, LOW, 0);
  }
  else if (server.arg("sw")== "off3"){
    SW(2, HIGH, 1);
  }
  else if (server.arg("sw")== "on3"){
    SW(2, LOW, 0);
  }
  else if (server.arg("sw")== "off4"){
    SW(3, HIGH, 1);
  }
  else if (server.arg("sw")== "on4"){
    SW(3, LOW, 0);
  }
  else if (server.arg("sw")== "off5"){
    SW(4, HIGH, 1);
  }
  else if (server.arg("sw")== "on5"){
    SW(4, LOW, 0);
  }
  else if (server.arg("sw")== "off6")
  {
     if(returnPumpStat == 1){SW(5, HIGH, 1);}
  }
  else if (server.arg("sw")== "on6"){
    SW(5, LOW, 0);
  }

  String ip = WiFi.localIP().toString();
  server.sendHeader("Location", String("http://") + ip, true);
  server.send ( 302, "text/plain", "");
  server.client().stop();
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
