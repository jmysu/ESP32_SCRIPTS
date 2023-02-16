/* Checks and writes out the files on a LittleFS filesystem on ESP8266 and ESP32 platforms
   This sketch only performs READ operations on the LittleFS filesystem and should not modify the filesystem's contents.

   NOTE: The LittleFS image must have already been uploaded prior to using this sketch.
*/
#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "_QEspNowScript.h"

StaticJsonDocument<512> jsonDoc; ///< JSON document

// Via https://github.com/espressif/arduino-esp32/blob/master/libraries/LittleFS/examples/LITTLEFS_test/LITTLEFS_test.ino
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
        }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
        }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
              listDir(fs, file.path(), levels -1);
              }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
            }
        file = root.openNextFile();
        }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

// via https://github.com/espressif/arduino-esp32/blob/master/libraries/LittleFS/examples/LITTLEFS_test/LITTLEFS_test.ino
void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
        }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
        }
    Serial.println("");
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("- file renamed");
    } else {
        Serial.println("- rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}

//=============================================================================
void LittleFSlistFS() {
  //Serial.begin(115200);
  //while (!Serial) {
  //  ; // wait for serial port to connect. Needed for native USB
  //  }

  // attempt to mount LittleFS filesystem
  Serial.print("Mounting LittleFS filesystem...");
  if(!LittleFS.begin()){
    Serial.println("ERROR: LittleFS Mount Failed!");
    return;
    }
  Serial.println("Mounted!");
  
  Serial.printf("Used/Total:%lu/%luKB\n", LittleFS.usedBytes()/1024L, LittleFS.totalBytes()/1024L);

  // list everything on the filesystem '/' directory where secrets is expected
  listDir(LittleFS, "/", 3);

  // check if WipperSnapper's `secrets.json` file exists on filesystem
  if (!LittleFS.exists("/secrets.json")) {
    Serial.println("ERROR: Unable to find secrets.json file on LittleFS filesystem!");
    //while (1); // wait forever
    Play_Pacman();
    return;
    }

  // read out the secrets.json file in plain-text
  //readFile(LittleFS, "/secrets.json");

  // Test we can open the secrets file using ArduinoJSON
  File secretsFile = LittleFS.open("/secrets.json", "r");
  if (!secretsFile) {
    Serial.println("ERROR: Unable to open secrets.json!");
    while (1); // wait forever
    }
/*
    size_t FileSize = secretsFile.size();
    Serial.println(FileSize);
    // Allocate a buffer to store contents of the file.
    std::unique_ptr<char[]> buf(new char[FileSize + 1]);
    secretsFile.readBytes(buf.get(), FileSize);
    Serial.print(F("\nJSON parseObject() result : "));
    DeserializationError err  = deserializeJson(jsonDoc, buf.get(), FileSize);
*/
  // Test we can deserialize the secrets file using ArduinoJSON
  DeserializationError err = deserializeJson(jsonDoc, secretsFile);
  if (err) {
    Serial.print("ERROR: Failed to deserialize secrets.json file with code: ");
    Serial.println(err.c_str());
    while (1); // wait forever
    }

  // Test parsing username from secrets.json file
  const char *io_username = jsonDoc["io_username"];
  // error check against default values [ArduinoJSON, 3.3.3]
  if (io_username == nullptr) {
    Serial.println("ERROR: Failed to parse io_username!");
    while(1); // wait forever
    }

  // Get IO key from JSON
  const char *io_key = jsonDoc["io_key"];
  // error check against default values [ArduinoJSON, 3.3.3]
  if (io_key == nullptr) {
    Serial.println("ERROR: Failed to parse io_key!");
    while(1); // wait forever
    }

  // Parse SSID
  const char *network_type_wifi_native_network_ssid = jsonDoc["network_type_wifi_native"]["network_ssid"];
  if (network_type_wifi_native_network_ssid == nullptr) {
    Serial.println("ERROR: Failed to parse SSID!");
    while(1); // wait forever
    }

  // Parse password
  const char *network_type_wifi_native_network_password = jsonDoc["network_type_wifi_native"]["network_password"];
  // error check against default values [ArduinoJSON, 3.3.3]
  if (network_type_wifi_native_network_password == nullptr) {
    Serial.println("ERROR: Failed to parse PASS!");
    while(1); // wait forever
    }

  Serial.println("Parsed `secrets.json` values...");
  Serial.print("\tio_username: "); Serial.println(io_username);
  Serial.print("\tio_key: "); Serial.println(io_key);

  String sSSID = String(network_type_wifi_native_network_ssid);
  Serial.print("\tWiFi SSID: "); 
  for (int i=0;i<sSSID.length();i++) Serial.print((i<5?sSSID.substring(i,i+1):"*"));
  Serial.println();

  String sPASS = String(network_type_wifi_native_network_password);
  Serial.print("\tWiFi Password: ");
  for (int i=0;i<sPASS.length();i++) Serial.print((i<5?sPASS.substring(i,i+1):"*"));
  Serial.println();
 
  // close the file
  secretsFile.close();

  // clear the JSON document and release all memory from the memory pool
  jsonDoc.clear();

  // close fs
  //LittleFS.end();

  Serial.println("DONE!");
  Play_RampUp();
}
