#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "IoT Hub";
const char* password = "letsbuildsomething";

// initialize server port (standard HTTP)
ESP8266WebServer server(80);

const int LED = D5;
boolean state = false;

void setup(void) {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  Serial.begin(115200);
  // sets the mode to "STAtion" mode (server)
  // vs Access Point (AP) mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
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

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  // request to root url
  server.on("/", handleRoot);

  // request to /inline url (path)
  server.on("/inline", []() {
    server.send(200, "text/html", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}

/**
 * Get the html string to
 * be displayed in the browser
 */
String getHtml() {
  // standard html document type declaration
  String html = "<!DOCTYPE html>";
  html.concat("<html lang=\"en\">");
  html.concat("<body>");
  html.concat("<h2>Control LED</h2>");
  // create a form with buttons to turn on/off LED
  html.concat("<form method=\"get\" action=\"/\">");
  // name and values on input to trigger on/off
  html.concat("<input type=\"submit\" name=\"led\" value=\"on\"/>");
  html.concat("<input type=\"submit\" name=\"led\" value=\"off\"/>"); 
  html.concat("</form>");
  // display details of http request
  html += "<p>HTTP Request Details:</p>";
  html += "<p>URI: ";
  html += server.uri();
  html += "</p><p>Method: ";
  html += (server.method() == HTTP_GET) ? "GET" : "POST";
  html += "</p>Arguments: ";
  html += server.args();
  html += "</p>";
  for (uint8_t i = 0; i < server.args(); i++) {
    html += " " + server.argName(i) + ": " + server.arg(i) + "<br>";
  }   
  html.concat("</body>");
  html.concat("</html>");
  return html;
}

/**
 * Handle the request querystring
 * or name/value pair to trigger LED state
 */
void handleRequest() {
  if (server.argName(0) == "led") {
    if (server.arg(0) == "on") {
      digitalWrite(LED, HIGH);
    } else {
      digitalWrite(LED, LOW);      
    }
  }
}

/**
 * Function handler for root (/) request
 */
void handleRoot() {
  handleRequest();
  String message = getHtml();
  server.send(200, "text/html", message);
  digitalWrite(LED, state);
  state = (state) ? false : true;
}

/**
 * File not found (404) handler
 */
void handleNotFound() {
  String message = "File Not Found\n\n";
  server.send(404, "text/plain", message);
  digitalWrite(LED, LOW);
}
