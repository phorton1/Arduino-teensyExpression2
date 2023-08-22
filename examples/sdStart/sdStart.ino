//------------------------------------------------------------
// sdStart
//------------------------------------------------------------
// Trying to figure out why BUILTIN_SDCARD does not begin()
// on a soft reboot (from Arduino IDE rebuild)

#include <SdFat.h>

SdFat32 SD;

void setup()
{
	Serial.begin(115200);
	delay(1000);
	bool ok = SD.restart();

	bool sd_ok = SD.begin(BUILTIN_SDCARD);
	Serial.print("sdStart.ino   sd_ok=");
	Serial.println(sd_ok);
}


void loop()
{
}
