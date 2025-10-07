"use client"

import { useEffect } from 'react'
import { useSimulation } from '@/lib/contexts/SimulationContext'

export function useSaveBehavior() {
  const { simulationData } = useSimulation()

  useEffect(() => {
    if (simulationData.saveBehaviorMode === 'page-wide') {
      const handleKeyDown = (e: KeyboardEvent) => {
        const isCtrlCmd = e.ctrlKey || e.metaKey
        if (isCtrlCmd && e.key === 's') {
          e.preventDefault()
          e.stopPropagation()
        }
      }

      document.addEventListener('keydown', handleKeyDown, { capture: true })

      return () => {
        document.removeEventListener('keydown', handleKeyDown, { capture: true })
      }
    }

    return undefined
  }, [simulationData.saveBehaviorMode])
}