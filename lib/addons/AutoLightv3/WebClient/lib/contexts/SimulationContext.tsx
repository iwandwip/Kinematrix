"use client"

import React, { createContext, useContext, useState, useCallback, useEffect, ReactNode, useRef } from 'react'
import { loadPatterns, savePattern as savePatternToStorage, deletePattern as deletePatternFromStorage } from '@/lib/utils/localStorage'
import { parseAndSimulateCppCode } from '@/lib/simulation/cppParser'
import { ExpressionFrameBuilder, type PatternExpression, type ExpressionFrame } from '@/lib/simulation/expressionParser'
import { JavaScriptEngine } from '@/lib/simulation/javascriptEngine'
import { getDefaultCode } from '@/lib/simulation/javascriptPresets'
import { usePrecisionPlayback } from '@/lib/hooks/usePrecisionPlayback'
import type { SaveBehaviorMode } from '@/lib/types/simulationTypes'
import { usePatternBuilderStore, getChannelMapFromStore } from '@/lib/store/patternBuilderStore'

export interface LEDFrame {
  id: string
  channels: boolean[]
  delay: number
  label?: string
}

export interface GridConfig {
  rows: number
  cols: number
  totalChannels: number
  layoutType: 'custom' | 'matrix' | 'line' | 'circle'
  channelMapping: Map<string, number | null>
}

export interface LEDPattern {
  id: string
  name: string
  description: string
  frames: LEDFrame[]
  settings: {
    defaultDelay: number
    totalChannels: number
    repeatCount: number
    direction: 'forward' | 'reverse' | 'alternate'
  }
  gridConfig?: GridConfig
}

interface SimulationData {
  currentPattern: LEDPattern
  selectedFrameIndex: number
  isPlaying: boolean
  isLooping: boolean
  playbackSpeed: number
  savedPatterns: LEDPattern[]
  configurationStep: 'grid' | 'mapping' | 'pattern'
  gridConfig: GridConfig | null
  patternMode: 'visual' | 'cpp' | 'expression' | 'javascript'
  cppCode: string
  expressionFrames: ExpressionFrame[]
  expressionChannelCount: number
  javascriptCode: string
  javascriptFrames: LEDFrame[]
  javascriptExecuting: boolean
  javascriptGenerating: boolean
  javascriptError: string | null
  isLoopingJavaScript: boolean
  loopCount: number
  javascriptCodeChanged: boolean
  saveBehaviorMode: SaveBehaviorMode
}

interface SimulationContextType {
  simulationData: SimulationData

  setPatternName: (name: string) => void
  setPatternDescription: (description: string) => void
  setDefaultDelay: (delay: number) => void
  setRepeatCount: (count: number) => void
  setDirection: (direction: 'forward' | 'reverse' | 'alternate') => void

  setConfigurationStep: (step: 'grid' | 'mapping' | 'pattern') => void
  setGridConfig: (config: GridConfig) => void
  updateChannelMapping: (mapping: Map<string, number | null>) => void

  addFrame: () => void
  removeFrame: (frameIndex: number) => void
  duplicateFrame: (frameIndex: number) => void
  selectFrame: (frameIndex: number) => void
  updateFrameChannels: (frameIndex: number, channels: boolean[]) => void
  updateFrameDelay: (frameIndex: number, delay: number) => void
  updateFrameLabel: (frameIndex: number, label: string) => void

  toggleChannel: (channelIndex: number) => void
  setAllChannels: (state: boolean) => void

  savePattern: () => void
  loadPattern: (patternId: string) => void
  deletePattern: (patternId: string) => void
  createNewPattern: () => void

  startPlayback: () => void
  stopPlayback: () => void
  startLooping: () => void
  stopLooping: () => void
  setPlaybackSpeed: (speed: number) => void

  setPatternMode: (mode: 'visual' | 'cpp' | 'expression' | 'javascript') => void
  setCppCode: (code: string) => void
  generateFramesFromCpp: (code: string) => void

  addExpressionFrame: (expression: PatternExpression, parameters?: Record<string, number | boolean | string>, duration?: number) => void
  clearExpressionFrames: () => void
  setExpressionChannelCount: (count: number) => void
  generatePatternFromExpressions: () => void

  setJavaScriptCode: (code: string) => void
  executeJavaScript: () => Promise<void>
  generateJavaScriptFrames: () => Promise<void>
  stopJavaScriptExecution: () => void
  clearJavaScriptFrames: () => void
  setJavaScriptError: (error: string | null) => void
  startJavaScriptLoop: () => void
  stopJavaScriptLoop: () => void
  setSaveBehaviorMode: (mode: SaveBehaviorMode) => void
}

const SimulationContext = createContext<SimulationContextType | undefined>(undefined)

const createEmptyFrame = (totalChannels: number = 24): LEDFrame => ({
  id: crypto.randomUUID(),
  channels: new Array(totalChannels).fill(false),
  delay: 50,
  label: ''
})

const createDefaultGridConfig = (): GridConfig => {
  const rows = 6
  const cols = 6
  const totalChannels = 24
  const channelMapping = new Map<string, number | null>()

  let channelIndex = 0
  for (let row = 0; row < rows; row++) {
    for (let col = 0; col < cols; col++) {
      if (channelIndex < totalChannels) {
        channelMapping.set(`${row},${col}`, channelIndex)
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

const createDefaultPattern = (gridConfig?: GridConfig): LEDPattern => {
  const config = gridConfig || createDefaultGridConfig()
  return {
    id: crypto.randomUUID(),
    name: 'New Pattern',
    description: '',
    frames: [createEmptyFrame(config.totalChannels)],
    settings: {
      defaultDelay: 30,
      totalChannels: config.totalChannels,
      repeatCount: 1,
      direction: 'forward'
    },
    gridConfig: config
  }
}

export function SimulationProvider({ children }: { children: ReactNode }) {
  const precisionPlayback = usePrecisionPlayback()
  const playbackCancelRef = useRef<(() => void) | null>(null)
  const zustandStore = usePatternBuilderStore()
  const isInitializedRef = useRef(false)

  const [simulationData, setSimulationData] = useState<SimulationData>({
    currentPattern: createDefaultPattern(),
    selectedFrameIndex: 0,
    isPlaying: false,
    isLooping: false,
    playbackSpeed: 1,
    savedPatterns: [],
    configurationStep: 'pattern',
    gridConfig: createDefaultGridConfig(),
    patternMode: 'javascript',
    expressionFrames: [],
    expressionChannelCount: 24,
    javascriptCode: getDefaultCode(),
    javascriptFrames: [],
    javascriptExecuting: false,
    javascriptGenerating: false,
    javascriptError: null,
    isLoopingJavaScript: false,
    loopCount: 0,
    javascriptCodeChanged: false,
    saveBehaviorMode: (typeof window !== 'undefined' ? localStorage.getItem('js-simulator-save-behavior') as SaveBehaviorMode : null) || 'page-wide',
    cppCode: `void BaseChannel::taskSequenceAdvanced() {
    // Advanced C++ Simulator - BaseChannelSequence.cpp Compatible

    // Pattern matrix support
    const int patternMode[9][8] = {
        {3, 4, 4, 1, 1, 4, 4, 3},  // For size 24
        {3, 3, 4, 1, 1, 4, 3, 3},  // For size 22
        {3, 3, 3, 1, 1, 3, 3, 3}   // For size 20
    };

    int arrData[config_data_ptr_->header.pin_size_];
    int arrSize = 0;
    int arrLastIndex = 0;

    // Lambda function example (BaseChannelSequence style)
    auto setRandomLed = [&](int value) -> void {
        int data[config_data_ptr_->header.pin_size_];
        int index;

        do {
            index = random(config_data_ptr_->header.pin_size_);
        } while (data[index] == value);

        data[index] = value;
        set(config_data_ptr_->header.pin_ptr_[index], value == 1 ? HIGH : LOW);
    };

    // Reset array utility
    resetArray(arrData, arrSize, arrLastIndex);

    // Mathematical operations
    int halfSize = config_data_ptr_->header.pin_size_ / 2;
    int maxChannels = findMax(halfSize, 8, 12);

    // Complex pattern with random elements
    for (int i = 0; i < 3; i++) {
        setRandomLed(1);
        sleep(channel_data_.delay_time_);
        setRandomLed(0);
        sleep(channel_data_.delay_time_);
    }

    // Traditional pattern with advanced math
    for (int i = 0; i < halfSize; i++) {
        int time = map(i, 0, halfSize, channel_data_.delay_time_ + 30, channel_data_.delay_time_ / 2);
        set(config_data_ptr_->header.pin_ptr_[i], HIGH);
        set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i], HIGH);
        sleep(time);
        set(config_data_ptr_->header.pin_ptr_[i], LOW);
        set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i], LOW);
    }

    off();
}`
  })

  useEffect(() => {
    const savedPatterns = loadPatterns()
    setSimulationData(prev => ({
      ...prev,
      savedPatterns
    }))
  }, [])

  useEffect(() => {
    if (!isInitializedRef.current && zustandStore.gridConfig) {
      const channelMapping = getChannelMapFromStore(zustandStore.gridConfig)

      const restoredGridConfig: GridConfig = {
        ...zustandStore.gridConfig,
        channelMapping
      }

      setSimulationData(prev => ({
        ...prev,
        configurationStep: zustandStore.configurationStep,
        gridConfig: restoredGridConfig,
        patternMode: zustandStore.patternMode,
        cppCode: zustandStore.cppCode,
        javascriptCode: zustandStore.javascriptCode,
        expressionChannelCount: zustandStore.expressionChannelCount,
        playbackSpeed: zustandStore.playbackSpeed,
        saveBehaviorMode: zustandStore.saveBehaviorMode,
        currentPattern: {
          ...prev.currentPattern,
          gridConfig: restoredGridConfig,
          settings: {
            ...prev.currentPattern.settings,
            totalChannels: restoredGridConfig.totalChannels
          }
        }
      }))

      isInitializedRef.current = true
    }
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [])

  useEffect(() => {
    if (!isInitializedRef.current) return
    zustandStore.setConfigurationStep(simulationData.configurationStep)
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [simulationData.configurationStep])

  useEffect(() => {
    if (!isInitializedRef.current || !simulationData.gridConfig) return
    const gridConfigWithArray = {
      ...simulationData.gridConfig,
      channelMapping: [...simulationData.gridConfig.channelMapping.entries()]
    }
    zustandStore.setGridConfig(gridConfigWithArray as typeof gridConfigWithArray & { channelMapping: [string, number | null][] })
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [simulationData.gridConfig])

  useEffect(() => {
    if (!isInitializedRef.current) return
    zustandStore.setPatternMode(simulationData.patternMode)
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [simulationData.patternMode])

  useEffect(() => {
    if (!isInitializedRef.current) return
    const timeoutId = setTimeout(() => {
      zustandStore.setCppCode(simulationData.cppCode)
    }, 500)
    return () => clearTimeout(timeoutId)
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [simulationData.cppCode])

  useEffect(() => {
    if (!isInitializedRef.current) return
    const timeoutId = setTimeout(() => {
      zustandStore.setJavaScriptCode(simulationData.javascriptCode)
    }, 500)
    return () => clearTimeout(timeoutId)
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [simulationData.javascriptCode])

  useEffect(() => {
    if (!isInitializedRef.current) return
    zustandStore.setExpressionChannelCount(simulationData.expressionChannelCount)
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [simulationData.expressionChannelCount])

  useEffect(() => {
    if (!isInitializedRef.current) return
    zustandStore.setPlaybackSpeed(simulationData.playbackSpeed)
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [simulationData.playbackSpeed])

  useEffect(() => {
    if (!isInitializedRef.current) return
    zustandStore.setSaveBehaviorMode(simulationData.saveBehaviorMode)
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [simulationData.saveBehaviorMode])

  const setPatternName = useCallback((name: string) => {
    setSimulationData(prev => ({
      ...prev,
      currentPattern: {
        ...prev.currentPattern,
        name
      }
    }))
  }, [])

  const setPatternDescription = useCallback((description: string) => {
    setSimulationData(prev => ({
      ...prev,
      currentPattern: {
        ...prev.currentPattern,
        description
      }
    }))
  }, [])

  const setDefaultDelay = useCallback((delay: number) => {
    setSimulationData(prev => ({
      ...prev,
      currentPattern: {
        ...prev.currentPattern,
        settings: {
          ...prev.currentPattern.settings,
          defaultDelay: delay
        }
      }
    }))
  }, [])

  const setRepeatCount = useCallback((count: number) => {
    setSimulationData(prev => ({
      ...prev,
      currentPattern: {
        ...prev.currentPattern,
        settings: {
          ...prev.currentPattern.settings,
          repeatCount: count
        }
      }
    }))
  }, [])

  const setDirection = useCallback((direction: 'forward' | 'reverse' | 'alternate') => {
    setSimulationData(prev => ({
      ...prev,
      currentPattern: {
        ...prev.currentPattern,
        settings: {
          ...prev.currentPattern.settings,
          direction
        }
      }
    }))
  }, [])

  const addFrame = useCallback(() => {
    setSimulationData(prev => {
      const newFrame = createEmptyFrame(prev.currentPattern.settings.totalChannels)
      return {
        ...prev,
        currentPattern: {
          ...prev.currentPattern,
          frames: [...prev.currentPattern.frames, newFrame]
        },
        selectedFrameIndex: prev.currentPattern.frames.length
      }
    })
  }, [])

  const removeFrame = useCallback((frameIndex: number) => {
    setSimulationData(prev => {
      if (prev.currentPattern.frames.length <= 1) return prev

      const newFrames = prev.currentPattern.frames.filter((_, index) => index !== frameIndex)
      const newSelectedIndex = Math.min(prev.selectedFrameIndex, newFrames.length - 1)

      return {
        ...prev,
        currentPattern: {
          ...prev.currentPattern,
          frames: newFrames
        },
        selectedFrameIndex: newSelectedIndex
      }
    })
  }, [])

  const duplicateFrame = useCallback((frameIndex: number) => {
    setSimulationData(prev => {
      const frameToClone = prev.currentPattern.frames[frameIndex]
      const duplicatedFrame: LEDFrame = {
        ...frameToClone,
        id: crypto.randomUUID(),
        channels: [...frameToClone.channels]
      }

      const newFrames = [...prev.currentPattern.frames]
      newFrames.splice(frameIndex + 1, 0, duplicatedFrame)

      return {
        ...prev,
        currentPattern: {
          ...prev.currentPattern,
          frames: newFrames
        },
        selectedFrameIndex: frameIndex + 1
      }
    })
  }, [])

  const selectFrame = useCallback((frameIndex: number) => {
    setSimulationData(prev => ({
      ...prev,
      selectedFrameIndex: frameIndex
    }))
  }, [])

  const updateFrameChannels = useCallback((frameIndex: number, channels: boolean[]) => {
    setSimulationData(prev => {
      const newFrames = [...prev.currentPattern.frames]
      newFrames[frameIndex] = {
        ...newFrames[frameIndex],
        channels: [...channels]
      }

      return {
        ...prev,
        currentPattern: {
          ...prev.currentPattern,
          frames: newFrames
        }
      }
    })
  }, [])

  const updateFrameDelay = useCallback((frameIndex: number, delay: number) => {
    setSimulationData(prev => {
      const newFrames = [...prev.currentPattern.frames]
      newFrames[frameIndex] = {
        ...newFrames[frameIndex],
        delay
      }

      return {
        ...prev,
        currentPattern: {
          ...prev.currentPattern,
          frames: newFrames
        }
      }
    })
  }, [])

  const updateFrameLabel = useCallback((frameIndex: number, label: string) => {
    setSimulationData(prev => {
      const newFrames = [...prev.currentPattern.frames]
      newFrames[frameIndex] = {
        ...newFrames[frameIndex],
        label
      }

      return {
        ...prev,
        currentPattern: {
          ...prev.currentPattern,
          frames: newFrames
        }
      }
    })
  }, [])

  const toggleChannel = useCallback((channelIndex: number) => {
    setSimulationData(prev => {
      const currentFrame = prev.currentPattern.frames[prev.selectedFrameIndex]
      const newChannels = [...currentFrame.channels]
      newChannels[channelIndex] = !newChannels[channelIndex]

      const newFrames = [...prev.currentPattern.frames]
      newFrames[prev.selectedFrameIndex] = {
        ...currentFrame,
        channels: newChannels
      }

      return {
        ...prev,
        currentPattern: {
          ...prev.currentPattern,
          frames: newFrames
        }
      }
    })
  }, [])

  const setAllChannels = useCallback((state: boolean) => {
    setSimulationData(prev => {
      const currentFrame = prev.currentPattern.frames[prev.selectedFrameIndex]
      const newChannels = new Array(prev.currentPattern.settings.totalChannels).fill(state)

      const newFrames = [...prev.currentPattern.frames]
      newFrames[prev.selectedFrameIndex] = {
        ...currentFrame,
        channels: newChannels
      }

      return {
        ...prev,
        currentPattern: {
          ...prev.currentPattern,
          frames: newFrames
        }
      }
    })
  }, [])

  const savePattern = useCallback(() => {
    setSimulationData(prev => {
      const patternToSave = { ...prev.currentPattern }
      savePatternToStorage(patternToSave)

      const existingIndex = prev.savedPatterns.findIndex(p => p.id === patternToSave.id)
      let newSavedPatterns

      if (existingIndex >= 0) {
        newSavedPatterns = [...prev.savedPatterns]
        newSavedPatterns[existingIndex] = patternToSave
      } else {
        newSavedPatterns = [...prev.savedPatterns, patternToSave]
      }

      return {
        ...prev,
        savedPatterns: newSavedPatterns
      }
    })
  }, [])

  const loadPattern = useCallback((patternId: string) => {
    setSimulationData(prev => {
      const pattern = prev.savedPatterns.find(p => p.id === patternId)
      if (!pattern) return prev

      return {
        ...prev,
        currentPattern: { ...pattern },
        selectedFrameIndex: 0,
        isPlaying: false
      }
    })
  }, [])

  const deletePattern = useCallback((patternId: string) => {
    setSimulationData(prev => {
      deletePatternFromStorage(patternId)
      const newSavedPatterns = prev.savedPatterns.filter(p => p.id !== patternId)

      return {
        ...prev,
        savedPatterns: newSavedPatterns
      }
    })
  }, [])

  const createNewPattern = useCallback(() => {
    setSimulationData(prev => ({
      ...prev,
      currentPattern: createDefaultPattern(),
      selectedFrameIndex: 0,
      isPlaying: false
    }))
  }, [])

  const startPlayback = useCallback(() => {
    precisionPlayback.startPlayback()
    setSimulationData(prev => ({
      ...prev,
      isPlaying: true,
      selectedFrameIndex: 0 // Start from beginning
    }))
  }, [precisionPlayback])

  const stopPlayback = useCallback(() => {
    precisionPlayback.stopPlayback()
    if (playbackCancelRef.current) {
      playbackCancelRef.current()
      playbackCancelRef.current = null
    }
    setSimulationData(prev => ({
      ...prev,
      isPlaying: false
    }))
  }, [precisionPlayback])

  const startLooping = useCallback(() => {
    precisionPlayback.startPlayback()
    setSimulationData(prev => ({
      ...prev,
      isLooping: true,
      isPlaying: true,
      selectedFrameIndex: 0
    }))
  }, [precisionPlayback])

  const stopLooping = useCallback(() => {
    precisionPlayback.stopPlayback()
    if (playbackCancelRef.current) {
      playbackCancelRef.current()
      playbackCancelRef.current = null
    }
    setSimulationData(prev => ({
      ...prev,
      isLooping: false,
      isPlaying: false,
      selectedFrameIndex: 0
    }))
  }, [precisionPlayback])

  const setPlaybackSpeed = useCallback((speed: number) => {
    setSimulationData(prev => ({
      ...prev,
      playbackSpeed: speed
    }))
  }, [])

  const setConfigurationStep = useCallback((step: 'grid' | 'mapping' | 'pattern') => {
    setSimulationData(prev => ({
      ...prev,
      configurationStep: step
    }))
  }, [])

  const setGridConfig = useCallback((config: GridConfig) => {
    setSimulationData(prev => {
      const newPattern = {
        ...prev.currentPattern,
        settings: {
          ...prev.currentPattern.settings,
          totalChannels: config.totalChannels
        },
        gridConfig: config,
        frames: prev.currentPattern.frames.map(frame => ({
          ...frame,
          channels: new Array(config.totalChannels).fill(false)
        }))
      }

      return {
        ...prev,
        currentPattern: newPattern,
        gridConfig: config
      }
    })
  }, [])

  const updateChannelMapping = useCallback((mapping: Map<string, number | null>) => {
    setSimulationData(prev => {
      if (!prev.gridConfig) return prev

      const newGridConfig = {
        ...prev.gridConfig,
        channelMapping: mapping
      }

      return {
        ...prev,
        gridConfig: newGridConfig,
        currentPattern: {
          ...prev.currentPattern,
          gridConfig: newGridConfig
        }
      }
    })
  }, [])

  const setPatternMode = useCallback((mode: 'visual' | 'cpp' | 'expression' | 'javascript') => {
    setSimulationData(prev => ({
      ...prev,
      patternMode: mode
    }))
  }, [])

  const setCppCode = useCallback((code: string) => {
    setSimulationData(prev => ({
      ...prev,
      cppCode: code
    }))
  }, [])

  const generateFramesFromCpp = useCallback((code: string) => {
    setSimulationData(prev => {
      const gridConfig = prev.gridConfig || prev.currentPattern.gridConfig
      if (!gridConfig) return prev

      const totalChannels = gridConfig.totalChannels

      try {
        const frames = parseAndSimulateCppCode(code, totalChannels, 50)

        return {
          ...prev,
          currentPattern: {
            ...prev.currentPattern,
            frames,
            name: prev.currentPattern.name + ' (C++ Generated)'
          },
          selectedFrameIndex: 0
        }
      } catch (error) {
        console.error('C++ parsing error:', error)
        return prev
      }
    })
  }, [])

  const addExpressionFrame = useCallback((expression: PatternExpression, parameters: Record<string, number | boolean | string> = {}, duration: number = 100) => {
    setSimulationData(prev => {
      const frameBuilder = new ExpressionFrameBuilder(prev.expressionChannelCount)
      const frame = frameBuilder.addFrame(expression, parameters, duration)

      return {
        ...prev,
        expressionFrames: [...prev.expressionFrames, frame]
      }
    })
  }, [])

  const clearExpressionFrames = useCallback(() => {
    setSimulationData(prev => ({
      ...prev,
      expressionFrames: []
    }))
  }, [])

  const setExpressionChannelCount = useCallback((count: number) => {
    setSimulationData(prev => ({
      ...prev,
      expressionChannelCount: count,
      expressionFrames: []
    }))
  }, [])

  const generatePatternFromExpressions = useCallback(() => {
    setSimulationData(prev => {
      if (prev.expressionFrames.length === 0) return prev

      const frames: LEDFrame[] = prev.expressionFrames.map(expressionFrame => ({
        id: crypto.randomUUID(),
        channels: expressionFrame.channels,
        delay: expressionFrame.duration,
        label: expressionFrame.label || ''
      }))

      const newPattern: LEDPattern = {
        ...prev.currentPattern,
        name: prev.currentPattern.name + ' (Expression Generated)',
        frames,
        settings: {
          ...prev.currentPattern.settings,
          totalChannels: prev.expressionChannelCount
        }
      }

      return {
        ...prev,
        currentPattern: newPattern,
        selectedFrameIndex: 0
      }
    })
  }, [])

  // JavaScript Simulator functions
  const javascriptEngineRef = React.useRef<JavaScriptEngine>(new JavaScriptEngine())

  const setJavaScriptCode = useCallback((code: string) => {
    setSimulationData(prev => ({
      ...prev,
      javascriptCode: code,
      javascriptError: null,
      javascriptCodeChanged: true
    }))
  }, [])

  const executeJavaScript = useCallback(async () => {
    setSimulationData(prev => ({
      ...prev,
      javascriptExecuting: true,
      javascriptError: null
    }))

    try {
      const gridConfig = simulationData.gridConfig || simulationData.currentPattern.gridConfig
      if (!gridConfig) {
        throw new Error('Grid configuration not found')
      }

      const totalChannels = gridConfig.totalChannels
      const delayTime = simulationData.currentPattern.settings.defaultDelay

      const frames = await javascriptEngineRef.current.executeCode(
        simulationData.javascriptCode,
        totalChannels,
        delayTime
      )

      setSimulationData(prev => ({
        ...prev,
        javascriptFrames: frames,
        javascriptExecuting: false,
        selectedFrameIndex: 0,
        currentPattern: {
          ...prev.currentPattern,
          frames,
          name: prev.currentPattern.name + ' (JavaScript Generated)',
          settings: {
            ...prev.currentPattern.settings,
            totalChannels: totalChannels
          }
        }
      }))
    } catch (error) {
      setSimulationData(prev => ({
        ...prev,
        javascriptExecuting: false,
        javascriptError: error instanceof Error ? error.message : String(error)
      }))
    }
  }, [simulationData.javascriptCode, simulationData.gridConfig, simulationData.currentPattern])

  const generateJavaScriptFrames = useCallback(async () => {
    setSimulationData(prev => ({
      ...prev,
      javascriptGenerating: true,
      javascriptError: null
    }))

    try {
      const gridConfig = simulationData.gridConfig || simulationData.currentPattern.gridConfig
      if (!gridConfig) {
        throw new Error('Grid configuration not found')
      }

      const totalChannels = gridConfig.totalChannels
      const delayTime = simulationData.currentPattern.settings.defaultDelay

      const frames = await javascriptEngineRef.current.generateFramesOnly(
        simulationData.javascriptCode,
        totalChannels,
        delayTime
      )

      setSimulationData(prev => ({
        ...prev,
        javascriptFrames: frames,
        javascriptGenerating: false,
        javascriptCodeChanged: false,
        selectedFrameIndex: 0,
        currentPattern: {
          ...prev.currentPattern,
          frames,
          name: prev.currentPattern.name + ' (Generated)',
          settings: {
            ...prev.currentPattern.settings,
            totalChannels: totalChannels
          }
        }
      }))
    } catch (error) {
      setSimulationData(prev => ({
        ...prev,
        javascriptGenerating: false,
        javascriptError: error instanceof Error ? error.message : String(error)
      }))
    }
  }, [simulationData.javascriptCode, simulationData.gridConfig, simulationData.currentPattern])

  const stopJavaScriptExecution = useCallback(() => {
    javascriptEngineRef.current.stopExecution()
    setSimulationData(prev => ({
      ...prev,
      javascriptExecuting: false,
      javascriptGenerating: false
    }))
  }, [])

  const clearJavaScriptFrames = useCallback(() => {
    setSimulationData(prev => ({
      ...prev,
      javascriptFrames: [],
      javascriptError: null,
      selectedFrameIndex: 0
    }))
  }, [])

  const setJavaScriptError = useCallback((error: string | null) => {
    setSimulationData(prev => ({
      ...prev,
      javascriptError: error
    }))
  }, [])

  const startJavaScriptLoop = useCallback(() => {
    if (simulationData.currentPattern.frames.length === 0) {
      console.warn('[JavaScriptLoop] Cannot start loop: No frames available')
      return
    }

    precisionPlayback.startPlayback()
    setSimulationData(prev => ({
      ...prev,
      isLoopingJavaScript: true,
      isPlaying: true,
      isLooping: true,
      loopCount: 0,
      selectedFrameIndex: 0
    }))
  }, [precisionPlayback, simulationData.currentPattern.frames.length])

  const stopJavaScriptLoop = useCallback(() => {
    precisionPlayback.stopPlayback()
    if (playbackCancelRef.current) {
      playbackCancelRef.current()
      playbackCancelRef.current = null
    }
    setSimulationData(prev => ({
      ...prev,
      isLoopingJavaScript: false,
      isPlaying: false,
      isLooping: false,
      selectedFrameIndex: 0
    }))
  }, [precisionPlayback])

  const setSaveBehaviorMode = useCallback((mode: SaveBehaviorMode) => {
    setSimulationData(prev => ({
      ...prev,
      saveBehaviorMode: mode
    }))
    if (typeof window !== 'undefined') {
      localStorage.setItem('js-simulator-save-behavior', mode)
    }
  }, [])

  // High-precision auto-playback effect for frame timing
  useEffect(() => {
    // Clear any existing playback
    if (playbackCancelRef.current) {
      playbackCancelRef.current()
      playbackCancelRef.current = null
    }

    if (simulationData.isPlaying && simulationData.currentPattern.frames.length > 0) {
      const currentFrame = simulationData.currentPattern.frames[simulationData.selectedFrameIndex]
      const delay = currentFrame?.delay || 100 // Default 100ms if no delay specified
      const adjustedDelay = delay / simulationData.playbackSpeed // Apply playback speed adjustment

      // Use precision timing for accurate delays
      playbackCancelRef.current = precisionPlayback.scheduleFrame({
        delay: adjustedDelay,
        onComplete: () => {
          const nextIndex = simulationData.selectedFrameIndex + 1

          if (nextIndex < simulationData.currentPattern.frames.length) {
            // Move to next frame
            setSimulationData(prev => ({
              ...prev,
              selectedFrameIndex: nextIndex
            }))
          } else if (simulationData.isLooping) {
            // Loop back to start
            setSimulationData(prev => ({
              ...prev,
              selectedFrameIndex: 0,
              loopCount: prev.loopCount + 1
            }))
          } else {
            // End of playback
            setSimulationData(prev => ({
              ...prev,
              isPlaying: false,
              selectedFrameIndex: 0
            }))
          }
        }
      })
    }

    return () => {
      if (playbackCancelRef.current) {
        playbackCancelRef.current()
        playbackCancelRef.current = null
      }
    }
  }, [simulationData.isPlaying, simulationData.selectedFrameIndex, simulationData.currentPattern.frames, simulationData.isLooping, simulationData.isLoopingJavaScript, simulationData.playbackSpeed, precisionPlayback])

  const contextValue: SimulationContextType = {
    simulationData,
    setPatternName,
    setPatternDescription,
    setDefaultDelay,
    setRepeatCount,
    setDirection,
    setConfigurationStep,
    setGridConfig,
    updateChannelMapping,
    addFrame,
    removeFrame,
    duplicateFrame,
    selectFrame,
    updateFrameChannels,
    updateFrameDelay,
    updateFrameLabel,
    toggleChannel,
    setAllChannels,
    savePattern,
    loadPattern,
    deletePattern,
    createNewPattern,
    startPlayback,
    stopPlayback,
    startLooping,
    stopLooping,
    setPlaybackSpeed,
    setPatternMode,
    setCppCode,
    generateFramesFromCpp,
    addExpressionFrame,
    clearExpressionFrames,
    setExpressionChannelCount,
    generatePatternFromExpressions,
    setJavaScriptCode,
    executeJavaScript,
    generateJavaScriptFrames,
    stopJavaScriptExecution,
    clearJavaScriptFrames,
    setJavaScriptError,
    startJavaScriptLoop,
    stopJavaScriptLoop,
    setSaveBehaviorMode
  }

  return (
    <SimulationContext.Provider value={contextValue}>
      {children}
    </SimulationContext.Provider>
  )
}

export function useSimulation() {
  const context = useContext(SimulationContext)
  if (context === undefined) {
    throw new Error('useSimulation must be used within a SimulationProvider')
  }
  return context
}