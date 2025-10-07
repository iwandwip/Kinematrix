export function map(value: number, fromLow: number, fromHigh: number, toLow: number, toHigh: number): number {
  return toLow + (toHigh - toLow) * ((value - fromLow) / (fromHigh - fromLow))
}

export function random(max: number): number {
  return Math.floor(Math.random() * max)
}

export function ceil(value: number): number {
  return Math.ceil(value)
}

export function floor(value: number): number {
  return Math.floor(value)
}

export function abs(value: number): number {
  return Math.abs(value)
}

export function min(a: number, b: number): number {
  return Math.min(a, b)
}

export function max(a: number, b: number): number {
  return Math.max(a, b)
}

export interface TemplateVariables {
  pin_ptr_: number[]
  pin_size_: number
  delay_time_: number
  [key: string]: unknown
}

export function createTemplateVariables(totalChannels: number = 24): TemplateVariables {
  const pin_ptr_ = Array.from({ length: totalChannels }, (_, i) => i)

  return {
    pin_ptr_,
    pin_size_: totalChannels,
    delay_time_: 30
  }
}

export function adaptTemplateForChannelCount(templateCode: string, targetChannels: number): string {
  const adaptedCode = templateCode
    .replace(/int pin_ptr_\[24\]/g, `int pin_ptr_[${targetChannels}]`)
    .replace(/int pin_size_ = 24/g, `int pin_size_ = ${targetChannels}`)
    .replace(/0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23/g,
      Array.from({ length: targetChannels }, (_, i) => i).join(','))

  return adaptedCode
}

export function getTemplateDescription(templateId: string): string {
  const descriptions: Record<string, string> = {
    sequence2: 'Creates comprehensive blinking patterns including all LEDs and half sections. Features dual-phase blinking with progressive timing.',
    sequence3: 'Implements two-point filling pattern starting from edges and progressing inward. Uses symmetric LED activation with cascading effects.',
    sequence4: 'Right-fill sequence where LEDs activate in cascading pattern from left to right, then deactivate in reverse order.',
    sequence5: 'Center-in filling pattern that creates symmetric operations from outer edges toward center with complex mathematical timing.',
    sequence6: 'Individual LED blinking with complex timing calculations. Features mathematical mapping and half-size logic for advanced patterns.',
    sequence7: 'Two-point blinking pattern that progressively fills from edges with completion logic. Simpler than sequence 3 but effective.',
    sequence8: 'Snake pattern that moves forward then reverses direction. Uses variable timing for acceleration/deceleration effects.',
    sequence9: 'Random LED activation using pseudo-lambda functions. Creates unpredictable patterns with three-phase random selection.',
    sequence10: 'Wave pattern with conditional logic that adapts behavior based on channel count. Features symmetric wave propagation.',
    sequence11: 'Multi-stage complex pattern featuring overlapping LED activations with multiple timing phases and mathematical progression.',
    sequence12: 'Matrix-based pattern system using predefined pattern matrices. Demonstrates advanced pattern lookup and distribution logic.',
    sequence13: 'Advanced blinking system with pattern matrices and multi-phase execution. Features complex pattern selection algorithms.',
    sequence14: 'Most complex pattern featuring multiple phases, mathematical calculations, and sophisticated LED coordination algorithms.'
  }

  return descriptions[templateId] || 'Professional LED sequence pattern from BaseChannelSequence.cpp'
}

export function estimateFrameCount(templateCode: string): number {
  const sleepMatches = templateCode.match(/sleep\(/g) || []
  const loopMatches = templateCode.match(/for\s*\(/g) || []

  const baseFrames = sleepMatches.length
  const loopMultiplier = Math.max(1, loopMatches.length * 2)

  return Math.min(100, baseFrames * loopMultiplier)
}

export function getComplexityLevel(templateId: string): 'Basic' | 'Intermediate' | 'Advanced' | 'Expert' {
  const complexity: Record<string, 'Basic' | 'Intermediate' | 'Advanced' | 'Expert'> = {
    sequence2: 'Basic',
    sequence3: 'Intermediate',
    sequence4: 'Basic',
    sequence5: 'Intermediate',
    sequence6: 'Advanced',
    sequence7: 'Intermediate',
    sequence8: 'Basic',
    sequence9: 'Advanced',
    sequence10: 'Advanced',
    sequence11: 'Expert',
    sequence12: 'Expert',
    sequence13: 'Expert',
    sequence14: 'Expert'
  }

  return complexity[templateId] || 'Advanced'
}

export function resetArray(arrData: number[], arrSize: { value: number }, arrLastIndex: { value: number }): void {
  arrData.fill(0)
  arrSize.value = 0
  arrLastIndex.value = 0
}

export function findMax(...numbers: number[]): number {
  const validNumbers = numbers.filter(n => n !== undefined && n !== null && !isNaN(n))
  return validNumbers.length > 0 ? Math.max(...validNumbers) : 0
}

export function shiftArrayAccordingToOrder(
  patterns: number[][],
  patternSize: number,
  mainArray: number[],
  mainArraySize: number,
  arrData: number[],
  arrLastIndex: { value: number },
  targetIndex: number,
  reverse: boolean = false
): number {
  if (!patterns || !patterns[targetIndex] || targetIndex >= patternSize) {
    return 0
  }

  const pattern = patterns[targetIndex]
  let addedCount = 0

  for (let i = 0; i < pattern.length && pattern[i] !== undefined; i++) {
    const value = pattern[i]
    if (value >= 0 && value < mainArraySize) {
      const arrayIndex = reverse ?
        arrLastIndex.value + (pattern.length - 1 - i) :
        arrLastIndex.value + addedCount

      if (arrayIndex < arrData.length) {
        arrData[arrayIndex] = mainArray[value]
        addedCount++
      }
    }
  }

  arrLastIndex.value += addedCount
  return addedCount
}