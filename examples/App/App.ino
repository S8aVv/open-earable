/* 
 * This sketch let's you control OpenEarable via the provided dashboard or edge-ml.org (embedded ML no-code framework).
 * 
 * OpenEarable Dashboard: openearable.github.io/dashboard/
 * edge-ml: app.edge-ml.org
 * 
 * Firmware-version: 1.3.0
 * Release-date: 6.10.2023
*/

#include "Arduino.h"
#include "OpenEarable.h"

// Set DEBUG to true in order to enable debug print
#define DEBUG false

void setup()
{
    Serial.begin(0);
    delay(200);
    Serial.println("=== App.ino setup START ===");
    Serial.flush();

#if DEBUG
  Serial.begin(115200);
  open_earable.debug(Serial);
#endif

    open_earable.begin();
}

void loop()
{
  // Update and then sleep
    open_earable.update();
}
