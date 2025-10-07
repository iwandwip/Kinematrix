"use client"

import React, { memo } from "react"
import { LEDFrame, GridConfig } from "@/lib/contexts/SimulationContext"

interface ExpandedGridPreviewProps {
  frame: LEDFrame
  gridConfig: GridConfig | null | undefined
  frameIndex: number
}

// Memoized component to prevent unnecessary re-renders during hover
const ExpandedGridPreview = memo<ExpandedGridPreviewProps>(({
  frame,
  gridConfig,
  frameIndex
}) => {
  const totalChannels = frame.channels.length
  const onChannels = frame.channels.filter(Boolean).length

  const expectedChannels = gridConfig?.totalChannels || 24
  const hasMismatch = totalChannels !== expectedChannels

  const renderGrid = () => {
    if (!gridConfig) {
      const maxVisible = Math.min(totalChannels, 64)
      const cols = Math.min(Math.ceil(Math.sqrt(maxVisible)), 8)

      return (
        <div className="flex flex-col gap-2 items-center">
          <div
            className="grid gap-1"
            style={{
              gridTemplateColumns: `repeat(${cols}, 1fr)`,
              maxWidth: '200px'
            }}
          >
            {frame.channels.slice(0, maxVisible).map((isOn, channelIndex) => (
              <div
                key={channelIndex}
                className={`w-4 h-4 rounded-sm border flex items-center justify-center text-xs font-mono ${
                  isOn
                    ? 'bg-yellow-400 border-yellow-600 text-yellow-900'
                    : 'bg-gray-100 dark:bg-gray-800 border-gray-300 dark:border-gray-600 text-gray-600 dark:text-gray-400'
                }`}
                title={`Channel ${channelIndex}: ${isOn ? 'ON' : 'OFF'}`}
              >
                {channelIndex}
              </div>
            ))}
          </div>
          {totalChannels > maxVisible && (
            <div className="text-xs text-muted-foreground">
              +{totalChannels - maxVisible} more channels
            </div>
          )}
        </div>
      )
    }

    const { rows, cols } = gridConfig
    const maxVisible = Math.min(totalChannels, rows * cols)
    const displayCols = Math.min(cols, 12)
    const displayRows = Math.min(rows, 8)
    const maxCells = Math.min(displayCols * displayRows, maxVisible)

    return (
      <div className="flex flex-col gap-2 items-center">
        <div
          className="grid gap-1"
          style={{
            gridTemplateColumns: `repeat(${displayCols}, 1fr)`,
            maxWidth: '300px'
          }}
        >
          {Array.from({ length: maxCells }, (_, index) => {
            const isOn = index < frame.channels.length ? frame.channels[index] : false
            const isValid = index < totalChannels

            return (
              <div
                key={index}
                className={`w-5 h-5 rounded-sm border flex items-center justify-center text-xs font-mono transition-colors duration-75 ${
                  !isValid
                    ? 'bg-red-50 border-red-200 text-red-400'
                    : isOn
                      ? 'bg-yellow-400 border-yellow-600 text-yellow-900'
                      : 'bg-gray-100 dark:bg-gray-800 border-gray-300 dark:border-gray-600 text-gray-600 dark:text-gray-400'
                }`}
                title={
                  !isValid
                    ? `Invalid channel ${index}`
                    : `Channel ${index}: ${isOn ? 'ON' : 'OFF'}`
                }
              >
                {index < 100 ? index : '••'}
              </div>
            )
          })}
        </div>
        {totalChannels > maxCells && (
          <div className="text-xs text-muted-foreground">
            +{totalChannels - maxCells} more channels
          </div>
        )}
      </div>
    )
  }

  return (
    <div className="w-72 p-3 space-y-2 select-none max-w-full overflow-hidden">
      <div className="flex items-center justify-between">
        <h4 className="font-semibold text-sm">
          Frame {frameIndex + 1} Preview
        </h4>
        {hasMismatch && (
          <span className="text-xs text-orange-600 dark:text-orange-400 font-medium">
            ⚠️ Mismatch
          </span>
        )}
      </div>

      <div className="space-y-2">
        <div className="text-xs text-muted-foreground space-y-1.5">
          <div className="flex justify-between items-center">
            <span>Channels ON:</span>
            <span className="font-mono font-medium">{onChannels}/{totalChannels}</span>
          </div>
          <div className="flex justify-between items-center">
            <span>Delay:</span>
            <span className="font-mono font-medium">{frame.delay}ms</span>
          </div>
          {frame.label && (
            <div className="flex justify-between items-center gap-2">
              <span className="flex-shrink-0">Label:</span>
              <span className="font-medium truncate text-right" title={frame.label}>
                {frame.label}
              </span>
            </div>
          )}
          {gridConfig && (
            <div className="flex justify-between items-center">
              <span>Grid:</span>
              <span className="font-mono font-medium text-right">
                {gridConfig.rows}×{gridConfig.cols} ({gridConfig.layoutType})
              </span>
            </div>
          )}
          {hasMismatch && (
            <div className="text-orange-600 dark:text-orange-400 text-xs p-2 bg-orange-50 dark:bg-orange-950 rounded-md">
              Expected {expectedChannels} channels, got {totalChannels}
            </div>
          )}
        </div>
      </div>

      <div className="border-t pt-2">
        {renderGrid()}
      </div>

      <div className="text-xs text-muted-foreground text-center pt-1 border-t truncate">
        Hover to view detailed channel states
      </div>
    </div>
  )
})

ExpandedGridPreview.displayName = 'ExpandedGridPreview'

export default ExpandedGridPreview