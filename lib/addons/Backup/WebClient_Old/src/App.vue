<script setup lang="ts">
// import Spinner from "@/components/ui/spinner/spinner.vue";

// import {delay, deviceName, mode, serialNumber} from "@/util/mockup.ts";
import {alsData, settingsData} from "@/state/state.ts";

import { onMounted } from 'vue';
// import MainPage from "@/components/MainPage.vue";
// import ModernTheme from "@/components/ModernTheme.vue";
// import ClassicTheme from "@/components/ClassicTheme.vue";
// import HelloWorld from "@/components/HelloWorld.vue";
import axios from "axios";
import {useColorMode} from '@vueuse/core'
const colorTheme = useColorMode()


onMounted(
    () => {
      syncMockDataToState();
      setInterval(fetchData, 2000);
      // setInterval(setDataDelayMode, 500);
    }
);


async function fetchDataDeviceChannel() {
  try {
    const response = await axios.get(`/get/device/ch`);
    return response.data
  }
  catch (e) {
    return 0
  }
}
async function fetchDataDeviceDelay() {
  try {
    const response = await axios.get(`/get/delay`);
    return response.data
  }
  catch (e) {
    return 0
  }
}

async function fetchDataDeviceName() {
  try {
    const response = await axios.get(`/get/device/name`);
    return response.data
  }
  catch (e) {
    return 0
  }
}

async function fetchDataDeviceSerialNumber() {
  try {
    const response = await axios.get(`/get/device/serial`);
    return response.data
  }
  catch (e) {
    return 0
  }
}

async function fetchDataDeviceMode() {
  try {
    const response = await axios.get(`/get/mode`);
    return response.data.toString()
  }
  catch (e) {
    return 0
  }
}




async function fetchData() {


  const ch = await fetchDataDeviceChannel();
  const deviceName = await fetchDataDeviceName();
  const serialNumber = await fetchDataDeviceSerialNumber();


  const mode = await fetchDataDeviceMode();
  const delay = await fetchDataDeviceDelay();

  alsData.delayALS.value = delay;
  // Sinkronisasi mode
  // alsData.setDelayALS(delay)
  alsData.setModeALS(mode)
  alsData.setDeviceNameALS(deviceName)
  alsData.setSerialDeviceALS(serialNumber)
  alsData.deviceChALS.value = ch;

}

async function syncMockDataToState() {
  // settingsData.startLoading(); // Memulai loading
  colorTheme.value = 'dark'

  // Memanggil fetchData setelah loading dimulai
  await fetchData();

  // Menunda pemanggilan stopLoading selama 1500 milidetik
  setTimeout(() => {
    settingsData.stopLoading(); // Menghentikan loading setelah 1500 milidetik
  }, 1000);
}






</script>

<template>
  <router-view></router-view>
</template>

<style scoped>

</style>
