#include <SoftwareSerial.h>

// this sketch will allow you to bypass the Atmega chip
// and connect the fingerprint sensor directly to the USB/Serial
// chip converter.
 
// Connect VIN to +5V
// Connect GND to Ground
// Connect GPS RX (data into GPS) to Digital 0
// Connect GPS TX (data out from GPS) to Digital 1
int RXPIN=2;
int TXPIN=3;

SoftwareSerial gps(RXPIN, TXPIN);
void setup() {
Serial.begin(115200);
Serial.println("Goodnight moon!");
gps.begin(9600);

}
void loop() {
if (gps.available())
    Serial.write(gps.read());

}
