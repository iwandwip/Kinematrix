export interface PresetTemplate {
  id: string
  name: string
  description: string
  mode: number
  cppCode: string
  estimatedFrames: number
  complexity: 'Basic' | 'Intermediate' | 'Advanced' | 'Expert'
}

export const presetTemplates: PresetTemplate[] = [
  {
    id: 'sequence2',
    name: 'Blink All',
    description: 'Creates comprehensive blinking patterns including all LEDs and half sections. Features dual-phase blinking with progressive timing.',
    mode: 2,
    complexity: 'Basic',
    estimatedFrames: 45,
    cppCode: `void taskSequence2BlinkAll() {
    // blink ////////////////////////////////////////
    {
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 15; ++j) {
                for (int k = 0; k < config_data_ptr_->header.pin_size_; k++) {
                    set(config_data_ptr_->header.pin_ptr_[k], HIGH);
                }
                sleep(channel_data_.delay_time_);
                for (int k = 0; k < config_data_ptr_->header.pin_size_; k++) {
                    set(config_data_ptr_->header.pin_ptr_[k], LOW);
                }
                sleep(channel_data_.delay_time_);
            }
            sleep(500);
        }
        off();
    }
    // half blink ////////////////////////////////////////
    {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < (config_data_ptr_->header.pin_size_ / 2); j++) {
                set(config_data_ptr_->header.pin_ptr_[j], HIGH);
            }
            sleep(channel_data_.delay_time_);
            for (int j = 0; j < (config_data_ptr_->header.pin_size_ / 2); j++) {
                set(config_data_ptr_->header.pin_ptr_[j], LOW);
            }
            sleep(channel_data_.delay_time_);
        }
        sleep(50);
        for (int i = 0; i < 4; ++i) {
            for (int j = (config_data_ptr_->header.pin_size_ / 2); j < config_data_ptr_->header.pin_size_; j++) {
                set(config_data_ptr_->header.pin_ptr_[j], HIGH);
            }
            sleep(channel_data_.delay_time_);
            for (int j = (config_data_ptr_->header.pin_size_ / 2); j < config_data_ptr_->header.pin_size_; j++) {
                set(config_data_ptr_->header.pin_ptr_[j], LOW);
            }
            sleep(channel_data_.delay_time_);
        }
        sleep(50);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < (config_data_ptr_->header.pin_size_ / 2); j++) {
                set(config_data_ptr_->header.pin_ptr_[j], HIGH);
            }
            sleep(channel_data_.delay_time_);
            for (int j = 0; j < (config_data_ptr_->header.pin_size_ / 2); j++) {
                set(config_data_ptr_->header.pin_ptr_[j], LOW);
            }
            sleep(channel_data_.delay_time_);
        }
        sleep(500);
        off();
    }
    // half blink ////////////////////////////////////////
    {
        for (int i = 0; i < 4; ++i) {
            for (int j = (config_data_ptr_->header.pin_size_ / 2); j < config_data_ptr_->header.pin_size_; j++) {
                set(config_data_ptr_->header.pin_ptr_[j], HIGH);
            }
            sleep(channel_data_.delay_time_);
            for (int j = (config_data_ptr_->header.pin_size_ / 2); j < config_data_ptr_->header.pin_size_; j++) {
                set(config_data_ptr_->header.pin_ptr_[j], LOW);
            }
            sleep(channel_data_.delay_time_);
        }
        sleep(50);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < (config_data_ptr_->header.pin_size_ / 2); j++) {
                set(config_data_ptr_->header.pin_ptr_[j], HIGH);
            }
            sleep(channel_data_.delay_time_);
            for (int j = 0; j < (config_data_ptr_->header.pin_size_ / 2); j++) {
                set(config_data_ptr_->header.pin_ptr_[j], LOW);
            }
            sleep(channel_data_.delay_time_);
        }
        sleep(50);
        for (int i = 0; i < 4; ++i) {
            for (int j = (config_data_ptr_->header.pin_size_ / 2); j < config_data_ptr_->header.pin_size_; j++) {
                set(config_data_ptr_->header.pin_ptr_[j], HIGH);
            }
            sleep(channel_data_.delay_time_);
            for (int j = (config_data_ptr_->header.pin_size_ / 2); j < config_data_ptr_->header.pin_size_; j++) {
                set(config_data_ptr_->header.pin_ptr_[j], LOW);
            }
            sleep(channel_data_.delay_time_);
        }
        off();
        sleep(500);
    }
}

taskSequence2BlinkAll();`
  },

  {
    id: 'sequence3',
    name: 'Fill Two Point',
    description: 'Implements two-point filling pattern starting from edges and progressing inward. Uses symmetric LED activation with cascading effects.',
    mode: 3,
    complexity: 'Intermediate',
    estimatedFrames: 30,
    cppCode: `void taskSequence3FillTwoPoint() {
    // fill 2 point ////////////////////////////////////////
    {
        for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; i += 2) {
            for (int j = 0; j < 8; ++j) {
                set(config_data_ptr_->header.pin_ptr_[i], HIGH);
                set(config_data_ptr_->header.pin_ptr_[i + 1], HIGH);
                sleep(channel_data_.delay_time_);
                set(config_data_ptr_->header.pin_ptr_[i], LOW);
                set(config_data_ptr_->header.pin_ptr_[i + 1], LOW);
                sleep(channel_data_.delay_time_);
            }
            set(config_data_ptr_->header.pin_ptr_[i], HIGH);
            set(config_data_ptr_->header.pin_ptr_[i + 1], HIGH);
            sleep(300);
            if (i < (config_data_ptr_->header.pin_size_ / 2) - 1) {
                for (int j = 0; j < 8; ++j) {
                    set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ - 1) - i], HIGH);
                    set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ - 1) - (i + 1)],
                        HIGH);
                    sleep(channel_data_.delay_time_);
                    set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ - 1) - i], LOW);
                    set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ - 1) - (i + 1)], LOW);
                    sleep(channel_data_.delay_time_);
                }
                set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ - 1) - i], HIGH);
                set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ - 1) - (i + 1)], HIGH);
                sleep(300);
            }
        }
        for (int i = config_data_ptr_->header.pin_size_; i > 0; --i) {
            set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
            sleep(channel_data_.delay_time_ * 2);
        }
        off();
        sleep(500);
    }
}

taskSequence3FillTwoPoint();`
  },

  {
    id: 'sequence4',
    name: 'Fill Right',
    description: 'Right-fill sequence where LEDs activate in cascading pattern from left to right, then deactivate in reverse order.',
    mode: 4,
    complexity: 'Basic',
    estimatedFrames: 25,
    cppCode: `void taskSequence4FillRight() {
    // fill right ////////////////////////////////////////
    {
        for (int i = config_data_ptr_->header.pin_size_; i > 0; --i) {
            for (int j = 0; j < i; j++) {
                set(config_data_ptr_->header.pin_ptr_[j], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                set(config_data_ptr_->header.pin_ptr_[j], LOW);
            }
            set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);
        }
        for (int i = config_data_ptr_->header.pin_size_; i > 0; --i) {
            set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
            sleep(channel_data_.delay_time_ * 2);
        }
        off();
        sleep(500);
    }
}

taskSequence4FillRight();`
  },

  {
    id: 'sequence5',
    name: 'Fill In',
    description: 'Center-in filling pattern that creates symmetric operations from outer edges toward center with complex mathematical timing.',
    mode: 5,
    complexity: 'Intermediate',
    estimatedFrames: 35,
    cppCode: `void taskSequence5FillIn() {
    // fill in ////////////////////////////////////////
    {
        for (int j = 0; j < config_data_ptr_->header.pin_size_ / 2; ++j) {
            for (int i = config_data_ptr_->header.pin_size_; i > config_data_ptr_->header.pin_size_ / 2 + j; --i) {
                set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);
                set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - i], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
                set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - i], LOW);
            }
            for (int i = j; i < config_data_ptr_->header.pin_size_ / 2; ++i) {
                set(config_data_ptr_->header.pin_ptr_[i + config_data_ptr_->header.pin_size_ / 2], HIGH);
                set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2 - 1) - i], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                set(config_data_ptr_->header.pin_ptr_[i + config_data_ptr_->header.pin_size_ / 2], LOW);
                set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2 - 1) - i], LOW);
            }
            for (int i = config_data_ptr_->header.pin_size_ - 1;
                 i > config_data_ptr_->header.pin_size_ / 2 + j; --i) {
                set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);
                set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - i], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
                set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - i], LOW);
            }
            set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ / 2 + j], HIGH);
            set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2 - 1) - j], HIGH);
        }
        for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; ++i) {
            set(config_data_ptr_->header.pin_ptr_[i + config_data_ptr_->header.pin_size_ / 2], LOW);
            set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2 - 1) - i], LOW);
            sleep(channel_data_.delay_time_ * 2);
        }
        off();
        sleep(500);
    }
}

taskSequence5FillIn();`
  },

  {
    id: 'sequence6',
    name: 'Blink One By One',
    description: 'Individual LED blinking with complex timing calculations. Features mathematical mapping and half-size logic for advanced patterns.',
    mode: 6,
    complexity: 'Advanced',
    estimatedFrames: 40,
    cppCode: `void taskSequence6BlinkOneByOne() {
    {
        int halfSize = config_data_ptr_->header.pin_size_ / 2;

        for (int i = config_data_ptr_->header.pin_size_; i > 0; --i) {
            int time = map(config_data_ptr_->header.pin_size_ - i, 1, config_data_ptr_->header.pin_size_, channel_data_.delay_time_ + (channel_data_.delay_time_ / 2), channel_data_.delay_time_ / 2);
            for (int j = config_data_ptr_->header.pin_size_ - 1; j >= config_data_ptr_->header.pin_size_ - i; --j) {
                if (j >= halfSize) {
                    set(config_data_ptr_->header.pin_ptr_[halfSize + (config_data_ptr_->header.pin_size_ - 1) - j], HIGH);
                    sleep(time);
                    set(config_data_ptr_->header.pin_ptr_[halfSize + (config_data_ptr_->header.pin_size_ - 1) - j], LOW);
                } else {
                    set(config_data_ptr_->header.pin_ptr_[j], HIGH);
                    sleep(time);
                    set(config_data_ptr_->header.pin_ptr_[j], LOW);
                }
            }
            if (i > halfSize) {
                set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - i], HIGH);
            } else {
                set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - (halfSize - i) - 1], HIGH);
            }
        }
        sleep(channel_data_.delay_time_ * 12);

        for (int i = config_data_ptr_->header.pin_size_; i > 0; --i) {
            if (i > halfSize) set(config_data_ptr_->header.pin_ptr_[halfSize + (config_data_ptr_->header.pin_size_ - i)], LOW);
            else set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
            sleep(channel_data_.delay_time_);
        }

        for (int j = 0; j < 2; ++j) {
            for (int i = 0; i < config_data_ptr_->header.pin_size_; ++i) {
                if (i < halfSize) set(config_data_ptr_->header.pin_ptr_[i], HIGH);
                else set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - (i - halfSize) - 1], HIGH);
                sleep(channel_data_.delay_time_);
            }
            for (int i = config_data_ptr_->header.pin_size_; i > 0; --i) {
                if (i > halfSize) set(config_data_ptr_->header.pin_ptr_[halfSize + (config_data_ptr_->header.pin_size_ - i)], LOW);
                else set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
                sleep(channel_data_.delay_time_);
            }
        }

        for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; ++i) {
            set(config_data_ptr_->header.pin_ptr_[i], HIGH);
            set(config_data_ptr_->header.pin_ptr_[halfSize + i], HIGH);
            sleep(channel_data_.delay_time_);
        }
        for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; ++i) {
            set(config_data_ptr_->header.pin_ptr_[i], LOW);
            set(config_data_ptr_->header.pin_ptr_[halfSize + i], LOW);
            sleep(channel_data_.delay_time_);
        }

        for (int j = 0; j < 2; ++j) {
            for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; ++i) {
                set(config_data_ptr_->header.pin_ptr_[halfSize - i - 1], HIGH);
                set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - i - 1], HIGH);
                sleep(channel_data_.delay_time_);
            }
            for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; ++i) {
                set(config_data_ptr_->header.pin_ptr_[i], LOW);
                set(config_data_ptr_->header.pin_ptr_[halfSize + i], LOW);
                sleep(channel_data_.delay_time_);
            }
        }

        sleep(channel_data_.delay_time_ * 18);

        for (int i = 0; i < 2; ++i) {
            on();
            sleep(channel_data_.delay_time_ * 3);
            off();
            sleep(channel_data_.delay_time_ * 3);
        }

        sleep(channel_data_.delay_time_ * 18);
    }
}

taskSequence6BlinkOneByOne();`
  },

  {
    id: 'sequence7',
    name: 'Blink Two Fill',
    description: 'Two-point blinking pattern that progressively fills from edges with completion logic. Simpler than sequence 3 but effective.',
    mode: 7,
    complexity: 'Intermediate',
    estimatedFrames: 20,
    cppCode: `void taskSequence7BlinkTwoFill() {
    // blink 2 fill ////////////////////////////////////////
    {
        for (int j = 0; j < config_data_ptr_->header.pin_size_ / 2; ++j) {
            for (int i = config_data_ptr_->header.pin_size_ / 2; i > j; --i) {
                for (int k = 0; k < 4; ++k) {
                    set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);
                    set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - i], HIGH);
                    sleep(channel_data_.delay_time_);
                    set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
                    set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - i], LOW);
                    sleep(channel_data_.delay_time_);
                }
            }
            set(config_data_ptr_->header.pin_ptr_[j], HIGH);
            set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ - 1) - j], HIGH);
        }
        off();
        sleep(500);
    }
}

taskSequence7BlinkTwoFill();`
  },

  {
    id: 'sequence8',
    name: 'Snake And Reverse',
    description: 'Snake pattern that moves forward then reverses direction. Uses variable timing for acceleration/deceleration effects.',
    mode: 8,
    complexity: 'Basic',
    estimatedFrames: 15,
    cppCode: `void taskSequence8SnakeAndReverse() {
    // snake and snake reverse ////////////////////////////////////////
    {
        off();
        for (float k = channel_data_.delay_time_ * 2;
             k >= channel_data_.delay_time_; k -= channel_data_.delay_time_) {
            for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
                set(config_data_ptr_->header.pin_ptr_[i], HIGH);
                sleep(k);
            }
            for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
                set(config_data_ptr_->header.pin_ptr_[i], LOW);
                sleep(k);
            }
            for (int j = config_data_ptr_->header.pin_size_; j > 0; j--) {
                set(config_data_ptr_->header.pin_ptr_[j - 1], HIGH);
                sleep(k);
            }
            for (int j = config_data_ptr_->header.pin_size_; j > 0; j--) {
                set(config_data_ptr_->header.pin_ptr_[j - 1], LOW);
                sleep(k);
            }
        }
        sleep(500);
    }
}

taskSequence8SnakeAndReverse();`
  },

  {
    id: 'sequence9',
    name: 'Random Pattern',
    description: 'Random LED activation using pseudo-lambda functions. Creates unpredictable patterns with three-phase random selection.',
    mode: 9,
    complexity: 'Advanced',
    estimatedFrames: 25,
    cppCode: `void taskSequence9Random() {
    auto setRandomLed = [&](int value) -> void {
        int data[config_data_ptr_->header.pin_size_];
        for (int i = 0; i < config_data_ptr_->header.pin_size_; ++i) {
            data[i] = !value;
        }
        for (int i = 0; i < config_data_ptr_->header.pin_size_; ++i) {
            int index;
            do {
                index = random(config_data_ptr_->header.pin_size_);
            } while (data[index] == value);
            data[index] = value;
            for (int j = 0; j < config_data_ptr_->header.pin_size_; ++j) {
                set(config_data_ptr_->header.pin_ptr_[j], data[j]);
            }
            sleep(channel_data_.delay_time_);
        }
    };
    for (int i = 0; i < 3; ++i) {
        setRandomLed(HIGH);
        setRandomLed(LOW);
    }
}

taskSequence9Random();`
  },

  {
    id: 'sequence10',
    name: 'Wave Pattern',
    description: 'Wave pattern with conditional logic that adapts behavior based on channel count. Features symmetric wave propagation.',
    mode: 10,
    complexity: 'Advanced',
    estimatedFrames: 30,
    cppCode: `void taskSequence10Wave() {
    if ((config_data_ptr_->header.pin_size_ / 2) % 2 == 1) {
        taskSequence7BlinkTwoFill();
    } else {
        auto setWaveLed = [&](int value) -> void {
            for (int i = 0; i < (config_data_ptr_->header.pin_size_ / 2); ++i) {
                if (i < ceil((config_data_ptr_->header.pin_size_ / 2) / 2)) {
                    set(config_data_ptr_->header.pin_ptr_[i], value);
                    set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - (i + 1)],
                        value);
                } else {
                    set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2)], value);
                    set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_) - i - 1], value);
                }
                sleep(channel_data_.delay_time_);
            }
        };

        auto setWaveLedReverse = [&](int value) -> void {
            for (int i = (config_data_ptr_->header.pin_size_ / 2) - 1; i >= 0; --i) {
                if (i >= ceil((config_data_ptr_->header.pin_size_ / 2) / 2)) {
                    set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2)], value);
                    set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_) - i - 1], value);
                } else {
                    set(config_data_ptr_->header.pin_ptr_[i], value);
                    set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - (i + 1)], value);
                }
                sleep(channel_data_.delay_time_);
            }
        };

        for (int i = 0; i < 2; ++i) {
            setWaveLed(HIGH);
            setWaveLed(LOW);
        }
        for (int i = 0; i < 2; ++i) {
            setWaveLedReverse(HIGH);
            setWaveLedReverse(LOW);
        }
    }
}

taskSequence10Wave();`
  }
]