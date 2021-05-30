#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <String.h>

#include "DHT.h"
#define DHTPIN 14
#define DHTTYPE DHT11

const char* ssid     = "ssid";              //Name of the WiFi connection
const char* password = "password";          //Password of WiFi connection

String header;

// Assign static IP of NodeMCU
IPAddress host_IP(192, 168, 8, 99);         //IP Address of NodeMCU
IPAddress gateway(192, 168, 8, 1);          //Network Gateway
IPAddress subnet(255, 255, 255, 0);         //Subnet Mask

ESP8266WebServer server(80);

const char* host_name = "node00";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(9600);
    WiFi.hostname(host_name);
    if (!WiFi.config(host_IP, gateway, subnet)) {
        Serial.println("STA Failed to configure");
    }
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected.");
    Serial.println(WiFi.localIP());

    server.on("/", handle_PageNotFound);
    server.on("/metrics", handle_Metrics);
    server.begin();
    Serial.println("HTTP server started");
    dht.begin();
}

void loop(){
    server.handleClient();
}

void handle_Metrics(){
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
        server.send(404, "text/plain", "Failed to read from DHT sensor.");
        return;
    }
    server.send(200, "text/plain", getMetrics(h, t));
}

void handle_PageNotFound(){
    server.send(404, "text/plain", "Page not found");
}

String getMetrics(float h, float t){
    float hic = dht.computeHeatIndex(t, h, false);
    String content ;
    content = "humidity ";
    content += String(h, 2) + "\n";
    content += "temperature ";
    content += String(t, 2) + "\n";
    content += "heat_index ";
    content += String(hic, 2);
    return content;
}