"use client"

import { useALS } from "@/lib/contexts/ALSContext"
import { useALSPolling } from "@/lib/hooks/useALSPolling"
import ModernTheme from "@/components/ModernTheme"
import ClassicTheme from "@/components/ClassicTheme"
import Spinner from "@/components/ui/spinner"
import { useEffect, useCallback } from "react"

export default function MainPage() {
  const { settingsData, stopLoading, setColors } = useALS()
  useALSPolling()

  const initializeApp = useCallback(async () => {
    setColors('dark')
    
    setTimeout(() => {
      stopLoading()
    }, 1000)
  }, [setColors, stopLoading])

  useEffect(() => {
    initializeApp()
  }, [initializeApp])

  if (settingsData.loader) {
    return <Spinner />
  }

  return (
    <>
      {settingsData.themes === 'classic' ? (
        <ClassicTheme />
      ) : (
        <ModernTheme />
      )}
    </>
  )
}