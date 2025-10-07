import { LEDFrame } from '@/lib/contexts/SimulationContext'
import { PrecisionTimer } from '@/lib/utils/precisionTimer'

export interface ExecutionStats {
  executionTime: number
  frameCount: number
  memoryUsage: number
  warningMessages: string[]
}

export interface GlobalAPI {
  set: (index: number, state: boolean) => void
  sleep: (ms: number) => Promise<void>
  off: () => void
  on: () => void
  map: (value: number, fromLow: number, fromHigh: number, toLow: number, toHigh: number) => number
  random: (minOrMax: number, max?: number) => number
  constrain: (value: number, min: number, max: number) => number
  ceil: (value: number) => number
  floor: (value: number) => number
  setRandomLed: (value: boolean) => void
  setWaveLed: (value: boolean) => void
  setWaveLedReverse: (value: boolean) => void
  resetArray: (array: number[], arrSizeRef: {value: number}, arrLastIndexRef: {value: number}) => void
  findMax: (...numbers: number[]) => number
  shiftArrayAccordingToOrder: (patterns: number[][], patternSize: number, mainArray: number[], mainArraySize: number, arr: number[], lastIndexRef: {value: number}, targetIndex: number, reverse?: boolean) => number
  totalChannels: number
  delayTime: number
  channels: boolean[]
  pin_size_: number
  delay_time_: number
  pin_ptr_: number[]
  halfSize: number
}

export class JavaScriptEngine {
  private frames: LEDFrame[] = []
  private currentChannels: boolean[] = []
  private isExecuting: boolean = false
  private executionTimeout: NodeJS.Timeout | null = null
  private frameCounter: number = 0
  private startTime: number = 0
  private onProgress?: (frameCount: number) => void
  private precisionTimer: PrecisionTimer = new PrecisionTimer()

  private readonly EXECUTION_TIMEOUT = 30000 // 30 seconds
  private readonly MAX_FRAMES = 1000 // Prevent excessive frame generation

  constructor() {
    this.reset()
  }

  async executeCode(
    code: string,
    totalChannels: number,
    delayTime: number,
    onProgress?: (frameCount: number) => void
  ): Promise<LEDFrame[]> {
    return this.executeCodeInternal(code, totalChannels, delayTime, onProgress, false)
  }

  async generateFramesOnly(
    code: string,
    totalChannels: number,
    delayTime: number,
    onProgress?: (frameCount: number) => void
  ): Promise<LEDFrame[]> {
    return this.executeCodeInternal(code, totalChannels, delayTime, onProgress, true)
  }

  private async executeCodeInternal(
    code: string,
    totalChannels: number,
    delayTime: number,
    onProgress?: (frameCount: number) => void,
    fastGeneration: boolean = false
  ): Promise<LEDFrame[]> {
    if (this.isExecuting) {
      throw new Error('JavaScript execution already in progress')
    }

    this.reset()
    this.isExecuting = true
    this.startTime = performance.now()
    this.currentChannels = Array(totalChannels).fill(false)
    this.onProgress = onProgress

    return new Promise(async (resolve, reject) => {
      this.executionTimeout = setTimeout(() => {
        this.isExecuting = false
        reject(new Error('Execution timeout: Pattern took too long to execute (30 seconds limit)'))
      }, this.EXECUTION_TIMEOUT)

      try {
        const globalAPI = this.createGlobalAPI(totalChannels, delayTime, fastGeneration)

        const wrappedCode = `
          (async function() {
            const { set, sleep, off, on, map, random, constrain, ceil, floor, setRandomLed, setWaveLed, setWaveLedReverse, resetArray, findMax, shiftArrayAccordingToOrder, totalChannels, delayTime, channels, pin_size_, delay_time_, pin_ptr_, halfSize } = arguments[0];

            ${code}
          })
        `

        const executeFunction = eval(wrappedCode)
        await executeFunction(globalAPI)

        if (this.executionTimeout) {
          clearTimeout(this.executionTimeout)
        }

        this.isExecuting = false
        resolve([...this.frames])
      } catch (error) {
        if (this.executionTimeout) {
          clearTimeout(this.executionTimeout)
        }

        this.isExecuting = false
        reject(new Error(`JavaScript execution error: ${error instanceof Error ? error.message : String(error)}`))
      }
    })
  }

  stopExecution(): void {
    if (this.executionTimeout) {
      clearTimeout(this.executionTimeout)
      this.executionTimeout = null
    }
    this.precisionTimer.clear()
    this.isExecuting = false
  }

  getExecutionStats(): ExecutionStats {
    const executionTime = this.startTime ? performance.now() - this.startTime : 0
    const warnings: string[] = []

    if (this.frames.length > 500) {
      warnings.push('High frame count detected. Consider optimizing your pattern.')
    }

    if (executionTime > 5000) {
      warnings.push('Slow execution detected. Pattern may be too complex.')
    }

    if (this.frames.length >= this.MAX_FRAMES) {
      warnings.push('Maximum frame limit reached. Pattern was truncated.')
    }

    return {
      executionTime,
      frameCount: this.frames.length,
      memoryUsage: this.estimateMemoryUsage(),
      warningMessages: warnings
    }
  }

  private createGlobalAPI(totalChannels: number, delayTime: number, fastGeneration: boolean = false): GlobalAPI {
    const pin_ptr_ = Array.from({ length: totalChannels }, (_, i) => i)
    const halfSize = Math.floor(totalChannels / 2)

    // Store references for lambda functions
    const currentChannels = this.currentChannels
    const captureFrame = this.captureFrame.bind(this)
    const onProgress = this.onProgress
    const frames = this.frames
    const precisionTimer = this.precisionTimer

    return {
      set: (index: number, state: boolean) => {
        if (index >= 0 && index < totalChannels) {
          currentChannels[index] = state
        }
      },

      sleep: async (ms: number) => {
        captureFrame(Math.max(0, ms))
        if (onProgress) {
          onProgress(frames.length)
        }

        if (fastGeneration) {
          // Fast generation: minimal delay, just yield control
          await new Promise(resolve => setTimeout(resolve, 1))
        } else {
          // High-precision timing for accurate delays
          await precisionTimer.sleep(Math.max(0, ms))
        }
      },

      off: () => {
        for (let i = 0; i < totalChannels; i++) {
          currentChannels[i] = false
        }
      },

      on: () => {
        for (let i = 0; i < totalChannels; i++) {
          currentChannels[i] = true
        }
      },

      map: (value: number, fromLow: number, fromHigh: number, toLow: number, toHigh: number): number => {
        return toLow + (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow)
      },

      random: (minOrMax: number, max?: number): number => {
        if (max === undefined) {
          // Arduino-style random(max) - returns 0 to max-1
          return Math.floor(Math.random() * minOrMax)
        }
        // Standard random(min, max) - returns min to max inclusive
        return Math.floor(Math.random() * (max - minOrMax + 1)) + minOrMax
      },

      ceil: (value: number): number => {
        return Math.ceil(value)
      },

      floor: (value: number): number => {
        return Math.floor(value)
      },

      constrain: (value: number, min: number, max: number): number => {
        return Math.max(min, Math.min(max, value))
      },

      totalChannels,
      delayTime,
      channels: currentChannels,
      pin_size_: totalChannels,
      delay_time_: delayTime,
      pin_ptr_,
      halfSize,

      // Lambda helper functions from BaseChannelSequence.cpp - EXACT 1:1 MAPPING
      setRandomLed: async (value: boolean) => {
        // Exact C++ algorithm from BaseChannelSequence.cpp:377-393
        const data = new Array(totalChannels).fill(!value)

        for (let i = 0; i < totalChannels; i++) {
          let index
          do {
            index = Math.floor(Math.random() * totalChannels)
          } while (data[index] === value)

          data[index] = value

          // Apply full array state to channels (exact C++ behavior)
          for (let j = 0; j < totalChannels; j++) {
            currentChannels[j] = data[j]
          }

          // Capture frame after each channel update
          if (!fastGeneration) {
            captureFrame(0)
          }
        }
      },

      setWaveLed: async (value: boolean) => {
        // EXACT C++ algorithm from BaseChannelSequence.cpp:406-418
        for (let i = 0; i < Math.floor(totalChannels / 2); i++) {
          if (i < Math.ceil(Math.floor(totalChannels / 2) / 2)) {
            // Front half: first quadrant and mirror
            currentChannels[pin_ptr_[i]] = value
            currentChannels[pin_ptr_[Math.floor(totalChannels / 2) - (i + 1)]] = value
          } else {
            // Back half: second quadrant and mirror
            currentChannels[pin_ptr_[i + Math.floor(totalChannels / 2)]] = value
            currentChannels[pin_ptr_[totalChannels - i - 1]] = value
          }

          // Sleep after each iteration (exact C++ behavior)
          if (!fastGeneration) {
            await precisionTimer.sleep(delayTime)
          }
        }
      },

      setWaveLedReverse: async (value: boolean) => {
        // EXACT C++ algorithm from BaseChannelSequence.cpp:420-431
        for (let i = Math.floor(totalChannels / 2) - 1; i >= 0; i--) {
          if (i >= Math.ceil(Math.floor(totalChannels / 2) / 2)) {
            // Back half first: second quadrant and mirror
            currentChannels[pin_ptr_[i + Math.floor(totalChannels / 2)]] = value
            currentChannels[pin_ptr_[totalChannels - i - 1]] = value
          } else {
            // Front half: first quadrant and mirror
            currentChannels[pin_ptr_[i]] = value
            currentChannels[pin_ptr_[Math.floor(totalChannels / 2) - (i + 1)]] = value
          }

          // Sleep after each iteration (exact C++ behavior)
          if (!fastGeneration) {
            await precisionTimer.sleep(delayTime)
          }
        }
      },

      // Array utility functions from BaseChannel.cpp - EXACT 1:1 MAPPING
      resetArray: (array: number[], arrSizeRef: {value: number}, arrLastIndexRef: {value: number}) => {
        // EXACT C++ algorithm from BaseChannel.cpp:665-671
        for (let i = 0; i < arrSizeRef.value; i++) {
          array[i] = 0
        }
        arrSizeRef.value = 0
        arrLastIndexRef.value = 0
      },

      findMax: (...numbers: number[]): number => {
        const validNumbers = numbers.filter(n => n !== -1 && !isNaN(n))
        return validNumbers.length > 0 ? Math.max(...validNumbers) : -1
      },

      // Advanced pattern matrix function from BaseChannel.cpp - EXACT 1:1 MAPPING
      shiftArrayAccordingToOrder: (patterns: number[][], patternSize: number, mainArray: number[], mainArraySize: number, arr: number[], lastIndexRef: {value: number}, targetIndex: number, reverse: boolean = false): number => {
        // EXACT C++ algorithm from BaseChannel.cpp:609-630
        const patternIndex = Math.floor((24 - mainArraySize) / 2)
        if (patternIndex < 0 || patternIndex >= patternSize) return 0

        const subArraySize = patterns[patternIndex][targetIndex]
        if (subArraySize === 0) return 0

        let currentIndex = 0
        for (let i = 0; i < targetIndex; i++) {
          currentIndex += patterns[patternIndex][i]
        }

        for (let i = 0; i < subArraySize; i++) {
          if (!reverse) {
            arr[lastIndexRef.value + i] = mainArray[currentIndex + i]
          } else {
            arr[lastIndexRef.value + i] = mainArray[currentIndex + subArraySize - 1 - i]
          }
        }

        lastIndexRef.value += subArraySize
        return subArraySize
      }
    }
  }

  private captureFrame(delay: number): void {
    if (this.frames.length >= this.MAX_FRAMES) {
      return
    }

    this.frameCounter++
    const frame: LEDFrame = {
      id: `js-frame-${this.frameCounter}`,
      channels: [...this.currentChannels],
      delay: delay,
      label: `Frame ${this.frameCounter}`
    }

    this.frames.push(frame)
  }

  private estimateMemoryUsage(): number {
    const bytesPerFrame = this.currentChannels.length + 50
    return this.frames.length * bytesPerFrame
  }

  private reset(): void {
    this.frames = []
    this.currentChannels = []
    this.frameCounter = 0
    this.isExecuting = false
    this.precisionTimer.clear()

    if (this.executionTimeout) {
      clearTimeout(this.executionTimeout)
      this.executionTimeout = null
    }
  }

  isCurrentlyExecuting(): boolean {
    return this.isExecuting
  }

  getFrameCount(): number {
    return this.frames.length
  }
}