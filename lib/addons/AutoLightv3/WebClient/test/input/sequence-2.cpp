void BaseChannel::taskSequence2BlinkAll() {
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