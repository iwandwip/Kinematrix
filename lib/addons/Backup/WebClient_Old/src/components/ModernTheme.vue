<script setup lang="ts">

import {
  ArrowUpRight,
  Users,
  Siren,
  TimerReset
} from 'lucide-vue-next'
// import {AspectRatio} from '@/components/ui/aspect-ratio'
import {Button} from '@/components/ui/button'
import {Card, CardContent, CardFooter, CardHeader, CardTitle} from '@/components/ui/card'
// import {Minus, Plus} from 'lucide-vue-next'
import MainHeader from "@/components/content/MainHeader.vue";
import {
  Drawer,
  DrawerClose,
  DrawerContent,
  DrawerDescription,
  DrawerFooter,
  DrawerHeader,
  DrawerTitle,
  DrawerTrigger,
} from '@/components/ui/drawer'

import {Label} from '@/components/ui/label'

import {RadioGroup, RadioGroupItem} from '@/components/ui/radio-group'
import {Textarea} from '@/components/ui/textarea'


function findModeNameByValue(value: string) {
  const foundMode = availableMode.find(item => item.value === value);
  return foundMode ? foundMode.name : null;
}

import {alsData, settingsData} from "@/state/state.ts";
import {availableMode} from "@/util/mockup.ts";
import axios from "axios";
// import {AlertDialogAction} from "@/components/ui/alert-dialog";
import {ref} from "vue";

const devName = ref(alsData.deviceNameALS.value)

// let alertChangeName = 0
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

async function setDataDelayModeModern(val: number) {
  try {
    if (settingsData.themes.value == 'modern') {
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
      // alertChangeName = 1
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
      <div class="grid gap-4 md:grid-cols-1 md:gap-8 lg:grid-cols-1">
        <Card>
          <CardHeader class="flex flex-row items-center justify-between space-y-2 pb-2">
            <CardTitle class="text-md font-medium">
              Informasi Sistem
            </CardTitle>
            <Users class="h-4 w-4 text-muted-foreground"/>
          </CardHeader>
          <CardContent>
            <div class="my-2">
              <p class="text-xs text-muted-foreground">
                Nama
              </p>
              <div class="text-2xl font-bold">
                {{ alsData.deviceNameALS.value }}
              </div>
            </div>
            <div class="my-2">
              <p class="text-xs text-muted-foreground">
                Serial/Nomor-Seri
              </p>
              <div class="text-2xl font-bold">
                {{ alsData.deviceSerialALS.value }}
              </div>
            </div>
            <div class="my-2">
              <p class="text-xs text-muted-foreground">
                Jumlah Channel
              </p>
              <div class="text-2xl font-bold">
                {{ alsData.deviceChALS.value }}
              </div>
            </div>

          </CardContent>
          <CardFooter class="border-t px-6 py-4 w-full">
            <Drawer>
              <DrawerTrigger as-child>
                <Button @click="devName=alsData.deviceNameALS.value ">
                  Ganti Informasi Sistem
                  <ArrowUpRight class="ml-2 h-4 w-4"/>
                </Button>
              </DrawerTrigger>
              <DrawerContent>
                <div class="mx-auto w-full max-w-sm">
                  <DrawerHeader>
                    <DrawerTitle class="text-2xl"> Ganti Informasi Sistem</DrawerTitle>
                    <DrawerDescription>Ganti Informasi Auto Light System Anda</DrawerDescription>
                  </DrawerHeader>
                  <div class="p-4 pb-0">
                    <div class="flex items-center justify-center space-x-2">

                      <div class="flex-1 text-center">
                        <div class=" font-bold tracking-tighter">
                          <Textarea placeholder="Isikan Informasi Auto Light System Baru" v-model="devName"/>
                        </div>

                      </div>

                    </div>
                    <div class="my-3 px-3 text-center h-auto">
                      <hr class="my-2">
                      <div class="text-[0.70rem] uppercase text-muted-foreground text-red-500">
                        Ketika Anda Menyimpan, Sistem akan restart, kemudian mengganti SSID/Nama Wifi Anda
                      </div>
                    </div>
                  </div>
                  <DrawerFooter>
                    <Button @click="setDataDeviceName(devName)">Simpan</Button>
                    <DrawerClose as-child>
                      <Button variant="outline">
                        Cancel
                      </Button>
                    </DrawerClose>
                  </DrawerFooter>
                </div>
              </DrawerContent>
            </Drawer>

          </CardFooter>
        </Card>
        <Card>
          <CardHeader class="flex flex-row items-center justify-between space-y-0 pb-2">
            <CardTitle class="text-md font-medium">
              Status Auto Light System
            </CardTitle>
            <Siren class="h-6 w-6 text-muted-foreground"/>
          </CardHeader>
          <CardContent>
            <div class="py-2">
              <p class="text-xs text-muted-foreground">
                Mode Sedeang Berjalan
              </p>
              <div class="text-2xl font-bold">
                MODE : {{ findModeNameByValue(alsData.modeALS.value) }}
              </div>
            </div>
          </CardContent>
          <CardFooter class="border-t px-6 py-4 w-full">
            <Drawer>
              <DrawerTrigger as-child>
                <Button>
                  Ganti Mode
                  <ArrowUpRight class="ml-2 h-4 w-4"/>
                </Button>
              </DrawerTrigger>
              <DrawerContent>
                <div class="mx-auto w-full max-w-sm">
                  <DrawerHeader>
                    <DrawerTitle>Ganti Mode Lampu</DrawerTitle>
                    <DrawerDescription>Pilih Mode Lampu Auto Light System</DrawerDescription>
                  </DrawerHeader>
                  <div class="p-4 pb-0">
                    <div class="flex items-center justify-center space-x-2">
                      <div class="flex-1 text-center">
                        <div class="text-6xl font-bold tracking-tighter">
                          MODE : {{ findModeNameByValue(alsData.modeALS.value) }}
                        </div>
                        <div class="text-[0.70rem] uppercase text-muted-foreground">
                          Mode Sekarang
                        </div>
                      </div>
                    </div>

                    <div class="my-2 px-3 h-auto">
                      <hr>
                      <RadioGroup v-model="alsData.modeALS.value" class="grid mt-2 py-2 grid-cols-4 gap-4">
                        <template v-for="item in availableMode" :key="item.value">
                          <div @click="setDataDeviceMode(item.value)">
                            <RadioGroupItem :id="item.value" :value="item.value" class="peer sr-only"/>
                            <Label
                                :for="item.value"
                                :class="[
                                  'flex flex-col items-center justify-between rounded-md border-2 border-muted bg-popover p-4 hover:bg-accent hover:text-accent-foreground',
                                  {'peer-data-[state=checked]:border-primary': true, '[&:has([data-state=checked])]:border-primary': true}
                                ]"
                            >
                              <Siren class="h-6 w-6 text-muted-foreground"/>
                              {{ item.name }}
                            </Label>
                          </div>
                        </template>

                      </RadioGroup>
                    </div>
                  </div>
                  <DrawerFooter>
                    <!--                    <Button>Submit</Button>-->
                    <DrawerClose as-child>
                      <Button variant="secondary">
                        Kembali
                      </Button>
                    </DrawerClose>
                  </DrawerFooter>
                </div>
              </DrawerContent>
            </Drawer>
          </CardFooter>
        </Card>

        <Card>
          <CardHeader class="flex flex-row items-center justify-between space-y-0 pb-2">
            <CardTitle class="text-md font-medium">
              Delay Auto Light System
            </CardTitle>
            <TimerReset class="h-6 w-6 text-muted-foreground"/>
          </CardHeader>
          <CardContent>
            <div class="my-2">
              <p class="text-xs text-muted-foreground">
                Delay Hidup-Mati Sistem
              </p>
              <div class="text-2xl font-bold">
                {{ alsData.delayALS.value / 1000 }} Detik
              </div>
            </div>
          </CardContent>
          <CardFooter class="border-t px-6 py-4 w-full">
            <Drawer>
              <DrawerTrigger as-child>
                <Button>
                  Ganti Delay
                  <ArrowUpRight class="ml-2 h-4 w-4"/>
                </Button>
              </DrawerTrigger>
              <DrawerContent>
                <div class="mx-auto w-full max-w-sm">
                  <DrawerHeader>
                    <DrawerTitle class="text-2xl">Ganti Nilai Delay</DrawerTitle>
                    <DrawerDescription>Ubah nilai waktu jeda Hidup-Mati sistem Auto Light</DrawerDescription>
                  </DrawerHeader>
                  <div class="p-4 pb-0">
                    <div class="flex items-center justify-center space-x-2">

                      <div class="flex-1 text-center">
                        <div class="text-7xl font-bold tracking-tighter">
                          {{ alsData.delayALS }}
                        </div>
                        <div class="text-[0.70rem] uppercase text-muted-foreground">
                          Satuan Mili Detik (Mili Sekon)
                        </div>
                      </div>

                    </div>
                    <div class="my-3 px-3 h-[120px]">
                      <Vueform>
                        <SliderElement  :default="alsData.delayALS.value"
                                        :ref="alsData.delayALS.value" name="slider" :tooltips="true"
                                        tooltip-position="bottom"
                                        :max="300" :min="30"
                                        @updated="(form$ : any) => {
                                       form$.value = alsData.delayALS.value
                                     }"
                                       @change="(val:number) => {setDataDelayModeModern(val)}"
                                       class="mt-5 w-[300px]"/>
                      </Vueform>
                    </div>
                  </div>
                  <DrawerFooter>
                    <DrawerClose as-child>
                      <Button variant="secondary">
                        Cancel
                      </Button>
                    </DrawerClose>
                  </DrawerFooter>
                </div>
              </DrawerContent>
            </Drawer>
          </CardFooter>
        </Card>

      </div>

    </main>
    <footer class="py-6 md:px-8 md:py-0">
      <div class="container flex flex-col items-center justify-between gap-4 md:h-24 md:flex-row">
        <div class="text-center text-sm leading-loose text-muted-foreground md:text-left"><span class="inline-block">
          Built and designed by <a
            href="#"
            class="underline underline-offset-4 font-bold decoration-foreground"> workalogi </a></span>
          <br>

        </div>
      </div>
    </footer>
  </div>
</template>
