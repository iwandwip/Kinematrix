"use client"

import { useState, useMemo } from "react"
import { Button } from "@/components/ui/button"
import { useSimulation } from "@/lib/contexts/SimulationContext"
import { Lightbulb, LightbulbOff } from "lucide-react"
import { useDynamicGridSize } from "@/lib/hooks/useDynamicGridSize"
import { GridViewControls } from "./GridViewControls"

interface DynamicPatternGridProps {
  disabled?: boolean
}

export default function DynamicPatternGrid({ disabled = false }: DynamicPatternGridProps) {
  const { simulationData, toggleChannel, setAllChannels } = useSimulation()
  const [viewMode, setViewMode] = useState<'default' | 'fit' | 'scroll'>('default')

  const currentFrame = simulationData.currentPattern.frames[simulationData.selectedFrameIndex]
  const channels = currentFrame?.channels || []
  const gridConfig = simulationData.gridConfig || simulationData.currentPattern.gridConfig

  const rows = gridConfig?.rows || 1
  const cols = gridConfig?.cols || 1
  const channelMapping = useMemo(() => gridConfig?.channelMapping || new Map(), [gridConfig])
  const totalChannels = gridConfig?.totalChannels || 0

  const { containerRef, cellSize: dynamicCellSize, isOverflowing } = useDynamicGridSize({
    rows,
    cols,
    gap: 8,
    minCellSize: 10,
    maxCellSize: 48
  })

  const cellSize = viewMode === 'default' ? 48 : dynamicCellSize

  // Memoize expensive grid calculations
  const gridData = useMemo(() => {
    if (!gridConfig) return []

    return Array.from({ length: rows * cols }, (_, index) => {
      const row = Math.floor(index / cols)
      const col = index % cols
      const key = `${row},${col}`
      const channel = channelMapping.get(key)
      const channelIndex = channel !== undefined ? channel : null
      const isActive = channelIndex !== null

      return {
        row,
        col,
        key: index,
        channelIndex,
        isActive
      }
    })
  }, [rows, cols, gridConfig, channelMapping])

  if (!gridConfig) {
    return (
      <div className="text-center text-muted-foreground p-8">
        <p>Grid configuration not found</p>
      </div>
    )
  }

  const getChannelAtPosition = (row: number, col: number): number | null => {
    const key = `${row},${col}`
    const channel = channelMapping.get(key)
    return channel !== undefined ? channel : null
  }

  const isPositionActive = (row: number, col: number): boolean => {
    return getChannelAtPosition(row, col) !== null
  }

  const isChannelOn = (row: number, col: number): boolean => {
    const channelIndex = getChannelAtPosition(row, col)
    if (channelIndex === null) return false
    return channels[channelIndex] || false
  }

  const handleCellClick = (row: number, col: number) => {
    if (disabled) return

    const channelIndex = getChannelAtPosition(row, col)
    if (channelIndex !== null) {
      toggleChannel(channelIndex)
    }
  }

  const handleAllOn = () => {
    if (disabled) return
    setAllChannels(true)
  }

  const handleAllOff = () => {
    if (disabled) return
    setAllChannels(false)
  }

  const getCellStyle = (row: number, col: number, size: number) => {
    const baseStyle = `rounded-lg border-2 font-mono font-bold transition-all duration-200 flex items-center justify-center`
    const sizeClass = size < 20 ? 'text-[8px]' : size < 30 ? 'text-[10px]' : 'text-xs'

    if (disabled) {
      return `${baseStyle} ${sizeClass} bg-gray-100 border-gray-300 text-gray-400 cursor-not-allowed`
    }

    if (!isPositionActive(row, col)) {
      return `${baseStyle} ${sizeClass} bg-gray-100 border-gray-300 text-gray-500 cursor-not-allowed opacity-50`
    }

    const isOn = isChannelOn(row, col)

    if (isOn) {
      return `${baseStyle} ${sizeClass} bg-yellow-400 border-yellow-500 text-yellow-900 shadow-lg shadow-yellow-400/30 hover:bg-yellow-300 cursor-pointer hover:scale-105`
    } else {
      return `${baseStyle} ${sizeClass} bg-gray-700 border-gray-600 text-gray-400 hover:bg-gray-600 cursor-pointer hover:scale-105`
    }
  }

  const getOverflowStyle = () => {
    if (viewMode === 'default') return 'auto' as const
    if (viewMode === 'scroll') return 'auto' as const
    return isOverflowing ? 'auto' as const : 'hidden' as const
  }

  const containerStyle = {
    width: '100%',
    height: '100%',
    overflow: getOverflowStyle(),
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    padding: '12px',
    minHeight: '200px'
  }

  const gridStyle = {
    display: 'grid',
    gridTemplateColumns: `repeat(${cols}, ${cellSize}px)`,
    gridTemplateRows: `repeat(${rows}, ${cellSize}px)`,
    gap: '8px',
    width: 'fit-content'
  }

  return (
    <div className="space-y-3">
      <GridViewControls
        viewMode={viewMode}
        onViewModeChange={setViewMode}
        gridInfo={{
          rows,
          cols,
          totalCells: totalChannels,
          cellSize
        }}
        isOverflowing={isOverflowing}
      />

      <div className="flex gap-2 justify-center">
        <Button
          onClick={handleAllOn}
          disabled={disabled}
          size="sm"
          variant="outline"
          className="flex items-center gap-1"
        >
          <Lightbulb className="h-3 w-3" />
          All ON
        </Button>
        <Button
          onClick={handleAllOff}
          disabled={disabled}
          size="sm"
          variant="outline"
          className="flex items-center gap-1"
        >
          <LightbulbOff className="h-3 w-3" />
          All OFF
        </Button>
      </div>

      <div
        ref={containerRef}
        style={containerStyle}
        className="bg-slate-50 dark:bg-slate-900 rounded-lg border"
      >
        <div style={gridStyle}>
          {gridData.map(({ row, col, key, channelIndex, isActive }) => (
            <button
              key={key}
              onClick={() => handleCellClick(row, col)}
              disabled={disabled || !isActive}
              className={getCellStyle(row, col, cellSize)}
              style={{
                width: `${cellSize}px`,
                height: `${cellSize}px`
              }}
              title={
                isActive
                  ? `Channel ${channelIndex} [${row},${col}] - ${isChannelOn(row, col) ? 'ON' : 'OFF'}`
                  : `Inactive position [${row},${col}]`
              }
            >
              {isActive ? channelIndex : 'X'}
            </button>
          ))}
        </div>
      </div>
    </div>
  )
}
