void BaseChannel::taskSequence3FillTwoPoint() {
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
                    set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ - 1) - (i + 1)], HIGH);
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