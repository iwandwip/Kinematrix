"use client"

import { useState } from "react"
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card"
import { Button } from "@/components/ui/button"
import { Badge } from "@/components/ui/badge"
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue
} from "@/components/ui/select"
import {
  Tooltip,
  TooltipContent,
  TooltipProvider,
  TooltipTrigger,
} from "@/components/ui/tooltip"
import {
  Download,
  Plus,
  Info,
  Zap,
  Code2,
  Layers,
  Sparkles
} from "lucide-react"
import { presetTemplates } from "@/lib/simulation/presetTemplates"
import { complexPresets } from "@/lib/simulation/complexPresets"
import { adaptTemplateForChannelCount } from "@/lib/simulation/templateUtils"

const allPresets = [...presetTemplates, ...complexPresets]

interface PresetSelectorProps {
  onInsertTemplate: (code: string) => void
  onAppendTemplate: (code: string) => void
  totalChannels: number
}

export default function PresetSelector({
  onInsertTemplate,
  onAppendTemplate,
  totalChannels
}: PresetSelectorProps) {
  const [selectedPresetId, setSelectedPresetId] = useState<string>("")

  const selectedPreset = allPresets.find(preset => preset.id === selectedPresetId)

  const handleInsertTemplate = () => {
    if (selectedPreset) {
      const adaptedCode = adaptTemplateForChannelCount(selectedPreset.cppCode, totalChannels)
      onInsertTemplate(adaptedCode)
      setSelectedPresetId("")
    }
  }

  const handleAppendTemplate = () => {
    if (selectedPreset) {
      const adaptedCode = adaptTemplateForChannelCount(selectedPreset.cppCode, totalChannels)
      onAppendTemplate(adaptedCode)
      setSelectedPresetId("")
    }
  }

  const getComplexityColor = (complexity: string) => {
    switch (complexity) {
      case 'Basic': return 'bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-300'
      case 'Intermediate': return 'bg-blue-100 text-blue-800 dark:bg-blue-900 dark:text-blue-300'
      case 'Advanced': return 'bg-orange-100 text-orange-800 dark:bg-orange-900 dark:text-orange-300'
      case 'Expert': return 'bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-300'
      default: return 'bg-gray-100 text-gray-800 dark:bg-gray-900 dark:text-gray-300'
    }
  }

  const getComplexityIcon = (complexity: string) => {
    switch (complexity) {
      case 'Basic': return <Zap className="h-3 w-3" />
      case 'Intermediate': return <Code2 className="h-3 w-3" />
      case 'Advanced': return <Layers className="h-3 w-3" />
      case 'Expert': return <Sparkles className="h-3 w-3" />
      default: return <Info className="h-3 w-3" />
    }
  }

  return (
    <Card className="mb-4">
      <CardHeader className="pb-3">
        <CardTitle className="flex items-center gap-2 text-sm">
          <Code2 className="h-4 w-4" />
          Preset Templates (Mode 2-14)
        </CardTitle>
      </CardHeader>
      <CardContent className="space-y-3">
        <div className="flex gap-2">
          <Select value={selectedPresetId} onValueChange={setSelectedPresetId}>
            <SelectTrigger className="flex-1">
              <SelectValue placeholder="Select BaseChannelSequence template..." />
            </SelectTrigger>
            <SelectContent>
              <div className="p-2">
                <div className="text-xs font-medium text-muted-foreground mb-2">
                  Basic & Intermediate Patterns
                </div>
                {presetTemplates.map((preset) => (
                  <SelectItem key={preset.id} value={preset.id} className="py-2">
                    <div className="flex items-center gap-2">
                      <span className="font-mono text-xs bg-muted px-1 rounded">
                        {preset.mode}
                      </span>
                      <span>{preset.name}</span>
                      <Badge
                        variant="outline"
                        className={`text-xs ${getComplexityColor(preset.complexity)}`}
                      >
                        {getComplexityIcon(preset.complexity)}
                        {preset.complexity}
                      </Badge>
                    </div>
                  </SelectItem>
                ))}
                <div className="text-xs font-medium text-muted-foreground mb-2 mt-3">
                  Advanced & Expert Patterns
                </div>
                {complexPresets.map((preset) => (
                  <SelectItem key={preset.id} value={preset.id} className="py-2">
                    <div className="flex items-center gap-2">
                      <span className="font-mono text-xs bg-muted px-1 rounded">
                        {preset.mode}
                      </span>
                      <span>{preset.name}</span>
                      <Badge
                        variant="outline"
                        className={`text-xs ${getComplexityColor(preset.complexity)}`}
                      >
                        {getComplexityIcon(preset.complexity)}
                        {preset.complexity}
                      </Badge>
                    </div>
                  </SelectItem>
                ))}
              </div>
            </SelectContent>
          </Select>

          <TooltipProvider>
            <Tooltip>
              <TooltipTrigger asChild>
                <Button
                  onClick={handleInsertTemplate}
                  disabled={!selectedPreset}
                  size="sm"
                  className="flex items-center gap-1"
                >
                  <Download className="h-3 w-3" />
                  Insert
                </Button>
              </TooltipTrigger>
              <TooltipContent>
                <p>Replace editor content with template</p>
              </TooltipContent>
            </Tooltip>
          </TooltipProvider>

          <TooltipProvider>
            <Tooltip>
              <TooltipTrigger asChild>
                <Button
                  onClick={handleAppendTemplate}
                  disabled={!selectedPreset}
                  size="sm"
                  variant="outline"
                  className="flex items-center gap-1"
                >
                  <Plus className="h-3 w-3" />
                  Append
                </Button>
              </TooltipTrigger>
              <TooltipContent>
                <p>Add template to existing code</p>
              </TooltipContent>
            </Tooltip>
          </TooltipProvider>
        </div>

        {selectedPreset && (
          <div className="p-3 bg-muted rounded-lg space-y-2">
            <div className="flex items-center justify-between">
              <div className="flex items-center gap-2">
                <Badge variant="outline" className="font-mono">
                  Mode {selectedPreset.mode}
                </Badge>
                <Badge
                  variant="outline"
                  className={getComplexityColor(selectedPreset.complexity)}
                >
                  {getComplexityIcon(selectedPreset.complexity)}
                  {selectedPreset.complexity}
                </Badge>
                <Badge variant="secondary" className="text-xs">
                  ~{selectedPreset.estimatedFrames} frames
                </Badge>
              </div>
            </div>
            <p className="text-sm text-muted-foreground">
              {selectedPreset.description}
            </p>
            <div className="text-xs text-muted-foreground">
              Adapted for {totalChannels} channels • From BaseChannelSequence.cpp
            </div>
          </div>
        )}

        <div className="flex flex-wrap gap-1">
          <Badge variant="outline" className="text-xs">
            13 Templates Available
          </Badge>
          <Badge variant="secondary" className="text-xs">
            Auto-adapted for {totalChannels} channels
          </Badge>
          <Badge variant="outline" className="text-xs">
            BaseChannelSequence.cpp Compatible
          </Badge>
        </div>
      </CardContent>
    </Card>
  )
}