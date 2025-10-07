"use client"

import { useState, useCallback } from "react"
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card"
import { Button } from "@/components/ui/button"
import { Input } from "@/components/ui/input"
import { Badge } from "@/components/ui/badge"
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select"
import { useSimulation } from "@/lib/contexts/SimulationContext"
import { Settings2, Calculator, Play, Trash2, Copy } from "lucide-react"
import ExpressionLibrary from "../expression/ExpressionLibrary"
import ExpressionPreview from "../expression/ExpressionPreview"
import ExpressionCodeGenerator from "../expression/ExpressionCodeGenerator"
import DynamicPatternGrid from "../grid/DynamicPatternGrid"

export default function ExpressionBuilderMode() {
  const {
    simulationData,
    addExpressionFrame,
    clearExpressionFrames,
    setExpressionChannelCount,
    generatePatternFromExpressions
  } = useSimulation()

  const [channelCount, setChannelCount] = useState(simulationData.expressionChannelCount)
  const [customExpression, setCustomExpression] = useState("")

  const handleChannelCountChange = useCallback((count: number) => {
    setChannelCount(count)
    setExpressionChannelCount(count)
  }, [setExpressionChannelCount])

  const handleClearFrames = useCallback(() => {
    clearExpressionFrames()
  }, [clearExpressionFrames])

  const handleGeneratePattern = useCallback(() => {
    generatePatternFromExpressions()
  }, [generatePatternFromExpressions])

  const channelOptions = [8, 12, 16, 20, 24, 32, 48, 64]

  const getGridLayout = (totalChannels: number) => {
    const layouts: Record<number, { rows: number; cols: number }> = {
      8: { rows: 2, cols: 4 },
      12: { rows: 3, cols: 4 },
      16: { rows: 4, cols: 4 },
      20: { rows: 4, cols: 5 },
      24: { rows: 4, cols: 6 },
      32: { rows: 4, cols: 8 },
      48: { rows: 6, cols: 8 },
      64: { rows: 8, cols: 8 }
    }
    return layouts[totalChannels] || { rows: 4, cols: 6 }
  }

  return (
    <div className="space-y-6">
      {/* Channel Configuration */}
      <Card>
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <Settings2 className="h-5 w-5" />
            Channel Configuration
          </CardTitle>
        </CardHeader>
        <CardContent className="space-y-4">
          <div className="flex items-center gap-4">
            <div className="flex items-center gap-2">
              <label className="text-sm font-medium">Total Channels:</label>
              <Select value={channelCount.toString()} onValueChange={(value) => handleChannelCountChange(parseInt(value))}>
                <SelectTrigger className="w-20">
                  <SelectValue />
                </SelectTrigger>
                <SelectContent>
                  {channelOptions.map(option => (
                    <SelectItem key={option} value={option.toString()}>
                      {option}
                    </SelectItem>
                  ))}
                </SelectContent>
              </Select>
            </div>
            <Badge variant="secondary" className="text-xs">
              Layout: {getGridLayout(channelCount).rows}×{getGridLayout(channelCount).cols} Matrix
            </Badge>
            <Badge variant="outline" className="text-xs">
              Compatible: 8-64 channels
            </Badge>
          </div>
        </CardContent>
      </Card>

      {/* Expression Library & Custom Editor */}
      <div className="grid gap-6 lg:grid-cols-2">
        <Card>
          <CardHeader>
            <CardTitle className="flex items-center gap-2">
              <Calculator className="h-5 w-5" />
              Pattern Library
            </CardTitle>
          </CardHeader>
          <CardContent>
            <ExpressionLibrary
              totalChannels={channelCount}
              onAddFrame={addExpressionFrame}
            />
          </CardContent>
        </Card>

        <Card>
          <CardHeader>
            <CardTitle className="flex items-center gap-2">
              <Calculator className="h-5 w-5" />
              Custom Expression
            </CardTitle>
          </CardHeader>
          <CardContent className="space-y-4">
            <div className="space-y-2">
              <label className="text-sm font-medium">Expression:</label>
              <Input
                value={customExpression}
                onChange={(e) => setCustomExpression(e.target.value)}
                placeholder="i % 2 === 0"
                className="font-mono text-sm"
              />
              <div className="text-xs text-muted-foreground">
                Variables: <code>i</code> (channel index), <code>totalChannels</code> (total count)
              </div>
            </div>

            <div className="flex gap-2 flex-wrap">
              <Badge variant="outline" className="text-xs">i &lt; totalChannels/2</Badge>
              <Badge variant="outline" className="text-xs">i % 2 === 0</Badge>
              <Badge variant="outline" className="text-xs">i &gt;= totalChannels/4</Badge>
            </div>

            <Button
              onClick={() => {
                if (customExpression.trim()) {
                  addExpressionFrame({
                    id: `custom_${Date.now()}`,
                    name: 'Custom Expression',
                    description: customExpression,
                    expression: customExpression,
                    category: 'custom'
                  })
                  setCustomExpression("")
                }
              }}
              disabled={!customExpression.trim()}
              className="w-full"
            >
              Add Custom Expression
            </Button>
          </CardContent>
        </Card>
      </div>

      {/* Frame Management & Preview */}
      <div className="grid gap-6 lg:grid-cols-3">
        <Card className="lg:col-span-2">
          <CardHeader className="pb-3">
            <div className="flex items-center justify-between">
              <CardTitle>Expression Frames</CardTitle>
              <div className="flex gap-2">
                <Button
                  onClick={handleClearFrames}
                  disabled={simulationData.expressionFrames.length === 0}
                  size="sm"
                  variant="outline"
                  className="flex items-center gap-1"
                >
                  <Trash2 className="h-3 w-3" />
                  Clear All
                </Button>
                <Button
                  onClick={handleGeneratePattern}
                  disabled={simulationData.expressionFrames.length === 0}
                  size="sm"
                  className="flex items-center gap-1"
                >
                  <Play className="h-3 w-3" />
                  Generate Pattern
                </Button>
              </div>
            </div>
          </CardHeader>
          <CardContent>
            <ExpressionPreview
              channelCount={channelCount}
              frames={simulationData.expressionFrames}
            />
          </CardContent>
        </Card>

        <Card>
          <CardHeader className="pb-3">
            <CardTitle>Live Preview</CardTitle>
          </CardHeader>
          <CardContent>
            <div className="text-center text-sm text-muted-foreground mb-4">
              {channelCount} channels • {getGridLayout(channelCount).rows}×{getGridLayout(channelCount).cols} layout
            </div>
            <DynamicPatternGrid disabled={true} />

            <div className="mt-4 space-y-2">
              <div className="flex justify-between text-xs text-muted-foreground">
                <span>Expression Frames:</span>
                <span>{simulationData.expressionFrames.length}</span>
              </div>
              <div className="flex justify-between text-xs text-muted-foreground">
                <span>Total Duration:</span>
                <span>
                  {simulationData.expressionFrames.reduce((total, frame) => total + frame.duration, 0)}ms
                </span>
              </div>
            </div>
          </CardContent>
        </Card>
      </div>

      {/* Code Generation */}
      <Card>
        <CardHeader className="pb-3">
          <CardTitle className="flex items-center gap-2">
            <Copy className="h-5 w-5" />
            Generated C++ Code
          </CardTitle>
        </CardHeader>
        <CardContent>
          <ExpressionCodeGenerator
            totalChannels={channelCount}
            frames={simulationData.expressionFrames}
          />
        </CardContent>
      </Card>

      {/* Expression Syntax Help */}
      <Card>
        <CardHeader className="pb-3">
          <CardTitle className="text-sm">Mathematical Expression Syntax</CardTitle>
        </CardHeader>
        <CardContent className="text-xs space-y-4">
          <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
            <div>
              <h4 className="font-medium mb-2 text-blue-600">Variables:</h4>
              <code className="block bg-muted p-2 rounded text-xs">
                i // Channel index (0 to totalChannels-1)<br/>
                totalChannels // Total number of channels<br/>
              </code>
            </div>

            <div>
              <h4 className="font-medium mb-2 text-green-600">Comparison Operators:</h4>
              <code className="block bg-muted p-2 rounded text-xs">
                i &lt; totalChannels / 2<br/>
                i &gt;= totalChannels / 4<br/>
                i === 0<br/>
                i !== totalChannels - 1
              </code>
            </div>

            <div>
              <h4 className="font-medium mb-2 text-purple-600">Mathematical Operators:</h4>
              <code className="block bg-muted p-2 rounded text-xs">
                i % 2 === 0 // Even channels<br/>
                i % 3 === 1 // Every 3rd + 1<br/>
                i / 4 &gt; 2 // Division<br/>
                i * 2 &lt; totalChannels // Multiplication
              </code>
            </div>

            <div>
              <h4 className="font-medium mb-2 text-orange-600">Logical Operators:</h4>
              <code className="block bg-muted p-2 rounded text-xs">
                i &lt; 5 && i &gt; 1 // AND<br/>
                i === 0 || i === totalChannels-1 // OR<br/>
                !(i % 2 === 0) // NOT (odd channels)
              </code>
            </div>

            <div>
              <h4 className="font-medium mb-2 text-red-600">Common Patterns:</h4>
              <code className="block bg-muted p-2 rounded text-xs">
                true // All channels ON<br/>
                false // All channels OFF<br/>
                i &lt; totalChannels / 2 // First half<br/>
                i &gt;= totalChannels / 2 // Second half
              </code>
            </div>

            <div>
              <h4 className="font-medium mb-2 text-teal-600">Complex Examples:</h4>
              <code className="block bg-muted p-2 rounded text-xs">
                i % 4 === 0 && i &lt; totalChannels/2<br/>
                (i + 1) % 3 === 0<br/>
                i &gt;= totalChannels/4 && i &lt; totalChannels*3/4
              </code>
            </div>
          </div>

          <div className="mt-4 p-3 bg-blue-50 dark:bg-blue-950 rounded-lg">
            <h4 className="font-medium mb-2 text-blue-700 dark:text-blue-300">✨ Expression Builder Features:</h4>
            <div className="text-xs text-blue-600 dark:text-blue-400 space-y-1">
              <p>• <strong>Dynamic Compatibility:</strong> Expressions automatically scale with any channel count (8-64)</p>
              <p>• <strong>Mathematical Power:</strong> Full JavaScript expression support with logical and arithmetic operators</p>
              <p>• <strong>Real-time Evaluation:</strong> Immediate visual feedback as you build expressions</p>
              <p>• <strong>Code Generation:</strong> Direct BaseChannelSequence.cpp compatible output</p>
              <p>• <strong>Built-in Library:</strong> Common patterns ready to use with customizable parameters</p>
            </div>
          </div>
        </CardContent>
      </Card>
    </div>
  )
}