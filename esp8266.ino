#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#define HOSTNAME "Fan"

int GPIOs[] = {0, 2};

int num_GPIO = sizeof(GPIOs)/sizeof(int);
String data;

ESP8266WebServer server(80);

void setup() {
  EEPROM.begin(96);
  delay(10);
  for (int i = 0; i < num_GPIO; i++) {
    pinMode(GPIOs[i], OUTPUT);
    digitalWrite(GPIOs[i], LOW);
  }
  if (!start_wifi()) start_ap();
  start_webserver();
  delay(10);
}

void reset() { while (1) {} }

bool start_wifi() {
  String wssid = "";
  String wpass = "";
  // Magic byte so we know at least something is written
  if (EEPROM.read(0) != 0b10101010) return false;
  for (int i = 1; i < 34; i++) {
    if (!EEPROM.read(i)) break;
    wssid += (char)EEPROM.read(i);
  }
  for (int i = 34; i < 96; i++) {
    if (!EEPROM.read(i)) break;
    wpass += (char)EEPROM.read(i);
  }
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.hostname(HOSTNAME);
  WiFi.begin(wssid.c_str(), wpass.c_str());
  // Try to connect to wifi for 6 seconds
  for (int i = 0; i < 12; i++) {
    if (WiFi.status() == WL_CONNECTED) return true;
    delay(500);
  }
  WiFi.disconnect();
  return false;
}

void start_ap() {
  WiFi.softAP(HOSTNAME);
}

void start_webserver() {
  server.begin();
  server.on("/", []() {
    // Summary page
    data  = "<!DOCTYPE HTML>\r\n<html>\r\n";
    for (int i = 0; i < num_GPIO; i++) {
      data += "GPIO"; data += GPIOs[i]; data+= ": ";
      data += digitalRead(GPIOs[i]) ? "High" : "Low";
      data += "<br>\r\n";
    }
    data += "</html>\n";
    server.send(200, "text/html", data);
  });
  server.on("/gpio", []() {
    for (int i = 0; i < num_GPIO; i++) {
      if (server.arg(String(GPIOs[i])) == "0") digitalWrite(GPIOs[i], LOW);
      if (server.arg(String(GPIOs[i])) == "1") digitalWrite(GPIOs[i], HIGH);
      if (server.arg(String(GPIOs[i])) == "toggle") digitalWrite(GPIOs[i], !digitalRead(GPIOs[i]));
    }
    server.sendHeader("Location", "/", true);
    server.send(307, "text/plain", "");
  });
  server.on("/settings", []() {
    String wssid = server.arg("ssid");
    String wpass = server.arg("pass");
    if (!wssid || !wpass) {
      server.send(200, "text/html", "<!DOCTYPE HTML>\r\n<html>You must specify an SSID and password</html>");
      return;
    }
    EEPROM.write(0, 0b10101010);
    for (int i = 0; i < wssid.length(); i++) {
      EEPROM.write(1+i, wssid[i]);
    }
    EEPROM.write(1+wssid.length(), 0);
    for (int i = 0; i < wpass.length(); i++) {
      EEPROM.write(34+i, wpass[i]);
    }
    EEPROM.write(34+wpass.length(), 0);
    EEPROM.commit();
    server.send(200, "text/html", "<!DOCTYPE HTML>\r\n<html>WiFi settings updated.<br>Now rebooting...</html>");
    delay(100);
    reset();
  });
}

void loop() {
  server.handleClient();
}
