import * as fs from 'fs'
import * as path from 'path'
import { SimpleCppSimulator } from './lib/simple-cpp-simulator'
import { FrameLogger } from './lib/frame-logger'
import { ParserConfig, TestResult } from './lib/types'

class TestRunner {
  private config: ParserConfig
  private simpleSimulator: SimpleCppSimulator
  private logger: FrameLogger

  constructor() {
    this.config = {
      totalChannels: 24,
      defaultDelay: 30,
      pinSize: 24,
      outputFormat: 'both',
      outputPath: './output'
    }

    this.simpleSimulator = new SimpleCppSimulator(this.config)
    this.logger = new FrameLogger(this.config.outputPath)
  }

  async runTests(): Promise<void> {
    const args = process.argv.slice(2)
    const command = args[0]
    const sequenceArg = args[1]

    console.log('=== AutoLight V3 C++ Parser Testing Framework ===\n')

    switch (command) {
      case '--sequence':
        if (sequenceArg) {
          await this.testSingleSequence(parseInt(sequenceArg))
        } else {
          console.error('Usage: npm run test -- --sequence <number>')
        }
        break

      case '--all':
        await this.testAllSequences()
        break

      case '--compare':
        await this.compareResults()
        break

      case '--validate':
        if (sequenceArg) {
          await this.validateSequence(parseInt(sequenceArg))
        } else {
          console.error('Usage: npm run test -- --validate <number>')
        }
        break

      default:
        await this.showUsage()
        break
    }
  }

  private async testSingleSequence(sequenceNumber: number): Promise<void> {
    console.log(`Testing sequence ${sequenceNumber}...\n`)

    const inputFile = path.join('./input', `sequence-${sequenceNumber}.cpp`)

    if (!fs.existsSync(inputFile)) {
      console.error(`Input file not found: ${inputFile}`)
      return
    }

    const cppCode = fs.readFileSync(inputFile, 'utf8')
    const startTime = Date.now()

    try {
      console.log('Using simple simulator for better loop handling...')
      const frames = this.simpleSimulator.simulate(cppCode)
      const duration = Date.now() - startTime

      console.log(`✓ Generated ${frames.length} frames`)
      console.log(`✓ Simulation completed in ${duration}ms\n`)

      this.logger.logFramesToConsole(frames, `Sequence ${sequenceNumber}`)

      if (this.config.outputFormat === 'file' || this.config.outputFormat === 'both') {
        const filepath = this.logger.saveFramesToFile(frames, `sequence-${sequenceNumber}`)
        console.log(`\n✓ Output saved to: ${filepath}`)
      }

      const result: TestResult = {
        sequenceName: `Sequence ${sequenceNumber}`,
        inputSize: cppCode.length,
        outputFrames: frames.length,
        duration,
        success: true,
        errors: [],
        frameData: frames
      }

      const resultFile = this.logger.saveTestResult(result)
      console.log(`✓ Test result saved to: ${resultFile}`)

    } catch (error) {
      console.error(`✗ Error testing sequence ${sequenceNumber}:`, error)
    }
  }

  private async testAllSequences(): Promise<void> {
    console.log('Testing all available sequences...\n')

    const inputDir = './input'
    const files = fs.readdirSync(inputDir)
      .filter(f => f.startsWith('sequence-') && f.endsWith('.cpp'))
      .sort()

    const results: TestResult[] = []

    for (const file of files) {
      const sequenceNumber = parseInt(file.match(/sequence-(\d+)\.cpp/)?.[1] || '0')

      console.log(`\n--- Testing ${file} ---`)

      const inputFile = path.join(inputDir, file)
      const cppCode = fs.readFileSync(inputFile, 'utf8')
      const startTime = Date.now()

      try {
        const frames = this.simpleSimulator.simulate(cppCode)
        const duration = Date.now() - startTime

        console.log(`✓ Sequence ${sequenceNumber}: ${frames.length} frames, ${duration}ms`)

        if (this.config.outputFormat === 'file' || this.config.outputFormat === 'both') {
          this.logger.saveFramesToFile(frames, `sequence-${sequenceNumber}`)
        }

        results.push({
          sequenceName: `Sequence ${sequenceNumber}`,
          inputSize: cppCode.length,
          outputFrames: frames.length,
          duration,
          success: true,
          errors: [],
          frameData: frames
        })

      } catch (error) {
        console.error(`✗ Sequence ${sequenceNumber}: Error - ${error}`)
        results.push({
          sequenceName: `Sequence ${sequenceNumber}`,
          inputSize: cppCode.length,
          outputFrames: 0,
          duration: Date.now() - startTime,
          success: false,
          errors: [error?.toString() || 'Unknown error'],
          frameData: []
        })
      }
    }

    this.logger.generateSummaryReport(results)
  }

  private async compareResults(): Promise<void> {
    console.log('Comparing parser results...\n')

    const sequences = [2, 3]

    for (const seq of sequences) {
      console.log(`Comparing Sequence ${seq}...`)

      const oldFrames = this.logger.loadFramesFromFile(`sequence-${seq}-old`)
      const newFrames = this.logger.loadFramesFromFile(`sequence-${seq}`)

      if (oldFrames.length === 0 && newFrames.length === 0) {
        console.log(`No data to compare for sequence ${seq}`)
        continue
      }

      const comparison = this.logger.compareResults(oldFrames, newFrames, `Sequence ${seq}`)
      this.logger.logComparisonResult(comparison)
    }
  }

  private async validateSequence(sequenceNumber: number): Promise<void> {
    console.log(`Validating Sequence ${sequenceNumber}...\n`)

    const frames = this.logger.loadFramesFromFile(`sequence-${sequenceNumber}`)

    if (frames.length === 0) {
      console.error(`No output data found for sequence ${sequenceNumber}`)
      console.log('Run the test first: npm run test -- --sequence ' + sequenceNumber)
      return
    }

    console.log(`Loaded ${frames.length} frames for validation`)

    let validFrames = 0
    let invalidFrames = 0

    frames.forEach((frame, index) => {
      const hasValidChannels = frame.channels.length === this.config.totalChannels
      const hasValidDelay = frame.delay > 0

      if (hasValidChannels && hasValidDelay) {
        validFrames++
      } else {
        invalidFrames++
        if (invalidFrames <= 5) {
          console.log(`Invalid frame ${index + 1}: channels=${frame.channels.length}, delay=${frame.delay}`)
        }
      }
    })

    console.log(`\nValidation Results:`)
    console.log(`Valid frames: ${validFrames}`)
    console.log(`Invalid frames: ${invalidFrames}`)
    console.log(`Success rate: ${((validFrames / frames.length) * 100).toFixed(2)}%`)

    if (invalidFrames === 0) {
      console.log(`✓ All frames are valid`)
    } else {
      console.log(`✗ Found ${invalidFrames} invalid frames`)
    }
  }

  private async showUsage(): Promise<void> {
    console.log('Usage:')
    console.log('  npm run test -- --sequence 2              # Test sequence 2')
    console.log('  npm run test -- --sequence 3              # Test sequence 3')
    console.log('  npm run test -- --all                     # Test all sequences')
    console.log('  npm run test -- --compare                 # Compare old vs new results')
    console.log('  npm run test -- --validate 2              # Validate sequence 2 output')
    console.log('')
    console.log('Available sequences: 2, 3')
    console.log('Output format: Frame-by-frame with channel states')
    console.log('Example output: "1 1;1;1;1;1;1;1;1;1;1;1;1;0;0;0;0;0;0;0;0;0;0;0;0;"')
  }
}

async function main(): Promise<void> {
  const runner = new TestRunner()
  await runner.runTests()
}

if (require.main === module) {
  main().catch(console.error)
}