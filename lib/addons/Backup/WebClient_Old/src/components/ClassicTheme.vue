<script setup lang="ts">
import {useColorMode} from '@vueuse/core'
import MainHeader from "@/components/content/MainHeader.vue";
import {AlertCircle} from 'lucide-vue-next'
import {Alert, AlertDescription, AlertTitle} from '@/components/ui/alert'
import {
  AlertDialog,
  AlertDialogAction,
  AlertDialogCancel,
  AlertDialogContent,
  AlertDialogDescription,
  AlertDialogFooter,
  AlertDialogHeader,
  AlertDialogTitle,
  AlertDialogTrigger,
} from '@/components/ui/alert-dialog';

import {availableMode} from "@/util/mockup.ts";
import {alsData, settingsData} from "@/state/state.ts";
import {Label} from "@/components/ui/label";
import {Siren} from "lucide-vue-next";
import {RadioGroup, RadioGroupItem} from "@/components/ui/radio-group";
// import { Slider } from '@/components/ui/slider';
import {Button} from '@/components/ui/button';
import {Input} from '@/components/ui/input';
import axios from "axios";
// import {context} from "radix-vue/dist/DismissableLayer/DismissableLayer";
import {ref} from 'vue'

const temaMode = useColorMode()
temaMode.value = 'dark'
const devName = ref(alsData.deviceNameALS.value)
let alertChangeName = 0

function findModeNameByValue(value: string) {
  const foundMode = availableMode.find(item => item.value === value);
  return foundMode ? foundMode.name : null;
}

async function setDataDeviceMode(val: any) {
  try {
    alsData.modeALS.value = val
    const response = await axios.get(`/set/mode?value=${alsData.modeALS.value}`);
    console.log(response)
    return response
  } catch (e) {
    console.log(e)
  }
}

async function setDataDelayMode(val: number) {
  try {
    if (settingsData.themes.value == 'classic') {
      console.log('run')
      if (val != alsData.delayALS.value) {
        console.log('TIdak Sinkron ' + val + ' - ' + alsData.delayALS.value)
        alsData.delayALS.value = val
        await axios.get(`/set/delay?value=${alsData.delayALS.value}`);
      }
    }
  } catch (e) {
    console.log(e)
  }
}

async function setDataDeviceName(val: any) {
  try {
    if (alsData.deviceNameALS.value != val) {
      alertChangeName = 1
      alsData.deviceNameALS.value = val
      const response = await axios.get(`/set/device/name?value=${alsData.deviceNameALS.value}`);
      console.log(response)
      return response
    }
  } catch (e) {
    console.log(e)
  }
}


</script>

<template>
  <div class="flex min-h-screen w-full flex-col">
    <MainHeader></MainHeader>
    <main class="flex flex-1 flex-col gap-3 p-4 md:gap-3 md:p-8">
      <section>
        <div
            class="mx-auto flex max-w-[980px] flex-col items-center gap-2 py-8 md:py-12 md:pb-8 lg:py-24 lg:pb-20 page-header pb-8 page-header pb-8">
          <h1 class="text-center text-3xl font-bold leading-tight   md:block"> Informasi Alat </h1>
        </div>
        <div class="grid gap-2 grid-cols-1 mb-5 ">
          <div v-if="alertChangeName == 1">
            <Alert variant="destructive">
              <AlertCircle class="w-8 h-8 mt-2"/>
              <AlertTitle class="text-2xl ml-2">Peringatan</AlertTitle>
              <AlertDescription class="text-md ml-2">
                Nama Device Sudah Berganti, Silahkan ganti WiFi anda ke : <br>
                <span class="text-red-600 text-xl font-bold">{{ alsData.deviceNameALS.value }}</span>
              </AlertDescription>
            </Alert>
          </div>
        </div>
        <div class="grid gap-2 grid-cols-3 text-center">
          <div>
            <h1 class="text-xs">Nama Device</h1>
            <p class="font-bold text-lg ">{{ alsData.deviceNameALS.value }}</p>
          </div>
          <div>
            <h1 class="text-xs">Serial Number</h1>
            <p class="font-bold text-lg ">{{ alsData.deviceSerialALS.value }}</p>
          </div>
          <div>
            <h1 class="text-xs">Jumlah Channel</h1>
            <p class="font-bold text-lg ">{{ alsData.deviceChALS.value }}</p>
          </div>
        </div>
        <div class="grid gap-2 mt-2  grid-cols-1 text-center">
          <AlertDialog>
            <AlertDialogTrigger as-child>
              <Button class="w-2/5 mx-auto  py-2" variant="link" @click="devName=alsData.deviceNameALS.value ">
                Ganti Device
              </Button>
            </AlertDialogTrigger>
            <AlertDialogContent>
              <AlertDialogHeader>
                <AlertDialogTitle>Ganti Informasi Device</AlertDialogTitle>
                <AlertDialogDescription>

                  <Label for="device" class="text-left mt-2">Nama Device Baru</Label>
                  <Input id="device" type="text" class="my-5 font-bold" placeholder="Nama Device" v-model="devName"/>
                </AlertDialogDescription>
              </AlertDialogHeader>
              <AlertDialogFooter>
                <AlertDialogCancel>TIdak Jadi</AlertDialogCancel>
                <AlertDialogAction @click="setDataDeviceName(devName)">Simpan</AlertDialogAction>
              </AlertDialogFooter>
            </AlertDialogContent>
          </AlertDialog>

        </div>
      </section>
      <section>
        <div
            class="mx-auto flex max-w-[980px] flex-col items-center text-center gap-2 py-4 md:py-12 md:pb-8 lg:py-24 lg:pb-20 page-header pb-8 page-header pb-8">
          <h1 class="text-center text-3xl font-bold leading-tight   md:block"> Ganti Mode </h1>
          <div class="mt-3">
            <h1 class="text-xs">Mode Sekarang</h1>
            <p class="font-bold text-lg ">MODE : {{ findModeNameByValue(alsData.modeALS.value) }}</p>
          </div>
        </div>
        <RadioGroup v-model="alsData.modeALS.value" class="grid mt-2 py-2 text-center  grid-cols-4 gap-4">
          <template v-for="item in availableMode" :key="item.value">
            <div @click="setDataDeviceMode(item.value)">
              <RadioGroupItem :id="item.value" :value="item.value" class="peer sr-only"/>
              <Label
                  :for="item.value"
                  :class="[
                                  'flex flex-col items-center text-md justify-between rounded-md border-2 border-muted bg-popover p-4 hover:bg-accent hover:text-accent-foreground',
                                  {'peer-data-[state=checked]:border-primary': true, '[&:has([data-state=checked])]:border-primary': true}
                                ]"
              >
                <Siren class="h-6 w-6 text-muted-foreground"/>
                {{ item.name }}
              </Label>
            </div>
          </template>
        </RadioGroup>
      </section>
      <section>
        <div
            class="mx-auto flex max-w-[980px] flex-col items-center gap-2 py-2 md:py-12 md:pb-8 lg:py-24 lg:pb-20 page-header pb-8 page-header pb-8">
          <h1 class="text-center text-3xl font-bold leading-tight   md:block"> Ganti Delay </h1>
          <div class="grid gap-2 grid-cols-1 text-center">
            <div>
              <h1 class="text-xs">Delay Sekarang</h1>
              <p class="font-bold text-lg ">{{ alsData.delayALS.value }} mS</p>
            </div>

          </div>
          <Vueform>
            <SliderElement :default="alsData.delayALS.value"
                           :ref="alsData.delayALS.value" name="slider" :tooltips="true" tooltip-position="bottom"
                           :max="300" :min="30"
                           @updated = "(form$ : any) => {
                             form$.value = alsData.delayALS.value
                           }"
                           @change="(val:number) => {setDataDelayMode(val)}"
                           class="w-[300px]"/>
          </Vueform>
        </div>

      </section>
    </main>
    <footer class="py-6 md:px-8 md:py-0">
      <div class="container flex flex-col items-center justify-between gap-4 md:h-24 md:flex-row">
        <div class="text-center text-xs leading-loose text-muted-foreground md:text-left"><span class="inline-block">
          Built and designed by <a
            href="#" target=""
            class="underline underline-offset-4 font-bold decoration-foreground"> workalogi </a></span>
          <br>

        </div>
      </div>
    </footer>
  </div>
</template>

<style scoped>
html {
  font-family: "Verdana", "Arial", sans-serif;
  display: inline-block;
  margin: 0px auto;
  text-align: center;
}

.button {
  background-color: #4CAF50;
  border: none;
  color: white;
  padding: 16px 40px;
  border-radius: 10px;
  text-decoration: none;
  font-size: 30px;
  margin: 2px;
  cursor: pointer;
}

.button2 {
  background-color: #555555;
  border-radius: 10px;
}
</style>