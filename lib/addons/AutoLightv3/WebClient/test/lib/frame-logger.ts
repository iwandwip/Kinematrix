import * as fs from 'fs'
import * as path from 'path'
import { LEDFrame, TestResult, ComparisonResult } from './types'

export class FrameLogger {
  private outputDir: string

  constructor(outputDir: string = './output') {
    this.outputDir = outputDir
    this.ensureOutputDir()
  }

  private ensureOutputDir(): void {
    if (!fs.existsSync(this.outputDir)) {
      fs.mkdirSync(this.outputDir, { recursive: true })
    }
  }

  logFramesToConsole(frames: LEDFrame[], sequenceName: string): void {
    console.log(`\n=== Testing Sequence: ${sequenceName} ===`)
    console.log(`Generated ${frames.length} frames\n`)

    frames.forEach((frame, index) => {
      const channelStates = frame.channels.map(c => c ? '1' : '0').join(';') + ';'
      console.log(`${frame.frameNumber} ${channelStates}`)

      if (index < 10 || (index + 1) === frames.length) {
        const activeChannels = frame.channels
          .map((state, i) => state ? i : null)
          .filter(x => x !== null)
        console.log(`  -> Active channels: [${activeChannels.join(', ')}], Delay: ${frame.delay}ms`)
      } else if (index === 10) {
        console.log(`  ... (${frames.length - 11} more frames) ...`)
      }
    })

    console.log(`\n✓ Generated ${frames.length} frames successfully`)
    console.log(`✓ Total simulation time: ${this.calculateTotalTime(frames)}ms`)
  }

  saveFramesToFile(frames: LEDFrame[], sequenceName: string): string {
    const filename = `${sequenceName.toLowerCase().replace(/\s+/g, '-')}.txt`
    const filepath = path.join(this.outputDir, filename)

    const content = frames.map(frame => {
      const channelStates = frame.channels.map(c => c ? '1' : '0').join(';') + ';'
      return `${frame.frameNumber} ${channelStates}`
    }).join('\n')

    fs.writeFileSync(filepath, content, 'utf8')
    return filepath
  }

  saveTestResult(result: TestResult): string {
    const filename = `test-result-${result.sequenceName.toLowerCase().replace(/\s+/g, '-')}.json`
    const filepath = path.join(this.outputDir, filename)

    fs.writeFileSync(filepath, JSON.stringify(result, null, 2), 'utf8')
    return filepath
  }

  loadFramesFromFile(sequenceName: string): LEDFrame[] {
    const filename = `${sequenceName.toLowerCase().replace(/\s+/g, '-')}.txt`
    const filepath = path.join(this.outputDir, filename)

    if (!fs.existsSync(filepath)) {
      return []
    }

    const content = fs.readFileSync(filepath, 'utf8')
    const lines = content.split('\n').filter(line => line.trim())

    return lines.map((line) => {
      const parts = line.split(' ')
      const frameNumber = parseInt(parts[0])
      const channelStates = parts[1].split(';').filter(s => s).map(s => s === '1')

      return {
        frameNumber,
        channels: channelStates,
        delay: 50
      }
    })
  }

  compareResults(oldFrames: LEDFrame[], newFrames: LEDFrame[], sequenceName: string): ComparisonResult {
    const maxFrames = Math.max(oldFrames.length, newFrames.length)
    let matchingFrames = 0
    const differences: ComparisonResult['differences'] = []

    for (let i = 0; i < maxFrames; i++) {
      const oldFrame = oldFrames[i]
      const newFrame = newFrames[i]

      if (!oldFrame || !newFrame) {
        differences.push({
          frame: i + 1,
          oldChannels: oldFrame?.channels || [],
          newChannels: newFrame?.channels || []
        })
        continue
      }

      const channelsMatch = oldFrame.channels.length === newFrame.channels.length &&
        oldFrame.channels.every((channel, idx) => channel === newFrame.channels[idx])

      if (channelsMatch) {
        matchingFrames++
      } else {
        differences.push({
          frame: i + 1,
          oldChannels: oldFrame.channels,
          newChannels: newFrame.channels
        })
      }
    }

    return {
      sequenceName,
      oldFrames: oldFrames.length,
      newFrames: newFrames.length,
      matchingFrames,
      accuracy: matchingFrames / maxFrames,
      differences
    }
  }

  logComparisonResult(comparison: ComparisonResult): void {
    console.log(`\n=== Comparison Result: ${comparison.sequenceName} ===`)
    console.log(`Old frames: ${comparison.oldFrames}`)
    console.log(`New frames: ${comparison.newFrames}`)
    console.log(`Matching frames: ${comparison.matchingFrames}`)
    console.log(`Accuracy: ${(comparison.accuracy * 100).toFixed(2)}%`)

    if (comparison.differences.length > 0) {
      console.log(`\nDifferences (first 5):`)
      comparison.differences.slice(0, 5).forEach(diff => {
        console.log(`Frame ${diff.frame}:`)
        console.log(`  Old: [${diff.oldChannels.map(c => c ? '1' : '0').join(', ')}]`)
        console.log(`  New: [${diff.newChannels.map(c => c ? '1' : '0').join(', ')}]`)
      })

      if (comparison.differences.length > 5) {
        console.log(`  ... and ${comparison.differences.length - 5} more differences`)
      }
    }
  }

  private calculateTotalTime(frames: LEDFrame[]): number {
    return frames.reduce((total, frame) => total + frame.delay, 0)
  }

  generateSummaryReport(results: TestResult[]): void {
    console.log('\n=== Test Summary Report ===')
    console.log(`Total sequences tested: ${results.length}`)

    const successful = results.filter(r => r.success).length
    const failed = results.length - successful

    console.log(`Successful: ${successful}`)
    console.log(`Failed: ${failed}`)

    if (failed > 0) {
      console.log('\nFailed sequences:')
      results.filter(r => !r.success).forEach(r => {
        console.log(`- ${r.sequenceName}: ${r.errors.join(', ')}`)
      })
    }

    console.log('\nFrame generation statistics:')
    results.forEach(r => {
      console.log(`${r.sequenceName}: ${r.outputFrames} frames, ${r.duration}ms`)
    })
  }
}