import { useCallback, useRef } from 'react'

// Debounce hook for preventing rapid function calls
export function useDebounce<T extends (...args: never[]) => void>(
  callback: T,
  delay: number
): T {
  const timeoutRef = useRef<NodeJS.Timeout | null>(null)

  return useCallback((...args: Parameters<T>) => {
    if (timeoutRef.current) {
      clearTimeout(timeoutRef.current)
    }

    timeoutRef.current = setTimeout(() => {
      callback(...args)
    }, delay)
  }, [callback, delay]) as T
}

// Throttle hook for rate limiting function calls
export function useThrottle<T extends (...args: never[]) => void>(
  callback: T,
  delay: number
): T {
  const lastCallRef = useRef<number>(0)

  return useCallback((...args: Parameters<T>) => {
    const now = Date.now()

    if (now - lastCallRef.current >= delay) {
      lastCallRef.current = now
      callback(...args)
    }
  }, [callback, delay]) as T
}

// Memoize expensive calculations with stable reference
export function useStableMemo<T>(
  factory: () => T,
  deps: React.DependencyList
): T {
  const ref = useRef<{ deps: React.DependencyList; value: T } | null>(null)

  if (!ref.current || !deps.every((dep, i) => dep === ref.current!.deps[i])) {
    ref.current = { deps, value: factory() }
  }

  return ref.current.value
}

// Performance monitor utility
export class PerformanceMonitor {
  private static measurements = new Map<string, number[]>()

  static start(label: string): () => void {
    const startTime = performance.now()

    return () => {
      const endTime = performance.now()
      const duration = endTime - startTime

      if (!this.measurements.has(label)) {
        this.measurements.set(label, [])
      }

      this.measurements.get(label)!.push(duration)

      // Log if operation takes longer than expected
      if (duration > 16) { // More than one frame
        console.warn(`Slow operation detected: ${label} took ${duration.toFixed(2)}ms`)
      }
    }
  }

  static getStats(label: string): { avg: number; max: number; count: number } | null {
    const measurements = this.measurements.get(label)
    if (!measurements || measurements.length === 0) return null

    const avg = measurements.reduce((sum, time) => sum + time, 0) / measurements.length
    const max = Math.max(...measurements)
    const count = measurements.length

    return { avg, max, count }
  }

  static clear(label?: string): void {
    if (label) {
      this.measurements.delete(label)
    } else {
      this.measurements.clear()
    }
  }
}

// Intersection Observer hook for lazy loading
export function useIntersectionObserver(
  callback: (entries: IntersectionObserverEntry[]) => void,
  options?: IntersectionObserverInit
): (node: Element | null) => void {
  const observerRef = useRef<IntersectionObserver | null>(null)

  const ref = useCallback((node: Element | null) => {
    if (observerRef.current) {
      observerRef.current.disconnect()
    }

    if (node) {
      observerRef.current = new IntersectionObserver(callback, options)
      observerRef.current.observe(node)
    }
  }, [callback, options])

  return ref
}