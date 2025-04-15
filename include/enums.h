
#ifndef enums
#define enums


//Enum variable for the parameter of the function that activates the waitingResponse
enum Setting {
    NO_WAIT,
    WAIT_FOR_RESPONSE,
    WAIT_A_BIT
  };
  
  //A way to know what sensor the device has and functions we will use 
  
  enum recognized_Sensor{
    NO_KNOWN_SENSOR,
    BME680,
    CCS811,
  };



#endif