import { LEDPattern } from "@/lib/contexts/SimulationContext"

const STORAGE_KEY = 'autolight-patterns'

export function savePatterns(patterns: LEDPattern[]): void {
  try {
    localStorage.setItem(STORAGE_KEY, JSON.stringify(patterns))
  } catch (error) {
    console.error('Failed to save patterns to localStorage:', error)
  }
}

export function loadPatterns(): LEDPattern[] {
  try {
    const stored = localStorage.getItem(STORAGE_KEY)
    if (stored) {
      return JSON.parse(stored)
    }
  } catch (error) {
    console.error('Failed to load patterns from localStorage:', error)
  }
  return []
}

export function savePattern(pattern: LEDPattern): void {
  const patterns = loadPatterns()
  const existingIndex = patterns.findIndex(p => p.id === pattern.id)

  if (existingIndex >= 0) {
    patterns[existingIndex] = pattern
  } else {
    patterns.push(pattern)
  }

  savePatterns(patterns)
}

export function deletePattern(patternId: string): void {
  const patterns = loadPatterns()
  const filteredPatterns = patterns.filter(p => p.id !== patternId)
  savePatterns(filteredPatterns)
}

export function getPattern(patternId: string): LEDPattern | null {
  const patterns = loadPatterns()
  return patterns.find(p => p.id === patternId) || null
}

export function exportPatternsAsJson(): string {
  const patterns = loadPatterns()
  return JSON.stringify(patterns, null, 2)
}

export function importPatternsFromJson(jsonString: string): LEDPattern[] {
  try {
    const patterns = JSON.parse(jsonString)
    if (Array.isArray(patterns)) {
      return patterns.filter(p =>
        p && typeof p === 'object' &&
        p.id && p.name && Array.isArray(p.frames)
      )
    }
  } catch (error) {
    console.error('Failed to import patterns from JSON:', error)
  }
  return []
}

export function clearAllPatterns(): void {
  try {
    localStorage.removeItem(STORAGE_KEY)
  } catch (error) {
    console.error('Failed to clear patterns from localStorage:', error)
  }
}