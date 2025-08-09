#ifndef BASE_CHANNEL_H
#define BASE_CHANNEL_H

#include "Arduino.h"
#include <stdarg.h>

#ifndef ENABLE_MODULE_NODEF_IO_EXPANDER
#define ENABLE_MODULE_NODEF_IO_EXPANDER
#endif
#ifndef ENABLE_MODULE_NODEF_DIGITAL_OUTPUT
#define ENABLE_MODULE_NODEF_DIGITAL_OUTPUT
#endif
#ifndef ENABLE_MODULE_NODEF_TIMER_TASK
#define ENABLE_MODULE_NODEF_TIMER_TASK
#endif

#include "KinematrixModulesNoDef.h"

#include "../Common/Constants.h"
#include "../Config/BaseConfig.h"
#include "../Config/ConfigManager.h"
#include "../ButtonInterrupt.h"
#include "../Cores/Task.h"
#include "../Web/WebManager.h"

namespace AutoLight {
    using namespace Constants;

    class WebManager;
    class PatternHelper;

    void taskCallback();

    class ChannelData {
    public:
        volatile uint32_t delay_time_;
        volatile uint32_t sequence_index_;
        volatile uint32_t sequence_index_apps_;
        volatile uint32_t last_active_sequence_;
        volatile bool is_reverse_;
        volatile bool is_mode_changed_;
        volatile bool is_mode_changed_apps_;
        volatile bool is_on_;
    };

    class ExpanderIo {
    public:
        IOExpander **io_base_;
        uint8_t io_length;
    };

    class BaseChannel {
        friend class PatternHelper;
    public:
        BaseChannel(bool _enable_unit_test = false);
        virtual ~BaseChannel();

        bool initialize();
        void enableI2CConfig();

        void attachConfig(ConfigData *_config_data_ptr);
        void attachInterrupt(ButtonInterrupt *_button_interrupt_ptr);

        void setParamCallback(void (*_callback)());
        void setInitDelay(uint32_t _time);
        void setInitSequence(uint8_t _sequence);
        void setTotalSequence(uint8_t _nums);
        void reverse(bool _is_reverse = false);

        volatile uint32_t getSequenceIndex();
        volatile uint32_t getDelayTime();
        ChannelData getChannelData();

        void nextMode();
        void previousMode();
        void onMode();
        void offMode();

        void singleButtonCycle();
        void toggleOnOff();
        void smartButtonPress(uint8_t button_index);
        void setButtonMode(button_mode_t mode);
        void setButtonConfig(ButtonConfig *config);
        void executeButtonAction(uint8_t button_index);

        void enableSequenceMapping(bool enable = true);
        void setActiveSequences(uint8_t *sequences, uint8_t count);
        void setActiveSequences(uint8_t count, ...);
        void reorderActiveSequences(uint8_t *new_order, uint8_t count);
        void printSequenceMapping();
        String getActiveMappingString();

#define SET_ACTIVE_SEQUENCES(channel, ...) \
            do { \
                uint8_t sequences[] = {__VA_ARGS__}; \
                channel.setActiveSequences(sequences, sizeof(sequences)/sizeof(sequences[0])); \
            } while(0)

        void enableWebServer(bool auto_task = true);
        void enableWebServerManual();

        WebManager *getWebManager() { return web_manager_; }

        void changeMode();
        void changeModeApp(uint32_t num);
        volatile bool isChangeMode();
        volatile bool isOn();

        void runAutoLight(void (*_callbackRun)(uint32_t sequence) = nullptr);

        void setStateHigh(int index, ...);
        void setStateLow(int index, ...);

        void set(uint8_t _pin, uint8_t _state);
        void sleep(uint32_t _delay);

        void forceOff();
        void off();
        void on();

        bool isReady();
        void debug();

        bool getLEDState(uint8_t led_index);

        uint8_t getTotalLEDs() { return config_data_ptr_ ? config_data_ptr_->header.channel_ : 0; }

        void taskSequence2BlinkAll();
        void taskSequence3FillTwoPoint();
        void taskSequence4FillRight();
        void taskSequence5FillIn();
        void taskSequence6BlinkOneByOne();
        void taskSequence7BlinkTwoFill();
        void taskSequence8SnakeAndReverse();
        void taskSequence9Random();
        void taskSequence10Wave();
        void taskSequence11Complex();
        void taskSequence12PatternMatrix();
        void taskSequence13BlinkPattern();
        void taskSequence14AdvancedPattern();
        void taskSequence15AllSequences();
        void taskSequencePatternEngineTest();

    private:
        bool addIoExpander(IOExpander *_io_expander);
        bool beginIoExpander();

        int findMax(int num, ...);
        int shiftArrayAccordingToOrder(const int patterns[][8], int patternSize, uint8_t *mainArray, int mainArraySize, int *arr, int &lastIndex, int targetIndex, bool reverse = false);
        int shiftArrayAccordingToPosition(int *mainArray, int mainArraySize, int *arr, int &lastIndex, int targetIndex);
        void resetArray(int *mainArray, int &arrSize, int &arrLastIndex);
        void splitArrayDynamically(int *mainArray, int mainArraySize);

        void setTaskSequenceFunction();

        bool is_using_i2c_config_;
        bool is_initialize_;
        bool is_unit_test_;

        uint8_t total_task_sequence_;
        uint32_t task_sequence_timer_;

        void (BaseChannel::*temp_mode_)();
        void (BaseChannel::*total_mode_[(Constants::MAXNUM_TOTAL_TASK_SEQUENCE)])();

        ExpanderIo expander_io_;
        ChannelData channel_data_;

        ConfigData *config_data_ptr_;
        ButtonInterrupt *button_interrupt_ptr_;
        ButtonConfig button_config_;
        button_mode_t current_button_mode_;
        SequenceMapper sequence_mapper_;

        WebManager *web_manager_;
        

        uint8_t mapApiIndexToActualSequence(uint8_t api_index);
        uint8_t mapActualSequenceToApiIndex(uint8_t actual_sequence);
        bool isApiIndexValid(uint8_t api_index);
        void initializeDefaultMapping();
    };
}

#endif