"use client"

import { Button } from "@/components/ui/button"
import { useSimulation } from "@/lib/contexts/SimulationContext"
import { Lightbulb, LightbulbOff } from "lucide-react"

interface PatternGridProps {
  disabled?: boolean
}

export default function PatternGrid({ disabled = false }: PatternGridProps) {
  const { simulationData, toggleChannel, setAllChannels } = useSimulation()

  const currentFrame = simulationData.currentPattern.frames[simulationData.selectedFrameIndex]
  const channels = currentFrame?.channels || new Array(28).fill(false)
  const totalChannels = simulationData.currentPattern.settings.totalChannels

  const handleChannelClick = (channelIndex: number) => {
    if (disabled) return
    toggleChannel(channelIndex)
  }

  const handleAllOn = () => {
    if (disabled) return
    setAllChannels(true)
  }

  const handleAllOff = () => {
    if (disabled) return
    setAllChannels(false)
  }

  const getChannelStyle = (channelIndex: number, isOn: boolean) => {
    const baseStyle = "w-12 h-12 rounded-lg border-2 font-mono text-xs font-bold transition-all duration-200 hover:scale-105"

    if (disabled) {
      return `${baseStyle} bg-gray-100 border-gray-300 text-gray-400 cursor-not-allowed`
    }

    if (isOn) {
      return `${baseStyle} bg-yellow-400 border-yellow-500 text-yellow-900 shadow-lg shadow-yellow-400/30 hover:bg-yellow-300`
    } else {
      return `${baseStyle} bg-gray-700 border-gray-600 text-gray-400 hover:bg-gray-600`
    }
  }

  return (
    <div className="space-y-4">
      {/* Control Buttons */}
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

      {/* LED Grid */}
      <div className="flex justify-center">
        <div className="grid grid-cols-6 gap-2 p-4 bg-slate-50 dark:bg-slate-900 rounded-lg border">
          {Array.from({ length: totalChannels }, (_, index) => (
            <button
              key={index}
              onClick={() => handleChannelClick(index)}
              disabled={disabled}
              className={getChannelStyle(index, channels[index])}
              title={`Channel ${index} - ${channels[index] ? 'ON' : 'OFF'}`}
            >
              {index}
            </button>
          ))}
        </div>
      </div>

      {/* Grid Info */}
      <div className="text-center text-sm text-muted-foreground">
        <p>6x6 LED Matrix - {totalChannels} Channels</p>
        <p>Click channels to toggle ON/OFF state</p>
        {currentFrame && (
          <p className="mt-1">
            Frame {simulationData.selectedFrameIndex + 1} -
            {channels.filter(Boolean).length} channels ON
          </p>
        )}
      </div>
    </div>
  )
}