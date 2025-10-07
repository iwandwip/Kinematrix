'use client'

import { useEffect, useState } from 'react'
import { usePatternBuilderStore } from '@/lib/store/patternBuilderStore'

interface PatternBuilderHydratedProps {
  children: React.ReactNode
}

export function PatternBuilderHydrated({ children }: PatternBuilderHydratedProps) {
  const [hydrated, setHydrated] = useState(false)

  useEffect(() => {
    usePatternBuilderStore.persist.rehydrate()
    setHydrated(true)
  }, [])

  if (!hydrated) {
    return (
      <div className="flex items-center justify-center min-h-screen bg-background">
        <div className="text-center space-y-4">
          <div className="animate-spin rounded-full h-12 w-12 border-b-2 border-primary mx-auto" />
          <p className="text-sm text-muted-foreground">Loading Pattern Builder...</p>
        </div>
      </div>
    )
  }

  return <>{children}</>
}
