export interface SafetyConfig {
  maxExecutionTime: number
  maxFrameCount: number
  maxMemoryUsage: number
  warningThresholds: {
    executionTime: number
    frameCount: number
    memoryUsage: number
  }
}

export interface SafetyResult {
  isValid: boolean
  warnings: string[]
  errors: string[]
  metrics: {
    executionTime: number
    frameCount: number
    estimatedMemory: number
  }
}

export class JavaScriptSafety {
  private config: SafetyConfig

  constructor(config?: Partial<SafetyConfig>) {
    this.config = {
      maxExecutionTime: 30000, // 30 seconds
      maxFrameCount: 1000,     // Maximum 1000 frames
      maxMemoryUsage: 50 * 1024 * 1024, // 50MB
      warningThresholds: {
        executionTime: 5000,   // Warn if > 5 seconds
        frameCount: 500,       // Warn if > 500 frames
        memoryUsage: 10 * 1024 * 1024 // Warn if > 10MB
      },
      ...config
    }
  }

  validateCode(code: string): { isValid: boolean; errors: string[] } {
    const errors: string[] = []

    if (!code || code.trim().length === 0) {
      errors.push('Code cannot be empty')
      return { isValid: false, errors }
    }

    try {
      // Wrap code in async function to allow await statements
      const wrappedCode = `(async function() { ${code} })`
      new Function(`return ${wrappedCode}`)()
    } catch (syntaxError) {
      errors.push(`Syntax error: ${syntaxError instanceof Error ? syntaxError.message : String(syntaxError)}`)
    }

    const suspiciousPatterns = [
      { pattern: /while\s*\(\s*true\s*\)/, message: 'Infinite while(true) loop detected' },
      { pattern: /for\s*\(\s*;\s*;\s*\)/, message: 'Infinite for(;;) loop detected' },
      { pattern: /eval\s*\(/, message: 'eval() function is not allowed for security' },
      { pattern: /Function\s*\(/, message: 'Function constructor is not allowed for security' },
      { pattern: /setTimeout|setInterval/, message: 'setTimeout/setInterval are not allowed' },
      { pattern: /document\.|window\.|global\./, message: 'DOM/Window access is not allowed' }
    ]

    for (const { pattern, message } of suspiciousPatterns) {
      if (pattern.test(code)) {
        errors.push(message)
      }
    }

    return { isValid: errors.length === 0, errors }
  }

  validateExecution(
    executionTime: number,
    frameCount: number,
    estimatedMemory: number
  ): SafetyResult {
    const warnings: string[] = []
    const errors: string[] = []

    if (executionTime > this.config.maxExecutionTime) {
      errors.push(`Execution time exceeded maximum limit (${this.config.maxExecutionTime / 1000}s)`)
    } else if (executionTime > this.config.warningThresholds.executionTime) {
      warnings.push(`Slow execution detected (${(executionTime / 1000).toFixed(2)}s). Consider optimizing your pattern.`)
    }

    if (frameCount > this.config.maxFrameCount) {
      errors.push(`Frame count exceeded maximum limit (${this.config.maxFrameCount})`)
    } else if (frameCount > this.config.warningThresholds.frameCount) {
      warnings.push(`High frame count (${frameCount}). Pattern may be too complex for smooth playback.`)
    }

    if (estimatedMemory > this.config.maxMemoryUsage) {
      errors.push(`Memory usage exceeded maximum limit (${this.formatBytes(this.config.maxMemoryUsage)})`)
    } else if (estimatedMemory > this.config.warningThresholds.memoryUsage) {
      warnings.push(`High memory usage (${this.formatBytes(estimatedMemory)}). Consider reducing pattern complexity.`)
    }

    return {
      isValid: errors.length === 0,
      warnings,
      errors,
      metrics: {
        executionTime,
        frameCount,
        estimatedMemory
      }
    }
  }

  createTimeoutWrapper<T>(
    operation: () => Promise<T>,
    timeoutMs: number = this.config.maxExecutionTime
  ): Promise<T> {
    return Promise.race([
      operation(),
      new Promise<never>((_, reject) =>
        setTimeout(
          () => reject(new Error(`Operation timeout after ${timeoutMs / 1000} seconds`)),
          timeoutMs
        )
      )
    ])
  }

  formatExecutionStats(
    executionTime: number,
    frameCount: number,
    estimatedMemory: number
  ): string {
    return [
      `Execution Time: ${(executionTime / 1000).toFixed(2)}s`,
      `Frame Count: ${frameCount}`,
      `Memory Usage: ${this.formatBytes(estimatedMemory)}`,
      `Performance: ${this.getPerformanceRating(executionTime, frameCount)}`
    ].join(' | ')
  }

  private formatBytes(bytes: number): string {
    const units = ['B', 'KB', 'MB', 'GB']
    let size = bytes
    let unitIndex = 0

    while (size >= 1024 && unitIndex < units.length - 1) {
      size /= 1024
      unitIndex++
    }

    return `${size.toFixed(2)} ${units[unitIndex]}`
  }

  private getPerformanceRating(executionTime: number, frameCount: number): string {
    const timeRatio = executionTime / this.config.warningThresholds.executionTime
    const frameRatio = frameCount / this.config.warningThresholds.frameCount

    if (timeRatio < 0.5 && frameRatio < 0.5) return 'Excellent'
    if (timeRatio < 1 && frameRatio < 1) return 'Good'
    if (timeRatio < 2 && frameRatio < 2) return 'Fair'
    return 'Poor'
  }

  getRecommendations(frameCount: number, executionTime: number): string[] {
    const recommendations: string[] = []

    if (frameCount > this.config.warningThresholds.frameCount) {
      recommendations.push('Consider reducing the number of frames by using longer delays')
      recommendations.push('Optimize loops to avoid redundant frame generation')
    }

    if (executionTime > this.config.warningThresholds.executionTime) {
      recommendations.push('Simplify complex calculations inside loops')
      recommendations.push('Use fewer nested loops or reduce iteration counts')
    }

    if (frameCount > 100 && executionTime < 1000) {
      recommendations.push('Pattern looks good! High frame count with fast execution.')
    }

    return recommendations
  }
}

export const defaultSafety = new JavaScriptSafety()

export { JavaScriptSafety as default }