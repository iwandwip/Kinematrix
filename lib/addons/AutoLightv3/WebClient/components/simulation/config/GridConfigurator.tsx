"use client"

import { useState } from "react"
import { Button } from "@/components/ui/button"
import { Input } from "@/components/ui/input"
import { Label } from "@/components/ui/label"
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card"
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select"
import { Grid3X3, Settings, ArrowRight } from "lucide-react"

interface GridConfig {
  rows: number
  cols: number
  totalChannels: number
  layoutType: 'custom' | 'matrix' | 'line' | 'circle'
}

interface GridConfiguratorProps {
  onGridConfigured: (config: GridConfig) => void
}

export default function GridConfigurator({ onGridConfigured }: GridConfiguratorProps) {
  const [rows, setRows] = useState(6)
  const [cols, setCols] = useState(6)
  const [totalChannels, setTotalChannels] = useState(24)
  const [layoutType, setLayoutType] = useState<'custom' | 'matrix' | 'line' | 'circle'>('custom')

  const maxPositions = rows * cols
  const isValid = totalChannels <= maxPositions && totalChannels > 0 && rows > 0 && cols > 0

  const handleRowsChange = (value: string) => {
    const newRows = Math.max(1, Math.min(64, parseInt(value) || 1))
    setRows(newRows)
    if (layoutType === 'matrix') {
      setTotalChannels(Math.min(totalChannels, newRows * cols))
    }
  }

  const handleColsChange = (value: string) => {
    const newCols = Math.max(1, Math.min(64, parseInt(value) || 1))
    setCols(newCols)
    if (layoutType === 'matrix') {
      setTotalChannels(Math.min(totalChannels, rows * newCols))
    }
  }

  const handleChannelsChange = (value: string) => {
    const newChannels = Math.max(1, Math.min(maxPositions, parseInt(value) || 1))
    setTotalChannels(newChannels)
  }

  const handleLayoutTypeChange = (value: string) => {
    const newLayoutType = value as 'custom' | 'matrix' | 'line' | 'circle'
    setLayoutType(newLayoutType)

    if (newLayoutType === 'matrix') {
      setTotalChannels(rows * cols)
    } else if (newLayoutType === 'line') {
      setTotalChannels(Math.min(totalChannels, Math.max(rows, cols)))
    }
  }

  const handleConfigure = () => {
    if (isValid) {
      onGridConfigured({
        rows,
        cols,
        totalChannels,
        layoutType
      })
    }
  }

  const getLayoutDescription = () => {
    switch (layoutType) {
      case 'matrix':
        return `Full matrix layout using all ${maxPositions} positions`
      case 'line':
        return `Linear arrangement of ${totalChannels} channels`
      case 'circle':
        return `Circular arrangement of ${totalChannels} channels`
      case 'custom':
        return `Custom layout with ${totalChannels} channels in ${rows}×${cols} grid`
      default:
        return ''
    }
  }

  return (
    <Card>
      <CardHeader className="pb-3">
        <CardTitle className="text-sm flex items-center gap-2">
          <Grid3X3 className="h-4 w-4" />
          Grid Configuration
        </CardTitle>
      </CardHeader>
      <CardContent className="space-y-4">
        {/* Grid Dimensions */}
        <div className="grid grid-cols-2 gap-3">
          <div className="space-y-1">
            <Label htmlFor="rows" className="text-xs">Rows</Label>
            <Input
              id="rows"
              type="number"
              value={rows}
              onChange={(e) => handleRowsChange(e.target.value)}
              min={1}
              max={20}
              className="h-8"
            />
          </div>
          <div className="space-y-1">
            <Label htmlFor="cols" className="text-xs">Columns</Label>
            <Input
              id="cols"
              type="number"
              value={cols}
              onChange={(e) => handleColsChange(e.target.value)}
              min={1}
              max={20}
              className="h-8"
            />
          </div>
        </div>

        {/* Layout Type */}
        <div className="space-y-1">
          <Label className="text-xs">Layout Type</Label>
          <Select value={layoutType} onValueChange={handleLayoutTypeChange}>
            <SelectTrigger className="h-8">
              <SelectValue />
            </SelectTrigger>
            <SelectContent>
              <SelectItem value="custom">Custom Layout</SelectItem>
              <SelectItem value="matrix">Full Matrix</SelectItem>
              <SelectItem value="line">Linear</SelectItem>
              <SelectItem value="circle">Circular</SelectItem>
            </SelectContent>
          </Select>
        </div>

        {/* Total Channels */}
        <div className="space-y-1">
          <Label htmlFor="channels" className="text-xs">
            Total Channels (max: {maxPositions})
          </Label>
          <Input
            id="channels"
            type="number"
            value={totalChannels}
            onChange={(e) => handleChannelsChange(e.target.value)}
            min={1}
            max={maxPositions}
            className="h-8"
            disabled={layoutType === 'matrix'}
          />
        </div>

        {/* Grid Preview */}
        <div className="space-y-2">
          <Label className="text-xs">Grid Preview</Label>
          <div className="p-3 bg-muted/30 rounded-md border">
            <div className="text-xs text-center space-y-1">
              <div className="font-mono">
                {rows} × {cols} = {maxPositions} positions
              </div>
              <div className="text-muted-foreground">
                {getLayoutDescription()}
              </div>
            </div>
          </div>
        </div>

        {/* Validation */}
        {!isValid && (
          <div className="text-xs text-destructive">
            {totalChannels > maxPositions && `Too many channels! Max: ${maxPositions}`}
            {totalChannels <= 0 && "Channels must be greater than 0"}
            {(rows <= 0 || cols <= 0) && "Rows and columns must be greater than 0"}
          </div>
        )}

        {/* Configure Button */}
        <Button
          onClick={handleConfigure}
          disabled={!isValid}
          size="sm"
          className="w-full flex items-center gap-1"
        >
          <Settings className="h-3 w-3" />
          Configure Grid
          <ArrowRight className="h-3 w-3" />
        </Button>
      </CardContent>
    </Card>
  )
}