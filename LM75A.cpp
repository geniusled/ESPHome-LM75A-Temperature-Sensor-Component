#include "esphome.h"
#include "esphome/core/log.h"
#include "LM75A.h"

#define INVALID_LM75A_TEMPERATURE 1000

namespace esphome {
namespace sensor {

static const char *TAG = "LM75A";
static const int LM75A_BASE_ADDRESS = 0x48;
static const float LM75A_DEGREES_RESOLUTION = 0.125;
static const int LM75A_REG_ADDR_TEMP = 0;
     
void LM75A::setup() {
  ESP_LOGCONFIG(TAG, "Setting up LM75A...");
  Wire.begin();
}
  
void LM75A::update() {
  uint16_t real_result = INVALID_LM75A_TEMPERATURE;
  uint16_t i2c_received = 0;

  // Go to temperature data register
  Wire.beginTransmission(LM75A_BASE_ADDRESS);
  Wire.write(LM75A_REG_ADDR_TEMP);
  if(Wire.endTransmission()) {
    ESP_LOGD(TAG, "Transmission error.");
    return;
  }

  // Get content
  if (Wire.requestFrom(LM75A_BASE_ADDRESS, 2)) {
    Wire.readBytes((uint8_t*)&i2c_received, 2);
  } else {
    ESP_LOGD(TAG, "Can't read temperature.");
    return;
  }

  // Modify the value (only 11 MSB are relevant if swapped)
  int16_t refactored_value;
  uint8_t* ptr = (uint8_t*)&refactored_value;

  // Swap bytes
  *ptr = *((uint8_t*)&i2c_received + 1);
  *(ptr + 1) = *(uint8_t*)&i2c_received;

  // Shift data (left-aligned)
  refactored_value >>= 5;

  float real_value;
  if (refactored_value & 0x0400) {
    // When sign bit is set, set upper unused bits, then 2's complement
    refactored_value |= 0xf800;
    refactored_value = ~refactored_value + 1;
    real_value = (float)refactored_value * (-1) * LM75A_DEGREES_RESOLUTION;
  } else {
    real_value = (float)refactored_value *  LM75A_DEGREES_RESOLUTION;
  }

  ESP_LOGD(TAG, "Got Temperature=%.1f°C", real_value);
  publish_state(real_value);
} 
}
}
