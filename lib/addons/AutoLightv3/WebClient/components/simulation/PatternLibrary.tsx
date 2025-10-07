"use client"

import { useState } from "react"
import { Button } from "@/components/ui/button"
import { Input } from "@/components/ui/input"
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card"
import { useSimulation, LEDPattern, LEDFrame } from "@/lib/contexts/SimulationContext"
import { Badge } from "@/components/ui/badge"
import { Dialog, DialogContent, DialogDescription, DialogHeader, DialogTitle, DialogTrigger } from "@/components/ui/dialog"
import { Trash2, Search, Eye } from "lucide-react"

export default function PatternLibrary() {
  const { simulationData, loadPattern, deletePattern } = useSimulation()
  const [searchTerm, setSearchTerm] = useState("")
  const [selectedPatternId, setSelectedPatternId] = useState<string | null>(null)

  const filteredPatterns = simulationData.savedPatterns.filter(pattern =>
    pattern.name.toLowerCase().includes(searchTerm.toLowerCase()) ||
    pattern.description.toLowerCase().includes(searchTerm.toLowerCase())
  )

  const selectedPattern = simulationData.savedPatterns.find(p => p.id === selectedPatternId)

  const handleLoadPattern = (patternId: string) => {
    loadPattern(patternId)
    setSelectedPatternId(null)
  }

  const handleDeletePattern = (patternId: string) => {
    deletePattern(patternId)
    if (selectedPatternId === patternId) {
      setSelectedPatternId(null)
    }
  }

  const getPatternStats = (pattern: LEDPattern) => {
    const totalFrames = pattern.frames.length
    const avgChannelsOn = Math.round(
      pattern.frames.reduce((total: number, frame: LEDFrame) =>
        total + frame.channels.filter(Boolean).length, 0
      ) / totalFrames
    )
    const duration = Math.round(
      pattern.frames.reduce((total: number, frame: LEDFrame) => total + frame.delay, 0) *
      pattern.settings.repeatCount / 1000 * 100
    ) / 100

    return { totalFrames, avgChannelsOn, duration }
  }

  const renderPatternPreview = (pattern: LEDPattern) => {
    if (!pattern.frames.length) return null

    return (
      <div className="space-y-2">
        {pattern.frames.slice(0, 3).map((frame: LEDFrame, index: number) => (
          <div key={frame.id} className="flex items-center gap-2">
            <span className="text-xs text-muted-foreground w-12">F{index + 1}:</span>
            <div className="grid grid-cols-7 gap-0.5 flex-1">
              {frame.channels.map((isOn: boolean, channelIndex: number) => (
                <div
                  key={channelIndex}
                  className={`w-2 h-2 rounded-sm ${
                    isOn ? 'bg-yellow-400' : 'bg-gray-300 dark:bg-gray-600'
                  }`}
                />
              ))}
            </div>
            <span className="text-xs text-muted-foreground w-12">{frame.delay}ms</span>
          </div>
        ))}
        {pattern.frames.length > 3 && (
          <div className="text-xs text-muted-foreground text-center">
            +{pattern.frames.length - 3} more frames
          </div>
        )}
      </div>
    )
  }

  return (
    <div className="space-y-4">
      {/* Search */}
      <div className="relative">
        <Search className="absolute left-2 top-2.5 h-4 w-4 text-muted-foreground" />
        <Input
          placeholder="Search patterns..."
          value={searchTerm}
          onChange={(e) => setSearchTerm(e.target.value)}
          className="pl-8"
        />
      </div>

      {/* Pattern List */}
      <div className="grid gap-3">
        {filteredPatterns.length === 0 ? (
          <Card>
            <CardContent className="p-6 text-center text-muted-foreground">
              {searchTerm ? "No patterns found matching your search." : "No saved patterns yet."}
            </CardContent>
          </Card>
        ) : (
          filteredPatterns.map((pattern) => {
            const stats = getPatternStats(pattern)
            const isCurrentPattern = pattern.id === simulationData.currentPattern.id

            return (
              <Card key={pattern.id} className={isCurrentPattern ? "ring-2 ring-blue-500" : ""}>
                <CardHeader className="pb-2">
                  <div className="flex items-center justify-between">
                    <div className="flex-1">
                      <CardTitle className="text-sm flex items-center gap-2">
                        {pattern.name}
                        {isCurrentPattern && (
                          <Badge variant="secondary" className="text-xs">Current</Badge>
                        )}
                      </CardTitle>
                      {pattern.description && (
                        <CardDescription className="text-xs mt-1">
                          {pattern.description}
                        </CardDescription>
                      )}
                    </div>
                    <div className="flex gap-1">
                      <Dialog>
                        <DialogTrigger asChild>
                          <Button
                            size="sm"
                            variant="ghost"
                            className="h-7 w-7 p-0"
                            onClick={() => setSelectedPatternId(pattern.id)}
                          >
                            <Eye className="h-3 w-3" />
                          </Button>
                        </DialogTrigger>
                        <DialogContent className="max-w-2xl">
                          <DialogHeader>
                            <DialogTitle>{selectedPattern?.name}</DialogTitle>
                            <DialogDescription>
                              {selectedPattern?.description || "No description"}
                            </DialogDescription>
                          </DialogHeader>
                          {selectedPattern && (
                            <div className="space-y-4">
                              <div className="grid grid-cols-3 gap-4 text-sm">
                                <div>
                                  <span className="text-muted-foreground">Frames:</span>
                                  <div className="font-medium">{selectedPattern.frames.length}</div>
                                </div>
                                <div>
                                  <span className="text-muted-foreground">Channels:</span>
                                  <div className="font-medium">{selectedPattern.settings.totalChannels}</div>
                                </div>
                                <div>
                                  <span className="text-muted-foreground">Duration:</span>
                                  <div className="font-medium">~{getPatternStats(selectedPattern).duration}s</div>
                                </div>
                              </div>
                              <div>
                                <h4 className="text-sm font-medium mb-2">Pattern Preview</h4>
                                {renderPatternPreview(selectedPattern)}
                              </div>
                            </div>
                          )}
                        </DialogContent>
                      </Dialog>
                      <Button
                        size="sm"
                        variant="ghost"
                        className="h-7 w-7 p-0 text-red-500 hover:text-red-700"
                        onClick={() => handleDeletePattern(pattern.id)}
                      >
                        <Trash2 className="h-3 w-3" />
                      </Button>
                    </div>
                  </div>
                </CardHeader>
                <CardContent className="pt-0">
                  <div className="flex items-center justify-between text-xs text-muted-foreground mb-3">
                    <span>{stats.totalFrames} frames</span>
                    <span>~{stats.avgChannelsOn} avg ON</span>
                    <span>{stats.duration}s</span>
                  </div>

                  <div className="mb-3">
                    {renderPatternPreview(pattern)}
                  </div>

                  <Button
                    size="sm"
                    onClick={() => handleLoadPattern(pattern.id)}
                    disabled={isCurrentPattern}
                    className="w-full"
                  >
                    {isCurrentPattern ? "Currently Loaded" : "Load Pattern"}
                  </Button>
                </CardContent>
              </Card>
            )
          })
        )}
      </div>

      {/* Library Stats */}
      {simulationData.savedPatterns.length > 0 && (
        <Card>
          <CardHeader className="pb-2">
            <CardTitle className="text-sm">Library Statistics</CardTitle>
          </CardHeader>
          <CardContent className="text-xs text-muted-foreground space-y-1">
            <div className="flex justify-between">
              <span>Total Patterns:</span>
              <span>{simulationData.savedPatterns.length}</span>
            </div>
            <div className="flex justify-between">
              <span>Total Frames:</span>
              <span>
                {simulationData.savedPatterns.reduce((total, pattern) =>
                  total + pattern.frames.length, 0
                )}
              </span>
            </div>
          </CardContent>
        </Card>
      )}
    </div>
  )
}