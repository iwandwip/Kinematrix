"use client"

import { Button } from "@/components/ui/button"
import { Badge } from "@/components/ui/badge"
import { Maximize2, Minimize2, Grid3x3 } from "lucide-react"

interface GridViewControlsProps {
  viewMode: 'default' | 'fit' | 'scroll'
  onViewModeChange: (mode: 'default' | 'fit' | 'scroll') => void
  gridInfo: {
    rows: number
    cols: number
    totalCells: number
    cellSize: number
  }
  isOverflowing: boolean
}

export function GridViewControls({
  viewMode,
  onViewModeChange,
  gridInfo,
  isOverflowing
}: GridViewControlsProps) {
  return (
    <div className="flex items-center justify-between px-2 py-1.5 bg-muted/30 rounded-md border">
      <div className="flex gap-1.5">
        <Button
          variant={viewMode === 'default' ? 'default' : 'outline'}
          size="sm"
          onClick={() => onViewModeChange('default')}
          className="h-7 px-2 text-xs"
        >
          <Grid3x3 className="h-3 w-3 mr-1" />
          Default
        </Button>
        <Button
          variant={viewMode === 'fit' ? 'default' : 'outline'}
          size="sm"
          onClick={() => onViewModeChange('fit')}
          className="h-7 px-2 text-xs"
        >
          <Maximize2 className="h-3 w-3 mr-1" />
          Fit to View
        </Button>
        <Button
          variant={viewMode === 'scroll' ? 'default' : 'outline'}
          size="sm"
          onClick={() => onViewModeChange('scroll')}
          className="h-7 px-2 text-xs"
        >
          <Minimize2 className="h-3 w-3 mr-1" />
          Scroll Mode
        </Button>
      </div>

      <div className="flex items-center gap-2">
        <Badge variant="secondary" className="text-xs font-mono px-2 py-0.5">
          {gridInfo.rows}×{gridInfo.cols}
        </Badge>
        <Badge variant="outline" className="text-xs font-mono px-2 py-0.5">
          {gridInfo.totalCells} cells
        </Badge>
        <Badge variant="outline" className="text-xs font-mono px-2 py-0.5">
          {gridInfo.cellSize}px
        </Badge>
        {isOverflowing && viewMode !== 'default' && (
          <Badge variant="destructive" className="text-xs px-2 py-0.5">
            Min size
          </Badge>
        )}
      </div>
    </div>
  )
}
