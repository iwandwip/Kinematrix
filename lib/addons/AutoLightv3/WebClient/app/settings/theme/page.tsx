"use client"

import { useState } from "react"
import { useRouter } from "next/navigation"
import { Button } from '@/components/ui/button'
import { Label } from '@/components/ui/label'
import { RadioGroup, RadioGroupItem } from '@/components/ui/radio-group'
import { Card, CardDescription, CardContent, CardFooter, CardHeader, CardTitle } from '@/components/ui/card'
import { LoaderCircle } from "lucide-react"
import { useALS } from "@/lib/contexts/ALSContext"

export default function SettingsThemePage() {
  const router = useRouter()
  const { settingsData, setThemes, startLoading, stopLoading } = useALS()
  const [temaSelection, setTemaSelection] = useState(settingsData.themes)

  const gantiTema = () => {
    try {
      startLoading()
      setThemes(temaSelection)
      stopLoading()
      router.push('/')
    } catch (e) {
      console.error('Error changing theme:', e)
    }
  }

  return (
    <Card>
      <CardHeader>
        <CardTitle>Pilih Tema</CardTitle>
        <CardDescription>
          Pilih tema aplikasi sesuai dengan selera anda
        </CardDescription>
      </CardHeader>
      <CardContent>
        <RadioGroup value={temaSelection} onValueChange={setTemaSelection}>
          <div className="flex items-center space-x-2">
            <RadioGroupItem id="r1" value="classic" />
            <Label htmlFor="r1">Klasik</Label>
          </div>
          <div className="flex items-center space-x-2">
            <RadioGroupItem id="r2" value="modern" />
            <Label htmlFor="r2">Modern</Label>
          </div>
        </RadioGroup>
      </CardContent>
      <CardFooter className="border-t px-6 py-4">
        <Button onClick={gantiTema}>
          Ganti 
          {settingsData.loader && (
            <LoaderCircle className="ml-2 animate-spin h-6 w-6" />
          )}
        </Button>
      </CardFooter>
    </Card>
  )
}