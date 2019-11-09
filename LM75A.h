#pragma once

#include "esphome.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sensor {

static const int UPDATE_INTERVAL_MS = 60000;

class LM75A : public PollingComponent, public Sensor {
 public:
  LM75A() : PollingComponent(UPDATE_INTERVAL_MS) {}
  void setup() override;
  void update() override;
};

}
}
