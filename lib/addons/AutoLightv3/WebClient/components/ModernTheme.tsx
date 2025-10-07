"use client"

import { useState } from "react"
import {
  ArrowUpRight,
  Users,
  Siren,
  TimerReset
} from 'lucide-react'
import { Button } from '@/components/ui/button'
import { Card, CardContent, CardFooter, CardHeader, CardTitle } from '@/components/ui/card'
import MainHeader from "@/components/content/MainHeader"
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
import { Label } from '@/components/ui/label'
import { RadioGroup, RadioGroupItem } from '@/components/ui/radio-group'
import { Textarea } from '@/components/ui/textarea'
import { Slider } from '@/components/ui/slider'
import { useALS } from "@/lib/contexts/ALSContext"
import { availableMode, findModeNameByValue } from "@/lib/utils/mockup"
import { setDataDeviceMode, setDataDeviceName, setDataDeviceDelay } from "@/lib/api/device"

export default function ModernTheme() {
  const { alsData, settingsData, setDeviceName, setMode, setDelay } = useALS()
  const [devName, setDevName] = useState("")

  const handleSetDataDeviceMode = async (val: string) => {
    try {
      setMode(val)
      const response = await setDataDeviceMode(val)
      console.log(response)
      return response
    } catch (e) {
      console.log(e)
    }
  }

  const handleSetDataDelayModeModern = async (val: number[]) => {
    try {
      if (settingsData.themes === 'modern') {
        console.log('run')
        const newDelay = val[0]
        if (newDelay !== alsData.delay) {
          console.log('Tidak Sinkron ' + newDelay + ' - ' + alsData.delay)
          setDelay(newDelay)
          await setDataDeviceDelay(newDelay)
        }
      }
    } catch (e) {
      console.log(e)
    }
  }

  const handleSetDataDeviceName = async (val: string) => {
    try {
      if (alsData.deviceName !== val) {
        setDeviceName(val)
        const response = await setDataDeviceName(val)
        console.log(response)
        return response
      }
    } catch (e) {
      console.log(e)
    }
  }

  return (
    <div className="flex min-h-screen w-full flex-col">
      <MainHeader />

      <main className="flex flex-1 flex-col gap-3 p-4 md:gap-3 md:p-8">
        <div className="grid gap-4 md:grid-cols-1 md:gap-8 lg:grid-cols-1">
          <Card>
            <CardHeader className="flex flex-row items-center justify-between space-y-2 pb-2">
              <CardTitle className="text-md font-medium">
                Informasi Sistem
              </CardTitle>
              <Users className="h-4 w-4 text-muted-foreground" />
            </CardHeader>
            <CardContent>
              <div className="my-2">
                <p className="text-xs text-muted-foreground">
                  Nama
                </p>
                <div className="text-2xl font-bold">
                  {alsData.deviceName}
                </div>
              </div>
              <div className="my-2">
                <p className="text-xs text-muted-foreground">
                  Serial/Nomor-Seri
                </p>
                <div className="text-2xl font-bold">
                  {alsData.deviceSerial}
                </div>
              </div>
              <div className="my-2">
                <p className="text-xs text-muted-foreground">
                  Jumlah Channel
                </p>
                <div className="text-2xl font-bold">
                  {alsData.deviceCh}
                </div>
              </div>
            </CardContent>
            <CardFooter className="border-t px-6 py-4 w-full">
              <Drawer>
                <DrawerTrigger asChild>
                  <Button onClick={() => setDevName(alsData.deviceName)}>
                    Ganti Informasi Sistem
                    <ArrowUpRight className="ml-2 h-4 w-4" />
                  </Button>
                </DrawerTrigger>
                <DrawerContent>
                  <div className="mx-auto w-full max-w-sm">
                    <DrawerHeader>
                      <DrawerTitle className="text-2xl">Ganti Informasi Sistem</DrawerTitle>
                      <DrawerDescription>Ganti Informasi Auto Light System Anda</DrawerDescription>
                    </DrawerHeader>
                    <div className="p-4 pb-0">
                      <div className="flex items-center justify-center space-x-2">
                        <div className="flex-1 text-center">
                          <div className="font-bold tracking-tighter">
                            <Textarea 
                              placeholder="Isikan Informasi Auto Light System Baru" 
                              value={devName}
                              onChange={(e) => setDevName(e.target.value)}
                            />
                          </div>
                        </div>
                      </div>
                      <div className="my-3 px-3 text-center h-auto">
                        <hr className="my-2" />
                        <div className="text-[0.70rem] uppercase text-muted-foreground text-red-500">
                          Ketika Anda Menyimpan, Sistem akan restart, kemudian mengganti SSID/Nama Wifi Anda
                        </div>
                      </div>
                    </div>
                    <DrawerFooter>
                      <Button onClick={() => handleSetDataDeviceName(devName)}>Simpan</Button>
                      <DrawerClose asChild>
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
            <CardHeader className="flex flex-row items-center justify-between space-y-0 pb-2">
              <CardTitle className="text-md font-medium">
                Status Auto Light System
              </CardTitle>
              <Siren className="h-6 w-6 text-muted-foreground" />
            </CardHeader>
            <CardContent>
              <div className="py-2">
                <p className="text-xs text-muted-foreground">
                  Mode Sedang Berjalan
                </p>
                <div className="text-2xl font-bold">
                  MODE : {findModeNameByValue(alsData.mode)}
                </div>
              </div>
            </CardContent>
            <CardFooter className="border-t px-6 py-4 w-full">
              <Drawer>
                <DrawerTrigger asChild>
                  <Button>
                    Ganti Mode
                    <ArrowUpRight className="ml-2 h-4 w-4" />
                  </Button>
                </DrawerTrigger>
                <DrawerContent>
                  <div className="mx-auto w-full max-w-sm">
                    <DrawerHeader>
                      <DrawerTitle>Ganti Mode Lampu</DrawerTitle>
                      <DrawerDescription>Pilih Mode Lampu Auto Light System</DrawerDescription>
                    </DrawerHeader>
                    <div className="p-4 pb-0">
                      <div className="flex items-center justify-center space-x-2">
                        <div className="flex-1 text-center">
                          <div className="text-6xl font-bold tracking-tighter">
                            MODE : {findModeNameByValue(alsData.mode)}
                          </div>
                          <div className="text-[0.70rem] uppercase text-muted-foreground">
                            Mode Sekarang
                          </div>
                        </div>
                      </div>

                      <div className="my-2 px-3 h-auto">
                        <hr />
                        <RadioGroup 
                          value={alsData.mode} 
                          onValueChange={handleSetDataDeviceMode}
                          className="grid mt-2 py-2 grid-cols-4 gap-4"
                        >
                          {availableMode.map((item) => (
                            <div key={item.value}>
                              <RadioGroupItem 
                                id={item.value} 
                                value={item.value} 
                                className="peer sr-only"
                              />
                              <Label
                                htmlFor={item.value}
                                className="flex flex-col items-center justify-between rounded-md border-2 border-muted bg-popover p-4 hover:bg-accent hover:text-accent-foreground peer-data-[state=checked]:border-primary [&:has([data-state=checked])]:border-primary cursor-pointer"
                              >
                                <Siren className="h-6 w-6 text-muted-foreground" />
                                {item.name}
                              </Label>
                            </div>
                          ))}
                        </RadioGroup>
                      </div>
                    </div>
                    <DrawerFooter>
                      <DrawerClose asChild>
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
            <CardHeader className="flex flex-row items-center justify-between space-y-0 pb-2">
              <CardTitle className="text-md font-medium">
                Delay Auto Light System
              </CardTitle>
              <TimerReset className="h-6 w-6 text-muted-foreground" />
            </CardHeader>
            <CardContent>
              <div className="my-2">
                <p className="text-xs text-muted-foreground">
                  Delay Hidup-Mati Sistem
                </p>
                <div className="text-2xl font-bold">
                  {alsData.delay / 1000} Detik
                </div>
              </div>
            </CardContent>
            <CardFooter className="border-t px-6 py-4 w-full">
              <Drawer>
                <DrawerTrigger asChild>
                  <Button>
                    Ganti Delay
                    <ArrowUpRight className="ml-2 h-4 w-4" />
                  </Button>
                </DrawerTrigger>
                <DrawerContent>
                  <div className="mx-auto w-full max-w-sm">
                    <DrawerHeader>
                      <DrawerTitle className="text-2xl">Ganti Nilai Delay</DrawerTitle>
                      <DrawerDescription>Ubah nilai waktu jeda Hidup-Mati sistem Auto Light</DrawerDescription>
                    </DrawerHeader>
                    <div className="p-4 pb-0">
                      <div className="flex items-center justify-center space-x-2">
                        <div className="flex-1 text-center">
                          <div className="text-7xl font-bold tracking-tighter">
                            {alsData.delay}
                          </div>
                          <div className="text-[0.70rem] uppercase text-muted-foreground">
                            Satuan Mili Detik (Mili Sekon)
                          </div>
                        </div>
                      </div>
                      <div className="my-3 px-3 h-[120px] flex items-center">
                        <Slider
                          defaultValue={[alsData.delay]}
                          max={300}
                          min={30}
                          step={1}
                          onValueChange={handleSetDataDelayModeModern}
                          className="w-full"
                        />
                      </div>
                    </div>
                    <DrawerFooter>
                      <DrawerClose asChild>
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

      <footer className="py-6 md:px-8 md:py-0">
        <div className="container flex flex-col items-center justify-between gap-4 md:h-24 md:flex-row">
          <div className="text-center text-sm leading-loose text-muted-foreground md:text-left">
            <span className="inline-block">
              Built and designed by <a
                href="#"
                className="underline underline-offset-4 font-bold decoration-foreground"
              >
                workalogi
              </a>
            </span>
            <br />
          </div>
        </div>
      </footer>
    </div>
  )
}