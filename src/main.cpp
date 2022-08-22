// [RMSHE Infinty] PLC Firmware 2022.08.22 Powered by RMSHE
// MCU: ESP8266; MODULE: ESP12F;
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
ESP8266WebServer Server(80);

void handleNotFound() {
    Server.send(404, "text/plain", "404: Not found");
}

void SetRelayState(String GPIO, String SetState) {
    // Check "SetState" whether the parameters are correct;
    if (SetState == "0" || SetState == "1") {
        // Traverse GPIO and Check if the parameters are correct;
        if (GPIO == "5") {
            // Set GPIO level;
            if (SetState == "0")
                digitalWrite(5, LOW);
            else
                digitalWrite(5, HIGH);

        } else if (GPIO == "4") {
            if (SetState == "0")
                digitalWrite(4, LOW);
            else
                digitalWrite(4, HIGH);

        } else if (GPIO == "13") {
            if (SetState == "0")
                digitalWrite(13, LOW);
            else
                digitalWrite(13, HIGH);

        } else if (GPIO == "16") {
            if (SetState == "0")
                digitalWrite(16, LOW);
            else
                digitalWrite(16, HIGH);

        } else {
            // Return error report if GPIO does not exist;
            Server.send(404, "text/html", "GPIOERROR: GPIO" + GPIO + " that does not exist.");
        }
    } else {
        // Return an error report if the POST Parameter(keyword) does not exist
        Server.send(404, "text/html", "POSTERROR: GPIO_SetState Unsupported keyword.");
    }

    // Back to Operational Report;
    String RELAY_STATE = "";
    if (SetState == "0")
        RELAY_STATE = "POWUP";
    else
        RELAY_STATE = "POWDO";

    Server.send(200, "text/html", "GPIO: " + GPIO + "; SETSTATE: " + SetState + "; RELAY: " + RELAY_STATE + ";");
}

void GetRelayState(String GPIO, String SetState) {
    // GPIOs are defined by hardware;
    // Traverse GPIO and Check if the parameters are correct;
    // Determine the GPIO status and return a report in JSON format;

    if (GPIO == "5") {
        if (digitalRead(5) == 0)
            Server.send(200, "text/html", "{ \"OBJECT\":\"GETSTATE\" , \"GPIO\":\"0\" , \"RELAY\":\"POWUP\" }");
        else
            Server.send(200, "text/html", "{ \"OBJECT\":\"GETSTATE\" , \"GPIO\":\"1\" , \"RELAY\":\"POWDO\" }");

    } else if (GPIO == "4") {
        if (digitalRead(4) == 0)
            Server.send(200, "text/html", "{ \"OBJECT\":\"GETSTATE\" , \"GPIO\":\"0\" , \"RELAY\":\"POWUP\" }");
        else
            Server.send(200, "text/html", "{ \"OBJECT\":\"GETSTATE\" , \"GPIO\":\"1\" , \"RELAY\":\"POWDO\" }");

    } else if (GPIO == "13") {
        if (digitalRead(13) == 0)
            Server.send(200, "text/html", "{ \"OBJECT\":\"GETSTATE\" , \"GPIO\":\"0\" , \"RELAY\":\"POWUP\" }");
        else
            Server.send(200, "text/html", "{ \"OBJECT\":\"GETSTATE\" , \"GPIO\":\"1\" , \"RELAY\":\"POWDO\" }");

    } else if (GPIO == "16") {
        if (digitalRead(16) == 0)
            Server.send(200, "text/html", "{ \"OBJECT\":\"GETSTATE\" , \"GPIO\":\"0\" , \"RELAY\":\"POWUP\" }");
        else
            Server.send(200, "text/html", "{ \"OBJECT\":\"GETSTATE\" , \"GPIO\":\"1\" , \"RELAY\":\"POWDO\" }");

    } else {
        // Return error report if GPIO does not exist;
        Server.send(404, "text/html", "GPIOERROR: GPIO" + GPIO + " that does not exist.");
    }
}

void Control() {
    // Get request parameters;
    String GPIO = Server.arg("GPIO");          // Select the relay to control;
    String SetState = Server.arg("SETSTATE");  // Set relay State (low level control);
    String GetState = Server.arg("GETSTATE");  // Get relay State;

    // Here is for compatibility with more syntax;
    if (SetState == "HIGH")  // GPIO is set to high level;
        SetState = "1";
    if (SetState == "LOW")  // GPIO is set to low level;
        SetState = "0";

    if (SetState == "POWUP")  // Power up the relay;
        SetState = "0";
    if (SetState == "POWDO")  // Power dowm the relay;
        SetState = "1";
    // Tips: relay is low level control;

    //"GetState" and "SetState" cannot exist at the same time in a request(POST) command.
    // If they exist at the same time, the "GetState" command is given priority to prevent misoperation;
    if (Server.hasArg("GETSTATE")) {
        GetRelayState(GPIO, GetState);
    } else {
        SetRelayState(GPIO, SetState);
    }
}

void WiFi_Connect() {
    Serial.print("\nConnecting");
    WiFi.begin("RMSHE", "XXXXXXXX");
    while (WiFi.status() != WL_CONNECTED) {
        delay(300);
        Serial.print(".");
    }
    Serial.println("WiFi Connected\n");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    Serial.println("\n");
}

void setup(void) {
    Serial.begin(115200);
    WiFi_Connect();

    // relay is low level control;
    // The main board is powered on by default and the relay is powered off;
    pinMode(16, OUTPUT);
    digitalWrite(16, HIGH);

    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);

    pinMode(5, OUTPUT);
    digitalWrite(5, HIGH);

    Server.begin();

    Server.on("/", HTTP_POST, Control);
    Server.onNotFound(handleNotFound);
}

void loop(void) {
    Server.handleClient();

    if (WiFi.status() != WL_CONNECTED) {
        WiFi_Connect();
    }
}
