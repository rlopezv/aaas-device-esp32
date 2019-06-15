#ifndef _ttgoplant_h_
#define _ttgoplant_h_

#include <lmic.h>
#include <hal/hal.h>

#define SENSOR_CLASS "plant"
#define SENSOR_TYPE 1
// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// This should also be in little endian format, see above.

//static const u1_t PROGMEM DEVEUI[8] = {0xea, 0xd3, 0x1f, 0x10, 0xc9, 0x61, 0x0e, 0x41};

static const u1_t PROGMEM DEVEUI[8] = {0x41, 0x0e, 0x61, 0xc9, 0x10, 0x1f, 0xd3, 0xea};

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
// The key shown here is the semtech default key.
static const u1_t PROGMEM APPKEY[16] = { 0x18, 0x79, 0xca, 0xfc, 0x3a, 0xc9, 0x49, 0xff, 0x96, 0x79, 0x4b, 0x8d, 0x13, 0xe0, 0xd9, 0xa5 };

#endif