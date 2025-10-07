import { create } from 'zustand'
import { persist, createJSONStorage } from 'zustand/middleware'
import type { SaveBehaviorMode } from '@/lib/types/simulationTypes'
import { getDefaultCode } from '@/lib/simulation/javascriptPresets'

interface GridConfig {
  rows: number
  cols: number
  totalChannels: number
  layoutType: 'custom' | 'matrix' | 'line' | 'circle'
  channelMapping: [string, number | null][]
}

interface PatternBuilderState {
  configurationStep: 'grid' | 'mapping' | 'pattern'
  gridConfig: GridConfig | null
  patternMode: 'visual' | 'cpp' | 'expression' | 'javascript'

  cppCode: string
  javascriptCode: string
  expressionChannelCount: number

  playbackSpeed: number
  saveBehaviorMode: SaveBehaviorMode

  setConfigurationStep: (step: 'grid' | 'mapping' | 'pattern') => void
  setGridConfig: (config: GridConfig) => void
  setPatternMode: (mode: 'visual' | 'cpp' | 'expression' | 'javascript') => void

  setCppCode: (code: string) => void
  setJavaScriptCode: (code: string) => void
  setExpressionChannelCount: (count: number) => void

  setPlaybackSpeed: (speed: number) => void
  setSaveBehaviorMode: (mode: SaveBehaviorMode) => void

  reset: () => void
}

const createDefaultGridConfig = (): GridConfig => {
  const rows = 6
  const cols = 6
  const totalChannels = 24
  const channelMapping: [string, number | null][] = []

  let channelIndex = 0
  for (let row = 0; row < rows; row++) {
    for (let col = 0; col < cols; col++) {
      if (channelIndex < totalChannels) {
        channelMapping.push([`${row},${col}`, channelIndex])
        channelIndex++
      }
    }
  }

  return {
    rows,
    cols,
    totalChannels,
    layoutType: 'matrix',
    channelMapping
  }
}

const defaultCppCode = `void BaseChannel::taskSequenceAdvanced() {
    for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
        set(config_data_ptr_->header.pin_ptr_[i], HIGH);
        sleep(channel_data_.delay_time_);
        set(config_data_ptr_->header.pin_ptr_[i], LOW);
    }

    off();
}`

const initialState = {
  configurationStep: 'pattern' as const,
  gridConfig: createDefaultGridConfig(),
  patternMode: 'javascript' as const,
  cppCode: defaultCppCode,
  javascriptCode: getDefaultCode(),
  expressionChannelCount: 24,
  playbackSpeed: 1,
  saveBehaviorMode: 'page-wide' as SaveBehaviorMode,
}

export const usePatternBuilderStore = create<PatternBuilderState>()(
  persist(
    (set) => ({
      ...initialState,

      setConfigurationStep: (step) => set({ configurationStep: step }),

      setGridConfig: (config) => set({
        gridConfig: {
          ...config,
          channelMapping: config.channelMapping instanceof Map
            ? [...config.channelMapping.entries()]
            : config.channelMapping
        } as GridConfig
      }),

      setPatternMode: (mode) => set({ patternMode: mode }),

      setCppCode: (code) => set({ cppCode: code }),

      setJavaScriptCode: (code) => set({ javascriptCode: code }),

      setExpressionChannelCount: (count) => set({ expressionChannelCount: count }),

      setPlaybackSpeed: (speed) => set({ playbackSpeed: speed }),

      setSaveBehaviorMode: (mode) => set({ saveBehaviorMode: mode }),

      reset: () => set(initialState),
    }),
    {
      name: 'pattern-builder-storage',
      version: 2,
      storage: createJSONStorage(() => localStorage),

      partialize: (state) => ({
        configurationStep: state.configurationStep,
        gridConfig: state.gridConfig,
        patternMode: state.patternMode,
        cppCode: state.cppCode,
        javascriptCode: state.javascriptCode,
        expressionChannelCount: state.expressionChannelCount,
        playbackSpeed: state.playbackSpeed,
        saveBehaviorMode: state.saveBehaviorMode,
      }),

      migrate: (persistedState: unknown, version: number) => {
        const state = persistedState as Partial<PatternBuilderState>

        if (version < 2) {
          return {
            ...state,
            javascriptCode: getDefaultCode(),
            saveBehaviorMode: state.saveBehaviorMode || 'page-wide',
          }
        }

        return state
      },
    }
  )
)

export const getChannelMapFromStore = (gridConfig: GridConfig | null): Map<string, number | null> => {
  if (!gridConfig) return new Map()
  return new Map(gridConfig.channelMapping)
}
