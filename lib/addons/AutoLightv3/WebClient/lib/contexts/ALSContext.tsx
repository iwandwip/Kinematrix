"use client"

import React, { createContext, useContext, useState, useEffect, ReactNode, useCallback, useMemo } from 'react'

interface ALSData {
  deviceName: string
  deviceSerial: string
  deviceCh: number
  mode: string
  delay: number
  apiConfig: {
    host: string
    port: number
    protocol: string
    connectionType: 'ip' | 'dns'
  }
}

interface SettingsData {
  themes: string
  colors: string
  loader: boolean
  page: string
  failedLoad: number
}

interface ALSContextType {
  alsData: ALSData
  settingsData: SettingsData
  
  setDeviceName: (name: string) => void
  setDeviceSerial: (serial: string) => void
  setDeviceCh: (ch: number) => void
  setMode: (mode: string) => void
  setDelay: (delay: number) => void
  incrementDelay: (val: number) => void
  decrementDelay: (val: number) => void
  
  setThemes: (theme: string) => void
  setColors: (color: string) => void
  startLoading: () => void
  stopLoading: () => void
  
  setApiConfig: (config: { host: string; port: number; protocol: string; connectionType: 'ip' | 'dns' }) => void
  resetApiConfig: () => void
}

const ALSContext = createContext<ALSContextType | undefined>(undefined)

export function ALSProvider({ children }: { children: ReactNode }) {
  const [alsData, setALSData] = useState<ALSData>({
    deviceName: '',
    deviceSerial: '',
    deviceCh: 24,
    mode: '',
    delay: 0,
    apiConfig: {
      host: '192.168.4.1',
      port: 8000,
      protocol: 'http',
      connectionType: 'ip'
    }
  })

  useEffect(() => {
    const savedConfig = localStorage.getItem('autolight-api-config')
    if (savedConfig) {
      try {
        const config = JSON.parse(savedConfig)
        const migratedConfig = {
          host: config.host || config.ip || '192.168.4.1',
          port: config.port || 8000,
          protocol: config.protocol || 'http',
          connectionType: config.connectionType || (config.ip ? 'ip' : 'ip')
        }
        setALSData(prev => {
          if (prev.apiConfig.host !== migratedConfig.host || 
              prev.apiConfig.port !== migratedConfig.port || 
              prev.apiConfig.protocol !== migratedConfig.protocol ||
              prev.apiConfig.connectionType !== migratedConfig.connectionType) {
            return { ...prev, apiConfig: migratedConfig }
          }
          return prev
        })
      } catch (error) {
        console.error('Failed to parse saved API config:', error)
      }
    }
  }, [])

  const [settingsData, setSettingsData] = useState<SettingsData>({
    themes: 'classic',
    colors: 'dark',
    loader: false,
    page: 'index',
    failedLoad: 0,
  })

  const setDeviceName = useCallback((name: string) => {
    setALSData(prev => prev.deviceName !== name ? { ...prev, deviceName: name } : prev)
  }, [])

  const setDeviceSerial = useCallback((serial: string) => {
    setALSData(prev => prev.deviceSerial !== serial ? { ...prev, deviceSerial: serial } : prev)
  }, [])

  const setDeviceCh = useCallback((ch: number) => {
    setALSData(prev => prev.deviceCh !== ch ? { ...prev, deviceCh: ch } : prev)
  }, [])

  const setMode = useCallback((mode: string) => {
    setALSData(prev => prev.mode !== mode ? { ...prev, mode } : prev)
  }, [])

  const setDelay = useCallback((delay: number) => {
    setALSData(prev => prev.delay !== delay ? { ...prev, delay } : prev)
  }, [])

  const incrementDelay = useCallback((val: number) => {
    setALSData(prev => ({ ...prev, delay: prev.delay + val }))
  }, [])

  const decrementDelay = useCallback((val: number) => {
    setALSData(prev => ({ ...prev, delay: Math.max(30, prev.delay - val) }))
  }, [])

  const setThemes = useCallback((theme: string) => {
    setSettingsData(prev => prev.themes !== theme ? { ...prev, themes: theme } : prev)
  }, [])

  const setColors = useCallback((color: string) => {
    setSettingsData(prev => prev.colors !== color ? { ...prev, colors: color } : prev)
  }, [])

  const startLoading = useCallback(() => {
    setSettingsData(prev => ({ ...prev, loader: true }))
  }, [])

  const stopLoading = useCallback(() => {
    setSettingsData(prev => ({ ...prev, loader: false }))
  }, [])

  const setApiConfig = useCallback((config: { host: string; port: number; protocol: string; connectionType: 'ip' | 'dns' }) => {
    setALSData(prev => {
      if (prev.apiConfig.host !== config.host || 
          prev.apiConfig.port !== config.port || 
          prev.apiConfig.protocol !== config.protocol ||
          prev.apiConfig.connectionType !== config.connectionType) {
        localStorage.setItem('autolight-api-config', JSON.stringify(config))
        return { ...prev, apiConfig: config }
      }
      return prev
    })
  }, [])

  const resetApiConfig = useCallback(() => {
    const defaultConfig = { host: '192.168.4.1', port: 8000, protocol: 'http', connectionType: 'ip' as const }
    setALSData(prev => {
      if (prev.apiConfig.host !== defaultConfig.host || 
          prev.apiConfig.port !== defaultConfig.port || 
          prev.apiConfig.protocol !== defaultConfig.protocol ||
          prev.apiConfig.connectionType !== defaultConfig.connectionType) {
        localStorage.setItem('autolight-api-config', JSON.stringify(defaultConfig))
        return { ...prev, apiConfig: defaultConfig }
      }
      return prev
    })
  }, [])

  const memoizedCallbacks = useMemo(() => ({
    setDeviceName,
    setDeviceSerial,
    setDeviceCh,
    setMode,
    setDelay,
    incrementDelay,
    decrementDelay,
    setThemes,
    setColors,
    startLoading,
    stopLoading,
    setApiConfig,
    resetApiConfig,
  }), [
    setDeviceName,
    setDeviceSerial,
    setDeviceCh,
    setMode,
    setDelay,
    incrementDelay,
    decrementDelay,
    setThemes,
    setColors,
    startLoading,
    stopLoading,
    setApiConfig,
    resetApiConfig,
  ])

  const contextValue = useMemo(() => ({
    alsData,
    settingsData,
    ...memoizedCallbacks,
  }), [
    alsData,
    settingsData,
    memoizedCallbacks,
  ])

  return (
    <ALSContext.Provider value={contextValue}>
      {children}
    </ALSContext.Provider>
  )
}

export function useALS() {
  const context = useContext(ALSContext)
  if (context === undefined) {
    throw new Error('useALS must be used within an ALSProvider')
  }
  return context
}