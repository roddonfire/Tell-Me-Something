#include "OurServer.h"
#include "Arduino.h"
#include "WiFi.h"

OurServer::OurServer(String host, String path, String &sensor, 
                        String &instruction, int &request_id, int esp_id):
                        _sensor(sensor), _instruction(instruction), 
                        _request_id(request_id) {
    
    _host = host;
    _path = path;
    _esp_id = esp_id;
    _network = "";
    _password = "";
}

void OurServer::start_wifi(String network) {
    _network = network;
    WiFi.begin(network.c_str()); //attempt to connect to wifi
    int count = 0;                                 //count used for Wifi check times
    while (WiFi.status() != WL_CONNECTED && count < 20)
    {
        Serial.print(".");
        delay(500);
        count++;
    }
    delay(1000);
    if (WiFi.isConnected())
    { //if we connected then print our IP, Mac, and SSID we're on
        Serial.println("Connected to WiFi");
        delay(500);
    }
    else
    {                  //if we failed to connect just ry again.
        ESP.restart(); // restart the ESP
    }
    return;
}

void OurServer::start_wifi(String network, String password) {
    _network = network;
    _password = password;
    WiFi.begin(network.c_str(), password.c_str()); //attempt to connect to wifi
    int count = 0;                                 //count used for Wifi check times
    while (WiFi.status() != WL_CONNECTED && count < 20)
    {
        Serial.print(".");
        delay(500);
        count++;
    }
    delay(1000);
    if (WiFi.isConnected())
    { //if we connected then print our IP, Mac, and SSID we're on
        Serial.println("Connected to WiFi");
        delay(500);
    }
    else
    { //if we failed to connect just ry again.
        ESP.restart(); // restart the ESP
    }
    return;
}

bool OurServer::get_instructions(String py_file) {
    String response;
    WiFiClient client; //instantiate a client object
    if (client.connect("iesc-s1.mit.edu", 80))
    { //try to connect to 6.08 server host
        Serial.println("Connected to client, sending GET request...");
        client.println();
        Serial.println("GET " + _path + py_file + "?esp_id=" + _esp_id + " HTTP/1.1");
        Serial.println("Host: " + _host);

        client.println("GET " + _path + py_file + "?esp_id=" + _esp_id + " HTTP/1.1");
        client.println("Host: " + _host);
        client.print("\r\n");
        unsigned long count = millis();
        while (client.connected())
        { //while we remain connected read out data coming back
            String line = client.readStringUntil('\n');
            if (line == "\r")
            { //found a blank line!
                break;
            }
            if (millis() - count > 1000)
                break;
        }
        count = millis();
        while (client.available())
        { //read out remaining text (body of response)
            response += (char)client.read();
        }
        client.stop();
        // parse response
        Serial.println("Response: " + response);
        if (response.indexOf('~') != -1) {
            _request_id = response.substring(0, response.indexOf('~')).toInt();
            response = response.substring(response.indexOf('~') + 1);
            _sensor = response.substring(0, response.indexOf('~'));
            response = response.substring(response.indexOf('~') + 1);
            _instruction = response;
            return true;
        }
        else { // incorrect format returned
            return false;
        }
    } // end if client connected
    else
    {
        client.stop();
        return false;
    }
    return response;
}

void OurServer::post_result(String py_file, String &result) {
    if (result.length() < 3000)
        Serial.println(result);
    WiFiClient client; //instantiate a client object
    if (client.connect(_host.c_str(), 80))
    { //try to connect to iesc-s1.mit.edu
        String body = "";
        body.reserve(15000);
        body = "update=T&request_id=" + (String)_request_id + "&result=";
        body = "{\"update\":\"T\", \"request_id\":\"" + (String)_request_id + "\", \"result\":\"";
        client.println("POST " + _path + py_file + " HTTP/1.1");
        client.println("Host: " + _host);
        client.println("Content-Type: application/json");
        client.println("Content-Length: " + String(body.length() + result.length() + 2));
        client.print("\r\n");
        if (result.length() < 3000) {
            client.print(body + result + "\"}");
            Serial.println(body + result + "\"}");
        }
        else { // chunked post
            Serial.println("Printing chunked post to client");
            int len = result.length();
            int ind = 0;
            int jump_size = 3000;
            client.print(body);
            while (ind < len)
            {
                delay(100); //experiment with this number!
                if (ind + jump_size < len)
                    client.print(result.substring(ind, ind + jump_size));
                else
                    client.print(result.substring(ind));
                ind += jump_size;
            }
            client.print("\"}");
        }
        unsigned long count = millis();
        while (client.connected())
        { //while we remain connected read out data coming back
            String line = client.readStringUntil('\n');
            if (line == "\r")
            { //found a blank line!
                //headers have been received! (indicated by blank line)
                break;
            }
            if (millis() - count > 6000)
                break;
        }
        count = millis();
        String op; //create empty String object
        while (client.available())
        { //read out remaining text (body of response)
            op += (char)client.read();
        }
        Serial.println(op);
        client.stop();
    }
    else
    {
        client.stop();
        delay(300);
    }
}

void OurServer::set_wifi_mode(bool on) {
    if (!on)
        WiFi.mode(WIFI_OFF);
    else
        WiFi.begin(_network.c_str(), _password.c_str());
}
