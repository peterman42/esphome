#include "l9110.h"

using namespace esphome::l9110;

ESPHOME_LOG_TAG(TAG, "l9110.stepper");

static const LogString *step_mode_to_log_string(L9110StepMode mode) {
  switch (mode) {
    case L9110_STEP_MODE_FULL_STEP:
      return LOG_STR("FULL STEP");
    case L9110_STEP_MODE_HALF_STEP:
      return LOG_STR("HALF STEP");
    default:
      return LOG_STR("UNKNOWN");
  }
}

void L9110::setup() {
  this->pin_a_->setup();
  this->pin_b_->setup();
  this->pin_c_->setup();
  this->pin_d_->setup();
  this->loop();
}

void L9110::loop() {
  int32_t dir = this->should_step_();
  if (dir == 0 && this->has_reached_target()) {
    this->high_freq_.stop();
    if (this->sleep_when_done) {
      this->pin_a_->digital_write(false);
      this->pin_b_->digital_write(false);
      this->pin_c_->digital_write(false);
      this->pin_d_->digital_write(false);
    }
    // do not write pos
    return;
  } else {
    this->high_freq_.start();
    this->current_l9110_pos_ += dir;
  }

  this->write_step_(this->current_l9110_pos_);
}

void L9110::dump_config() {
  ESP_LOGCONFIG(TAG,
                "L9110:\n"
                "  Sleep when done: %s",
                YESNO(this->sleep_when_done_));
  LOG_PIN("  Pin A: ", this->pin_a_);
  LOG_PIN("  Pin B: ", this->pin_b_);
  LOG_PIN("  Pin C: ", this->pin_c_);
  LOG_PIN("  Pin D: ", this->pin_d_);
  ESP_LOGCONFIG(TAG, "  Step Mode: %s", LOG_STR_ARG(step_mode_to_log_string(this->step_mode_)));
}

void L9110::write_step_(int32_t step, int32_t dir) {
  if (dir == 0) {
    return;
  }
  uint8_t total_steps = this->step_mode_ == L9110_STEP_MODE_HALF_STEP ? 8 : 4;
  auto i = this->get_step_sequence_index_(step, total_steps, dir);
  switch (this->step_mode) {
    case L9110_STEP_MODE_FULL_STEP: {
      this->pin_a_->digital_write(L9110_FULL_STEP_SEQUENCE[i][0]);
      this->pin_b_->digital_write(L9110_FULL_STEP_SEQUENCE[i][1]);
      this->pin_c_->digital_write(L9110_FULL_STEP_SEQUENCE[i][2]);
      this->pin_d_->digital_write(L9110_FULL_STEP_SEQUENCE[i][3]);
      break;
    }
    case L9110_STEP_MODE_HALF_STEP: {
      this->pin_a_->digital_write(L9110_HALF_STEP_SEQUENCE[i][0]);
      this->pin_b_->digital_write(L9110_HALF_STEP_SEQUENCE[i][1]);
      this->pin_c_->digital_write(L9110_HALF_STEP_SEQUENCE[i][2]);
      this->pin_d_->digital_write(L9110_HALF_STEP_SEQUENCE[i][3]);
      break;
    }
    default: {
      ESP_LOGE(TAG, "Unknown step mode");
      break;
    }
  }
}

uint8_t L9110::get_step_sequence_index_(int32_t step, uint8_t total_steps, int32_t dir) {
  if (dir < 0) {
    return (total_steps - (step % total_steps)) % total_steps;
  } else {
    return step % total_steps;
  }
}
