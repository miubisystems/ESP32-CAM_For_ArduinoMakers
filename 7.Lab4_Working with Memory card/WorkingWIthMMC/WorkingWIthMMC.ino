#include "FS.h"                // Contains Declarations of Functions required for File System Creation, Access and Operations
#include "SD_MMC.h"            // Contains Declarations of Functions required for SDCard / MMC Acess, Read, Write etc
#include <EEPROM.h>            // Contains Declarations of Functions required for EEPROM Acess, Read, Write etc

// define the number of bytes you want to access
#define EEPROM_SIZE 1

int FileNumber = 0;

void setup() {//First Function That would Run when any Arduino Based Board Starts
   
  //Necessary Serial Function call to be done
  Serial.begin(115200);//Set Baud Rate
  Serial.setDebugOutput(true);// Let Debug Messages Appear
  Serial.println();//Just Prints a NewLine
  
  //Serial.println("Starting SD Card");
  if(!SD_MMC.begin()){
    Serial.println("SD Card Mount Failed");
    return;
  }

  //Inquire for Card Type
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached");
    return;
  }

  // initialize EEPROM with predefined size
  EEPROM.begin(EEPROM_SIZE);
  //Read the First Byte of EEPROM to know the running file number before Power off
  FileNumber = EEPROM.read(0) + 1;

  // Path where new file will be saved in SD Card
  String path = "/text" + String(FileNumber) +".txt";
  Serial.printf("New file name: %s\n", path.c_str());
  
  //Open the File in Write mode, Open will also Create the file if not present
  File file = SD_MMC.open(path.c_str(), FILE_WRITE);
  if(!file){//File Creation or Acess Error
    Serial.println("Failed to open file in writing mode");
  } 
  else {//If No Error
    file.println("This is Text File"); // random text Written to File
    Serial.printf("Saved file to path: %s\n", path.c_str());//Path of File Saved
    EEPROM.write(0, FileNumber);//Write Current Running File Number to First Byte of EEPROM
    EEPROM.commit();// Approve the Write Operation.
  }
  file.close();//Relinquish the memory allocation done to file handler
  delay(2000);
    
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10000);
}
