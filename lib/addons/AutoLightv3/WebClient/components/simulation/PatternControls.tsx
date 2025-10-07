"use client"

import { useState } from "react"
import { Button } from "@/components/ui/button"
import { Input } from "@/components/ui/input"
import { Label } from "@/components/ui/label"
import { Textarea } from "@/components/ui/textarea"
import { Slider } from "@/components/ui/slider"
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select"
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card"
import { useSimulation } from "@/lib/contexts/SimulationContext"
import { Save, FilePlus, FolderOpen } from "lucide-react"
import PatternLibrary from "./PatternLibrary"
import { Dialog, DialogContent, DialogHeader, DialogTitle, DialogTrigger } from "@/components/ui/dialog"

export default function PatternControls() {
  const {
    simulationData,
    setPatternName,
    setPatternDescription,
    setDefaultDelay,
    setRepeatCount,
    setDirection,
    savePattern,
    createNewPattern
  } = useSimulation()

  const [localName, setLocalName] = useState(simulationData.currentPattern.name)
  const [localDescription, setLocalDescription] = useState(simulationData.currentPattern.description)

  const handleNameChange = (value: string) => {
    setLocalName(value)
    setPatternName(value)
  }

  const handleDescriptionChange = (value: string) => {
    setLocalDescription(value)
    setPatternDescription(value)
  }

  const handleDelayChange = (values: number[]) => {
    setDefaultDelay(values[0])
  }

  const handleRepeatChange = (values: number[]) => {
    setRepeatCount(values[0])
  }

  const handleDirectionChange = (value: string) => {
    setDirection(value as 'forward' | 'reverse' | 'alternate')
  }

  const handleSave = () => {
    savePattern()
  }

  const handleNew = () => {
    createNewPattern()
    setLocalName("New Pattern")
    setLocalDescription("")
  }

  return (
    <div className="grid gap-4 md:grid-cols-2 lg:grid-cols-4 mb-6">
      {/* Pattern Info */}
      <Card>
        <CardHeader className="pb-3">
          <CardTitle className="text-sm">Pattern Information</CardTitle>
        </CardHeader>
        <CardContent className="space-y-3">
          <div className="space-y-1">
            <Label htmlFor="pattern-name" className="text-xs">Pattern Name</Label>
            <Input
              id="pattern-name"
              value={localName}
              onChange={(e) => handleNameChange(e.target.value)}
              placeholder="Enter pattern name"
              className="h-8"
            />
          </div>

          <div className="space-y-1">
            <Label htmlFor="pattern-description" className="text-xs">Description</Label>
            <Textarea
              id="pattern-description"
              value={localDescription}
              onChange={(e) => handleDescriptionChange(e.target.value)}
              placeholder="Pattern description (optional)"
              className="min-h-[60px] text-xs"
            />
          </div>
        </CardContent>
      </Card>

      {/* Pattern Settings */}
      <Card>
        <CardHeader className="pb-3">
          <CardTitle className="text-sm">Pattern Settings</CardTitle>
        </CardHeader>
        <CardContent className="space-y-4">
          <div className="space-y-2">
            <Label className="text-xs">Default Delay: {simulationData.currentPattern.settings.defaultDelay}ms</Label>
            <Slider
              value={[simulationData.currentPattern.settings.defaultDelay]}
              onValueChange={handleDelayChange}
              min={10}
              max={500}
              step={10}
              className="w-full"
            />
            <div className="flex justify-between text-xs text-muted-foreground">
              <span>10ms</span>
              <span>500ms</span>
            </div>
          </div>

          <div className="space-y-2">
            <Label className="text-xs">Repeat Count: {simulationData.currentPattern.settings.repeatCount}x</Label>
            <Slider
              value={[simulationData.currentPattern.settings.repeatCount]}
              onValueChange={handleRepeatChange}
              min={1}
              max={10}
              step={1}
              className="w-full"
            />
            <div className="flex justify-between text-xs text-muted-foreground">
              <span>1x</span>
              <span>10x</span>
            </div>
          </div>

          <div className="space-y-1">
            <Label className="text-xs">Direction</Label>
            <Select value={simulationData.currentPattern.settings.direction} onValueChange={handleDirectionChange}>
              <SelectTrigger className="h-8">
                <SelectValue />
              </SelectTrigger>
              <SelectContent>
                <SelectItem value="forward">Forward</SelectItem>
                <SelectItem value="reverse">Reverse</SelectItem>
                <SelectItem value="alternate">Alternate</SelectItem>
              </SelectContent>
            </Select>
          </div>
        </CardContent>
      </Card>

      {/* Pattern Actions */}
      <Card>
        <CardHeader className="pb-3">
          <CardTitle className="text-sm">Actions</CardTitle>
        </CardHeader>
        <CardContent className="space-y-2">
          <Button
            onClick={handleSave}
            size="sm"
            className="w-full flex items-center gap-1"
          >
            <Save className="h-3 w-3" />
            Save Pattern
          </Button>

          <Button
            onClick={handleNew}
            variant="outline"
            size="sm"
            className="w-full flex items-center gap-1"
          >
            <FilePlus className="h-3 w-3" />
            New Pattern
          </Button>

          <Dialog>
            <DialogTrigger asChild>
              <Button
                variant="outline"
                size="sm"
                className="w-full flex items-center gap-1"
              >
                <FolderOpen className="h-3 w-3" />
                Pattern Library
              </Button>
            </DialogTrigger>
            <DialogContent className="max-w-md max-h-[80vh] overflow-y-auto">
              <DialogHeader>
                <DialogTitle>Pattern Library</DialogTitle>
              </DialogHeader>
              <PatternLibrary />
            </DialogContent>
          </Dialog>
        </CardContent>
      </Card>

      {/* Pattern Stats */}
      <Card>
        <CardHeader className="pb-3">
          <CardTitle className="text-sm">Pattern Statistics</CardTitle>
        </CardHeader>
        <CardContent className="text-xs text-muted-foreground space-y-1">
          <div className="flex justify-between">
            <span>Total Frames:</span>
            <span>{simulationData.currentPattern.frames.length}</span>
          </div>
          <div className="flex justify-between">
            <span>Total Channels:</span>
            <span>{simulationData.currentPattern.settings.totalChannels}</span>
          </div>
          <div className="flex justify-between">
            <span>Pattern Duration:</span>
            <span>
              ~{Math.round(
                simulationData.currentPattern.frames.reduce((total, frame) => total + frame.delay, 0) *
                simulationData.currentPattern.settings.repeatCount / 1000 * 100
              ) / 100}s
            </span>
          </div>
        </CardContent>
      </Card>
    </div>
  )
}