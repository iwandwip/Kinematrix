export interface PatternExpression {
  id: string
  name: string
  description: string
  expression: string
  category: ExpressionCategory
  parameters?: ExpressionParameter[]
}

export type ExpressionCategory =
  | 'basic'
  | 'mathematical'
  | 'sequential'
  | 'custom'

export interface ExpressionParameter {
  name: string
  type: 'number' | 'boolean'
  defaultValue: number | boolean
  min?: number
  max?: number
}

export interface ExpressionFrame {
  frameNumber: number
  expression: PatternExpression
  channels: boolean[]
  parameters: Record<string, number | boolean | string>
  duration: number
  label?: string
}

export class PatternExpressionEvaluator {
  private totalChannels: number
  private parameters: Record<string, number | boolean | string>

  constructor(totalChannels: number) {
    this.totalChannels = totalChannels
    this.parameters = {}
  }

  setParameters(params: Record<string, number | boolean | string>) {
    this.parameters = params
  }

  evaluateExpression(expression: string): boolean[] {
    const channels: boolean[] = new Array(this.totalChannels).fill(false)

    const processedExpression = this.substituteParameters(expression)

    for (let i = 0; i < this.totalChannels; i++) {
      try {
        const channelExpression = processedExpression
          .replace(/\bi\b/g, i.toString())
          .replace(/\btotalChannels\b/g, this.totalChannels.toString())

        channels[i] = this.safeEvaluate(channelExpression)
      } catch (error) {
        console.warn(`Expression evaluation error for channel ${i}:`, error)
        channels[i] = false
      }
    }

    return channels
  }

  private substituteParameters(expression: string): string {
    let result = expression

    Object.entries(this.parameters).forEach(([key, value]) => {
      const placeholder = `\\$\\{${key}\\}`
      result = result.replace(new RegExp(placeholder, 'g'), value.toString())
    })

    return result
  }

  private safeEvaluate(expression: string): boolean {
    const allowedTokens = /^[0-9+\-*/%()&|!<>=\s]+$/

    if (!allowedTokens.test(expression)) {
      throw new Error('Expression contains invalid characters')
    }

    return new Function('return ' + expression)()
  }
}

export class ExpressionFrameBuilder {
  private evaluator: PatternExpressionEvaluator
  private frames: ExpressionFrame[] = []

  constructor(totalChannels: number) {
    this.evaluator = new PatternExpressionEvaluator(totalChannels)
  }

  addFrame(expression: PatternExpression, parameters: Record<string, number | boolean | string> = {}, duration: number = 100): ExpressionFrame {
    this.evaluator.setParameters(parameters)

    const frame: ExpressionFrame = {
      frameNumber: this.frames.length + 1,
      expression,
      channels: this.evaluator.evaluateExpression(expression.expression),
      parameters: { ...parameters },
      duration,
      label: `${expression.name}${Object.keys(parameters).length > 0 ? ` (${JSON.stringify(parameters)})` : ''}`
    }

    this.frames.push(frame)
    return frame
  }

  getFrames(): ExpressionFrame[] {
    return [...this.frames]
  }

  clear() {
    this.frames = []
  }

  updateTotalChannels(totalChannels: number) {
    this.evaluator = new PatternExpressionEvaluator(totalChannels)
    this.frames = []
  }
}

export const BUILTIN_EXPRESSIONS: PatternExpression[] = [
  {
    id: 'all_on',
    name: 'All Channels ON',
    description: 'Turn on all available channels',
    expression: 'true',
    category: 'basic'
  },
  {
    id: 'all_off',
    name: 'All Channels OFF',
    description: 'Turn off all channels',
    expression: 'false',
    category: 'basic'
  },
  {
    id: 'first_half',
    name: 'First Half',
    description: 'Turn on first half of channels',
    expression: 'i < totalChannels / 2',
    category: 'basic'
  },
  {
    id: 'second_half',
    name: 'Second Half',
    description: 'Turn on second half of channels',
    expression: 'i >= totalChannels / 2',
    category: 'basic'
  },
  {
    id: 'even_channels',
    name: 'Even Channels',
    description: 'Turn on even numbered channels (0,2,4...)',
    expression: 'i % 2 === 0',
    category: 'mathematical'
  },
  {
    id: 'odd_channels',
    name: 'Odd Channels',
    description: 'Turn on odd numbered channels (1,3,5...)',
    expression: 'i % 2 === 1',
    category: 'mathematical'
  },
  {
    id: 'every_third',
    name: 'Every 3rd Channel',
    description: 'Turn on every 3rd channel (0,3,6,9...)',
    expression: 'i % 3 === 0',
    category: 'mathematical'
  },
  {
    id: 'every_fourth',
    name: 'Every 4th Channel',
    description: 'Turn on every 4th channel (0,4,8,12...)',
    expression: 'i % 4 === 0',
    category: 'mathematical'
  },
  {
    id: 'first_quarter',
    name: 'First Quarter',
    description: 'Turn on first 25% of channels',
    expression: 'i < totalChannels / 4',
    category: 'sequential'
  },
  {
    id: 'last_quarter',
    name: 'Last Quarter',
    description: 'Turn on last 25% of channels',
    expression: 'i >= (totalChannels * 3) / 4',
    category: 'sequential'
  },
  {
    id: 'middle_range',
    name: 'Middle Range',
    description: 'Turn on middle 50% of channels',
    expression: 'i >= totalChannels / 4 && i < (totalChannels * 3) / 4',
    category: 'sequential'
  },
  {
    id: 'edges',
    name: 'Edge Channels',
    description: 'Turn on first and last channels only',
    expression: 'i < 2 || i >= totalChannels - 2',
    category: 'sequential'
  },
  {
    id: 'custom_modulo',
    name: 'Custom Modulo',
    description: 'Turn on channels based on custom divisor and offset',
    expression: 'i % ${divisor} === ${offset}',
    category: 'custom',
    parameters: [
      { name: 'divisor', type: 'number', defaultValue: 3, min: 2, max: 32 },
      { name: 'offset', type: 'number', defaultValue: 0, min: 0, max: 31 }
    ]
  },
  {
    id: 'custom_range',
    name: 'Custom Range',
    description: 'Turn on channels within custom start and end range',
    expression: 'i >= ${start} && i < ${end}',
    category: 'custom',
    parameters: [
      { name: 'start', type: 'number', defaultValue: 0, min: 0, max: 63 },
      { name: 'end', type: 'number', defaultValue: 12, min: 1, max: 64 }
    ]
  }
]

export class ExpressionCodeGenerator {
  private totalChannels: number
  private frames: ExpressionFrame[]

  constructor(totalChannels: number, frames: ExpressionFrame[]) {
    this.totalChannels = totalChannels
    this.frames = frames
  }

  generateModernAPI(): string {
    return `void BaseChannel::taskSequenceCustomExpression() {
    // Generated by Expression Builder - Modern API
    // Total Frames: ${this.frames.length}
    // Target Channels: ${this.totalChannels} (dynamic)

    ${this.frames.map(frame => this.generateFrameModernAPI(frame)).join('\n    \n    ')}

    off(); // Turn off all channels at end
}`
  }

  generateNativeAPI(): string {
    return `void BaseChannel::taskSequenceCustomExpression() {
    // Generated by Expression Builder - Native API
    // Total Frames: ${this.frames.length}
    // Target Channels: ${this.totalChannels} (dynamic)

    ${this.frames.map(frame => this.generateFrameNativeAPI(frame)).join('\n    \n    ')}

    off(); // Turn off all channels at end
}`
  }

  private generateFrameModernAPI(frame: ExpressionFrame): string {
    const comment = `// Frame ${frame.frameNumber}: ${frame.label}`
    const expressionComment = `// Expression: ${frame.expression.expression}`

    return `${comment}
    ${expressionComment}
    ${this.generateExpressionLogic(frame)}
    sleep(${frame.duration});`
  }

  private generateFrameNativeAPI(frame: ExpressionFrame): string {
    const comment = `// Frame ${frame.frameNumber}: ${frame.label}`
    const expressionComment = `// Expression: ${frame.expression.expression}`

    return `${comment}
    ${expressionComment}
    ${this.generateNativeLogic(frame)}
    sleep(${frame.duration});`
  }

  private generateExpressionLogic(frame: ExpressionFrame): string {
    const expr = frame.expression.expression

    if (expr === 'true') {
      return `for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
        set(config_data_ptr_->header.pin_ptr_[i], HIGH);
    }`
    }

    if (expr === 'false') {
      return `// All channels OFF (no action needed)`
    }

    if (expr === 'i < totalChannels / 2') {
      return `for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; i++) {
        set(config_data_ptr_->header.pin_ptr_[i], HIGH);
    }`
    }

    if (expr === 'i >= totalChannels / 2') {
      return `for (int i = config_data_ptr_->header.pin_size_ / 2; i < config_data_ptr_->header.pin_size_; i++) {
        set(config_data_ptr_->header.pin_ptr_[i], HIGH);
    }`
    }

    if (expr.includes('i % 2 === 0')) {
      return `for (int i = 0; i < config_data_ptr_->header.pin_size_; i += 2) {
        set(config_data_ptr_->header.pin_ptr_[i], HIGH);
    }`
    }

    if (expr.includes('i % 2 === 1')) {
      return `for (int i = 1; i < config_data_ptr_->header.pin_size_; i += 2) {
        set(config_data_ptr_->header.pin_ptr_[i], HIGH);
    }`
    }

    return this.generateGenericExpressionLoop(frame)
  }

  private generateGenericExpressionLoop(frame: ExpressionFrame): string {
    const cppCondition = this.convertToCppCondition(frame.expression.expression, frame.parameters)

    return `for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
        if (${cppCondition}) {
            set(config_data_ptr_->header.pin_ptr_[i], HIGH);
        }
    }`
  }

  private convertToCppCondition(expression: string, parameters: Record<string, number | boolean | string>): string {
    let cppExpr = expression
      .replace(/totalChannels/g, 'config_data_ptr_->header.pin_size_')
      .replace(/===/g, '==')
      .replace(/!==/g, '!=')
      .replace(/&&/g, ' && ')
      .replace(/\|\|/g, ' || ')

    Object.entries(parameters).forEach(([key, value]) => {
      const placeholder = `\\$\\{${key}\\}`
      cppExpr = cppExpr.replace(new RegExp(placeholder, 'g'), value.toString())
    })

    return cppExpr
  }

  private generateNativeLogic(frame: ExpressionFrame): string {
    const activeChannels = frame.channels
      .map((isOn, index) => isOn ? index : -1)
      .filter(index => index !== -1)

    if (activeChannels.length === 0) {
      return '// No channels active for this frame'
    }

    return activeChannels
      .map(channelIndex => `    set(config_data_ptr_->header.pin_ptr_[${channelIndex}], HIGH);`)
      .join('\n')
  }
}