"use client"

import { useState, useEffect, useRef, useMemo, useCallback } from "react"
import { Button } from "@/components/ui/button"
import { Input } from "@/components/ui/input"
import { HoverCard, HoverCardTrigger, HoverCardContent } from "@/components/ui/hover-card"
import { useSimulation } from "@/lib/contexts/SimulationContext"
import { Plus, Copy, Trash2, Edit3, ChevronLeft, ChevronRight } from "lucide-react"
import ExpandedGridPreview from "./ExpandedGridPreview"
import { useDebounce, PerformanceMonitor } from "@/lib/utils/performanceOptimizations"

export default function FrameTimeline() {
  const {
    simulationData,
    addFrame,
    removeFrame,
    duplicateFrame,
    selectFrame,
    updateFrameDelay,
    updateFrameLabel
  } = useSimulation()

  const [editingFrameIndex, setEditingFrameIndex] = useState<number | null>(null)
  const [editingLabel, setEditingLabel] = useState("")
  const [currentPage, setCurrentPage] = useState(0)
  const lastSelectedFrameRef = useRef(simulationData.selectedFrameIndex)
  const isManualNavigationRef = useRef(false)
  const [hoveredFrameIndex, setHoveredFrameIndex] = useState<number | null>(null)

  const ITEMS_PER_PAGE = 8

  // Memoized calculations to prevent unnecessary re-computation
  const totalFrames = useMemo(() => simulationData.currentPattern.frames.length, [simulationData.currentPattern.frames.length])
  const totalPages = useMemo(() => Math.ceil(totalFrames / ITEMS_PER_PAGE), [totalFrames, ITEMS_PER_PAGE])
  const startIndex = useMemo(() => currentPage * ITEMS_PER_PAGE, [currentPage, ITEMS_PER_PAGE])
  const endIndex = useMemo(() => startIndex + ITEMS_PER_PAGE, [startIndex, ITEMS_PER_PAGE])
  const currentFrames = useMemo(() => simulationData.currentPattern.frames.slice(startIndex, endIndex), [simulationData.currentPattern.frames, startIndex, endIndex])

  // Debounced hover handler to prevent rapid DOM creation
  const debouncedSetHoveredFrame = useDebounce((frameIndex: number | null) => {
    setHoveredFrameIndex(frameIndex)
  }, 100)

  useEffect(() => {
    const selectedFrameIndex = simulationData.selectedFrameIndex

    // Only auto-navigate if frame selection actually changed AND it's not manual navigation
    if (selectedFrameIndex !== lastSelectedFrameRef.current && !isManualNavigationRef.current) {
      const requiredPage = Math.floor(selectedFrameIndex / ITEMS_PER_PAGE)
      if (requiredPage !== currentPage && requiredPage < totalPages) {
        setCurrentPage(requiredPage)
      }
    }

    // Reset manual navigation flag and update last selected frame
    isManualNavigationRef.current = false
    lastSelectedFrameRef.current = selectedFrameIndex
  }, [simulationData.selectedFrameIndex, ITEMS_PER_PAGE, currentPage, totalPages])

  const handleFrameClick = (frameIndex: number) => {
    selectFrame(frameIndex)
  }

  const handleAddFrame = () => {
    addFrame()
  }

  const handleDuplicateFrame = (frameIndex: number, e: React.MouseEvent) => {
    e.stopPropagation()
    duplicateFrame(frameIndex)
  }

  const handleRemoveFrame = (frameIndex: number, e: React.MouseEvent) => {
    e.stopPropagation()
    if (simulationData.currentPattern.frames.length > 1) {
      removeFrame(frameIndex)
    }
  }

  const handleEditLabel = (frameIndex: number, e: React.MouseEvent) => {
    e.stopPropagation()
    setEditingFrameIndex(frameIndex)
    setEditingLabel(simulationData.currentPattern.frames[frameIndex].label || "")
  }

  const handleSaveLabel = () => {
    if (editingFrameIndex !== null) {
      updateFrameLabel(editingFrameIndex, editingLabel)
      setEditingFrameIndex(null)
      setEditingLabel("")
    }
  }

  const handleCancelEdit = () => {
    setEditingFrameIndex(null)
    setEditingLabel("")
  }

  const handleDelayChange = useCallback((frameIndex: number, delay: number) => {
    // Only allow delay changes in non-JavaScript modes
    if (simulationData.patternMode !== 'javascript') {
      updateFrameDelay(frameIndex, delay)
    }
  }, [simulationData.patternMode, updateFrameDelay])

  const isJavaScriptMode = simulationData.patternMode === 'javascript'

  // Memoized frame preview calculation
  const getFramePreview = useCallback((frameIndex: number) => {
    const finishTiming = PerformanceMonitor.start('getFramePreview')

    const frame = simulationData.currentPattern.frames[frameIndex]
    const gridConfig = simulationData.gridConfig || simulationData.currentPattern.gridConfig
    const onChannels = frame.channels.filter(Boolean).length
    const totalChannels = frame.channels.length

    // Show warning if there's mismatch between frame channels and grid config
    const expectedChannels = gridConfig?.totalChannels || simulationData.currentPattern.settings.totalChannels
    const hasMismatch = totalChannels !== expectedChannels

    const result = hasMismatch ? `${onChannels}/${totalChannels}⚠️` : `${onChannels}/${totalChannels}`

    finishTiming()
    return result
  }, [simulationData.currentPattern.frames, simulationData.gridConfig, simulationData.currentPattern.gridConfig, simulationData.currentPattern.settings.totalChannels])

  // Memoized channel mini grid calculation
  const getChannelMiniGrid = useCallback((frameIndex: number) => {
    const finishTiming = PerformanceMonitor.start('getChannelMiniGrid')

    const frame = simulationData.currentPattern.frames[frameIndex]
    const gridConfig = simulationData.gridConfig || simulationData.currentPattern.gridConfig
    const totalChannels = frame.channels.length

    if (!gridConfig) {
      const maxVisible = Math.min(totalChannels, 16)
      const cols = Math.min(maxVisible, 8)

      return (
        <div className="flex flex-col gap-0.5 items-center">
          <div
            className="grid gap-0.5"
            style={{
              gridTemplateColumns: `repeat(${cols}, 1fr)`,
              maxWidth: '32px'
            }}
          >
            {frame.channels.slice(0, maxVisible).map((isOn, channelIndex) => (
              <div
                key={channelIndex}
                className={`w-0.5 h-0.5 rounded-sm ${
                  isOn ? 'bg-yellow-400' : 'bg-gray-300 dark:bg-gray-600'
                }`}
              />
            ))}
          </div>
          {totalChannels > maxVisible && (
            <span className="text-xs text-muted-foreground">+{totalChannels - maxVisible}</span>
          )}
        </div>
      )
    }

    const { rows, cols } = gridConfig
    const maxVisible = Math.min(totalChannels, 24)
    const displayCols = Math.min(cols, 6)
    const displayRows = Math.min(rows, 4)
    const maxCells = Math.min(displayCols * displayRows, maxVisible)

    finishTiming()

    return (
      <div className="flex flex-col gap-0.5 items-center">
        <div
          className="grid gap-0.5 mx-auto w-fit"
          style={{
            gridTemplateColumns: `repeat(${displayCols}, 1fr)`,
            maxWidth: '32px'
          }}
        >
          {frame.channels.slice(0, maxCells).map((isOn, channelIndex) => (
            <div
              key={channelIndex}
              className={`w-0.5 h-0.5 rounded-sm ${
                isOn ? 'bg-yellow-400' : 'bg-gray-300 dark:bg-gray-600'
              }`}
            />
          ))}
        </div>
        {totalChannels > maxCells && (
          <span className="text-xs text-muted-foreground">+{totalChannels - maxCells}</span>
        )}
      </div>
    )
  }, [simulationData.currentPattern.frames, simulationData.gridConfig, simulationData.currentPattern.gridConfig])

  return (
    <div className="space-y-4">
      {/* Timeline Header */}
      <div className="flex items-center justify-between">
        <h3 className="text-sm font-medium">Frame Timeline</h3>
        <Button
          onClick={handleAddFrame}
          size="sm"
          variant="outline"
          className="flex items-center gap-1"
        >
          <Plus className="h-3 w-3" />
          Add Frame
        </Button>
      </div>

      {/* Timeline Table */}
      <div className="border rounded-lg overflow-hidden">
        <div className="h-[288px]">
          <table className="w-full text-sm table-fixed">
            <thead className="bg-background">
              <tr className="border-b-2 border-border bg-muted">
                <th className="px-2 py-1 text-left w-12 font-medium">#</th>
                <th className="px-2 py-1 text-left font-medium">Label</th>
                <th className="px-2 py-1 text-center w-16 font-medium">Preview</th>
                <th className="px-2 py-1 text-center w-20 font-medium">Channels</th>
                <th className="px-2 py-1 text-center w-24 font-medium">
                  {isJavaScriptMode ? (
                    <div className="flex items-center justify-center gap-1" title="Delays from JavaScript sleep() calls">
                      Delay
                      <span className="text-xs text-muted-foreground font-normal">(JS)</span>
                    </div>
                  ) : (
                    'Delay'
                  )}
                </th>
                <th className="px-2 py-1 text-center w-24 font-medium">Actions</th>
              </tr>
            </thead>
            <tbody>
              {currentFrames.map((frame, localIndex) => {
                const frameIndex = startIndex + localIndex
                return (
                <tr
                  key={frame.id}
                  className={`border-b hover:bg-muted/30 cursor-pointer transition-colors h-8 ${
                    frameIndex === simulationData.selectedFrameIndex
                      ? 'bg-blue-100 dark:bg-blue-900 border-l-4 border-l-blue-500 shadow-sm'
                      : ''
                  }`}
                  onClick={() => handleFrameClick(frameIndex)}
                >
                  {/* Frame Number */}
                  <td className="px-2 py-1 font-mono text-center">
                    {frameIndex + 1}
                  </td>

                  {/* Frame Label */}
                  <td className="px-2 py-1">
                    <div className="text-xs min-h-[12px] flex items-center">
                      {frame.label ? (
                        <span className="truncate max-w-24" title={frame.label}>
                          {frame.label}
                        </span>
                      ) : (
                        <span className="text-muted-foreground italic">No label</span>
                      )}
                    </div>
                  </td>

                  {/* Mini Grid Preview */}
                  <td className="px-2 py-1 text-center">
                    <HoverCard
                      openDelay={100}
                      closeDelay={0}
                      open={hoveredFrameIndex === frameIndex}
                      onOpenChange={(open) => {
                        if (!open) {
                          debouncedSetHoveredFrame(null)
                        }
                      }}
                    >
                      <HoverCardTrigger asChild>
                        <div
                          className="cursor-pointer hover:opacity-80 transition-opacity duration-150"
                          onMouseEnter={() => debouncedSetHoveredFrame(frameIndex)}
                        >
                          {getChannelMiniGrid(frameIndex)}
                        </div>
                      </HoverCardTrigger>
                      <HoverCardContent
                        className="animate-in fade-in-0 zoom-in-95 duration-150 max-w-[320px]"
                        side="right"
                        align="center"
                        sideOffset={8}
                        collisionPadding={12}
                      >
                        <ExpandedGridPreview
                          frame={simulationData.currentPattern.frames[frameIndex]}
                          gridConfig={simulationData.gridConfig || simulationData.currentPattern.gridConfig}
                          frameIndex={frameIndex}
                        />
                      </HoverCardContent>
                    </HoverCard>
                  </td>

                  {/* Channel Count */}
                  <td className="px-2 py-1 text-center text-xs">
                    <div className="font-mono" title={(() => {
                      const frame = simulationData.currentPattern.frames[frameIndex]
                      const gridConfig = simulationData.gridConfig || simulationData.currentPattern.gridConfig
                      const expectedChannels = gridConfig?.totalChannels || simulationData.currentPattern.settings.totalChannels
                      const hasMismatch = frame.channels.length !== expectedChannels
                      return hasMismatch ? `Warning: Frame has ${frame.channels.length} channels but grid config expects ${expectedChannels}` : `${frame.channels.filter(Boolean).length} channels ON out of ${frame.channels.length} total`
                    })()}>
                      {getFramePreview(frameIndex)} ON
                    </div>
                  </td>

                  {/* Frame Delay */}
                  <td className="px-2 py-1" onClick={(e) => e.stopPropagation()}>
                    <div className="flex items-center justify-center">
                      {isJavaScriptMode ? (
                        <div
                          className="h-5 text-xs w-14 text-center font-mono text-muted-foreground bg-muted/30 rounded border px-1 flex items-center justify-center"
                          title="Delay value from JavaScript sleep() call - read only"
                        >
                          {frame.delay}ms
                        </div>
                      ) : (
                        <Input
                          type="number"
                          value={frame.delay}
                          onChange={(e) => handleDelayChange(frameIndex, parseInt(e.target.value) || 0)}
                          min={10}
                          max={5000}
                          className="h-5 text-xs w-14 text-center"
                          placeholder="ms"
                        />
                      )}
                    </div>
                  </td>

                  {/* Actions */}
                  <td className="px-2 py-1" onClick={(e) => e.stopPropagation()}>
                    {editingFrameIndex === frameIndex ? (
                      <div className="relative">
                        <div className="absolute right-0 top-0 bg-background border border-border rounded-lg shadow-lg p-3 z-40 min-w-48">
                          <Input
                            value={editingLabel}
                            onChange={(e) => setEditingLabel(e.target.value)}
                            placeholder="Enter frame label"
                            className="h-5 text-xs mb-1"
                            onKeyDown={(e) => {
                              if (e.key === 'Enter') handleSaveLabel()
                              if (e.key === 'Escape') handleCancelEdit()
                            }}
                            autoFocus
                          />
                          <div className="flex gap-1">
                            <Button size="sm" onClick={handleSaveLabel} className="h-4 text-xs flex-1">
                              Save
                            </Button>
                            <Button size="sm" variant="outline" onClick={handleCancelEdit} className="h-4 text-xs flex-1">
                              Cancel
                            </Button>
                          </div>
                        </div>
                        <Edit3 className="h-3 w-3 opacity-50" />
                      </div>
                    ) : (
                      <div className="flex gap-0.5 justify-center">
                        <Button
                          size="sm"
                          variant="ghost"
                          className="h-5 w-5 p-0"
                          onClick={(e) => handleEditLabel(frameIndex, e)}
                          title="Edit label"
                        >
                          <Edit3 className="h-3 w-3" />
                        </Button>
                        <Button
                          size="sm"
                          variant="ghost"
                          className="h-5 w-5 p-0"
                          onClick={(e) => handleDuplicateFrame(frameIndex, e)}
                          title="Duplicate frame"
                        >
                          <Copy className="h-3 w-3" />
                        </Button>
                        {simulationData.currentPattern.frames.length > 1 && (
                          <Button
                            size="sm"
                            variant="ghost"
                            className="h-5 w-5 p-0 text-red-500 hover:text-red-700"
                            onClick={(e) => handleRemoveFrame(frameIndex, e)}
                            title="Delete frame"
                          >
                            <Trash2 className="h-3 w-3" />
                          </Button>
                        )}
                      </div>
                    )}
                  </td>
                </tr>
                )
              })}

              {/* Placeholder rows to maintain consistent height - only when needed */}
              {currentFrames.length < ITEMS_PER_PAGE && Array.from({ length: ITEMS_PER_PAGE - currentFrames.length }, (_, index) => (
                <tr key={`placeholder-${index}`} className="border-b opacity-0 pointer-events-none h-8">
                  <td className="px-2 py-1 font-mono text-center">-</td>
                  <td className="px-2 py-1">
                    <div className="text-xs min-h-[12px] flex items-center">-</div>
                  </td>
                  <td className="px-2 py-1 text-center">-</td>
                  <td className="px-2 py-1 text-center text-xs">
                    <div className="font-mono">-</div>
                  </td>
                  <td className="px-2 py-1">
                    <div className="flex items-center justify-center">-</div>
                  </td>
                  <td className="px-2 py-1">
                    <div className="flex gap-0.5 justify-center">-</div>
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>

        {/* Pagination Controls */}
        {totalPages > 1 && (
          <div className="flex items-center justify-between p-2 border-t bg-muted/30">
            <div className="text-xs text-muted-foreground">
              Showing {startIndex + 1}-{Math.min(endIndex, totalFrames)} of {totalFrames} frames
            </div>

            <div className="flex items-center gap-1">
              <Button
                size="sm"
                variant="outline"
                onClick={() => {
                  isManualNavigationRef.current = true
                  setCurrentPage(prev => Math.max(0, prev - 1))
                }}
                disabled={currentPage === 0}
                className="h-6 w-6 p-0"
              >
                <ChevronLeft className="h-3 w-3" />
              </Button>

              <span className="text-xs px-2 font-mono">
                {currentPage + 1}/{totalPages}
              </span>

              <Button
                size="sm"
                variant="outline"
                onClick={() => {
                  isManualNavigationRef.current = true
                  setCurrentPage(prev => Math.min(totalPages - 1, prev + 1))
                }}
                disabled={currentPage === totalPages - 1}
                className="h-6 w-6 p-0"
              >
                <ChevronRight className="h-3 w-3" />
              </Button>
            </div>
          </div>
        )}
      </div>

      {/* Timeline Info */}
      <div className="text-xs text-muted-foreground text-center">
        {totalFrames} frames total • Frame {simulationData.selectedFrameIndex + 1} selected
        {totalPages > 1 && ` • Page ${currentPage + 1}/${totalPages}`}
      </div>
    </div>
  )
}