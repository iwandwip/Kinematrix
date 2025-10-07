import { useCallback, useRef, useEffect } from 'react'
import { PrecisionTimer } from '@/lib/utils/precisionTimer'

interface PlaybackFrame {
  delay: number
  onComplete: () => void
}

export function usePrecisionPlayback() {
  const timerRef = useRef<PrecisionTimer | null>(null)
  const isPlayingRef = useRef(false)

  useEffect(() => {
    timerRef.current = new PrecisionTimer()

    return () => {
      timerRef.current?.clear()
    }
  }, [])

  const scheduleFrame = useCallback((frame: PlaybackFrame) => {
    if (!timerRef.current || !isPlayingRef.current) {
      return () => {}
    }

    const adjustedDelay = Math.max(1, frame.delay) // Minimum 1ms to prevent tight loops

    return timerRef.current.scheduleCallback(adjustedDelay, () => {
      if (isPlayingRef.current) {
        frame.onComplete()
      }
    })
  }, [])

  const startPlayback = useCallback(() => {
    isPlayingRef.current = true
  }, [])

  const stopPlayback = useCallback(() => {
    isPlayingRef.current = false
    timerRef.current?.clear()
  }, [])

  const getTimingAccuracy = useCallback(() => {
    return timerRef.current?.getTimingAccuracy()
  }, [])

  return {
    scheduleFrame,
    startPlayback,
    stopPlayback,
    getTimingAccuracy,
    isPlaying: () => isPlayingRef.current
  }
}