"use client"

import { useState, useCallback, useMemo } from "react"
import MainHeader from "@/components/content/MainHeader"
import { AlertCircle, Siren } from 'lucide-react'
import { Alert, AlertDescription, AlertTitle } from '@/components/ui/alert'
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
} from '@/components/ui/alert-dialog'
import { Label } from "@/components/ui/label"
import { RadioGroup, RadioGroupItem } from "@/components/ui/radio-group"
import { Button } from '@/components/ui/button'
import { Input } from '@/components/ui/input'
import { Slider } from '@/components/ui/slider'
import { useALS } from "@/lib/contexts/ALSContext"
import { availableMode, findModeNameByValue } from "@/lib/utils/mockup"
import { setDataDeviceMode, setDataDeviceName, setDataDeviceDelay } from "@/lib/api/device"

export default function ClassicTheme() {
  const { alsData, settingsData, setDeviceName, setMode, setDelay } = useALS()
  const [devName, setDevName] = useState("")
  const [alertChangeName, setAlertChangeName] = useState(0)

  const handleSetDataDeviceMode = useCallback(async (val: string) => {
    try {
      if (alsData.mode === val) return
      setMode(val)
      const response = await setDataDeviceMode(val)
      console.log(response)
      return response
    } catch (e) {
      console.log(e)
    }
  }, [alsData.mode, setMode])

  const handleSetDataDelayMode = useCallback(async (val: number[]) => {
    try {
      if (settingsData.themes === 'classic') {
        const newDelay = val[0]
        if (newDelay !== alsData.delay) {
          setDelay(newDelay)
          await setDataDeviceDelay(newDelay)
        }
      }
    } catch (e) {
      console.log(e)
    }
  }, [settingsData.themes, alsData.delay, setDelay])

  const handleSetDataDeviceName = useCallback(async (val: string) => {
    try {
      if (alsData.deviceName !== val && val.trim()) {
        setAlertChangeName(1)
        setDeviceName(val)
        const response = await setDataDeviceName(val)
        console.log(response)
        return response
      }
    } catch (e) {
      console.log(e)
    }
  }, [alsData.deviceName, setDeviceName])

  const currentModeName = useMemo(() => {
    return findModeNameByValue(alsData.mode)
  }, [alsData.mode])

  return (
    <div className="flex min-h-screen w-full flex-col">
      <MainHeader />
      
      <main className="flex flex-1 flex-col gap-3 p-4 md:gap-3 md:p-8">
        <section>
          <div className="mx-auto flex max-w-[980px] flex-col items-center gap-2 py-8 md:py-12 md:pb-8 lg:py-24 lg:pb-20 page-header pb-8">
            <h1 className="text-center text-3xl font-bold leading-tight md:block">Informasi Alat</h1>
          </div>
          
          <div className="grid gap-2 grid-cols-1 mb-5">
            {alertChangeName === 1 && (
              <Alert variant="destructive">
                <AlertCircle className="w-8 h-8 mt-2" />
                <AlertTitle className="text-2xl ml-2">Peringatan</AlertTitle>
                <AlertDescription className="text-md ml-2">
                  Nama Device Sudah Berganti, Silahkan ganti WiFi anda ke : <br />
                  <span className="text-red-600 text-xl font-bold">{alsData.deviceName}</span>
                </AlertDescription>
              </Alert>
            )}
          </div>
          
          <div className="grid gap-2 grid-cols-3 text-center">
            <div>
              <h1 className="text-xs">Nama Device</h1>
              <p className="font-bold text-lg">{alsData.deviceName}</p>
            </div>
            <div>
              <h1 className="text-xs">Serial Number</h1>
              <p className="font-bold text-lg">{alsData.deviceSerial}</p>
            </div>
            <div>
              <h1 className="text-xs">Jumlah Channel</h1>
              <p className="font-bold text-lg">{alsData.deviceCh}</p>
            </div>
          </div>
          
          <div className="grid gap-2 mt-2 grid-cols-1 text-center">
            <AlertDialog>
              <AlertDialogTrigger asChild>
                <Button 
                  className="w-2/5 mx-auto py-2" 
                  variant="link" 
                  onClick={() => setDevName(alsData.deviceName)}
                >
                  Ganti Device
                </Button>
              </AlertDialogTrigger>
              <AlertDialogContent>
                <AlertDialogHeader>
                  <AlertDialogTitle>Ganti Informasi Device</AlertDialogTitle>
                  <AlertDialogDescription>
                    <Label htmlFor="device" className="text-left mt-2">Nama Device Baru</Label>
                    <Input 
                      id="device" 
                      type="text" 
                      className="my-5 font-bold" 
                      placeholder="Nama Device" 
                      value={devName}
                      onChange={(e) => setDevName(e.target.value)}
                    />
                  </AlertDialogDescription>
                </AlertDialogHeader>
                <AlertDialogFooter>
                  <AlertDialogCancel>Tidak Jadi</AlertDialogCancel>
                  <AlertDialogAction onClick={() => handleSetDataDeviceName(devName)}>
                    Simpan
                  </AlertDialogAction>
                </AlertDialogFooter>
              </AlertDialogContent>
            </AlertDialog>
          </div>
        </section>

        <section>
          <div className="mx-auto flex max-w-[980px] flex-col items-center text-center gap-2 py-4 md:py-12 md:pb-8 lg:py-24 lg:pb-20 page-header pb-8">
            <h1 className="text-center text-3xl font-bold leading-tight md:block">Ganti Mode</h1>
            <div className="mt-3">
              <h1 className="text-xs">Mode Sekarang</h1>
              <p className="font-bold text-lg">MODE : {currentModeName}</p>
            </div>
          </div>
          
          <RadioGroup 
            value={alsData.mode} 
            onValueChange={handleSetDataDeviceMode}
            className="grid mt-2 py-2 text-center grid-cols-4 gap-4"
          >
            {availableMode.map((item) => (
              <div key={item.value} onClick={() => handleSetDataDeviceMode(item.value)}>
                <RadioGroupItem 
                  id={item.value} 
                  value={item.value} 
                  className="peer sr-only"
                />
                <Label
                  htmlFor={item.value}
                  className="flex flex-col items-center text-md justify-between rounded-md border-2 border-muted bg-popover p-4 hover:bg-accent hover:text-accent-foreground peer-data-[state=checked]:border-primary [&:has([data-state=checked])]:border-primary cursor-pointer"
                >
                  <Siren className="h-6 w-6 text-muted-foreground" />
                  {item.name}
                </Label>
              </div>
            ))}
          </RadioGroup>
        </section>

        <section>
          <div className="mx-auto flex max-w-[980px] flex-col items-center gap-2 py-2 md:py-12 md:pb-8 lg:py-24 lg:pb-20 page-header pb-8">
            <h1 className="text-center text-3xl font-bold leading-tight md:block">Ganti Delay</h1>
            <div className="grid gap-2 grid-cols-1 text-center">
              <div>
                <h1 className="text-xs">Delay Sekarang</h1>
                <p className="font-bold text-lg">{alsData.delay} mS</p>
              </div>
            </div>
            <div className="mt-4 w-[300px]">
              <Slider
                defaultValue={[alsData.delay]}
                max={300}
                min={30}
                step={1}
                onValueChange={handleSetDataDelayMode}
                className="w-full"
              />
            </div>
          </div>
        </section>
      </main>

      <footer className="py-6 md:px-8 md:py-0">
        <div className="container flex flex-col items-center justify-between gap-4 md:h-24 md:flex-row">
          <div className="text-center text-xs leading-loose text-muted-foreground md:text-left">
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