#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include "_QEspNowScript.h"

//extern void readFile(fs::FS &fs, const char * path);

String readScriptFile(const char* path) {
 
  // attempt to mount LittleFS filesystem
  Serial.print("Mounting LittleFS filesystem...");
  if(!LittleFS.begin()){
    Serial.println("ERROR: LittleFS Mount Failed!");
    return "";
    }
  Serial.println("Mounted!");
  Serial.printf("Used/Total:%lu/%luKB\n", LittleFS.usedBytes()/1024L, LittleFS.totalBytes()/1024L);

  //readFile(LittleFS, "/command.txt");
  //const char * path="/command.txt";
  Serial.printf("---Starting Script File:Reading %s---\n", path);

  File file = LittleFS.open(path);
  if(!file || file.isDirectory()){
      Serial.println("- failed to open file for reading");
      return "";
      }
  String sScript;    
  while(file.available()){
        String sScriptLine = file.readStringUntil('\n');
        //Serial.printf("ScriptFile>%s\n",sScriptLine.c_str());
        sScript += "\t"+sScriptLine +"\n"; //\t for printing alignment!
        }
  Serial.printf("ScriptFile>\n%s",sScript.c_str());
  file.close();

  return sScript;
}