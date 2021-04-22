//Conexion Wifi
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include "DHT.h"

int TempHum= D1;
int Temp, Hum;
DHT dht (TempHum,DHT11);

//-------------------VARIABLES GLOBALES--------------------------
int contconexion = 0;

const char *ssid = "MikroTik_Home";
const char *password = "0m3r0.2020*";

unsigned long previousMillis = 0;

char host[48];
String strhost = "192.168.5.24";
String strurl = "/Domotica/Php/enviardatos.php";
String Estacion = "1002";
String chipid = "";

//-------Función para Enviar Datos a la Base de Datos SQL--------

String enviardatos(String datos) {
  String linea = "error";
  WiFiClient client;
  strhost.toCharArray(host, 49);
  if (!client.connect(host, 80)) {
    Serial.println("Fallo de conexion mysql");
    return linea;
  }

  client.print(String("POST ") + strurl + " HTTP/1.1" + "\r\n" + 
               "Host: " + strhost + "\r\n" +
               "Accept: */*" + "*\r\n" +
               "Content-Length: " + datos.length() + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded" + "\r\n" +
               "\r\n" + datos);           
  delay(10);             
  
  //Serial.print("Enviando datos a SQL...");
  Serial.print("On Line...");
  unsigned long timeout = millis();
  
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println("Cliente fuera de tiempo!");
      client.stop();
      return linea;
    }
  }
  // Lee todas las lineas que recibe del servidor y las imprime por la terminal serial
  while(client.available()){
    linea = client.readStringUntil('\r');
  }  
  Serial.println(linea);
  return linea;
}

//-------------------------------------------------------------------------

void setup() {

  // Inicia Serial
  Serial.begin(115200);
  dht.begin();
  Serial.println("");
  
  //Serial.print("chipId: "); 
  chipid = String(ESP.getChipId());
  //chipid = 1500;
 // Serial.println(chipid); 

  // Conexión WIFI
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED and contconexion <50) { //Cuenta hasta 50 si no se puede conectar lo cancela
    ++contconexion;
    delay(500);
    Serial.print(".");
  }
  if (contconexion <50) {
      //para usar con ip fija
      IPAddress ip(192,168,5,100); 
      IPAddress gateway(192,168,5,1); 
      IPAddress subnet(255,255,255,0); 
      WiFi.config(ip, gateway, subnet); 
      
      Serial.println("");
      Serial.println("WiFi conectado");
      Serial.println(WiFi.localIP());
  }
  else { 
      Serial.println("");
      Serial.println("Error de conexion wifi");
  }
}

//--------------------------LOOP--------------------------------
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 30000) { //envia la temperatura cada 30 segundos
    previousMillis = currentMillis;
 //========================================
   Hum= dht.readHumidity();
   Temp= dht.readTemperature();
 //========================================    
    enviardatos("Estacion=" + Estacion +"&chipid=" + chipid + "&temperatura=" + String(Temp) + "&Humedad=" + String(Hum));
  }
}
