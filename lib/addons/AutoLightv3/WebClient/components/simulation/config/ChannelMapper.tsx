"use client"

import { useState, useCallback } from "react"
import { Button } from "@/components/ui/button"
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card"
import { Badge } from "@/components/ui/badge"
import { Switch } from "@/components/ui/switch"
import { Tooltip, TooltipContent, TooltipProvider, TooltipTrigger } from "@/components/ui/tooltip"
import { ArrowRight, RotateCcw, Shuffle, ArrowLeft, Zap } from "lucide-react"
import { GridConfig } from "@/lib/contexts/SimulationContext"

interface ChannelMapperProps {
  gridConfig: GridConfig
  onMappingComplete: (mapping: Map<string, number | null>) => void
  onBack: () => void
}

export default function ChannelMapper({ gridConfig, onMappingComplete, onBack }: ChannelMapperProps) {
  const [channelMapping, setChannelMapping] = useState<Map<string, number | null>>(new Map())
  const [selectedChannel, setSelectedChannel] = useState<number | null>(null)
  const [autoIncrement, setAutoIncrement] = useState<boolean>(true)
  const [nextChannel, setNextChannel] = useState<number>(0)

  const { rows, cols, totalChannels, layoutType } = gridConfig

  const getPositionKey = (row: number, col: number) => `${row},${col}`

  const isPositionActive = useCallback((row: number, col: number) => {
    if (layoutType === 'matrix') return true
    if (layoutType === 'custom') return true

    if (layoutType === 'line') {
      const mid = Math.floor(cols / 2)
      return col === mid && row < totalChannels
    }

    if (layoutType === 'circle') {
      const centerRow = Math.floor(rows / 2)
      const centerCol = Math.floor(cols / 2)
      const maxRadius = Math.min(centerRow, centerCol)
      const distance = Math.sqrt(Math.pow(row - centerRow, 2) + Math.pow(col - centerCol, 2))
      return distance <= maxRadius && channelMapping.size < totalChannels
    }

    return false
  }, [layoutType, cols, rows, totalChannels, channelMapping.size])

  const getAssignedChannel = (row: number, col: number) => {
    const key = getPositionKey(row, col)
    return channelMapping.has(key) ? channelMapping.get(key) : null
  }

  const assignChannel = useCallback((row: number, col: number, channel: number | null) => {
    if (!isPositionActive(row, col)) return

    const key = getPositionKey(row, col)
    setChannelMapping(prev => {
      const newMapping = new Map(prev)

      if (channel === null) {
        newMapping.delete(key)
      } else {
        for (const [existingKey, existingChannel] of newMapping.entries()) {
          if (existingChannel === channel) {
            newMapping.delete(existingKey)
          }
        }
        newMapping.set(key, channel)
      }

      return newMapping
    })
  }, [isPositionActive])

  const handleCellClick = (row: number, col: number) => {
    if (!isPositionActive(row, col)) return

    const currentChannel = getAssignedChannel(row, col)

    if (currentChannel !== null) {
      assignChannel(row, col, null)
      return
    }

    if (autoIncrement) {
      assignChannel(row, col, nextChannel)
      if (nextChannel < totalChannels - 1) {
        setNextChannel(nextChannel + 1)
      }
    } else if (selectedChannel !== null) {
      assignChannel(row, col, selectedChannel)
      setSelectedChannel(null)
    }
  }

  const handleChannelSelect = (channel: number) => {
    if (selectedChannel === channel) {
      setSelectedChannel(null)
    } else {
      setSelectedChannel(channel)
    }
  }

  const getUsedChannels = () => {
    const used = new Set<number>()
    for (const channel of channelMapping.values()) {
      if (channel !== null) {
        used.add(channel)
      }
    }
    return used
  }

  const isChannelUsed = (channel: number) => getUsedChannels().has(channel)

  const autoAssign = () => {
    const newMapping = new Map<string, number | null>()
    let channelIndex = 0

    for (let row = 0; row < rows; row++) {
      for (let col = 0; col < cols; col++) {
        if (isPositionActive(row, col) && channelIndex < totalChannels) {
          const key = getPositionKey(row, col)
          newMapping.set(key, channelIndex)
          channelIndex++
        }
      }
    }

    setChannelMapping(newMapping)
    setNextChannel(channelIndex >= totalChannels ? totalChannels - 1 : channelIndex)
  }

  const clearMapping = () => {
    setChannelMapping(new Map())
    setSelectedChannel(null)
    setNextChannel(0)
  }

  const handleComplete = () => {
    onMappingComplete(channelMapping)
  }

  const getCellStyle = (row: number, col: number) => {
    const baseStyle = "w-10 h-10 rounded border-2 text-xs font-bold transition-all duration-200 flex items-center justify-center cursor-pointer"

    if (!isPositionActive(row, col)) {
      return `${baseStyle} bg-gray-100 border-gray-300 text-gray-400 cursor-not-allowed`
    }

    const assignedChannel = getAssignedChannel(row, col)

    if (assignedChannel !== null) {
      return `${baseStyle} bg-blue-500 border-blue-600 text-white hover:bg-red-400`
    }

    return `${baseStyle} bg-gray-200 border-gray-400 text-gray-600 hover:bg-blue-300 hover:border-blue-400 border-dashed`
  }

  const getChannelBadgeStyle = (channel: number) => {
    const isUsed = isChannelUsed(channel)
    const isSelected = selectedChannel === channel

    if (isSelected) {
      return "bg-yellow-500 text-yellow-900 border-yellow-600"
    }

    if (isUsed) {
      return "bg-blue-500 text-white"
    }

    return "bg-gray-200 text-gray-700 hover:bg-gray-300 cursor-pointer"
  }

  const getTooltipContent = (row: number, col: number) => {
    const assignedChannel = getAssignedChannel(row, col)

    if (!isPositionActive(row, col)) {
      return `Inactive position [${row},${col}]`
    }

    if (assignedChannel !== null) {
      return `Channel ${assignedChannel} at [${row},${col}] - Click to clear`
    }

    if (autoIncrement) {
      return `Will assign: Channel ${nextChannel}`
    }

    if (selectedChannel !== null) {
      return `Will assign: Channel ${selectedChannel}`
    }

    return `Empty position [${row},${col}] - Click to assign`
  }

  const usedChannels = getUsedChannels()
  const mappingComplete = usedChannels.size === totalChannels

  return (
    <TooltipProvider delayDuration={300}>
      <div className="space-y-4">
        <Card>
          <CardHeader className="pb-3">
            <CardTitle className="text-sm">Channel Mapping</CardTitle>
            <p className="text-xs text-muted-foreground">
              Assign {totalChannels} channels to {rows}×{cols} grid positions
            </p>
          </CardHeader>
          <CardContent className="space-y-4">
            {/* Auto-Increment Controls */}
            <div className="space-y-3 p-3 bg-blue-50 dark:bg-blue-950/20 rounded-lg border border-blue-200 dark:border-blue-800">
              <div className="flex items-center justify-between">
                <div className="flex items-center gap-2">
                  <Zap className="h-4 w-4 text-blue-600" />
                  <span className="text-sm font-medium">Auto-Increment Mode</span>
                  <Switch
                    checked={autoIncrement}
                    onCheckedChange={(checked) => {
                      setAutoIncrement(checked)
                      if (!checked) {
                        setSelectedChannel(null)
                      }
                    }}
                  />
                </div>
                {autoIncrement && (
                  <Badge variant="secondary" className="bg-blue-100 text-blue-700 dark:bg-blue-900 dark:text-blue-300">
                    Next: {nextChannel}
                  </Badge>
                )}
              </div>

              {autoIncrement ? (
                <p className="text-xs text-muted-foreground">
                  Click any empty cell to assign channel {nextChannel}, then auto-advance to {nextChannel + 1}
                </p>
              ) : (
                <p className="text-xs text-muted-foreground">
                  Manual mode: Select a channel below, then click a cell to assign it
                </p>
              )}
            </div>

            {/* Available Channels */}
            {!autoIncrement && (
              <div className="space-y-2">
                <div className="text-xs font-medium">Available Channels ({usedChannels.size}/{totalChannels})</div>
                <div className="flex flex-wrap gap-1">
                  {Array.from({ length: totalChannels }, (_, i) => (
                    <Badge
                      key={i}
                      variant="outline"
                      className={getChannelBadgeStyle(i)}
                      onClick={() => handleChannelSelect(i)}
                    >
                      {i}
                    </Badge>
                  ))}
                </div>
                {selectedChannel !== null && (
                  <div className="text-xs text-yellow-600">
                    Click a grid position to assign channel {selectedChannel}
                  </div>
                )}
              </div>
            )}

            {/* Grid Layout */}
            <div className="space-y-2">
              <div className="text-xs font-medium">Grid Layout ({layoutType})</div>
              <div className="flex justify-center">
                <div
                  className="grid gap-1 p-3 bg-slate-50 dark:bg-slate-900 rounded-lg border"
                  style={{ gridTemplateColumns: `repeat(${cols}, 1fr)` }}
                >
                  {Array.from({ length: rows * cols }, (_, index) => {
                    const row = Math.floor(index / cols)
                    const col = index % cols
                    const assignedChannel = getAssignedChannel(row, col)

                    return (
                      <Tooltip key={index}>
                        <TooltipTrigger asChild>
                          <div
                            className={getCellStyle(row, col)}
                            onClick={() => handleCellClick(row, col)}
                          >
                            {isPositionActive(row, col) ? (assignedChannel !== null ? assignedChannel : '?') : 'X'}
                          </div>
                        </TooltipTrigger>
                        <TooltipContent side="top">
                          <p className="text-xs">{getTooltipContent(row, col)}</p>
                        </TooltipContent>
                      </Tooltip>
                    )
                  })}
                </div>
              </div>
            </div>

            {/* Controls */}
            <div className="flex gap-2">
              <Button
                onClick={autoAssign}
                size="sm"
                variant="outline"
                className="flex items-center gap-1"
              >
                <Shuffle className="h-3 w-3" />
                Auto Assign
              </Button>
              <Button
                onClick={clearMapping}
                size="sm"
                variant="outline"
                className="flex items-center gap-1"
              >
                <RotateCcw className="h-3 w-3" />
                Clear
              </Button>
            </div>

            {/* Navigation */}
            <div className="flex justify-between">
              <Button
                onClick={onBack}
                size="sm"
                variant="outline"
                className="flex items-center gap-1"
              >
                <ArrowLeft className="h-3 w-3" />
                Back to Grid Config
              </Button>
              <Button
                onClick={handleComplete}
                disabled={!mappingComplete}
                size="sm"
                className="flex items-center gap-1"
              >
                Continue to Pattern Builder
                <ArrowRight className="h-3 w-3" />
              </Button>
            </div>

            {!mappingComplete && (
              <div className="text-xs text-destructive">
                Please assign all {totalChannels} channels before continuing
              </div>
            )}
          </CardContent>
        </Card>
      </div>
    </TooltipProvider>
  )
}