import { GridConfig } from "@/lib/contexts/SimulationContext"

export function getGridPositionFromChannel(channel: number, gridConfig: GridConfig): { row: number; col: number } | null {
  for (const [positionKey, mappedChannel] of gridConfig.channelMapping.entries()) {
    if (mappedChannel === channel) {
      const [row, col] = positionKey.split(',').map(Number)
      return { row, col }
    }
  }
  return null
}

export function getChannelFromGridPosition(row: number, col: number, gridConfig: GridConfig): number | null {
  const positionKey = `${row},${col}`
  const channel = gridConfig.channelMapping.get(positionKey)
  return channel !== undefined ? channel : null
}

export function getPhysicalChannelMapping(gridConfig: GridConfig): Map<number, number> {
  const physicalMapping = new Map<number, number>()

  for (const [, logicalChannel] of gridConfig.channelMapping.entries()) {
    if (logicalChannel !== null) {
      physicalMapping.set(logicalChannel, logicalChannel)
    }
  }

  return physicalMapping
}

export function generateChannelComment(channel: number, gridConfig: GridConfig): string {
  const position = getGridPositionFromChannel(channel, gridConfig)
  if (position) {
    return `Channel ${channel} at [${position.row},${position.col}]`
  }
  return `Channel ${channel}`
}

export function isChannelMapped(channel: number, gridConfig: GridConfig): boolean {
  return getGridPositionFromChannel(channel, gridConfig) !== null
}

export function getMappedChannels(gridConfig: GridConfig): number[] {
  const channels: number[] = []
  for (const channel of gridConfig.channelMapping.values()) {
    if (channel !== null) {
      channels.push(channel)
    }
  }
  return channels.sort((a, b) => a - b)
}

export function getGridLayoutDescription(gridConfig: GridConfig): string {
  const { rows, cols, totalChannels, layoutType } = gridConfig

  switch (layoutType) {
    case 'matrix':
      return `${rows}×${cols} Matrix Layout (${totalChannels} channels)`
    case 'line':
      return `Linear Layout (${totalChannels} channels in line)`
    case 'circle':
      return `Circular Layout (${totalChannels} channels in circle)`
    case 'custom':
      return `Custom Layout (${totalChannels} channels)`
    default:
      return `${totalChannels} channels`
  }
}