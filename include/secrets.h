

#ifndef secrets
#define secrets
#include "library_dependencies.h"



typedef struct wifi_Information{

    String ssid; 
    String password; 
    String* serverIp;

}wifi_Information;

extern wifi_Information* connectionInformation;

extern const int numberOfWifiRouters;

extern const int numberOfPotentialServers;

#endif
