import { createGlobalState } from '@vueuse/core';
import { ref } from 'vue';


// Buat state global
const alsDataStateManagement = createGlobalState(() => {
    // Getter
    const  deviceNameALS = ref('');
    const  deviceSerialALS = ref('');
    const  deviceChALS = ref(24);


    const modeALS = ref();
    const delayALS  = ref();


    // Setters
    const setSerialDeviceALS = (newSerial : string) => {
        try {
            deviceSerialALS.value = newSerial;
            // Requesst AXIOS
        }
        catch (e) {
            // e
        }
    };
    const setDeviceNameALS = (newDeviceName : string) => {
        try {
            deviceNameALS.value = newDeviceName;
            // Requesst AXIOS
        }
        catch (e) {
            // e
        }
    };
    const setModeALS = (newMode : string) => {
        try {
            modeALS.value = newMode;
            // Requesst AXIOS
        }
        catch (e) {
            // e
        }
    };
    const setDelayALS = (newDelay : number) => {
        try {
            delayALS.value = newDelay;
            // Requesst AXIOS
        }
        catch (e) {
            // e
        }
    };

    const decrementDelayALS = (decrementVal : number) => {
        delayALS.value -= decrementVal;
    }
    const incrementDelayALS = (incrementVal : number) => {
        delayALS.value += incrementVal;
    }

    return {
        deviceNameALS, deviceSerialALS, deviceChALS,

        modeALS, delayALS,

        setDeviceNameALS,setSerialDeviceALS,
        setModeALS,
        setDelayALS, incrementDelayALS, decrementDelayALS
    };
});

export default alsDataStateManagement;