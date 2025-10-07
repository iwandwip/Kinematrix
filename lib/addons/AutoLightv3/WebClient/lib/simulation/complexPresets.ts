import { PresetTemplate } from './presetTemplates'

export const complexPresets: PresetTemplate[] = [
  {
    id: 'sequence11',
    name: 'Complex Pattern',
    description: 'Multi-stage complex pattern featuring overlapping LED activations with multiple timing phases and mathematical progression.',
    mode: 11,
    complexity: 'Expert',
    estimatedFrames: 60,
    cppCode: `void taskSequence11Complex() {
    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2 + 2; i++) {
            if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[i], HIGH);
            if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2)], HIGH);
            if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i - 2], HIGH);
            if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);
            if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2) - 2], HIGH);
            if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2) - 1], HIGH);
            sleep(channel_data_.delay_time_ * 2);
            if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[i], LOW);
            if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2)], LOW);
            if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i - 2], LOW);
            if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
            if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2) - 2], LOW);
            if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2) - 1], LOW);
        }
    }
    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2 + 2; i++) {
            if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i], HIGH);
            if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i], HIGH);
            if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i + 2], HIGH);
            if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i + 1], HIGH);
            if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i + 2], HIGH);
            if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i + 1], HIGH);
            sleep(channel_data_.delay_time_ * 2);
            if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i], LOW);
            if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i], LOW);
            if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i + 2], LOW);
            if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i + 1], LOW);
            if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i + 2], LOW);
            if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i + 1], LOW);
        }
    }
    sleep(channel_data_.delay_time_ * 2);

    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; i++) {
            set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i], HIGH);
            set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2)], HIGH);
            sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
        }
        for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; i++) {
            set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i], LOW);
            set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2)], LOW);
            sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
        }
    }
}

taskSequence11Complex();`
  },

  {
    id: 'sequence12',
    name: 'Pattern Matrix',
    description: 'Matrix-based pattern system using predefined pattern matrices. Demonstrates advanced pattern lookup and distribution logic.',
    mode: 12,
    complexity: 'Expert',
    estimatedFrames: 80,
    cppCode: `void taskSequence12PatternMatrix() {
    if (config_data_ptr_->header.pin_size_ < 8) return;
    const int patternMode[9][8] = {
            {3, 4, 4, 1, 1, 4, 4, 3},  // For size 24
            {3, 3, 4, 1, 1, 4, 3, 3},  // For size 22
            {3, 3, 3, 1, 1, 3, 3, 3},  // For size 20
            {3, 2, 3, 1, 1, 3, 2, 3},  // For size 18
            {3, 2, 2, 1, 1, 2, 2, 3},  // For size 16
            {3, 1, 2, 1, 1, 1, 2, 3},  // For size 14
            {3, 1, 1, 1, 1, 1, 1, 3},  // For size 12
            {3, 0, 1, 1, 1, 1, 0, 3},  // For size 10
            {3, 0, 0, 1, 1, 0, 0, 3}   // For size 8
    };

    int arrData[config_data_ptr_->header.pin_size_];
    int arrSize = 0;
    int arrLastIndex = 0;

    resetArray(arrData, arrSize, arrLastIndex);
    arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 0);
    arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 7);
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < arrSize; i++) {
            set(arrData[i], HIGH);
        }
        sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
        for (int i = 0; i < arrSize; i++) {
            set(arrData[i], LOW);
        }
        sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
    }
    sleep(channel_data_.delay_time_ * 4);

    resetArray(arrData, arrSize, arrLastIndex);
    arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 1);
    arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 6);
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < arrSize; i++) {
            set(arrData[i], HIGH);
        }
        sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
        for (int i = 0; i < arrSize; i++) {
            set(arrData[i], LOW);
        }
        sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
    }
    sleep(channel_data_.delay_time_ * 4);

    resetArray(arrData, arrSize, arrLastIndex);
    arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 2);
    arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 3);
    arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 4);
    arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 5);
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < arrSize; i++) {
            set(arrData[i], HIGH);
        }
        sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
        for (int i = 0; i < arrSize; i++) {
            set(arrData[i], LOW);
        }
        sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
    }
    sleep(channel_data_.delay_time_ * 4);
}

taskSequence12PatternMatrix();`
  },

  {
    id: 'sequence13',
    name: 'Blink Pattern',
    description: 'Advanced blinking system with pattern matrices and multi-phase execution. Features complex pattern selection algorithms.',
    mode: 13,
    complexity: 'Expert',
    estimatedFrames: 90,
    cppCode: `void taskSequence13BlinkPattern() {
    if (config_data_ptr_->header.pin_size_ < 8) return;

    const int patternMode[9][8] = {
            {3, 3, 3, 3, 3, 3, 3, 3},  // Untuk size 24
            {3, 2, 3, 3, 3, 3, 2, 3},  // Untuk size 22
            {3, 2, 2, 3, 3, 2, 2, 3},  // Untuk size 20
            {3, 2, 2, 2, 2, 2, 2, 3},  // Untuk size 18
            {3, 1, 2, 2, 2, 2, 1, 3},  // Untuk size 16
            {3, 1, 1, 2, 2, 1, 1, 3},  // Untuk size 14
            {3, 1, 1, 1, 1, 1, 1, 3},  // Untuk size 12
            {3, 0, 1, 1, 1, 1, 0, 3},  // Untuk size 10
            {3, 0, 0, 1, 1, 0, 0, 3}   // Untuk size 8
    };

    int arrData[config_data_ptr_->header.pin_size_];
    int arrSize = 0;
    int arrLastIndex = 0;

    for (int k = 0; k < 3; k++) {
        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 3);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 7);

        for (int j = 0; j < 3; j++) {
            for (int i = 0; i < arrSize; i++) {
                set(arrData[i], HIGH);
            }
            sleep(channel_data_.delay_time_ * 2);
            for (int i = 0; i < arrSize; i++) {
                set(arrData[i], LOW);
            }
            sleep(channel_data_.delay_time_ * 2);
        }
        sleep(channel_data_.delay_time_ * 2);

        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 0);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 4);

        for (int j = 0; j < 3; j++) {
            for (int i = 0; i < arrSize; i++) {
                set(arrData[i], HIGH);
            }
            sleep(channel_data_.delay_time_ * 2);
            for (int i = 0; i < arrSize; i++) {
                set(arrData[i], LOW);
            }
            sleep(channel_data_.delay_time_ * 2);
        }
        sleep(channel_data_.delay_time_ * 2);
    }
}

taskSequence13BlinkPattern();`
  },

  {
    id: 'sequence14',
    name: 'Advanced Pattern',
    description: 'Most complex pattern featuring multiple phases, mathematical calculations, and sophisticated LED coordination algorithms.',
    mode: 14,
    complexity: 'Expert',
    estimatedFrames: 100,
    cppCode: `void taskSequence14AdvancedPattern() {
    const int size = config_data_ptr_->header.pin_size_;
    if (config_data_ptr_->header.pin_size_ < 8) return;

    const int patternMode[9][8] = {
            {3, 4, 4, 1, 1, 4, 4, 3},  // for size 24
            {3, 3, 4, 1, 1, 4, 3, 3},  // for size 22
            {3, 3, 3, 1, 1, 3, 3, 3},  // for size 20
            {3, 2, 3, 1, 1, 3, 2, 3},  // for size 18
            {3, 2, 2, 1, 1, 2, 2, 3},  // for size 16
            {3, 1, 2, 1, 1, 1, 2, 3},  // for size 14
            {3, 1, 1, 1, 1, 1, 1, 3},  // for size 12
            {3, 0, 1, 1, 1, 1, 0, 3},  // for size 10
            {3, 0, 0, 1, 1, 0, 0, 3}   // for size 8
    };

    for (int i = 0; i < size / 2; i++) {
        for (int j = 0; j < size / 2; j++) {
            set(config_data_ptr_->header.pin_ptr_[j], HIGH);
            set(config_data_ptr_->header.pin_ptr_[size - 1 - j], HIGH);
            sleep(channel_data_.delay_time_);
        }
        for (int j = 0; j < i; j++) {
            set(config_data_ptr_->header.pin_ptr_[j], HIGH);
            set(config_data_ptr_->header.pin_ptr_[size - 1 - j], HIGH);
            sleep(channel_data_.delay_time_ * 2);
            set(config_data_ptr_->header.pin_ptr_[j], LOW);
            set(config_data_ptr_->header.pin_ptr_[size - 1 - j], LOW);
            sleep(channel_data_.delay_time_ * 2);
        }
        set(config_data_ptr_->header.pin_ptr_[i], LOW);
        set(config_data_ptr_->header.pin_ptr_[size - 1 - i], LOW);
        sleep(channel_data_.delay_time_ * 2);
    }

    for (int k = 0; k < 2; k++) {
        for (int i = 0; i < size / 2 + 3; i++) {
            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[i], HIGH);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i - 3], HIGH);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i - 2], HIGH);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);

            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[size - 1 - i], HIGH);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 3], HIGH);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 2], HIGH);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 1], HIGH);
            sleep(channel_data_.delay_time_ * 2);
            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[i], LOW);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i - 3], LOW);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i - 2], LOW);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);

            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[size - 1 - i], LOW);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 3], LOW);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 2], LOW);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 1], LOW);
        }

        for (int i = 0; i < size / 2 + 3; i++) {
            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i], HIGH);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 3], HIGH);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 2], HIGH);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 1], HIGH);

            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[i + (size / 2)], HIGH);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 3], HIGH);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 2], HIGH);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 1], HIGH);
            sleep(channel_data_.delay_time_ * 2);
            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i], LOW);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 3], LOW);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 2], LOW);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 1], LOW);

            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[i + (size / 2)], LOW);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 3], LOW);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 2], LOW);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 1], LOW);
        }
    }

    int arrData[size];
    int arrSize = 0;
    int arrLastIndex = 0;

    resetArray(arrData, arrSize, arrLastIndex);
    arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 1, true);
    arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 6);

    for (int i = 0; i < arrSize / 2; i++) {
        set(arrData[i], HIGH);
        set(arrData[arrSize / 2 + i], HIGH);
        sleep(channel_data_.delay_time_ * 3);
    }

    for (int i = 0; i < arrSize / 2; i++) {
        set(arrData[i], LOW);
        set(arrData[arrSize / 2 + i], LOW);
        sleep(channel_data_.delay_time_ * 3);
    }
}

taskSequence14AdvancedPattern();`
  }
]