
#ifndef enums
#define enums


//Enum variable for the parameter of the function that activates the waitingResponse
enum Setting {
    WAITING,
    FINISHED_SUCCESSFULLY,
    FINISHED_UNSUCCESSFULLY
  };
  
  //A way to know what sensor the device has and functions we will use 
  
  enum recognized_Sensor{
    NO_KNOWN_SENSOR,
    BME680,
    CSS811,
  };



#endif