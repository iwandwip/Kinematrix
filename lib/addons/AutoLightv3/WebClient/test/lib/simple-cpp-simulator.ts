import { LEDFrame, ParserConfig } from './types'

export class SimpleCppSimulator {
  private config: ParserConfig
  private channels: boolean[]
  private frameCounter: number
  private frames: LEDFrame[]

  constructor(config: ParserConfig) {
    this.config = config
    this.channels = new Array(config.totalChannels).fill(false)
    this.frameCounter = 0
    this.frames = []
  }

  simulate(cppCode: string): LEDFrame[] {
    this.frames = []
    this.frameCounter = 0
    this.channels = new Array(this.config.totalChannels).fill(false)

    const variables = new Map<string, number>([
      ['pin_size_', this.config.totalChannels],
      ['delay_time_', this.config.defaultDelay]
    ])

    console.log('Starting simulation with enhanced expression evaluation...')
    const expandedCode = this.expandLoops(cppCode, variables)
    console.log(`Expanded from ${cppCode.split('\n').length} to ${expandedCode.split('\n').length} lines`)

    const lines = expandedCode.split('\n')
    let sleepCount = 0
    let setCount = 0

    for (const line of lines) {
      const trimmed = line.trim()
      if (this.isSetCommand(trimmed)) {
        this.executeSetCommand(trimmed)
        setCount++
      } else if (this.isSleepCommand(trimmed)) {
        this.executeSleepCommand(trimmed)
        sleepCount++
      } else if (this.isOffCommand(trimmed)) {
        this.executeOffCommand()
      } else if (this.isOnCommand(trimmed)) {
        this.executeOnCommand()
      }
    }

    console.log(`Processed ${setCount} SET commands and ${sleepCount} SLEEP commands`)
    console.log(`Generated ${this.frames.length} frames total`)
    return this.frames
  }

  private expandLoops(code: string, variables: Map<string, number>): string {
    const lines = code.split('\n')
    const result: string[] = []

    let i = 0
    while (i < lines.length) {
      const line = lines[i].trim()

      if (line.includes('for (')) {
        const forMatch = line.match(/for\s*\(\s*int\s+(\w+)\s*=\s*([^;]+);\s*(\w+)\s*([<>=]+)\s*([^;]+);\s*([^)]+)\)/)
        if (forMatch) {
          const variable = forMatch[1]
          const startExpr = forMatch[2].trim()
          forMatch[3].trim() // conditionVar not used
          const operator = forMatch[4].trim()
          const endExpr = forMatch[5].trim()
          const stepExpr = forMatch[6].trim()

          const start = this.evaluateExpression(startExpr, variables, new Map())
          const end = this.evaluateExpression(endExpr, variables, new Map())
          console.log(`  Start expression: '${startExpr}' = ${start}`)
          console.log(`  End expression: '${endExpr}' = ${end}`)
          let step = 1

          if (stepExpr.includes('++')) {
            step = 1
          } else if (stepExpr.includes('--')) {
            step = -1
          } else if (stepExpr.includes('+=')) {
            const match = stepExpr.match(/\+=\s*(\d+)/)
            if (match) {
              step = parseInt(match[1])
            }
          } else if (stepExpr.includes('-=')) {
            const match = stepExpr.match(/-=\s*(\d+)/)
            if (match) {
              step = -parseInt(match[1])
            }
          }

          console.log(`Found loop: ${variable} from ${start} to ${end} step ${step} (operator: ${operator})`)

          const body: string[] = []
          let braceCount = 0
          let j = i + 1

          while (j < lines.length) {
            const bodyLine = lines[j].trim()
            if (bodyLine === '{') {
              braceCount++
            } else if (bodyLine === '}') {
              braceCount--
              if (braceCount < 0) break
            } else if (braceCount >= 0) {
              body.push(lines[j])
            }
            j++
          }

          const shouldContinue = (() => {
            if (operator === '<') {
              return (current: number) => current < end
            } else if (operator === '<=') {
              return (current: number) => current <= end
            } else if (operator === '>') {
              return (current: number) => current > end
            } else if (operator === '>=') {
              return (current: number) => current >= end
            } else {
              // default fallback
              return step > 0
                ? (current: number) => current < end
                : (current: number) => current > end
            }
          })()

          for (let current = start; shouldContinue(current); current += step) {
            const loopVars = new Map([[variable, current]])

            for (const bodyLine of body) {
              if (bodyLine.trim()) {
                if (bodyLine.includes('if (')) {
                  const conditionalResult = this.processConditional(bodyLine, body, body.indexOf(bodyLine), variables, loopVars)
                  if (conditionalResult) {
                    conditionalResult.lines.forEach(line => {
                      if (line.trim()) {
                        result.push(line)
                      }
                    })
                  }
                } else {
                  const substituted = this.substituteVariables(bodyLine, variables, loopVars)
                  substituted.split('\n').forEach(line => {
                    if (line.trim()) {
                      result.push(line)
                    }
                  })
                }
              }
            }
          }

          i = j + 1
          continue
        }
      }

      if (!line.startsWith('}') && !line.startsWith('{') && line.length > 0) {
        result.push(lines[i])
      }
      i++
    }

    return result.join('\n')
  }

  private substituteVariables(code: string, globals: Map<string, number>, locals: Map<string, number>): string {
    let result = code

    result = result.replace(/config_data_ptr_->header\.pin_size_/g, globals.get('pin_size_')?.toString() || '24')
    result = result.replace(/channel_data_\.delay_time_/g, globals.get('delay_time_')?.toString() || '30')

    locals.forEach((value, variable) => {
      result = result.replace(new RegExp(`\\[${variable}\\s*\\+\\s*(\\d+)\\]`, 'g'), (match, offset) => {
        return `[${value + parseInt(offset)}]`
      })
      result = result.replace(new RegExp(`\\[${variable}\\s*-\\s*(\\d+)\\]`, 'g'), (match, offset) => {
        return `[${value - parseInt(offset)}]`
      })
      result = result.replace(new RegExp(`\\[(\\d+)\\s*-\\s*(\\d+)\\s*-\\s*${variable}\\]`, 'g'), (match, num1, num2) => {
        const calculated = parseInt(num1) - parseInt(num2) - value
        return `[${calculated}]`
      })
      result = result.replace(new RegExp(`\\[\\(([^)]+)\\)\\s*-\\s*\\(${variable}\\s*\\+\\s*(\\d+)\\)\\]`, 'g'), (match, expr, offset) => {
        try {
          const leftSide = this.evaluateExpression(expr, globals, new Map())
          const rightSide = value + parseInt(offset)
          return `[${leftSide - rightSide}]`
        } catch {
          return match
        }
      })
      result = result.replace(new RegExp(`\\[\\(([^)]+)\\)\\s*-\\s*${variable}\\]`, 'g'), (match, expr) => {
        try {
          const leftSide = this.evaluateExpression(expr, globals, new Map())
          return `[${leftSide - value}]`
        } catch {
          return match
        }
      })
      result = result.replace(new RegExp(`\\[${variable}\\]`, 'g'), `[${value}]`)
      result = result.replace(new RegExp(`\\b${variable}\\b`, 'g'), value.toString())
    })

    return result
  }

  private processConditional(ifLine: string, body: string[], startIndex: number, globals: Map<string, number>, locals: Map<string, number>): { lines: string[], nextIndex: number } | null {
    try {
      const conditionMatch = ifLine.match(/if\s*\(([^)]+)\)/)
      if (!conditionMatch) return null

      const condition = conditionMatch[1].trim()
      const conditionResult = this.evaluateCondition(condition, globals, locals)

      const blockLines: string[] = []
      let braceCount = 0
      let currentIndex = startIndex + 1

      while (currentIndex < body.length) {
        const line = body[currentIndex].trim()
        if (line === '{') {
          braceCount++
        } else if (line === '}') {
          braceCount--
          if (braceCount < 0) break
        } else if (braceCount >= 0) {
          blockLines.push(body[currentIndex])
        }
        currentIndex++
      }

      if (conditionResult) {
        const processedLines: string[] = []
        for (const blockLine of blockLines) {
          if (blockLine.trim()) {
            const substituted = this.substituteVariables(blockLine, globals, locals)
            substituted.split('\n').forEach(line => {
              if (line.trim()) {
                processedLines.push(line)
              }
            })
          }
        }
        return { lines: processedLines, nextIndex: currentIndex + 1 }
      } else {
        return { lines: [], nextIndex: currentIndex + 1 }
      }
    } catch {
      return null
    }
  }

  private evaluateCondition(condition: string, globals: Map<string, number>, locals: Map<string, number>): boolean {
    try {
      const substituted = this.substituteVariables(condition, globals, locals)

      if (substituted.includes('<')) {
        const parts = substituted.split('<').map(p => p.trim())
        if (parts.length === 2) {
          const left = this.evaluateExpression(parts[0], globals, locals)
          const right = this.evaluateExpression(parts[1], globals, locals)
          return left < right
        }
      }

      if (substituted.includes('>')) {
        const parts = substituted.split('>').map(p => p.trim())
        if (parts.length === 2) {
          const left = this.evaluateExpression(parts[0], globals, locals)
          const right = this.evaluateExpression(parts[1], globals, locals)
          return left > right
        }
      }

      if (substituted.includes('==')) {
        const parts = substituted.split('==').map(p => p.trim())
        if (parts.length === 2) {
          const left = this.evaluateExpression(parts[0], globals, locals)
          const right = this.evaluateExpression(parts[1], globals, locals)
          return left === right
        }
      }

      return false
    } catch {
      return false
    }
  }

  private evaluateExpression(expr: string, globals: Map<string, number>, locals: Map<string, number>): number {
    try {
      let result = expr.trim()

      // Handle specific config patterns first
      result = result.replace(/config_data_ptr_->header\.pin_size_/g, globals.get('pin_size_')?.toString() || '24')

      globals.forEach((value, variable) => {
        result = result.replace(new RegExp(`\\b${variable}\\b`, 'g'), value.toString())
      })

      locals.forEach((value, variable) => {
        result = result.replace(new RegExp(`\\b${variable}\\b`, 'g'), value.toString())
      })

      if (result.includes('*')) {
        const parts = result.split('*').map(p => p.trim())
        if (parts.length === 2) {
          const left = parseInt(parts[0])
          const right = parseInt(parts[1])
          if (!isNaN(left) && !isNaN(right)) {
            return left * right
          }
        }
      }

      if (result.includes('/')) {
        const parts = result.split('/').map(p => p.trim())
        if (parts.length === 2) {
          const left = this.evaluateExpression(parts[0], globals, locals)
          const right = this.evaluateExpression(parts[1], globals, locals)
          if (!isNaN(left) && !isNaN(right) && right !== 0) {
            return Math.floor(left / right)
          }
        }
      }

      if (result.includes('+')) {
        const parts = result.split('+').map(p => p.trim())
        if (parts.length === 2) {
          const left = this.evaluateExpression(parts[0], globals, locals)
          const right = this.evaluateExpression(parts[1], globals, locals)
          if (!isNaN(left) && !isNaN(right)) {
            return left + right
          }
        }
      }

      if (result.includes('-')) {
        const parts = result.split('-').map(p => p.trim())
        if (parts.length === 2) {
          const left = this.evaluateExpression(parts[0], globals, locals)
          const right = this.evaluateExpression(parts[1], globals, locals)
          if (!isNaN(left) && !isNaN(right)) {
            return left - right
          }
        }
      }

      const num = parseInt(result)
      return isNaN(num) ? 0 : num
    } catch {
      return 0
    }
  }

  private isSetCommand(line: string): boolean {
    return line.includes('set(config_data_ptr_->header.pin_ptr_[')
  }

  private isSleepCommand(line: string): boolean {
    return line.includes('sleep(')
  }

  private isOffCommand(line: string): boolean {
    return line.includes('off()')
  }

  private isOnCommand(line: string): boolean {
    return line.includes('on()')
  }

  private executeSetCommand(line: string): void {
    const setMatch = line.match(/set\([^[]+\[([^\]]+)\],\s*(HIGH|LOW)\)/)
    if (setMatch) {
      const channelExpr = setMatch[1]
      const state = setMatch[2] === 'HIGH'

      let channelNum = parseInt(channelExpr)

      if (isNaN(channelNum)) {
        channelNum = this.evaluateExpression(channelExpr, new Map([['pin_size_', this.config.totalChannels]]), new Map())
      }

      if (!isNaN(channelNum) && channelNum >= 0 && channelNum < this.config.totalChannels) {
        this.channels[channelNum] = state
      }
    }
  }

  private executeSleepCommand(line: string): void {
    const sleepMatch = line.match(/sleep\(([^)]+)\)/)
    if (sleepMatch) {
      const delayExpr = sleepMatch[1].trim()
      let delay = this.config.defaultDelay

      if (delayExpr === 'channel_data_.delay_time_') {
        delay = this.config.defaultDelay
      } else if (delayExpr.includes('*')) {
        delay = this.evaluateExpression(delayExpr, new Map([['delay_time_', this.config.defaultDelay]]), new Map())
      } else {
        const numericDelay = parseInt(delayExpr)
        if (!isNaN(numericDelay)) {
          delay = numericDelay
        }
      }

      this.frameCounter++
      this.frames.push({
        frameNumber: this.frameCounter,
        channels: [...this.channels],
        delay,
        timestamp: Date.now()
      })
    }
  }

  private executeOffCommand(): void {
    this.channels.fill(false)
    this.frameCounter++
    this.frames.push({
      frameNumber: this.frameCounter,
      channels: [...this.channels],
      delay: 30,
      timestamp: Date.now()
    })
  }

  private executeOnCommand(): void {
    this.channels.fill(true)
    this.frameCounter++
    this.frames.push({
      frameNumber: this.frameCounter,
      channels: [...this.channels],
      delay: 30,
      timestamp: Date.now()
    })
  }
}