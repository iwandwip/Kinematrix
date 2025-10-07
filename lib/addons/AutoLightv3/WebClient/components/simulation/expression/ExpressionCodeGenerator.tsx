"use client"

import { useState, useCallback } from "react"
import { Button } from "@/components/ui/button"
import { Badge } from "@/components/ui/badge"
import { Textarea } from "@/components/ui/textarea"
import { Copy, Download, Code, Settings } from "lucide-react"
import { ExpressionCodeGenerator, type ExpressionFrame } from "@/lib/simulation/expressionParser"

interface ExpressionCodeGeneratorProps {
  totalChannels: number
  frames: ExpressionFrame[]
}

export default function ExpressionCodeGeneratorComponent({
  totalChannels,
  frames
}: ExpressionCodeGeneratorProps) {
  const [apiType, setApiType] = useState<'modern' | 'native'>('modern')
  const [copied, setCopied] = useState(false)

  const codeGenerator = new ExpressionCodeGenerator(totalChannels, frames)

  const generatedCode = apiType === 'modern'
    ? codeGenerator.generateModernAPI()
    : codeGenerator.generateNativeAPI()

  const handleCopy = useCallback(async () => {
    try {
      await navigator.clipboard.writeText(generatedCode)
      setCopied(true)
      setTimeout(() => setCopied(false), 2000)
    } catch (error) {
      console.error('Failed to copy code:', error)
    }
  }, [generatedCode])

  const handleDownload = useCallback(() => {
    const blob = new Blob([generatedCode], { type: 'text/plain' })
    const url = URL.createObjectURL(blob)
    const a = document.createElement('a')
    a.href = url
    a.download = `expression_sequence_${totalChannels}ch_${apiType}.cpp`
    document.body.appendChild(a)
    a.click()
    document.body.removeChild(a)
    URL.revokeObjectURL(url)
  }, [generatedCode, totalChannels, apiType])

  const getCodeStats = () => {
    const lines = generatedCode.split('\n').length
    const sleepCalls = (generatedCode.match(/sleep\(/g) || []).length
    const setCalls = (generatedCode.match(/set\(/g) || []).length
    const loops = (generatedCode.match(/for\s*\(/g) || []).length

    return { lines, sleepCalls, setCalls, loops }
  }

  const stats = getCodeStats()

  if (frames.length === 0) {
    return (
      <div className="text-center py-8 text-muted-foreground">
        <Code className="h-8 w-8 mx-auto mb-2 opacity-50" />
        <p className="text-sm">No expression frames to generate code from</p>
        <p className="text-xs">Add some expression frames to see the generated C++ code</p>
      </div>
    )
  }

  return (
    <div className="space-y-4">
      {/* Code Generation Controls */}
      <div className="flex items-center justify-between">
        <div className="flex items-center gap-4">
          <div className="flex items-center gap-2">
            <Settings className="h-4 w-4" />
            <span className="text-sm font-medium">API Style:</span>
          </div>
          <div className="flex gap-2">
            <Button
              size="sm"
              variant={apiType === 'modern' ? 'default' : 'outline'}
              onClick={() => setApiType('modern')}
            >
              Modern API
            </Button>
            <Button
              size="sm"
              variant={apiType === 'native' ? 'default' : 'outline'}
              onClick={() => setApiType('native')}
            >
              Native API
            </Button>
          </div>
        </div>

        <div className="flex gap-2">
          <Button
            size="sm"
            variant="outline"
            onClick={handleCopy}
            className="flex items-center gap-1"
          >
            <Copy className="h-3 w-3" />
            {copied ? 'Copied!' : 'Copy'}
          </Button>
          <Button
            size="sm"
            variant="outline"
            onClick={handleDownload}
            className="flex items-center gap-1"
          >
            <Download className="h-3 w-3" />
            Download
          </Button>
        </div>
      </div>

      {/* API Type Info */}
      <div className="grid grid-cols-1 md:grid-cols-2 gap-4 text-xs">
        <div className="p-3 bg-blue-50 dark:bg-blue-950 rounded-lg">
          <h4 className="font-medium mb-2 text-blue-700 dark:text-blue-300">Modern API</h4>
          <div className="text-blue-600 dark:text-blue-400 space-y-1">
            <p>• Loop-based optimization for common patterns</p>
            <p>• Mathematical expressions converted to C++ loops</p>
            <p>• Better performance for large channel counts</p>
            <p>• Recommended for production use</p>
          </div>
        </div>

        <div className="p-3 bg-green-50 dark:bg-green-950 rounded-lg">
          <h4 className="font-medium mb-2 text-green-700 dark:text-green-300">Native API</h4>
          <div className="text-green-600 dark:text-green-400 space-y-1">
            <p>• Individual set() calls for each channel</p>
            <p>• Direct channel state translation</p>
            <p>• Easier debugging and visualization</p>
            <p>• Good for testing and development</p>
          </div>
        </div>
      </div>

      {/* Code Stats */}
      <div className="flex flex-wrap gap-2">
        <Badge variant="secondary" className="text-xs">
          {stats.lines} lines
        </Badge>
        <Badge variant="outline" className="text-xs">
          {frames.length} frames
        </Badge>
        <Badge variant="outline" className="text-xs">
          {stats.sleepCalls} sleep calls
        </Badge>
        <Badge variant="outline" className="text-xs">
          {stats.setCalls} set calls
        </Badge>
        <Badge variant="outline" className="text-xs">
          {stats.loops} loops
        </Badge>
        <Badge variant="secondary" className="text-xs">
          {totalChannels} channels
        </Badge>
      </div>

      {/* Generated Code */}
      <div className="space-y-2">
        <div className="flex items-center gap-2">
          <Code className="h-4 w-4" />
          <span className="text-sm font-medium">Generated C++ Code:</span>
        </div>
        <Textarea
          value={generatedCode}
          readOnly
          className="font-mono text-sm bg-slate-950 text-green-400 border-0 min-h-[400px] max-h-[500px] overflow-y-auto"
          style={{
            backgroundColor: '#0f172a',
            color: '#4ade80',
            fontFamily: 'JetBrains Mono, Consolas, Monaco, "Courier New", monospace'
          }}
        />
      </div>

      {/* Usage Instructions */}
      <div className="p-3 bg-muted rounded-lg text-xs space-y-2">
        <h4 className="font-medium">Usage Instructions:</h4>
        <ol className="space-y-1 list-decimal list-inside text-muted-foreground">
          <li>Copy the generated code above</li>
          <li>Open your BaseChannelSequence.cpp file</li>
          <li>Replace or add the function to your sequence implementations</li>
          <li>Call the function from your mode selection logic</li>
          <li>Compile and upload to your AutoLight V3 device</li>
        </ol>
        <div className="mt-2 p-2 bg-amber-50 dark:bg-amber-950 rounded text-amber-700 dark:text-amber-300">
          <strong>Note:</strong> Generated code is fully compatible with BaseChannelSequence.cpp and uses
          dynamic <code>config_data_ptr_{'->'} header.pin_size_</code> for channel count compatibility.
        </div>
      </div>
    </div>
  )
}