#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/stepper/stepper.h"

namespace esphome::l9110 {

enum L9110StepMode { L9110_STEP_MODE_FULL_STEP, L9110_STEP_MODE_HALF_STEP };

static const uint8_t L9110_FULL_STEP_SEQUENCE[4][4] = {{1, 0, 1, 0}, {1, 0, 0, 1}, {0, 0, 0, 1}, {0, 1, 1, 0}};
static const uint8_t L9110_HALF_STEP_SEQUENCE[8][4] = {{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0},
                                                       {0, 1, 1, 1}, {0, 0, 1, 1}, {0, 0, 0, 1}, {1, 0, 0, 1}};

class L9110 final : public stepper::Stepper, public Component {
 public:
  void set_pin_a(GPIOPin *pin_a) { pin_a_ = pin_a; }
  void set_pin_b(GPIOPin *pin_b) { pin_b_ = pin_b; }
  void set_pin_c(GPIOPin *pin_c) { pin_c_ = pin_c; }
  void set_pin_d(GPIOPin *pin_d) { pin_d_ = pin_d; }

  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }
  void set_sleep_when_done(bool sleep_when_done) { this->sleep_when_done_ = sleep_when_done; }
  void set_step_mode(L9110StepMode step_mode) { this->step_mode_ = step_mode; }

 protected:
  void write_step_(int32_t step, int32_t dir);
  uint8_t get_step_sequence_index_(int32_t step, uint8_t total_steps, int32_t dir);

  bool sleep_when_done_{false};
  GPIOPin *pin_a_;
  GPIOPin *pin_b_;
  GPIOPin *pin_c_;
  GPIOPin *pin_d_;
  L9110StepMode step_mode_{L9110_STEP_MODE_FULL_STEP};
  HighFrequencyLoopRequester high_freq_;
  int32_t current_l9110_pos_{0};
};
};  // end of namespace esphome::l9110
