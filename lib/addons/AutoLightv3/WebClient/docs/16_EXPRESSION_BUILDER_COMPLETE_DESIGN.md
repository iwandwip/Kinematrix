# 10_EXPRESSION_BUILDER_COMPLETE_DESIGN.md

# Dynamic LED Pattern Builder - Expression Builder Mode
**Complete Design Document**

---

## 1. Architecture Overview

### 1.1 Konsep Dasar
Expression Builder mode adalah mode ke-3 di Pattern Builder yang menggunakan **mathematical expressions** untuk generate LED patterns yang dinamis dan compatible dengan BaseChannelSequence.cpp system.

### 1.2 Core Philosophy
- **Data Structure**: LED tetap sebagai 1D array `boolean[]`
- **Visual Interface**: Grid hanya untuk preview/visualization
- **Pattern Logic**: Mathematical expressions yang variable-based
- **Code Generation**: Generate BaseChannelSequence.cpp compatible code

### 1.3 Integration dengan Existing System
```typescript
// Existing SimulationContext
patternMode: 'visual' | 'cpp' | 'expression'  // Add new mode

// New mode component
components/simulation/modes/ExpressionBuilderMode.tsx
```

---

## 2. Core Features

### 2.1 Mathematical Expression System

#### Pattern Expression Interface
```typescript
interface PatternExpression {
  id: string
  name: string
  description: string
  expression: string           // "i < totalChannels/2"
  category: ExpressionCategory
  parameters?: ExpressionParameter[]
}

type ExpressionCategory =
  | 'basic'        // All, Half, None
  | 'mathematical' // Even/Odd, Modulo operations
  | 'sequential'   // Progressive, Snake, Wave
  | 'custom'       // User-defined expressions

interface ExpressionParameter {
  name: string        // "divisor", "offset"
  type: 'number' | 'boolean'
  defaultValue: any
  min?: number
  max?: number
}
```

#### Built-in Expression Library
```typescript
const BUILTIN_EXPRESSIONS: PatternExpression[] = [
  // Basic Patterns
  {
    id: 'all_on',
    name: 'All Channels ON',
    description: 'Turn on all available channels',
    expression: 'true',
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

  // Mathematical Patterns
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

  // Sequential Patterns
  {
    id: 'first_quarter',
    name: 'First Quarter',
    description: 'Turn on first 25% of channels',
    expression: 'i < totalChannels / 4',
    category: 'sequential'
  },
  {
    id: 'middle_range',
    name: 'Middle Range',
    description: 'Turn on middle 50% of channels',
    expression: 'i >= totalChannels / 4 && i < (totalChannels * 3) / 4',
    category: 'sequential'
  },

  // Custom Expression Template
  {
    id: 'custom_modulo',
    name: 'Custom Modulo',
    description: 'Turn on channels based on custom divisor',
    expression: 'i % ${divisor} === ${offset}',
    category: 'custom',
    parameters: [
      { name: 'divisor', type: 'number', defaultValue: 3, min: 2, max: 32 },
      { name: 'offset', type: 'number', defaultValue: 0, min: 0, max: 31 }
    ]
  }
]
```

### 2.2 Dynamic Channel Configuration
```typescript
interface ChannelConfiguration {
  totalChannels: number        // 8, 12, 16, 24, 32, 48, 64
  layoutSuggestion: GridLayout // Auto-suggested optimal layout
  customLayout?: GridLayout    // User override
}

interface GridLayout {
  rows: number
  cols: number
  arrangement: 'linear' | 'matrix' | 'custom'
}

// Auto-layout suggestions based on channel count
const LAYOUT_SUGGESTIONS: Record<number, GridLayout[]> = {
  8: [
    { rows: 2, cols: 4, arrangement: 'matrix' },
    { rows: 1, cols: 8, arrangement: 'linear' }
  ],
  12: [
    { rows: 3, cols: 4, arrangement: 'matrix' },
    { rows: 2, cols: 6, arrangement: 'matrix' }
  ],
  24: [
    { rows: 4, cols: 6, arrangement: 'matrix' },
    { rows: 3, cols: 8, arrangement: 'matrix' }
  ],
  // ... more configurations
}
```

---

## 3. Pattern Expression Evaluation System

### 3.1 Expression Evaluator
```typescript
class PatternExpressionEvaluator {
  private totalChannels: number
  private parameters: Record<string, any>

  constructor(totalChannels: number) {
    this.totalChannels = totalChannels
    this.parameters = {}
  }

  setParameters(params: Record<string, any>) {
    this.parameters = params
  }

  evaluateExpression(expression: string): boolean[] {
    const channels: boolean[] = new Array(this.totalChannels).fill(false)

    // Substitute parameters in expression
    let processedExpression = this.substituteParameters(expression)

    // Evaluate for each channel index
    for (let i = 0; i < this.totalChannels; i++) {
      try {
        // Replace 'i' with current index and 'totalChannels' with actual count
        const channelExpression = processedExpression
          .replace(/\bi\b/g, i.toString())
          .replace(/\btotalChannels\b/g, this.totalChannels.toString())

        // Safe evaluation (using Function constructor with restricted scope)
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

    // Replace parameter placeholders: ${paramName}
    Object.entries(this.parameters).forEach(([key, value]) => {
      const placeholder = `\${${key}}`
      result = result.replace(new RegExp(placeholder.replace('$', '\\$'), 'g'), value.toString())
    })

    return result
  }

  private safeEvaluate(expression: string): boolean {
    // Whitelist of allowed operations and functions
    const allowedTokens = /^[0-9+\-*/%()&|!<>=\s]+$/

    if (!allowedTokens.test(expression)) {
      throw new Error('Expression contains invalid characters')
    }

    // Use Function constructor for safe evaluation
    return new Function('return ' + expression)()
  }
}
```

### 3.2 Pattern Frame Generation
```typescript
interface ExpressionFrame {
  frameNumber: number
  expression: PatternExpression
  channels: boolean[]
  parameters: Record<string, any>
  duration: number
  label?: string
}

class ExpressionFrameBuilder {
  private evaluator: PatternExpressionEvaluator
  private frames: ExpressionFrame[] = []

  constructor(totalChannels: number) {
    this.evaluator = new PatternExpressionEvaluator(totalChannels)
  }

  addFrame(expression: PatternExpression, parameters: Record<string, any> = {}, duration: number = 100): ExpressionFrame {
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
}
```

---

## 4. Code Generation Strategy

### 4.1 BaseChannelSequence.cpp Code Generator
```typescript
class ExpressionCodeGenerator {
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
    // Convert expression to C++ loop logic
    const expr = frame.expression.expression

    // Handle common patterns
    if (expr === 'true') {
      return `for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
        set(config_data_ptr_->header.pin_ptr_[i], HIGH);
    }`
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

    // Generic expression conversion
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

  private convertToCppCondition(expression: string, parameters: Record<string, any>): string {
    // Convert JavaScript expression to C++ equivalent
    let cppExpr = expression
      .replace(/totalChannels/g, 'config_data_ptr_->header.pin_size_')
      .replace(/===/g, '==')
      .replace(/!==/g, '!=')
      .replace(/&&/g, ' && ')
      .replace(/\|\|/g, ' || ')

    // Substitute parameters
    Object.entries(parameters).forEach(([key, value]) => {
      const placeholder = `\${${key}}`
      cppExpr = cppExpr.replace(new RegExp(placeholder.replace('$', '\\$'), 'g'), value.toString())
    })

    return cppExpr
  }

  private generateNativeLogic(frame: ExpressionFrame): string {
    // Generate individual set() calls for each active channel
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
```

---

## 5. User Interface Design

### 5.1 Component Structure
```
components/simulation/modes/ExpressionBuilderMode.tsx
├── ExpressionChannelConfig.tsx      # Channel count input & layout
├── ExpressionLibrary.tsx            # Built-in pattern library
├── ExpressionEditor.tsx             # Custom expression editor
├── ExpressionPreview.tsx            # Real-time pattern preview
├── ExpressionTimeline.tsx           # Frame sequence timeline
└── ExpressionCodeGenerator.tsx      # Code output display
```

### 5.2 Main Mode Component
```typescript
// ExpressionBuilderMode.tsx
export default function ExpressionBuilderMode() {
  const { simulationData, setExpressionConfig } = useSimulation()
  const [channelConfig, setChannelConfig] = useState<ChannelConfiguration>({
    totalChannels: 24,
    layoutSuggestion: { rows: 4, cols: 6, arrangement: 'matrix' }
  })
  const [frameBuilder] = useState(() => new ExpressionFrameBuilder(channelConfig.totalChannels))

  const handleChannelConfigChange = (config: ChannelConfiguration) => {
    setChannelConfig(config)
    // Reinitialize frame builder with new channel count
    frameBuilder = new ExpressionFrameBuilder(config.totalChannels)
  }

  const handleAddFrame = (expression: PatternExpression, parameters: Record<string, any>, duration: number) => {
    const frame = frameBuilder.addFrame(expression, parameters, duration)
    // Update simulation context
    setExpressionConfig({
      totalChannels: channelConfig.totalChannels,
      frames: frameBuilder.getFrames()
    })
  }

  return (
    <div className="space-y-6">
      {/* Channel Configuration */}
      <Card>
        <CardHeader>
          <CardTitle>Channel Configuration</CardTitle>
        </CardHeader>
        <CardContent>
          <ExpressionChannelConfig
            config={channelConfig}
            onChange={handleChannelConfigChange}
          />
        </CardContent>
      </Card>

      {/* Expression Library & Editor */}
      <div className="grid gap-6 lg:grid-cols-2">
        <Card>
          <CardHeader>
            <CardTitle>Pattern Library</CardTitle>
          </CardHeader>
          <CardContent>
            <ExpressionLibrary onAddFrame={handleAddFrame} />
          </CardContent>
        </Card>

        <Card>
          <CardHeader>
            <CardTitle>Custom Expression</CardTitle>
          </CardHeader>
          <CardContent>
            <ExpressionEditor
              totalChannels={channelConfig.totalChannels}
              onAddFrame={handleAddFrame}
            />
          </CardContent>
        </Card>
      </div>

      {/* Preview & Timeline */}
      <div className="grid gap-6 lg:grid-cols-3">
        <Card className="lg:col-span-2">
          <CardHeader>
            <CardTitle>Pattern Preview</CardTitle>
          </CardHeader>
          <CardContent>
            <ExpressionPreview
              channelConfig={channelConfig}
              frames={frameBuilder.getFrames()}
            />
          </CardContent>
        </Card>

        <Card>
          <CardHeader>
            <CardTitle>Frame Timeline</CardTitle>
          </CardHeader>
          <CardContent>
            <ExpressionTimeline frames={frameBuilder.getFrames()} />
          </CardContent>
        </Card>
      </div>

      {/* Code Generation */}
      <Card>
        <CardHeader>
          <CardTitle>Generated C++ Code</CardTitle>
        </CardHeader>
        <CardContent>
          <ExpressionCodeGenerator
            totalChannels={channelConfig.totalChannels}
            frames={frameBuilder.getFrames()}
          />
        </CardContent>
      </Card>
    </div>
  )
}
```

### 5.3 Expression Library Component
```typescript
// ExpressionLibrary.tsx
interface ExpressionLibraryProps {
  onAddFrame: (expression: PatternExpression, parameters: Record<string, any>, duration: number) => void
}

export default function ExpressionLibrary({ onAddFrame }: ExpressionLibraryProps) {
  const [selectedCategory, setSelectedCategory] = useState<ExpressionCategory>('basic')
  const [parameterValues, setParameterValues] = useState<Record<string, Record<string, any>>>({})

  const filteredExpressions = BUILTIN_EXPRESSIONS.filter(expr => expr.category === selectedCategory)

  const handleAddExpression = (expression: PatternExpression, duration: number = 100) => {
    const parameters = expression.parameters
      ? parameterValues[expression.id] || {}
      : {}

    onAddFrame(expression, parameters, duration)
  }

  const handleParameterChange = (expressionId: string, paramName: string, value: any) => {
    setParameterValues(prev => ({
      ...prev,
      [expressionId]: {
        ...prev[expressionId],
        [paramName]: value
      }
    }))
  }

  return (
    <div className="space-y-4">
      {/* Category Selector */}
      <div className="flex gap-2 flex-wrap">
        {(['basic', 'mathematical', 'sequential', 'custom'] as ExpressionCategory[]).map(category => (
          <Button
            key={category}
            variant={selectedCategory === category ? 'default' : 'outline'}
            size="sm"
            onClick={() => setSelectedCategory(category)}
            className="capitalize"
          >
            {category}
          </Button>
        ))}
      </div>

      {/* Expression List */}
      <div className="space-y-3 max-h-96 overflow-y-auto">
        {filteredExpressions.map(expression => (
          <Card key={expression.id} className="p-3">
            <div className="flex justify-between items-start">
              <div className="flex-1">
                <h4 className="font-medium">{expression.name}</h4>
                <p className="text-sm text-muted-foreground">{expression.description}</p>
                <code className="text-xs bg-muted px-2 py-1 rounded mt-1 inline-block">
                  {expression.expression}
                </code>

                {/* Parameter Controls */}
                {expression.parameters && (
                  <div className="mt-2 space-y-2">
                    {expression.parameters.map(param => (
                      <div key={param.name} className="flex items-center gap-2">
                        <Label className="text-xs">{param.name}:</Label>
                        {param.type === 'number' ? (
                          <Input
                            type="number"
                            size="sm"
                            min={param.min}
                            max={param.max}
                            defaultValue={param.defaultValue}
                            onChange={(e) => handleParameterChange(expression.id, param.name, parseInt(e.target.value))}
                            className="w-20"
                          />
                        ) : (
                          <Checkbox
                            defaultChecked={param.defaultValue}
                            onCheckedChange={(checked) => handleParameterChange(expression.id, param.name, checked)}
                          />
                        )}
                      </div>
                    ))}
                  </div>
                )}
              </div>

              <Button
                size="sm"
                onClick={() => handleAddExpression(expression)}
                className="ml-2"
              >
                <Plus className="h-3 w-3" />
                Add
              </Button>
            </div>
          </Card>
        ))}
      </div>
    </div>
  )
}
```

---

## 6. Implementation Roadmap

### 6.1 Phase 1: Core Infrastructure (Week 1)
- [ ] Update SimulationContext untuk support Expression mode
- [ ] Create PatternExpression interfaces dan types
- [ ] Implement PatternExpressionEvaluator class
- [ ] Setup ExpressionFrameBuilder class
- [ ] Create basic ExpressionBuilderMode component structure

### 6.2 Phase 2: UI Components (Week 2)
- [ ] Build ExpressionChannelConfig component
- [ ] Implement ExpressionLibrary dengan built-in patterns
- [ ] Create ExpressionEditor untuk custom expressions
- [ ] Build ExpressionPreview dengan real-time grid visualization
- [ ] Implement ExpressionTimeline untuk frame management

### 6.3 Phase 3: Code Generation (Week 3)
- [ ] Implement ExpressionCodeGenerator class
- [ ] Add Modern API code generation
- [ ] Add Native API code generation
- [ ] Create ExpressionCodeGenerator UI component
- [ ] Add copy-to-clipboard functionality

### 6.4 Phase 4: Testing & Polish (Week 4)
- [ ] Unit tests untuk expression evaluator
- [ ] Integration tests dengan different channel counts
- [ ] UI/UX improvements dan bug fixes
- [ ] Documentation updates
- [ ] Performance optimization

### 6.5 Dependencies
- Existing SimulationContext system
- Current Pattern Builder infrastructure
- shadcn/ui components
- Lucide React icons
- TypeScript support

---

## 7. Usage Example

### 7.1 Typical User Workflow

1. **Channel Configuration**
   ```
   User Input: 24 channels
   Auto-suggest: 4x6 matrix layout
   ```

2. **Pattern Selection**
   ```
   Category: Mathematical
   Pattern: "Even Channels"
   Expression: i % 2 === 0
   Duration: 100ms
   ```

3. **Frame Building**
   ```
   Frame 1: Even channels ON (100ms)
   Frame 2: Odd channels ON (100ms)
   Frame 3: All channels ON (200ms)
   Frame 4: All channels OFF (50ms)
   ```

4. **Code Generation**
   ```cpp
   void BaseChannel::taskSequenceCustomExpression() {
     // Frame 1: Even Channels
     for (int i = 0; i < config_data_ptr_->header.pin_size_; i += 2) {
       set(config_data_ptr_->header.pin_ptr_[i], HIGH);
     }
     sleep(100);

     // Frame 2: Odd Channels
     for (int i = 1; i < config_data_ptr_->header.pin_size_; i += 2) {
       set(config_data_ptr_->header.pin_ptr_[i], HIGH);
     }
     sleep(100);

     // ... etc
   }
   ```

### 7.2 Advanced Custom Expression
```
Expression: i >= totalChannels/4 && i < totalChannels*3/4 && i % 3 === 0
Description: Every 3rd channel in middle 50% range
Generated: Channels 6,9,12,15 (for 24 total channels)
```

---

## 8. Technical Considerations

### 8.1 Performance
- Expression evaluation cached per frame
- Real-time preview throttled untuk performance
- Code generation on-demand only

### 8.2 Security
- Expression evaluation menggunakan restricted Function scope
- Whitelist allowed characters dan operations
- Parameter validation dan sanitization

### 8.3 Compatibility
- Generated code 100% compatible dengan BaseChannelSequence.cpp
- Support all channel counts (8-64)
- Maintain existing Pattern Builder architecture

### 8.4 Extensibility
- Easy untuk add new built-in expressions
- Plugin system untuk custom expression libraries
- Export/import expression templates

---

## 9. Conclusion

Expression Builder mode menyediakan powerful dan flexible approach untuk create LED patterns yang dinamis. Dengan mathematical expression system, users dapat create complex patterns yang automatically scale dengan any channel count, perfect untuk AutoLight V3's dynamic architecture.

Key benefits:
- **Dynamic Compatibility**: Works dengan 8-64 channels
- **Mathematical Power**: Complex expressions dengan parameters
- **Code Generation**: Direct BaseChannelSequence.cpp compatibility
- **User Friendly**: Visual preview dengan intuitive interface
- **Extensible**: Easy untuk add new patterns dan capabilities

Implementation ini akan significantly enhance Pattern Builder capabilities sambil maintaining compatibility dengan existing AutoLight V3 system.