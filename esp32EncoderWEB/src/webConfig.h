#include <libraries.h>
const char* ssid = "sciEncoder";  // Enter SSID here
const char* password = "12345678";  //Enter Password here
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

//webFunctions
void handle_OnConnect();
void handle_ledon() ;
void handle_ledoff() ;
void handle_encoder() ;
void handle_NotFound();
String SendHTML(float val);


void handle_encoder(){
  server.send(200, "text/html", SendHTML(1));
}

String SendHTML(float encoderVal){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP32 Weather Report</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>ESP32 Weather Report</h1>\n";
  
  ptr +="<p>Temperature: ";
  ptr +=(int)encoderVal;
  ptr +="°C</p>";
  
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}