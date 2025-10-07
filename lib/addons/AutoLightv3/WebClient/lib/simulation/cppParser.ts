// Removed unused imports to fix ESLint warnings

export interface ParsedCommand {
  type: 'set' | 'sleep' | 'off' | 'on' | 'for_start' | 'for_end' | 'if' | 'else' | 'endif' | 'variable_decl' | 'function_call' | 'do_while_start' | 'do_while_end' | 'lambda_decl' | 'function_def' | 'custom_function_call'
  channel?: number
  channelExpression?: string
  state?: boolean
  delay?: number
  loopCount?: number
  loopVariable?: string
  loopStep?: number
  loopDirection?: 'forward' | 'reverse'

  // Conditional statements
  condition?: string
  conditionResult?: boolean
  elseBlock?: boolean

  // Variable declarations
  variableName?: string
  variableType?: 'int' | 'const int' | 'bool'
  variableValue?: number | string | boolean
  arraySize?: string | number
  isArray?: boolean
  arrayInitializer?: (number | string | boolean)[][]

  // Function calls
  functionName?: string
  parameters?: (string | number)[]
  returnValue?: number | string | boolean

  // Lambda functions
  lambdaName?: string
  lambdaCapture?: string
  lambdaParams?: string[]
  lambdaReturnType?: string
  lambdaBody?: ParsedCommand[]

  // Custom function definitions
  customFunctionName?: string
  customFunctionParams?: string[]
  customFunctionBody?: ParsedCommand[]

  // Do-while loops
  doWhileCondition?: string
  doWhileBody?: ParsedCommand[]

  // Advanced for loops
  multipleVariables?: {name: string; initialValue: number}[]
  commaOperator?: string[]
}

export interface LEDFrame {
  id: string
  channels: boolean[]
  delay: number
  label?: string
}

export class CppSimulationState {
  channels: boolean[]
  totalChannels: number
  frames: LEDFrame[] = []
  variables: Map<string, number | string | boolean> = new Map()
  arrays: Map<string, (number | string | boolean)[]> = new Map()
  scopes: Array<Map<string, number | string | boolean>> = []
  conditionalStack: Array<{condition: boolean; executing: boolean}> = []

  // Function registry for custom and lambda functions
  customFunctions: Map<string, {params: string[]; body: ParsedCommand[]}> = new Map()
  lambdaFunctions: Map<string, {capture: string; params: string[]; body: ParsedCommand[]; returnType?: string}> = new Map()

  // Random number generator state
  randomSeed: number = 12345

  // Do-while loop stack
  doWhileStack: Array<{startIndex: number; condition: string}> = []

  constructor(totalChannels: number) {
    this.totalChannels = totalChannels
    this.channels = new Array(totalChannels).fill(false)

    // Initialize built-in variables
    this.variables.set('pin_size_', totalChannels)
    this.variables.set('delay_time_', 30)
  }

  setChannel(index: number, state: boolean) {
    if (index >= 0 && index < this.totalChannels) {
      this.channels[index] = state
    }
  }

  setAllChannels(state: boolean) {
    this.channels.fill(state)
  }

  addFrame(delay: number, label?: string) {
    this.frames.push({
      id: crypto.randomUUID(),
      channels: [...this.channels],
      delay,
      label
    })
  }

  // Variable management
  declareVariable(name: string, type: string, value?: number | string | boolean, isArray = false, arraySize?: number, initializer?: (number | string | boolean)[]) {
    if (isArray) {
      const size = arraySize || 0
      const arr = initializer || new Array(size).fill(0)
      this.arrays.set(name, arr)
    } else {
      this.variables.set(name, value || 0)
    }
  }

  getVariable(name: string): number | string | boolean | (number | string | boolean)[] {
    if (this.variables.has(name)) {
      return this.variables.get(name)!
    }
    if (this.arrays.has(name)) {
      return this.arrays.get(name)!
    }
    return 0
  }

  setVariable(name: string, value: number | string | boolean) {
    if (this.variables.has(name)) {
      this.variables.set(name, value)
    }
  }

  // Mathematical functions (Arduino compatible)
  map(value: number, fromLow: number, fromHigh: number, toLow: number, toHigh: number): number {
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow
  }

  ceil(value: number): number {
    return Math.ceil(value)
  }

  floor(value: number): number {
    return Math.floor(value)
  }

  abs(value: number): number {
    return Math.abs(value)
  }

  findMax(...args: number[]): number {
    return Math.max(...args)
  }

  // Random number generation (pseudo-random for consistent testing)
  random(max: number): number {
    this.randomSeed = (this.randomSeed * 9301 + 49297) % 233280
    return Math.floor((this.randomSeed / 233280) * max)
  }

  // Custom function management
  defineCustomFunction(name: string, params: string[], body: ParsedCommand[]): void {
    this.customFunctions.set(name, { params, body })
  }

  defineLambdaFunction(name: string, capture: string, params: string[], body: ParsedCommand[], returnType?: string): void {
    this.lambdaFunctions.set(name, { capture, params, body, returnType })
  }

  callCustomFunction(name: string, args: (number | string | boolean)[]): number | string | boolean {
    // Check if it's a lambda function first
    const lambdaFunc = this.lambdaFunctions.get(name)
    if (lambdaFunc) {
      return this.callLambdaFunction(name, args)
    }

    // Otherwise try regular custom function
    const func = this.customFunctions.get(name)
    if (!func) return 0

    // Create new scope for function parameters
    const paramScope = new Map<string, number | string | boolean>()
    for (let i = 0; i < func.params.length && i < args.length; i++) {
      paramScope.set(func.params[i], args[i])
    }
    this.scopes.push(paramScope)

    // Execute function body (simplified - would need full simulation context)
    const result: number | string | boolean = 0

    // Remove function scope
    this.scopes.pop()
    return result
  }

  callLambdaFunction(name: string, args: (number | string | boolean)[]): number | string | boolean {
    const lambdaFunc = this.lambdaFunctions.get(name)
    if (!lambdaFunc) return 0

    // Create new scope for lambda parameters
    const paramScope = new Map<string, number | string | boolean>()
    for (let i = 0; i < lambdaFunc.params.length && i < args.length; i++) {
      paramScope.set(lambdaFunc.params[i], args[i])
    }
    this.scopes.push(paramScope)

    // Execute lambda body (simplified - in real implementation would execute parsed commands)
    // For now, handle common lambda patterns like setRandomLed
    if (name === 'setRandomLed' && args.length >= 1) {
      const value = Number(args[0])
      // Simulate random LED setting
      const randomIndex = this.random(this.totalChannels)
      this.setChannel(randomIndex, value === 1)
    }

    // Remove lambda scope
    this.scopes.pop()
    return 0
  }

  // BaseChannelSequence.cpp utility functions
  resetArray(arr: (number | string | boolean)[], size: {value: number}, lastIndex: {value: number}): void {
    arr.length = 0
    size.value = 0
    lastIndex.value = 0
  }

  shiftArrayAccordingToOrder(
    patternMode: number[][],
    patternRows: number,
    pinPtr: number[],
    pinSize: number,
    arrData: number[],
    lastIndex: {value: number},
    orderIndex: number,
    reverse: boolean = false
  ): number {
    if (orderIndex >= patternMode[0].length) return 0

    let addedCount = 0
    const patternIndex = Math.min(Math.floor((24 - pinSize) / 2), patternRows - 1)

    if (patternIndex >= 0 && patternIndex < patternMode.length) {
      const repeatCount = patternMode[patternIndex][orderIndex]

      for (let i = 0; i < repeatCount; i++) {
        let channelIndex = lastIndex.value
        if (reverse) {
          channelIndex = pinSize - 1 - lastIndex.value
        }

        if (channelIndex >= 0 && channelIndex < pinSize) {
          arrData.push(pinPtr[channelIndex])
          addedCount++
        }
        lastIndex.value++
      }
    }

    return addedCount
  }

  // Conditional execution management
  evaluateCondition(condition: string): boolean {
    try {
      // Replace variables in condition
      let evalCondition = condition
      for (const [name, value] of this.variables) {
        const regex = new RegExp(`\\b${name}\\b`, 'g')
        evalCondition = evalCondition.replace(regex, value.toString())
      }

      // Replace config_data_ptr_->header.pin_size_ with actual value
      evalCondition = evalCondition.replace(/config_data_ptr_->header\.pin_size_/g, this.totalChannels.toString())

      return eval(evalCondition)
    } catch {
      return false
    }
  }

  callFunction(name: string, params: (number | string | boolean)[]): number | string | boolean {
    switch (name) {
      case 'map':
        if (params.length === 5) {
          return this.map(
            Number(params[0]),
            Number(params[1]),
            Number(params[2]),
            Number(params[3]),
            Number(params[4])
          )
        }
        break
      case 'ceil':
        if (params.length === 1) {
          return this.ceil(Number(params[0]))
        }
        break
      case 'floor':
        if (params.length === 1) {
          return this.floor(Number(params[0]))
        }
        break
      case 'abs':
        if (params.length === 1) {
          return this.abs(Number(params[0]))
        }
        break
      case 'random':
        if (params.length === 1) {
          return this.random(Number(params[0]))
        }
        break
      case 'findMax':
        return this.findMax(...params.map(p => Number(p)))
      case 'resetArray':
        if (params.length >= 3) {
          const arrayName = String(params[0])
          const sizeVar = String(params[1])
          const indexVar = String(params[2])

          // Get array from variables
          const array = this.arrays.get(arrayName)
          if (array) {
            // Reset array to zeros
            array.fill(0)
            // Reset size and index variables
            this.variables.set(sizeVar, 0)
            this.variables.set(indexVar, 0)
          }
        }
        return 0
      case 'shiftArrayAccordingToOrder':
        if (params.length >= 7) {
          // This is a complex function, implement basic version for now
          return 0
        }
        return 0
      default:
        // Check for custom functions
        return this.callCustomFunction(name, params)
    }
    return 0
  }
}

export function parseAutoLightCode(cppCode: string): ParsedCommand[] {
  const commands: ParsedCommand[] = []
  const lines = cppCode.split('\n')

  for (const line of lines) {
    const trimmed = line.trim()

    if (trimmed.includes('set(') && trimmed.includes('pin_ptr_')) {
      const setMatch = trimmed.match(/set\(.*?pin_ptr_\[(.*?)\].*?(HIGH|LOW)\)/)
      if (setMatch) {
        const channelExpression = setMatch[1].trim()
        const state = setMatch[2] === 'HIGH'

        if (/^\d+$/.test(channelExpression)) {
          commands.push({
            type: 'set',
            channel: parseInt(channelExpression),
            state
          })
        } else {
          commands.push({
            type: 'set',
            channel: -1,
            channelExpression,
            state
          })
        }
      }
    }

    else if (trimmed.includes('sleep(')) {
      const sleepMatch = trimmed.match(/sleep\((\d+|.*?delay_time_.*?)\)/)
      if (sleepMatch) {
        const delayStr = sleepMatch[1]
        let delay = 30

        if (/^\d+$/.test(delayStr)) {
          delay = parseInt(delayStr)
        }

        commands.push({
          type: 'sleep',
          delay
        })
      }
    }

    else if (trimmed.includes('off()')) {
      commands.push({
        type: 'off'
      })
    }

    else if (trimmed.includes('on()')) {
      commands.push({
        type: 'on'
      })
    }

    else if (trimmed.includes('for (') || trimmed.includes('for(')) {
      const forwardMatch = trimmed.match(/for\s*\(\s*int\s+([a-z]+)\s*=\s*(\d+)\s*;\s*\1\s*<\s*(.*?);\s*(.*?)\)/)
      const reverseMatch = trimmed.match(/for\s*\(\s*int\s+([a-z]+)\s*=\s*(.*?)\s*;\s*\1\s*>\s*(\d+)\s*;\s*(.*?)\)/)

      if (forwardMatch) {
        const variable = forwardMatch[1]
        const startValue = parseInt(forwardMatch[2])
        const limitStr = forwardMatch[3].trim()
        const stepStr = forwardMatch[4].trim()

        let loopCount = 1
        let loopStep = 1

        if (/^\d+$/.test(limitStr)) {
          loopCount = parseInt(limitStr) - startValue
        } else if (limitStr.includes('pin_size_')) {
          loopCount = -1
        }

        if (stepStr.includes('+=')) {
          const stepMatch = stepStr.match(/\+=\s*(\d+)/)
          if (stepMatch) {
            loopStep = parseInt(stepMatch[1])
            loopCount = Math.ceil(loopCount / loopStep)
          }
        }

        commands.push({
          type: 'for_start',
          loopCount,
          loopVariable: variable,
          loopStep,
          loopDirection: 'forward'
        })
      } else if (reverseMatch) {
        const variable = reverseMatch[1]
        const startStr = reverseMatch[2].trim()
        const endValue = parseInt(reverseMatch[3])

        let loopCount = 1

        if (startStr.includes('pin_size_')) {
          loopCount = -1
        } else if (/^\d+$/.test(startStr)) {
          loopCount = parseInt(startStr) - endValue
        }

        commands.push({
          type: 'for_start',
          loopCount,
          loopVariable: variable,
          loopStep: 1,
          loopDirection: 'reverse'
        })
      }
    }

    // Conditional statements
    else if (trimmed.startsWith('if (') || trimmed.startsWith('if(')) {
      const conditionMatch = trimmed.match(/if\s*\((.*?)\)/)
      if (conditionMatch) {
        commands.push({
          type: 'if',
          condition: conditionMatch[1].trim()
        })
      }
    }

    else if (trimmed === 'else {' || trimmed === 'else') {
      commands.push({
        type: 'else'
      })
    }

    // Variable declarations
    else if (trimmed.startsWith('int ') || trimmed.startsWith('const int ')) {
      const isConst = trimmed.startsWith('const int')
      const varType = isConst ? 'const int' : 'int'

      // Parse array declarations like "int arr[size];" or "const int arr[rows][cols] = {{...}};"
      const arrayMatch = trimmed.match(/(const\s+)?int\s+(\w+)\s*\[\s*(.*?)\s*\](?:\s*\[\s*(.*?)\s*\])?\s*(?:=\s*(\{.*?\}))?/)
      if (arrayMatch) {
        const varName = arrayMatch[2]
        const size1 = arrayMatch[3]
        const initializer = arrayMatch[5]

        commands.push({
          type: 'variable_decl',
          variableName: varName,
          variableType: varType,
          isArray: true,
          arraySize: size1,
          arrayInitializer: initializer ? parseArrayInitializer(initializer) : undefined
        })
      }
      // Parse simple variable declarations like "int halfSize = value;"
      else {
        const varMatch = trimmed.match(/(const\s+)?int\s+(\w+)\s*(?:=\s*(.*?))?;/)
        if (varMatch) {
          const varName = varMatch[2]
          const varValue = varMatch[3]

          commands.push({
            type: 'variable_decl',
            variableName: varName,
            variableType: varType,
            variableValue: varValue
          })
        }
      }
    }

    // Function calls (map, ceil, etc)
    else if (trimmed.includes('map(') || trimmed.includes('ceil(') || trimmed.includes('floor(') || trimmed.includes('abs(') || trimmed.includes('random(') || trimmed.includes('findMax(') || trimmed.includes('resetArray(') || trimmed.includes('shiftArrayAccordingToOrder(')) {
      const funcMatch = trimmed.match(/(\w+)\s*\((.*?)\)/)
      if (funcMatch) {
        const funcName = funcMatch[1]
        const params = funcMatch[2].split(',').map(p => p.trim())

        commands.push({
          type: 'function_call',
          functionName: funcName,
          parameters: params
        })
      }
    }

    // Custom function calls (resetArray, shiftArrayAccordingToOrder)
    else if (trimmed.includes('resetArray(') || trimmed.includes('shiftArrayAccordingToOrder(')) {
      const funcMatch = trimmed.match(/(\w+)\s*\((.*?)\)/)
      if (funcMatch) {
        const funcName = funcMatch[1]
        const params = funcMatch[2].split(',').map(p => p.trim())

        commands.push({
          type: 'custom_function_call',
          functionName: funcName,
          parameters: params
        })
      }
    }

    // Lambda function declarations (auto setRandomLed = [&](int value) -> void {)
    else if (trimmed.includes('auto ') && trimmed.includes('[') && trimmed.includes('](')) {
      const lambdaMatch = trimmed.match(/auto\s+(\w+)\s*=\s*\[([^\]]*)\]\s*\(([^)]*)\)\s*(?:->\s*(\w+))?\s*\{?/)
      if (lambdaMatch) {
        const lambdaName = lambdaMatch[1]
        const capture = lambdaMatch[2]
        const params = lambdaMatch[3].split(',').map(p => p.trim()).filter(p => p.length > 0)
        const returnType = lambdaMatch[4]

        commands.push({
          type: 'lambda_decl',
          lambdaName,
          lambdaCapture: capture,
          lambdaParams: params,
          lambdaReturnType: returnType
        })
      }
    }

    // Do-while loop start
    else if (trimmed === 'do {' || trimmed === 'do') {
      commands.push({
        type: 'do_while_start'
      })
    }

    // While condition for do-while
    else if (trimmed.startsWith('} while (') || trimmed.startsWith('while (')) {
      const conditionMatch = trimmed.match(/while\s*\((.*?)\)/)
      if (conditionMatch) {
        commands.push({
          type: 'do_while_end',
          doWhileCondition: conditionMatch[1].trim()
        })
      }
    }

    else if (trimmed === '}') {
      // Check if this is closing a conditional block or loop
      commands.push({
        type: 'for_end'  // Will be handled contextually in simulation
      })
    }
  }

  return commands
}

function parseArrayInitializer(initStr: string): (number | string | boolean)[][] {
  try {
    // Remove outer braces and split by inner braces
    const cleanStr = initStr.replace(/^\{|\}$/g, '').trim()
    const rows: (number | string | boolean)[][] = []

    // Match inner array patterns like {1,2,3}
    const rowMatches = cleanStr.match(/\{[^}]*\}/g)
    if (rowMatches) {
      for (const rowMatch of rowMatches) {
        const rowStr = rowMatch.replace(/^\{|\}$/g, '').trim()
        const values = rowStr.split(',').map(v => {
          const trimmed = v.trim()
          return isNaN(Number(trimmed)) ? trimmed : Number(trimmed)
        })
        rows.push(values)
      }
    }

    return rows
  } catch {
    return []
  }
}

function evaluateChannelExpression(
  expression: string,
  loopStack: { variable: string; count: number; startIndex: number; currentIteration: number; step: number; direction: 'forward' | 'reverse' }[],
  totalChannels: number,
  variables?: Map<string, number | string | boolean>
): number {
  let result = expression

  // Replace loop variables
  for (const loop of loopStack) {
    const actualValue = loop.direction === 'reverse'
      ? totalChannels - loop.currentIteration - 1
      : loop.currentIteration * loop.step

    result = result.replace(new RegExp(`\\b${loop.variable}\\b`, 'g'), actualValue.toString())
  }

  // Replace user-defined variables
  if (variables) {
    for (const [name, value] of variables) {
      const regex = new RegExp(`\\b${name}\\b`, 'g')
      result = result.replace(regex, value.toString())
    }
  }

  // Replace built-in variables
  result = result.replace(/config_data_ptr_->header\.pin_size_/g, totalChannels.toString())
  result = result.replace(/channel_data_\.delay_time_/g, '30')

  try {
    return Math.max(0, Math.min(totalChannels - 1, eval(result)))
  } catch {
    return 0
  }
}

export function simulateExecution(
  commands: ParsedCommand[],
  totalChannels: number,
  defaultDelay: number
): LEDFrame[] {
  console.log('Starting simulation with', commands.length, 'commands for', totalChannels, 'channels')
  const state = new CppSimulationState(totalChannels)
  const loopStack: { variable: string; count: number; startIndex: number; currentIteration: number; step: number; direction: 'forward' | 'reverse' }[] = []
  let commandIndex = 0

  while (commandIndex < commands.length) {
    const command = commands[commandIndex]

    // Debug logging for problematic cases
    if (commandIndex % 50 === 0) {
      console.log(`Processing command ${commandIndex}/${commands.length}:`, command.type)
    }

    // Check if we should execute this command based on conditional stack
    const shouldExecute = state.conditionalStack.length === 0 ||
                         state.conditionalStack.every(cond => cond.executing)

    if (shouldExecute) {
      switch (command.type) {
        case 'set':
          if (command.channel !== undefined) {
            if (command.channel === -1 && command.channelExpression) {
              const channelIndex = evaluateChannelExpression(command.channelExpression, loopStack, totalChannels, state.variables)
              state.setChannel(channelIndex, command.state!)
            } else if (command.channel >= 0) {
              state.setChannel(command.channel, command.state!)
            }
          }
          break

        case 'sleep':
          let delay = command.delay || defaultDelay
          // Handle dynamic delay expressions
          if (typeof delay === 'string') {
            delay = evaluateExpression(delay, loopStack, totalChannels, state.variables)
          }
          state.addFrame(delay)
          break

        case 'off':
          state.setAllChannels(false)
          state.addFrame(defaultDelay, 'All OFF')
          break

        case 'on':
          state.setAllChannels(true)
          state.addFrame(defaultDelay, 'All ON')
          break

        case 'variable_decl':
          if (command.variableName) {
            let value = command.variableValue
            if (typeof value === 'string') {
              value = evaluateExpression(value, loopStack, totalChannels, state.variables)
            }

            if (command.isArray) {
              const arraySize = typeof command.arraySize === 'string'
                ? evaluateExpression(command.arraySize, loopStack, totalChannels, state.variables)
                : command.arraySize || 0

              state.declareVariable(
                command.variableName,
                command.variableType || 'int',
                value,
                true,
                arraySize,
                command.arrayInitializer?.[0] // Flatten 2D array for simplicity
              )
            } else {
              state.declareVariable(command.variableName, command.variableType || 'int', value)
            }
          }
          break

        case 'function_call':
          if (command.functionName && command.parameters) {
            const evaluatedParams = command.parameters.map(p =>
              typeof p === 'string' ? evaluateExpression(p, loopStack, totalChannels, state.variables) : p
            )
            state.callFunction(command.functionName, evaluatedParams)
          }
          break

        case 'custom_function_call':
          if (command.functionName && command.parameters) {
            const evaluatedParams = command.parameters.map(p =>
              typeof p === 'string' ? evaluateExpression(p, loopStack, totalChannels, state.variables) : p
            )

            // Handle specific custom functions
            switch (command.functionName) {
              case 'resetArray':
                if (evaluatedParams.length >= 3) {
                  // resetArray(arrData, arrSize, arrLastIndex)
                  // For simulation, we'll track these as special variables
                  state.variables.set('arrSize', 0)
                  state.variables.set('arrLastIndex', 0)
                  if (state.arrays.has('arrData')) {
                    state.arrays.get('arrData')!.length = 0
                  }
                }
                break

              case 'shiftArrayAccordingToOrder':
                if (evaluatedParams.length >= 7) {
                  // Simplified implementation for simulation
                  const orderIndex = Number(evaluatedParams[6])

                  // Simulate adding some channels to array
                  const arrData = state.arrays.get('arrData') || []
                  const addedCount = Math.min(3, totalChannels - arrData.length) // Simplified logic

                  for (let i = 0; i < addedCount; i++) {
                    arrData.push(orderIndex + i)
                  }

                  state.arrays.set('arrData', arrData)
                  state.variables.set('arrSize', arrData.length)

                  // Return value simulation
                  state.variables.set('_lastFunctionReturn', addedCount)
                }
                break
            }
          }
          break

        case 'lambda_decl':
          if (command.lambdaName && command.lambdaCapture !== undefined && command.lambdaParams) {
            // Register lambda function
            state.defineLambdaFunction(
              command.lambdaName,
              command.lambdaCapture,
              command.lambdaParams,
              [], // Body will be parsed separately
              command.lambdaReturnType
            )
          }
          break

        case 'do_while_start':
          // Mark the start of do-while loop
          state.doWhileStack.push({
            startIndex: commandIndex,
            condition: '' // Will be set when we hit the while condition
          })
          break

        case 'do_while_end':
          if (command.doWhileCondition && state.doWhileStack.length > 0) {
            const doWhileInfo = state.doWhileStack[state.doWhileStack.length - 1]
            doWhileInfo.condition = command.doWhileCondition

            // Evaluate condition
            const conditionResult = state.evaluateCondition(command.doWhileCondition)

            if (conditionResult) {
              // Continue loop - go back to start
              commandIndex = doWhileInfo.startIndex
            } else {
              // Exit loop
              state.doWhileStack.pop()
            }
          }
          break

        case 'if':
          if (command.condition) {
            const conditionResult = state.evaluateCondition(command.condition)
            state.conditionalStack.push({
              condition: conditionResult,
              executing: conditionResult
            })
          }
          break

        case 'else':
          if (state.conditionalStack.length > 0) {
            const currentCond = state.conditionalStack[state.conditionalStack.length - 1]
            currentCond.executing = !currentCond.condition
          }
          break

        case 'for_start':
          if (command.loopCount !== undefined && command.loopVariable) {
            let actualCount = command.loopCount
            if (actualCount === -1) {
              actualCount = totalChannels
            } else if (typeof actualCount === 'string') {
              actualCount = evaluateExpression(actualCount, loopStack, totalChannels, state.variables)
            }

            const step = command.loopStep || 1
            const direction = command.loopDirection || 'forward'

            loopStack.push({
              variable: command.loopVariable,
              count: actualCount,
              startIndex: commandIndex,
              currentIteration: 0,
              step,
              direction
            })
          }
          break

        case 'for_end':
          // Check if this closes a conditional or loop block
          if (state.conditionalStack.length > 0) {
            // Check if this is the end of a conditional block
            let foundConditionalEnd = false
            for (let i = commandIndex + 1; i < commands.length; i++) {
              if (commands[i].type === 'if' || commands[i].type === 'else') {
                foundConditionalEnd = false
                break
              }
              if (commands[i].type === 'for_start') {
                foundConditionalEnd = false
                break
              }
              if (i === commands.length - 1) {
                foundConditionalEnd = true
              }
            }

            if (foundConditionalEnd) {
              state.conditionalStack.pop()
              break
            }
          }

          // Handle loop end
          const currentLoop = loopStack[loopStack.length - 1]
          if (currentLoop) {
            currentLoop.currentIteration++
            if (currentLoop.currentIteration < currentLoop.count) {
              commandIndex = currentLoop.startIndex
            } else {
              loopStack.pop()
            }
          }
          break
      }
    } else {
      // Handle conditional block structure even when not executing
      switch (command.type) {
        case 'if':
          state.conditionalStack.push({
            condition: false,
            executing: false
          })
          break

        case 'else':
          if (state.conditionalStack.length > 0) {
            const currentCond = state.conditionalStack[state.conditionalStack.length - 1]
            currentCond.executing = !currentCond.condition
          }
          break

        case 'for_end':
          if (state.conditionalStack.length > 0) {
            state.conditionalStack.pop()
          }
          break
      }
    }

    commandIndex++

    if (commandIndex > commands.length * 10) { // Much more aggressive protection
      console.warn('Infinite loop detected, breaking execution. CommandIndex:', commandIndex, 'Commands length:', commands.length)
      break
    }
  }

  if (state.frames.length === 0) {
    state.addFrame(defaultDelay, 'Empty Pattern')
  }

  return state.frames
}

function evaluateExpression(
  expression: string,
  loopStack: { variable: string; count: number; startIndex: number; currentIteration: number; step: number; direction: 'forward' | 'reverse' }[],
  totalChannels: number,
  variables: Map<string, number | string | boolean>
): number {
  let result = expression

  // Replace loop variables
  for (const loop of loopStack) {
    const actualValue = loop.direction === 'reverse'
      ? totalChannels - loop.currentIteration - 1
      : loop.currentIteration * loop.step

    result = result.replace(new RegExp(`\\b${loop.variable}\\b`, 'g'), actualValue.toString())
  }

  // Replace user-defined variables
  for (const [name, value] of variables) {
    const regex = new RegExp(`\\b${name}\\b`, 'g')
    result = result.replace(regex, value.toString())
  }

  // Replace built-in variables
  result = result.replace(/config_data_ptr_->header\.pin_size_/g, totalChannels.toString())
  result = result.replace(/channel_data_\.delay_time_/g, '30')

  // Handle function calls in expressions
  try {
    // Create context with Arduino functions available
    const context = {
      map: (value: number, fromLow: number, fromHigh: number, toLow: number, toHigh: number) =>
        (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow,
      ceil: Math.ceil,
      floor: Math.floor,
      abs: Math.abs,
      random: (max: number) => Math.floor(Math.random() * max),
      findMax: (...args: number[]) => Math.max(...args.filter(n => !isNaN(n))),
      min: Math.min,
      max: Math.max
    }

    // Try to evaluate with function context
    const func = new Function(...Object.keys(context), `return ${result}`)
    return func(...Object.values(context))
  } catch {
    return 0
  }
}

export function parseAndSimulateCppCode(
  code: string,
  totalChannels: number,
  defaultDelay: number
): LEDFrame[] {
  try {
    const commands = parseAutoLightCode(code)
    return simulateExecution(commands, totalChannels, defaultDelay)
  } catch (error) {
    console.error('C++ parsing/simulation error:', error)
    return [{
      id: crypto.randomUUID(),
      channels: new Array(totalChannels).fill(false),
      delay: defaultDelay,
      label: 'Parse Error'
    }]
  }
}