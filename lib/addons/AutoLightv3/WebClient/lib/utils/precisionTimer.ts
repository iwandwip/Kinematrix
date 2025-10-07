export class PrecisionTimer {
  private startTime: number = 0
  private targetDelay: number = 0
  private callback: (() => void) | null = null
  private animationFrameId: number | null = null
  private timeoutId: NodeJS.Timeout | null = null

  async sleep(ms: number): Promise<void> {
    return new Promise((resolve) => {
      this.preciseSleep(ms, resolve)
    })
  }

  private preciseSleep(ms: number, callback: () => void): void {
    if (ms <= 0) {
      callback()
      return
    }

    const startTime = performance.now()
    const targetTime = startTime + ms

    const tick = () => {
      const currentTime = performance.now()
      const remaining = targetTime - currentTime

      if (remaining <= 0) {
        callback()
        return
      }

      // Simplified: Use single approach based on remaining time
      if (remaining > 16) {
        // For longer delays, use setTimeout to avoid RAF overhead
        this.timeoutId = setTimeout(tick, Math.min(remaining, 100))
      } else {
        // For short delays, use RAF for precision
        this.animationFrameId = requestAnimationFrame(tick)
      }
    }

    tick()
  }

  scheduleCallback(delay: number, callback: () => void): () => void {
    this.startTime = performance.now()
    this.targetDelay = delay
    this.callback = callback

    this.preciseSleep(delay, callback)

    return () => this.clear()
  }

  clear(): void {
    if (this.animationFrameId) {
      cancelAnimationFrame(this.animationFrameId)
      this.animationFrameId = null
    }
    if (this.timeoutId) {
      clearTimeout(this.timeoutId)
      this.timeoutId = null
    }
    this.callback = null
  }

  // Get timing accuracy for debugging
  getTimingAccuracy(): { expected: number; actual: number; accuracy: number } {
    const actual = performance.now() - this.startTime
    const expected = this.targetDelay
    const accuracy = Math.abs(expected - actual)

    return { expected, actual, accuracy }
  }
}

export const createPrecisionTimer = () => new PrecisionTimer()