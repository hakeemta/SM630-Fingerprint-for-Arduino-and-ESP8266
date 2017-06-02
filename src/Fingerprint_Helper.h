/***************************************************
  This is a helper library for optical Fingerprint
 ****************************************************/

#include <Adafruit_Fingerprint.h>
#include <Pt2Debug.h>

#define TIMEOUT 5000  // milliseconds

class Fingerprint_Helper: public Adafruit_Fingerprint {
 public:
#if defined(__AVR__) || defined(ARDUINO_ESP8266_NODEMCU)
  Fingerprint_Helper(SoftwareSerial *, Pt2Debug::Char debug_);
#endif
  Fingerprint_Helper(HardwareSerial *, Pt2Debug::Char debug_);

  //
  void debug(const char* s);
  void debugln(const char* s);
  boolean captureImage(uint16_t timeout);
  boolean convertImage(uint8_t image);
  boolean fingerRemoved(uint16_t timeout);
  boolean matchModel(void);
  boolean saveModel(uint8_t id);
  boolean enrollPrint(uint8_t id);
  boolean loadModel(uint8_t id);
  boolean getModel(void);
  uint8_t * streamPrint(void);
  void displayPrint(uint8_t *p, Pt2Debug::Uint8_t disp);
  boolean uploadPrint(uint8_t id);
  boolean fastSearch();
  int identifyPrint(void);
  boolean deletePrint(uint8_t id);
  //uint16_t ...

 private:
  uint8_t state;
  Stream *mySerial;
  Pt2Debug::Char charDebug = NULL;
};
