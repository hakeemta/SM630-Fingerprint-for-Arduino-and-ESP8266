# Fingerprint
SM630 Fingerprint Helper Library

This is a helper library for SM630 Fingerprint Module.

It provides a higher abstraction layer for adafruit/Adafruit-Fingerprint-Sensor-Library
[https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library]

NB: This library supports Arduino/ESP8266, while the base library does not support because it does not work with SoftwareSerial.
Thanks to @gilangwinduasmara for the PR [https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library/pull/24]

# Installation
1. Download and place in your Arduino project library

2. Include the header in your main program
    #include <Fingerprint_Helper.h>

3. Create an instance
		<br/>
    Fingerprint_Helper &nbsp;fingerH(&fpSerial, &FpDebug);
    
    <strong>Hints:</strong>
		<br/>
    fpSerial - can be either HardwareSerial or SoftwareSerial
		<br/>
    FpDebug - a simple callback for debugging
		<br/><br/>
    // Debug Callback
		<br/>
    void FpDebug(const char* s){
		<br/>
    	&nbsp; Serial.print(s);
			<br/>
    }

4. Initialize
    fingerH.begin(57600); // SM630 default baudrate 57600

5. Verify the presence of the module
		<br/>
    if(fingerH.verifyPassword()){
		<br/>
    	&nbsp;// Module found
			<br/>
    }
    
    
# Use cases:
1. Enroll a fingerprint
		<br/>
    int fpId = 20;
		<br/>
    fingerH.enrollPrint(fpId);

2. Identify a fingerprint
		<br/>
    int fpId;
		<br/>
    fpId = fingerH.identifyPrint();

3. Delete a fingerprint
		<br/>
		fpId = 20;
	  <br/>
   if(fingerH.deletePrint(fpId)){
	  <br/>
	 		&nbsp;// Fingerprint 20 deleted
			<br/>
    }
   

# TODO:
1. Provide examples
2. Document more use cases
