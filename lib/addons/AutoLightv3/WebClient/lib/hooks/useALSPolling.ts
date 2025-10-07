"use client"

import { useEffect, useRef, useCallback } from 'react'
import { useALS } from '@/lib/contexts/ALSContext'

function deepEqual(obj1: unknown, obj2: unknown): boolean {
  if (obj1 === obj2) return true
  if (obj1 == null || obj2 == null) return false
  if (typeof obj1 !== typeof obj2) return false
  
  if (typeof obj1 === 'object') {
    const keys1 = Object.keys(obj1)
    const keys2 = Object.keys(obj2)
    if (keys1.length !== keys2.length) return false
    
    for (const key of keys1) {
      if (!keys2.includes(key) || !deepEqual((obj1 as Record<string, unknown>)[key], (obj2 as Record<string, unknown>)[key])) return false
    }
    return true
  }
  
  return false
}

export function useALSPolling() {
  const { setDeviceName, setDeviceSerial, setDeviceCh, setMode, setDelay, alsData } = useALS()
  const isPollingRef = useRef(false)
  const timeoutRef = useRef<NodeJS.Timeout | null>(null)
  const settersRef = useRef({ setDeviceName, setDeviceSerial, setDeviceCh, setMode, setDelay })
  const apiConfigRef = useRef(alsData.apiConfig)
  const prevApiConfigRef = useRef(alsData.apiConfig)
  // Grace period refs to avoid brief UI reverts after local changes
  const lastModeChangeAtRef = useRef<number>(0)
  const lastDelayChangeAtRef = useRef<number>(0)
  const prevModeRef = useRef<string>(alsData.mode)
  const prevDelayRef = useRef<number>(alsData.delay)
  const GRACE_MS = 1000
  
  useEffect(() => {
    settersRef.current = { setDeviceName, setDeviceSerial, setDeviceCh, setMode, setDelay }
    
    if (!deepEqual(prevApiConfigRef.current, alsData.apiConfig)) {
      apiConfigRef.current = alsData.apiConfig
      prevApiConfigRef.current = alsData.apiConfig
    }
  })

  // Track recent local changes (mode/delay) to apply a short grace period
  useEffect(() => {
    if (alsData.mode !== prevModeRef.current) {
      prevModeRef.current = alsData.mode
      lastModeChangeAtRef.current = Date.now()
    }
  }, [alsData.mode])

  useEffect(() => {
    if (alsData.delay !== prevDelayRef.current) {
      prevDelayRef.current = alsData.delay
      lastDelayChangeAtRef.current = Date.now()
    }
  }, [alsData.delay])

  const getBaseUrl = useCallback(() => {
    const config = apiConfigRef.current
    return `${config.protocol}://${config.host}:${config.port}/api/v1`
  }, [])


  const fetchData = useCallback(async () => {
    if (isPollingRef.current) return
    
    isPollingRef.current = true
    
    try {
      const baseUrl = getBaseUrl()
      
      const [channelResult, deviceNameResult, serialNumberResult, modeResult, delayResult] = await Promise.allSettled([
        fetch(`${baseUrl}/data/get/device/ch`).then(res => res.json()).catch(() => 0),
        fetch(`${baseUrl}/data/get/device/name`).then(res => res.text()).catch(() => ''),
        fetch(`${baseUrl}/data/get/device/serial`).then(res => res.text()).catch(() => ''),
        fetch(`${baseUrl}/data/get/mode`).then(res => res.json()).then(data => data.toString()).catch(() => '0'),
        fetch(`${baseUrl}/data/get/delay`).then(res => res.json()).catch(() => 0)
      ])

      if (delayResult.status === 'fulfilled' && delayResult.value !== undefined) {
        const now = Date.now()
        const withinGrace = now - lastDelayChangeAtRef.current < GRACE_MS
        // Only apply remote delay if either it matches current or grace period elapsed
        if (!withinGrace || delayResult.value === prevDelayRef.current) {
          settersRef.current.setDelay(delayResult.value)
        }
      }
      if (modeResult.status === 'fulfilled' && modeResult.value !== undefined) {
        const now = Date.now()
        const withinGrace = now - lastModeChangeAtRef.current < GRACE_MS
        // Only apply remote mode if either it matches current or grace period elapsed
        if (!withinGrace || modeResult.value === prevModeRef.current) {
          settersRef.current.setMode(modeResult.value)
        }
      }
      if (deviceNameResult.status === 'fulfilled' && deviceNameResult.value !== undefined) {
        settersRef.current.setDeviceName(deviceNameResult.value)
      }
      if (serialNumberResult.status === 'fulfilled' && serialNumberResult.value !== undefined) {
        settersRef.current.setDeviceSerial(serialNumberResult.value)
      }
      if (channelResult.status === 'fulfilled' && channelResult.value !== undefined) {
        settersRef.current.setDeviceCh(channelResult.value)
      }
    } catch (error) {
      console.error('Failed to fetch ALS data:', error)
    } finally {
      isPollingRef.current = false
    }
  }, [getBaseUrl])

  const startPolling = useCallback(() => {
    const poll = () => {
      fetchData().finally(() => {
        timeoutRef.current = setTimeout(poll, 2000)
      })
    }
    poll()
  }, [fetchData])

  const stopPolling = useCallback(() => {
    if (timeoutRef.current) {
      clearTimeout(timeoutRef.current)
      timeoutRef.current = null
    }
    isPollingRef.current = false
  }, [])

  useEffect(() => {
    let isInitialized = false
    
    const initPolling = async () => {
      if (isInitialized) return
      isInitialized = true
      
      try {
        await fetchData()
        startPolling()
      } catch (error) {
        console.error('Failed to initialize polling:', error)
      }
    }
    
    initPolling()
    
    return () => {
      stopPolling()
      isInitialized = true
    }
  }, [fetchData, startPolling, stopPolling])

  return { fetchData, startPolling, stopPolling }
}
