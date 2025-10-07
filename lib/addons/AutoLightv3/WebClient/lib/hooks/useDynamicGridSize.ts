import { useEffect, useRef, useState, RefObject } from 'react'

interface UseDynamicGridSizeOptions {
  rows: number
  cols: number
  gap?: number
  minCellSize?: number
  maxCellSize?: number
}

interface UseDynamicGridSizeReturn {
  containerRef: RefObject<HTMLDivElement | null>
  cellSize: number
  isOverflowing: boolean
  containerSize: { width: number; height: number }
}

export function useDynamicGridSize({
  rows,
  cols,
  gap = 8,
  minCellSize = 10,
  maxCellSize = 48
}: UseDynamicGridSizeOptions): UseDynamicGridSizeReturn {
  const containerRef = useRef<HTMLDivElement>(null)
  const [cellSize, setCellSize] = useState(48)
  const [containerSize, setContainerSize] = useState({ width: 0, height: 0 })
  const [isOverflowing, setIsOverflowing] = useState(false)

  useEffect(() => {
    const container = containerRef.current
    if (!container) return

    const resizeObserver = new ResizeObserver((entries) => {
      for (const entry of entries) {
        const { width, height } = entry.contentRect
        setContainerSize({ width, height })

        const padding = 24
        const availableWidth = width - padding * 2
        const availableHeight = height - padding * 2

        const totalGapWidth = (cols - 1) * gap
        const totalGapHeight = (rows - 1) * gap

        const maxCellWidth = (availableWidth - totalGapWidth) / cols
        const maxCellHeight = (availableHeight - totalGapHeight) / rows

        let calculatedSize = Math.floor(Math.min(maxCellWidth, maxCellHeight))

        calculatedSize = Math.max(minCellSize, Math.min(calculatedSize, maxCellSize))

        setIsOverflowing(calculatedSize === minCellSize)

        setCellSize(calculatedSize)
      }
    })

    resizeObserver.observe(container)
    return () => resizeObserver.disconnect()
  }, [rows, cols, gap, minCellSize, maxCellSize])

  return { containerRef, cellSize, isOverflowing, containerSize }
}
