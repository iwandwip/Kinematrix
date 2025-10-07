"use client"

import { Badge } from "@/components/ui/badge"
import { Card } from "@/components/ui/card"
import { Button } from "@/components/ui/button"
import { Trash2, Eye, Clock } from "lucide-react"
import { type ExpressionFrame } from "@/lib/simulation/expressionParser"

interface ExpressionPreviewProps {
  channelCount: number
  frames: ExpressionFrame[]
  onRemoveFrame?: (frameIndex: number) => void
}

export default function ExpressionPreview({
  channelCount,
  frames,
  onRemoveFrame
}: ExpressionPreviewProps) {

  const getChannelVisualization = (channels: boolean[], maxDisplay: number = 16) => {
    if (channels.length <= maxDisplay) {
      return channels.map((isOn, index) => (
        <div
          key={index}
          className={`w-3 h-3 rounded border ${
            isOn
              ? 'bg-green-500 border-green-600'
              : 'bg-gray-200 dark:bg-gray-700 border-gray-300 dark:border-gray-600'
          }`}
          title={`Channel ${index}: ${isOn ? 'ON' : 'OFF'}`}
        />
      ))
    }

    // For large channel counts, show simplified view
    const segments = 8
    const segmentSize = Math.ceil(channels.length / segments)

    return Array.from({ length: segments }, (_, segmentIndex) => {
      const start = segmentIndex * segmentSize
      const end = Math.min(start + segmentSize, channels.length)
      const segmentChannels = channels.slice(start, end)
      const segmentActiveCount = segmentChannels.filter(Boolean).length
      const intensity = segmentActiveCount / segmentChannels.length

      return (
        <div
          key={segmentIndex}
          className={`w-4 h-4 rounded border flex items-center justify-center text-xs font-mono ${
            intensity > 0.5
              ? 'bg-green-500 border-green-600 text-white'
              : intensity > 0
              ? 'bg-yellow-400 border-yellow-500 text-black'
              : 'bg-gray-200 dark:bg-gray-700 border-gray-300 dark:border-gray-600 text-gray-500'
          }`}
          title={`Channels ${start}-${end-1}: ${segmentActiveCount}/${segmentChannels.length} active`}
        >
          {segmentActiveCount}
        </div>
      )
    })
  }

  const getActiveChannelsSummary = (channels: boolean[]) => {
    const activeChannels = channels
      .map((isOn, index) => isOn ? index : -1)
      .filter(index => index !== -1)

    if (activeChannels.length === 0) return "None"
    if (activeChannels.length === channels.length) return "All"
    if (activeChannels.length <= 8) return activeChannels.join(", ")
    return `${activeChannels.slice(0, 6).join(", ")}... (+${activeChannels.length - 6} more)`
  }

  if (frames.length === 0) {
    return (
      <div className="text-center py-8 text-muted-foreground">
        <Eye className="h-8 w-8 mx-auto mb-2 opacity-50" />
        <p className="text-sm">No expression frames added yet</p>
        <p className="text-xs">Use the Pattern Library or Custom Expression to add frames</p>
      </div>
    )
  }

  return (
    <div className="space-y-3 max-h-96 overflow-y-auto">
      {frames.map((frame, index) => (
        <Card key={`${frame.frameNumber}-${index}`} className="p-3">
          <div className="space-y-3">
            {/* Frame Header */}
            <div className="flex items-start justify-between">
              <div className="space-y-1">
                <div className="flex items-center gap-2">
                  <Badge variant="outline" className="text-xs">
                    Frame {frame.frameNumber}
                  </Badge>
                  <Badge variant="secondary" className="text-xs">
                    {frame.expression.category}
                  </Badge>
                  <Badge variant="outline" className="text-xs flex items-center gap-1">
                    <Clock className="h-2 w-2" />
                    {frame.duration}ms
                  </Badge>
                </div>
                <h4 className="font-medium text-sm">{frame.expression.name}</h4>
                <p className="text-xs text-muted-foreground">{frame.expression.description}</p>
              </div>

              {onRemoveFrame && (
                <Button
                  size="sm"
                  variant="ghost"
                  onClick={() => onRemoveFrame(index)}
                  className="h-6 w-6 p-0 text-muted-foreground hover:text-destructive"
                >
                  <Trash2 className="h-3 w-3" />
                </Button>
              )}
            </div>

            {/* Expression Details */}
            <div className="space-y-2">
              <div className="text-xs">
                <span className="font-medium">Expression:</span>
                <code className="ml-2 bg-muted px-1 py-0.5 rounded text-xs">
                  {frame.expression.expression}
                </code>
              </div>

              {Object.keys(frame.parameters).length > 0 && (
                <div className="text-xs">
                  <span className="font-medium">Parameters:</span>
                  <span className="ml-2 text-muted-foreground">
                    {JSON.stringify(frame.parameters)}
                  </span>
                </div>
              )}
            </div>

            {/* Channel Visualization */}
            <div className="space-y-2">
              <div className="text-xs font-medium">Channel States:</div>
              <div className="flex flex-wrap gap-1">
                {getChannelVisualization(frame.channels)}
              </div>

              <div className="text-xs text-muted-foreground">
                <span className="font-medium">Active:</span> {getActiveChannelsSummary(frame.channels)}
                <span className="ml-2">
                  ({frame.channels.filter(Boolean).length}/{frame.channels.length} channels)
                </span>
              </div>
            </div>

            {/* Channel Pattern Preview */}
            {channelCount <= 24 && (
              <div className="space-y-1">
                <div className="text-xs font-medium">Pattern Preview:</div>
                <div className="font-mono text-xs bg-muted p-2 rounded">
                  {frame.channels.map(isOn => isOn ? '█' : '░').join('')}
                </div>
              </div>
            )}
          </div>
        </Card>
      ))}

      {/* Summary */}
      <Card className="p-3 bg-muted/50">
        <div className="grid grid-cols-2 gap-4 text-xs">
          <div>
            <span className="font-medium">Total Frames:</span>
            <span className="ml-2">{frames.length}</span>
          </div>
          <div>
            <span className="font-medium">Total Duration:</span>
            <span className="ml-2">
              {frames.reduce((total, frame) => total + frame.duration, 0)}ms
            </span>
          </div>
          <div>
            <span className="font-medium">Channel Count:</span>
            <span className="ml-2">{channelCount}</span>
          </div>
          <div>
            <span className="font-medium">Avg. Active:</span>
            <span className="ml-2">
              {frames.length > 0 ?
                Math.round(
                  frames.reduce((sum, frame) =>
                    sum + frame.channels.filter(Boolean).length, 0
                  ) / frames.length
                ) : 0
              } channels
            </span>
          </div>
        </div>
      </Card>
    </div>
  )
}