"use client"

import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select"
import { Download, FileCode, Shield, Info } from "lucide-react"
import { useSimulation } from "@/lib/contexts/SimulationContext"
import { SAVE_BEHAVIOR_OPTIONS } from "@/lib/types/simulationTypes"
import { Tooltip, TooltipContent, TooltipProvider, TooltipTrigger } from "@/components/ui/tooltip"
import { Label } from "@/components/ui/label"

const iconMap = {
  Download,
  FileCode,
  Shield
}

export function SaveBehaviorSelector() {
  const { simulationData, setSaveBehaviorMode } = useSimulation()

  return (
    <div className="flex items-center gap-2">
      <Label htmlFor="save-behavior-select" className="text-xs text-muted-foreground cursor-pointer whitespace-nowrap">
        Ctrl+S:
      </Label>

      <Select value={simulationData.saveBehaviorMode} onValueChange={setSaveBehaviorMode}>
        <SelectTrigger id="save-behavior-select" className="h-7 w-[160px] text-xs">
          <SelectValue />
        </SelectTrigger>
        <SelectContent>
          {SAVE_BEHAVIOR_OPTIONS.map((option) => {
            const Icon = iconMap[option.icon as keyof typeof iconMap]
            return (
              <SelectItem key={option.value} value={option.value} className="text-xs">
                <div className="flex items-center gap-2">
                  <Icon className="h-3 w-3" />
                  <span>{option.label}</span>
                </div>
              </SelectItem>
            )
          })}
        </SelectContent>
      </Select>

      <TooltipProvider>
        <Tooltip>
          <TooltipTrigger asChild>
            <Info className="h-3.5 w-3.5 text-muted-foreground cursor-help" />
          </TooltipTrigger>
          <TooltipContent side="right" className="max-w-xs">
            <div className="space-y-2 text-xs">
              {SAVE_BEHAVIOR_OPTIONS.map((option) => (
                <div key={option.value}>
                  <strong>{option.label}:</strong> {option.description}
                </div>
              ))}
            </div>
          </TooltipContent>
        </Tooltip>
      </TooltipProvider>
    </div>
  )
}