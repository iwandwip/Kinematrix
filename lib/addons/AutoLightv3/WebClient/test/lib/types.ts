export interface LEDFrame {
  frameNumber: number
  channels: boolean[]
  delay: number
  timestamp?: number
}

export interface ParsedCommand {
  type: 'set' | 'sleep' | 'off' | 'on' | 'for_start' | 'for_end' | 'variable' | 'block_start' | 'block_end'
  channel?: number
  state?: boolean
  delay?: number
  variable?: string
  value?: number | string
  loopStart?: number
  loopEnd?: number
  loopStep?: number
  loopVariable?: string
  expression?: string
}

export interface ParserConfig {
  totalChannels: number
  defaultDelay: number
  pinSize: number
  outputFormat: 'console' | 'file' | 'both'
  outputPath?: string
}

export interface SimulationState {
  channels: boolean[]
  variables: Map<string, number>
  frameCounter: number
  totalChannels: number
  currentDelay: number
}

export interface TestResult {
  sequenceName: string
  inputSize: number
  outputFrames: number
  duration: number
  success: boolean
  errors: string[]
  frameData: LEDFrame[]
}

export interface ComparisonResult {
  sequenceName: string
  oldFrames: number
  newFrames: number
  matchingFrames: number
  accuracy: number
  differences: Array<{
    frame: number
    oldChannels: boolean[]
    newChannels: boolean[]
  }>
}