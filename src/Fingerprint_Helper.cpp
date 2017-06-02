/***************************************************
  This is a helper library for optical Fingerprint
 ****************************************************/

#include "Fingerprint_Helper.h"


#if defined(__AVR__) || defined(ARDUINO_ESP8266_NODEMCU)
Fingerprint_Helper::Fingerprint_Helper(SoftwareSerial *ss, Pt2Debug::Char debug_) : Adafruit_Fingerprint(ss) {
  mySerial = ss;
  charDebug = debug_;
}
#endif
Fingerprint_Helper::Fingerprint_Helper(HardwareSerial *ss, Pt2Debug::Char debug_) : Adafruit_Fingerprint(ss) {
  mySerial = ss;
  charDebug = debug_;
}

void Fingerprint_Helper::debug(const char* s){
  charDebug(s);
}

void Fingerprint_Helper::debugln(const char* s){
  debug(s);
  debug("\r\n");
}


boolean Fingerprint_Helper::captureImage(uint16_t timeout){
  state = -1;
  int start_at = millis();
  while (state != FINGERPRINT_OK && millis()-start_at < timeout) {
    state = getImage();
    switch (state) {
      case FINGERPRINT_OK:
        debugln("Image Taken");
        return true;
      case FINGERPRINT_NOFINGER:
        debug(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        debug("Comm. Error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        debug("Imaging Error");
        break;
      default:
        debug("Unknown Error");
        break;
    }
  }

  debug("\r\n");
  return false;
}


boolean Fingerprint_Helper::convertImage(uint8_t image){
  if(image == 0){
    state = image2Tz();
  }
  else{
    state = image2Tz(image);
  }

  switch (state) {
    case FINGERPRINT_OK:
      debugln("Image Converted");
      return true;
    case FINGERPRINT_IMAGEMESS:
      debugln("Messy Image");
      return false;
    case FINGERPRINT_PACKETRECIEVEERR:
      debugln("Comm. Error");
      return false;
    case FINGERPRINT_FEATUREFAIL:
      debugln("Feature Fail");
      return false;
    case FINGERPRINT_INVALIDIMAGE:
      debugln("Invalid Image");
      return false;
    default:
      debugln("Unknown Error");
      return false;
  }
}


boolean Fingerprint_Helper::fingerRemoved(uint16_t timeout){
  state = 0;
  int start_at = millis();
  while (state != FINGERPRINT_NOFINGER && millis()-start_at < timeout) {
    state = getImage();
    debug(".");
  }

  debug("\r\n");

  if(state != FINGERPRINT_NOFINGER){
    debugln("Timeout");
    return false;
  }
  return true;
}


boolean Fingerprint_Helper::matchModel(){
  state = createModel();
  switch (state) {
    case FINGERPRINT_OK:
      debugln("Prints Matched");
      return true;
    case FINGERPRINT_PACKETRECIEVEERR:
      debugln("Comm. Error");
      return false;
    case FINGERPRINT_ENROLLMISMATCH:
      debugln("Prints Mismatch");
      return false;
    default:
      debugln("Unknown Error");
      return false;
    }
}


boolean Fingerprint_Helper::saveModel(uint8_t id){
  state = storeModel(id);
  switch (state) {
    case FINGERPRINT_OK:
      debugln("Print Stored");
      return true;
    case FINGERPRINT_PACKETRECIEVEERR:
      debugln("Comm. Error");
      return false;
    case FINGERPRINT_BADLOCATION:
      debugln("Bad Location");
      return false;
    case FINGERPRINT_FLASHERR:
      debugln("Flash Error");
      return false;
    default:
      debugln("Unknown Error");
      return false;
  }
}


boolean Fingerprint_Helper::enrollPrint(uint8_t id) {
  // Capture print, return false if failed
  if(!captureImage(10000)){
    return false;
  }

  debugln("Remove Finger");
  debugln("Converting Image");
  // Convert the first image, return if failed
  if(!convertImage(1)){
    return false;
  }

  // return false if finger not removed
  if(!fingerRemoved(10000)){
    return false;
  }

  // Then revalidate
  debugln("Place Same Finger Again to Validate");
  // Return false if recapture is not successful
  if(!captureImage(10000)){
    return false;
  }

  debugln("Converting Image");
  // Convert the second image
  if(!convertImage(2)){
    return false;
  }

  debugln("Creating Model");
  // Compare and create model, return false if failed
  if(!matchModel()){
    return false;
  }

  debugln("Print Enrolled");
  // Then store model
  if(!saveModel(id)){
    return false;
  }

  // Finally
  return true;
}


boolean Fingerprint_Helper::loadModel(uint8_t id){
  state = loadModel(id);
  switch (state) {
    case FINGERPRINT_OK:
      debugln("Print Loaded");
      return true;
    case FINGERPRINT_PACKETRECIEVEERR:
      debugln("Comm. Error");
      return false;
    default:
      debugln("Unknown Error");
      return false;
  }
}


boolean Fingerprint_Helper::getModel(){
  state = getModel();
  switch (state) {
    case FINGERPRINT_OK:
      debugln("Transferring Model");
      return true;
    default:
      debugln("Unknown Error");
      return false;
  }
}

uint8_t * Fingerprint_Helper::streamPrint(){
  debugln("Streaming Print");
  uint8_t templateBuffer[256];
  memset(templateBuffer, 0xff, 256);  //zero out template buffer
  int index=0;
  uint32_t starttime = millis();

  while ((index < 256) && ((millis() - starttime) < 1000))
  {
    if (mySerial->available())
    {
      templateBuffer[index] = mySerial->read();
      debug(".");
      index++;
    }
  }

  debug("\r\n");
  // debug((const char*) index);
  debugln(" Bytes Read");
  debugln("Streamed Print");
  return templateBuffer;
}


void Fingerprint_Helper::displayPrint(uint8_t *p, Pt2Debug::Uint8_t disp){
  debug("\r\n");

// #if defined(ARDUINO_ESP8266_NODEMCU)
//   ESP.wdtDisable();
// #endif

  //dump entire printBuf. This prints out 16 lines of 16 bytes
  for (int count= 0; count < 16; count++)
  {
    delay(0);
    for (int i = 0; i < 16; i++)
    {
      delay(0);
      debug("0x");
      disp(p[count*16+i]);
      debug(", ");
    }
    debugln("");
  }
  debug("\r\n");

  // #if defined(ARDUINO_ESP8266_NODEMCU)
  //   ESP.wdtEnable(0);
  // #endif
}


boolean Fingerprint_Helper::uploadPrint(uint8_t id){
  debugln("Loading Print");
  // Load print, return false if failed
  if(!loadModel(id)){
    return false;
  }

  debugln("Retrieving Model");
  // Retrive model, return false if failed
  if(!getModel()){
    return false;
  }

  return true;
}

boolean Fingerprint_Helper::fastSearch(){
  state = fingerFastSearch();
  switch (state) {
    case FINGERPRINT_OK:
      debugln("Found a Match");
      return true;
    case FINGERPRINT_PACKETRECIEVEERR:
      debugln("Comm. Error");
      return false;
    case FINGERPRINT_NOTFOUND:
      debugln("Match Not Found");
      return false;
    default:
      debugln("Unknown Error");
      return false;
  }
}


int Fingerprint_Helper::identifyPrint() {
  // Capture print, return false if failed
  if(!captureImage(10000)){
    return -1;
  }

  debugln("Converting Image");
  // Convert the first image, return if failed
  if(!convertImage(0)){
    return -1;
  }

  debugln("Searching Print");
  // Convert the first image, return if failed
  if(!fastSearch()){
    return -1;
  }

  // debug("Found ID: #"); debug(fingerID); debug(" with Confidence: "); debugln(confidence);
  return fingerID;
}


boolean Fingerprint_Helper::deletePrint(uint8_t id) {
  state = -1;
  state = deleteModel(id);

  switch (state) {
    case FINGERPRINT_OK:
      debugln("Print Deleted");
      return true;
    case FINGERPRINT_PACKETRECIEVEERR:
      debugln("Comm. Error");
      return false;
    case FINGERPRINT_BADLOCATION:
      debugln("Bad Location");
      return false;
    case FINGERPRINT_FLASHERR:
      debugln("Flash Error");
      return false;
    default:
      debugln("Unknown Error");
      return false;
  }

}
