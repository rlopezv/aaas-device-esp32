#ifndef _ttgoweather_h_
#define _ttgoweather_h_

#include <lmic.h>
#include <hal/hal.h>

#define SENSOR_CLASS "weather"
#define SENSOR_TYPE 2


// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8] = {0x4d, 0xf4, 0x1e, 0xc4, 0xd0, 0x12, 0x35, 0x6d};

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
// The key shown here is the semtech default key.
static const u1_t PROGMEM APPKEY[16] = { 0xd8, 0x7f, 0xfb, 0x6a, 0x99, 0x9d, 0x55, 0x89, 0x60, 0x7a, 0x30, 0x1f, 0x86, 0xd7, 0xee, 0x0e };

#endif