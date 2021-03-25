//These could single time tasks to make it more efficient, one core could wait for wifi input while the other core reads SD card (if that works that way)
void generalSetup(void){
  // Scan for Wifi, prompt for User to input (?)
  wifiSetup();


  //Read SD card (could be concurrent due to dualcore?)
  SDCardSetup();


  //Connect to WebServer
  webServerSetup();


  //Set local time
  localTimeSetup();


  //Set Alarm interrupts
  AlarmSetup();


  //Set SDCard interrupts
  SDCardInterruptSetup();

  //etc
};

//these could be moved merged with general setup or into another file

void wifiSetup(void){
  
};


void SDCardSetup(void){
  
};

 
void webServerSetup(void){
  
};


void localTimeSetup(){
  
};


void AlarmSetup(){
  
};

void SDCardInterruptSetup(){
  
};
