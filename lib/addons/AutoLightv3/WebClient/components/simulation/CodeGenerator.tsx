"use client"

import { useState } from "react"
import { Button } from "@/components/ui/button"
import { Input } from "@/components/ui/input"
import { Label } from "@/components/ui/label"
import { Textarea } from "@/components/ui/textarea"
import { Checkbox } from "@/components/ui/checkbox"
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card"
import { useSimulation } from "@/lib/contexts/SimulationContext"
import {
  generateCppCode,
  generateNativeCppCode,
  generateFunctionDeclaration,
  generateModeMapping,
  getNextAvailableSequenceNumber,
  type CodeGenerationOptions
} from "@/lib/utils/codeGenerator"
import { Copy, Download, RefreshCw, Code, FileText } from "lucide-react"
import { RadioGroup, RadioGroupItem } from "@/components/ui/radio-group"

export default function CodeGenerator() {
  const { simulationData } = useSimulation()
  const [options, setOptions] = useState<CodeGenerationOptions>({
    functionNumber: getNextAvailableSequenceNumber(),
    functionName: '',
    includeComments: true,
    optimizeCode: true,
    codeStyle: 'modern'
  })
  const [copiedStates, setCopiedStates] = useState({
    function: false,
    declaration: false,
    mapping: false
  })

  const pattern = simulationData.currentPattern
  const functionName = options.functionName || pattern.name.replace(/[^a-zA-Z0-9]/g, '') || 'CustomPattern'
  const sanitizedName = functionName.replace(/[^a-zA-Z0-9]/g, '')

  const generatedCode = options.codeStyle === 'native'
    ? generateNativeCppCode(pattern, {
        ...options,
        functionName: sanitizedName
      })
    : generateCppCode(pattern, {
        ...options,
        functionName: sanitizedName
      })

  const functionDeclaration = generateFunctionDeclaration(
    options.functionNumber || 16,
    sanitizedName
  )

  const modeMapping = generateModeMapping(
    options.functionNumber || 16,
    sanitizedName,
    options.functionNumber || 16
  )

  const handleCopy = async (text: string, type: 'function' | 'declaration' | 'mapping') => {
    try {
      await navigator.clipboard.writeText(text)
      setCopiedStates(prev => ({ ...prev, [type]: true }))
      setTimeout(() => {
        setCopiedStates(prev => ({ ...prev, [type]: false }))
      }, 2000)
    } catch (err) {
      console.error('Failed to copy text:', err)
    }
  }

  const handleDownload = () => {
    const content = `${generatedCode}\n\n// Add this declaration to BaseChannel.h:\n// ${functionDeclaration}\n\n// Add this line to mode mapping in BaseChannel.cpp:\n// ${modeMapping}`

    const blob = new Blob([content], { type: 'text/plain' })
    const url = URL.createObjectURL(blob)
    const a = document.createElement('a')
    a.href = url
    a.download = `taskSequence${options.functionNumber}${sanitizedName}.cpp`
    document.body.appendChild(a)
    a.click()
    document.body.removeChild(a)
    URL.revokeObjectURL(url)
  }

  const handleRefresh = () => {
    setOptions(prev => ({
      ...prev,
      functionNumber: getNextAvailableSequenceNumber(),
      functionName: pattern.name.replace(/[^a-zA-Z0-9]/g, '') || 'CustomPattern'
    }))
  }

  const updateOption = <K extends keyof CodeGenerationOptions>(
    key: K,
    value: CodeGenerationOptions[K]
  ) => {
    setOptions(prev => ({ ...prev, [key]: value }))
  }

  return (
    <div className="space-y-4">
      {/* Generation Options */}
      <Card>
        <CardHeader className="pb-3">
          <CardTitle className="text-sm flex items-center gap-2">
            <Code className="h-4 w-4" />
            Code Generation Options
          </CardTitle>
        </CardHeader>
        <CardContent className="space-y-3">
          <div className="grid grid-cols-2 gap-3">
            <div className="space-y-1">
              <Label htmlFor="function-number" className="text-xs">Sequence Number</Label>
              <Input
                id="function-number"
                type="number"
                value={options.functionNumber || ''}
                onChange={(e) => updateOption('functionNumber', parseInt(e.target.value) || 16)}
                min={16}
                max={999}
                className="h-8"
              />
            </div>
            <div className="space-y-1">
              <Label htmlFor="function-name" className="text-xs">Function Name</Label>
              <Input
                id="function-name"
                value={options.functionName || ''}
                onChange={(e) => updateOption('functionName', e.target.value)}
                placeholder={pattern.name}
                className="h-8"
              />
            </div>
          </div>

          <div className="flex items-center space-x-4">
            <div className="flex items-center space-x-2">
              <Checkbox
                id="include-comments"
                checked={options.includeComments}
                onCheckedChange={(checked) => updateOption('includeComments', checked as boolean)}
              />
              <Label htmlFor="include-comments" className="text-xs">Include Comments</Label>
            </div>
            <div className="flex items-center space-x-2">
              <Checkbox
                id="optimize-code"
                checked={options.optimizeCode}
                onCheckedChange={(checked) => updateOption('optimizeCode', checked as boolean)}
              />
              <Label htmlFor="optimize-code" className="text-xs">Optimize Code</Label>
            </div>
          </div>

          <div className="space-y-2">
            <Label className="text-xs font-medium">Code Style</Label>
            <RadioGroup
              value={options.codeStyle || 'modern'}
              onValueChange={(value) => updateOption('codeStyle', value as 'modern' | 'native')}
              className="flex flex-col space-y-1"
            >
              <div className="flex items-center space-x-2">
                <RadioGroupItem value="modern" id="modern" />
                <Label htmlFor="modern" className="text-xs cursor-pointer">
                  Direct (BaseChannel API)
                </Label>
              </div>
              <div className="flex items-center space-x-2">
                <RadioGroupItem value="native" id="native" />
                <Label htmlFor="native" className="text-xs cursor-pointer">
                  Native/Legacy (Direct API)
                </Label>
              </div>
            </RadioGroup>
            <div className="text-xs text-muted-foreground">
              {options.codeStyle === 'modern'
                ? 'Uses BaseChannel direct methods with set(), sleep(), off() functions'
                : 'Uses direct BaseChannel methods with set(), sleep(), off() functions'
              }
            </div>
          </div>

          <div className="flex gap-2">
            <Button
              onClick={handleRefresh}
              size="sm"
              variant="outline"
              className="flex items-center gap-1"
            >
              <RefreshCw className="h-3 w-3" />
              Refresh
            </Button>
            <Button
              onClick={handleDownload}
              size="sm"
              variant="outline"
              className="flex items-center gap-1"
            >
              <Download className="h-3 w-3" />
              Download
            </Button>
          </div>
        </CardContent>
      </Card>

      {/* Generated Function */}
      <Card>
        <CardHeader className="pb-3">
          <div className="flex items-center justify-between">
            <CardTitle className="text-sm">
              Generated Function ({options.codeStyle === 'modern' ? 'Direct API' : 'Native API'})
            </CardTitle>
            <Button
              onClick={() => handleCopy(generatedCode, 'function')}
              size="sm"
              variant="outline"
              className="flex items-center gap-1"
            >
              <Copy className="h-3 w-3" />
              {copiedStates.function ? 'Copied!' : 'Copy'}
            </Button>
          </div>
        </CardHeader>
        <CardContent>
          <div className="relative border rounded-md bg-muted/30">
            <pre className="font-mono text-xs p-3 max-h-[400px] min-h-[200px] overflow-y-auto whitespace-pre-wrap">
              {generatedCode}
            </pre>
          </div>
        </CardContent>
      </Card>

      {/* Integration Instructions */}
      <Card>
        <CardHeader className="pb-3">
          <CardTitle className="text-sm flex items-center gap-2">
            <FileText className="h-4 w-4" />
            Integration Instructions
          </CardTitle>
        </CardHeader>
        <CardContent className="space-y-4">
          <div className="space-y-2">
            <div className="flex items-center justify-between">
              <Label className="text-xs font-medium">1. Add declaration to BaseChannel.h:</Label>
              <Button
                onClick={() => handleCopy(functionDeclaration, 'declaration')}
                size="sm"
                variant="outline"
                className="h-6 text-xs"
              >
                <Copy className="h-2 w-2 mr-1" />
                {copiedStates.declaration ? 'Copied!' : 'Copy'}
              </Button>
            </div>
            <Textarea
              value={functionDeclaration}
              readOnly
              className="font-mono text-xs h-8 resize-none"
            />
          </div>

          <div className="space-y-2">
            <div className="flex items-center justify-between">
              <Label className="text-xs font-medium">2. Add mode mapping to BaseChannel.cpp:</Label>
              <Button
                onClick={() => handleCopy(modeMapping, 'mapping')}
                size="sm"
                variant="outline"
                className="h-6 text-xs"
              >
                <Copy className="h-2 w-2 mr-1" />
                {copiedStates.mapping ? 'Copied!' : 'Copy'}
              </Button>
            </div>
            <Textarea
              value={modeMapping}
              readOnly
              className="font-mono text-xs h-8 resize-none"
            />
          </div>

          <div className="text-xs text-muted-foreground space-y-1">
            <p><strong>Manual Integration Steps:</strong></p>
            <ol className="list-decimal list-inside space-y-1 ml-2">
              <li>Copy the generated function and paste it into <code>BaseChannelSequence.cpp</code></li>
              <li>Add the function declaration to <code>BaseChannel.h</code> in the private methods section</li>
              <li>Add the mode mapping line to the <code>initializeSequence()</code> method in <code>BaseChannel.cpp</code></li>
              <li>Compile and test your pattern</li>
            </ol>
          </div>
        </CardContent>
      </Card>

      {/* Pattern Summary */}
      <Card>
        <CardHeader className="pb-3">
          <CardTitle className="text-sm">Pattern Summary</CardTitle>
        </CardHeader>
        <CardContent className="text-xs text-muted-foreground space-y-1">
          <div className="flex justify-between">
            <span>Pattern Name:</span>
            <span>{pattern.name}</span>
          </div>
          <div className="flex justify-between">
            <span>Generated Function:</span>
            <span>taskSequence{options.functionNumber}{sanitizedName}</span>
          </div>
          <div className="flex justify-between">
            <span>Total Frames:</span>
            <span>{pattern.frames.length}</span>
          </div>
          <div className="flex justify-between">
            <span>Total Channels:</span>
            <span>{pattern.settings.totalChannels}</span>
          </div>
          <div className="flex justify-between">
            <span>Repeat Count:</span>
            <span>{pattern.settings.repeatCount}x</span>
          </div>
          <div className="flex justify-between">
            <span>Direction:</span>
            <span>{pattern.settings.direction}</span>
          </div>
          <div className="flex justify-between">
            <span>Estimated Duration:</span>
            <span>
              ~{Math.round(
                pattern.frames.reduce((total, frame) => total + frame.delay, 0) *
                pattern.settings.repeatCount / 1000 * 100
              ) / 100}s
            </span>
          </div>
        </CardContent>
      </Card>
    </div>
  )
}