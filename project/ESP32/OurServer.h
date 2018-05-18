#ifndef OurServer_h
#define OurServer_h

#include "Arduino.h"
#include "WiFi.h"

class OurServer {
    public:
        OurServer(String host, String path, String &sensor, String &instruction, 
                int &request_id, int esp_id);

        void start_wifi(String network);
        
        void start_wifi(String network, String password);

        bool get_instructions(String py_file);

        void post_result(String py_file, String &result);

        void set_wifi_mode(bool on);
    
    private:
        String& _sensor;
        String& _instruction;
        int& _request_id;
        String _host;
        String _path;
        int _esp_id;
        String _network;
        String _password;
};


#endif // !OurServer_h