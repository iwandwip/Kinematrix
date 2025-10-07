"use client"

import { useCallback, useEffect, useRef, useState } from "react"
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card"
import { Button } from "@/components/ui/button"
import { Badge } from "@/components/ui/badge"
import { Alert, AlertDescription } from "@/components/ui/alert"
import { Slider } from "@/components/ui/slider"
import { Switch } from "@/components/ui/switch"
import { Label } from "@/components/ui/label"
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from "@/components/ui/select"
import Editor, { useMonaco } from "@monaco-editor/react"
import type * as monaco from 'monaco-editor'
import { useSimulation } from "@/lib/contexts/SimulationContext"
import { useMonacoTheme } from "@/lib/hooks/useMonacoTheme"
import {
  Code,
  Play,
  RotateCcw,
  ChevronLeft,
  ChevronRight,
  Pause,
  Square,
  AlertTriangle,
  CheckCircle,
  Clock,
  Zap,
  HelpCircle,
  Sun,
  Moon,
  Repeat,
  Maximize2
} from "lucide-react"
import DynamicPatternGrid from "../grid/DynamicPatternGrid"
import FrameTimeline from "../FrameTimeline"
import { JavaScriptEngine, ExecutionStats } from "@/lib/simulation/javascriptEngine"
import { javascriptPresets, getDefaultCode, getTemplateHelp } from "@/lib/simulation/javascriptPresets"
import { defaultSafety } from "@/lib/utils/javascriptSafety"
import { SaveBehaviorSelector } from "../config/SaveBehaviorSelector"
import { useSaveBehavior } from "@/lib/hooks/useSaveBehavior"

const HEIGHT_PRESETS = [
  { label: "Compact (300px)", value: "300px" },
  { label: "Medium (400px)", value: "400px" },
  { label: "Large (500px)", value: "500px" },
  { label: "Extra Large (600px)", value: "600px" },
  { label: "Half Screen (50vh)", value: "50vh" },
  { label: "Full Screen (90vh)", value: "90vh" },
] as const

export default function JavaScriptSimulatorMode() {
  const {
    simulationData,
    selectFrame,
    setJavaScriptCode,
    generateJavaScriptFrames,
    stopJavaScriptExecution,
    setJavaScriptError,
    setPlaybackSpeed,
  } = useSimulation()

  useSaveBehavior()

  const [isExecuting, setIsExecuting] = useState(false)
  const [isGenerating, setIsGenerating] = useState(false)
  const [executionStats, setExecutionStats] = useState<ExecutionStats | null>(null)
  const [showHelp, setShowHelp] = useState(false)
  const [editorHeight, setEditorHeight] = useState<string>("400px")
  const [selectedPresetId, setSelectedPresetId] = useState<string>("")
  const { theme: monacoTheme, toggleTheme, isDarkMode } = useMonacoTheme()
  const monaco = useMonaco()
  const editorRef = useRef<monaco.editor.IStandaloneCodeEditor | null>(null)

  const keyDownDisposableRef = useRef<monaco.IDisposable | null>(null)
  const keyUpDisposableRef = useRef<monaco.IDisposable | null>(null)

  const engineRef = useRef<JavaScriptEngine>(new JavaScriptEngine())
  const autoPlayTimerRef = useRef<NodeJS.Timeout | null>(null)
  const isLocalPlayingRef = useRef(false)
  const isLocalLoopingRef = useRef(false)
  const localLoopCountRef = useRef(0)

  const handleCodeChange = useCallback((value: string) => {
    setJavaScriptCode(value)
    setJavaScriptError(null)
  }, [setJavaScriptCode, setJavaScriptError])

  const handleGenerateFrames = async () => {
    if (isGenerating || simulationData.javascriptGenerating) return

    setIsGenerating(true)
    setJavaScriptError(null)
    setExecutionStats(null)

    const codeValidation = defaultSafety.validateCode(simulationData.javascriptCode)
    if (!codeValidation.isValid) {
      setJavaScriptError(`Code validation failed: ${codeValidation.errors.join(', ')}`)
      setIsGenerating(false)
      return
    }

    try {
      await generateJavaScriptFrames()

      const stats = engineRef.current.getExecutionStats()
      const safetyResult = defaultSafety.validateExecution(
        stats.executionTime,
        stats.frameCount,
        stats.memoryUsage
      )

      if (!safetyResult.isValid) {
        setJavaScriptError(`Execution validation failed: ${safetyResult.errors.join(', ')}`)
      } else {
        setExecutionStats(stats)

        if (safetyResult.warnings.length > 0) {
          setJavaScriptError(`Warnings: ${safetyResult.warnings.join(', ')}`)
        }
      }
    } catch (error) {
      setJavaScriptError(error instanceof Error ? error.message : String(error))
    } finally {
      setIsGenerating(false)
    }
  }

  const handleInsertTemplate = (templateCode: string) => {
    setJavaScriptCode(templateCode)
    setJavaScriptError(null)
  }

  const handleResetCode = () => {
    setJavaScriptCode(getDefaultCode())
    setJavaScriptError(null)
    setExecutionStats(null)
  }

  const handlePresetSelect = (presetId: string) => {
    setSelectedPresetId(presetId)
    const selectedPreset = javascriptPresets.find(preset => preset.id === presetId)
    if (selectedPreset) {
      handleInsertTemplate(selectedPreset.code)
    }
  }

  const handleStopExecution = () => {
    stopJavaScriptExecution()
    setIsExecuting(false)
    setIsGenerating(false)

    if (autoPlayTimerRef.current) {
      clearTimeout(autoPlayTimerRef.current)
      autoPlayTimerRef.current = null
    }
    isLocalPlayingRef.current = false
    isLocalLoopingRef.current = false
  }

  const handleToggleHelp = () => {
    setShowHelp(!showHelp)
  }

  const handleHeightChange = (value: string) => {
    setEditorHeight(value)
    if (typeof window !== 'undefined') {
      localStorage.setItem('monaco-editor-height', value)
    }
  }

  const scheduleNextFrame = useCallback(() => {
    if (!isLocalPlayingRef.current && !isLocalLoopingRef.current) {
      return
    }

    const currentFrame = simulationData.currentPattern.frames[simulationData.selectedFrameIndex]
    const delay = currentFrame?.delay || 50
    // Performance: Enforce minimum delay for 60fps (16ms)
    const adjustedDelay = Math.max(16, delay / simulationData.playbackSpeed)

    // Clear existing timer
    if (autoPlayTimerRef.current) {
      clearTimeout(autoPlayTimerRef.current)
      autoPlayTimerRef.current = null
    }

    const timer = setTimeout(() => {
      const nextIndex = simulationData.selectedFrameIndex + 1
      if (nextIndex < simulationData.currentPattern.frames.length) {
        selectFrame(nextIndex)
      } else {
        if (isLocalLoopingRef.current) {
          localLoopCountRef.current += 1
          selectFrame(0)
        } else {
          isLocalPlayingRef.current = false
          isLocalLoopingRef.current = false
        }
      }
    }, adjustedDelay)

    autoPlayTimerRef.current = timer
  }, [simulationData.selectedFrameIndex, simulationData.currentPattern.frames, simulationData.playbackSpeed, selectFrame])

  const handleAutoPlay = () => {
    if (isLocalPlayingRef.current) {
      isLocalPlayingRef.current = false
      if (autoPlayTimerRef.current) {
        clearTimeout(autoPlayTimerRef.current)
        autoPlayTimerRef.current = null
      }
    } else {
      if (simulationData.currentPattern.frames.length === 0) return
      isLocalPlayingRef.current = true
      isLocalLoopingRef.current = false
      selectFrame(0)
      scheduleNextFrame()
    }
  }

  const handleStartLooping = () => {
    if (simulationData.currentPattern.frames.length === 0) return
    isLocalPlayingRef.current = true
    isLocalLoopingRef.current = true
    localLoopCountRef.current = 0
    selectFrame(0)
    scheduleNextFrame()
  }

  const handleStopLooping = () => {
    isLocalPlayingRef.current = false
    isLocalLoopingRef.current = false
    if (autoPlayTimerRef.current) {
      clearTimeout(autoPlayTimerRef.current)
      autoPlayTimerRef.current = null
    }
  }

  useEffect(() => {
    if ((isLocalPlayingRef.current || isLocalLoopingRef.current) && simulationData.currentPattern.frames.length > 0) {
      scheduleNextFrame()
    }
  }, [simulationData.selectedFrameIndex, scheduleNextFrame, simulationData.currentPattern.frames.length])

  useEffect(() => {
    if (typeof window !== 'undefined') {
      const savedHeight = localStorage.getItem('monaco-editor-height')
      if (savedHeight) {
        setEditorHeight(savedHeight)
      }
    }
  }, [])

  useEffect(() => {
    if (monaco) {
      monaco.editor.setTheme(monacoTheme)
    }
  }, [monaco, monacoTheme])

  useEffect(() => {
    if (!editorRef.current || !monaco) return

    const editor = editorRef.current

    keyDownDisposableRef.current?.dispose()
    keyUpDisposableRef.current?.dispose()

    if (simulationData.saveBehaviorMode === 'editor-only') {
      keyDownDisposableRef.current = editor.onKeyDown((e) => {
        const isCtrlCmd = e.ctrlKey || e.metaKey
        if (isCtrlCmd && e.keyCode === monaco.KeyCode.KeyS) {
          e.preventDefault()
          e.stopPropagation()
          e.browserEvent?.preventDefault()
        }
      })

      keyUpDisposableRef.current = editor.onKeyUp((e) => {
        const isCtrlCmd = e.ctrlKey || e.metaKey
        if (isCtrlCmd && e.keyCode === monaco.KeyCode.KeyS) {
          e.preventDefault()
        }
      })
    }

    return () => {
      keyDownDisposableRef.current?.dispose()
      keyUpDisposableRef.current?.dispose()
    }
  }, [simulationData.saveBehaviorMode, monaco])

  useEffect(() => {
    const timer = autoPlayTimerRef
    const engine = engineRef
    return () => {
      if (timer.current) {
        clearTimeout(timer.current)
      }
      engine.current.stopExecution()
      keyDownDisposableRef.current?.dispose()
      keyUpDisposableRef.current?.dispose()
      isLocalPlayingRef.current = false
      isLocalLoopingRef.current = false
    }
  }, [])

  const renderExecutionStatus = () => {
    if (simulationData.javascriptExecuting || isExecuting) {
      return (
        <Badge variant="secondary" className="flex items-center gap-1">
          <Clock className="h-3 w-3 animate-spin" />
          Executing...
        </Badge>
      )
    }

    if (simulationData.javascriptGenerating || isGenerating) {
      return (
        <Badge variant="default" className="flex items-center gap-1">
          <Zap className="h-3 w-3 animate-pulse" />
          Generating...
        </Badge>
      )
    }

    if (simulationData.javascriptError) {
      return (
        <Badge variant="destructive" className="flex items-center gap-1">
          <AlertTriangle className="h-3 w-3" />
          Error
        </Badge>
      )
    }

    if (simulationData.currentPattern.frames.length > 0) {
      return (
        <Badge variant="default" className="flex items-center gap-1">
          <CheckCircle className="h-3 w-3" />
          {simulationData.currentPattern.frames.length} frames
        </Badge>
      )
    }

    return (
      <Badge variant="outline" className="flex items-center gap-1">
        <Code className="h-3 w-3" />
        Ready
      </Badge>
    )
  }

  const renderExecutionStats = () => {
    if (!executionStats) return null

    return (
      <div className="grid grid-cols-3 gap-4 text-sm">
        <div className="flex items-center gap-2">
          <Clock className="h-4 w-4 text-muted-foreground" />
          <span>{(executionStats.executionTime / 1000).toFixed(2)}s</span>
        </div>
        <div className="flex items-center gap-2">
          <Zap className="h-4 w-4 text-muted-foreground" />
          <span>{executionStats.frameCount} frames</span>
        </div>
        <div className="flex items-center gap-2">
          <Code className="h-4 w-4 text-muted-foreground" />
          <span>{(executionStats.memoryUsage / 1024).toFixed(1)} KB</span>
        </div>
      </div>
    )
  }

  const isPlaying = isLocalPlayingRef.current && !isLocalLoopingRef.current
  const isLooping = isLocalLoopingRef.current

  return (
    <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
      <div className="space-y-4">
        <Card>
          <CardHeader className="pb-3">
            <div className="flex items-center justify-between">
              <CardTitle className="flex items-center gap-2">
                <Code className="h-4 w-4" />
                JavaScript Simulator
              </CardTitle>
              <div className="flex items-center gap-3">
                {renderExecutionStatus()}
                <Button
                  onClick={handleToggleHelp}
                  variant="ghost"
                  size="sm"
                  className="flex items-center gap-1"
                >
                  <HelpCircle className="h-3 w-3" />
                  {showHelp ? 'Hide' : 'Help'}
                </Button>
              </div>
            </div>
            <div className="flex items-center justify-between gap-4 pt-2 border-t border-border mt-3">
              <div className="flex items-center gap-2">
                <Maximize2 className="h-3.5 w-3.5 text-muted-foreground" />
                <Label htmlFor="editor-height-select" className="text-xs text-muted-foreground cursor-pointer">
                  Height:
                </Label>
                <Select value={editorHeight} onValueChange={handleHeightChange}>
                  <SelectTrigger id="editor-height-select" className="h-7 w-[140px] text-xs">
                    <SelectValue />
                  </SelectTrigger>
                  <SelectContent>
                    {HEIGHT_PRESETS.map((preset) => (
                      <SelectItem key={preset.value} value={preset.value} className="text-xs">
                        {preset.label}
                      </SelectItem>
                    ))}
                  </SelectContent>
                </Select>
              </div>
              <div className="flex items-center gap-2">
                <Sun className="h-3.5 w-3.5 text-muted-foreground" />
                <Switch
                  id="monaco-theme-toggle"
                  checked={isDarkMode}
                  onCheckedChange={toggleTheme}
                  aria-label="Toggle Monaco editor theme"
                />
                <Moon className="h-3.5 w-3.5 text-muted-foreground" />
                <Label
                  htmlFor="monaco-theme-toggle"
                  className="text-xs text-muted-foreground cursor-pointer"
                >
                  Editor: {isDarkMode ? 'Dark' : 'Light'}
                </Label>
              </div>
            </div>
            <div className="flex items-center justify-between gap-4 pt-2 border-t border-border mt-2">
              <SaveBehaviorSelector />
            </div>
          </CardHeader>
          <CardContent className="space-y-4">
            <div className="flex gap-2">
              <Select value={selectedPresetId} onValueChange={handlePresetSelect}>
                <SelectTrigger className="flex-1 max-w-sm">
                  <SelectValue placeholder="Select a preset pattern..." />
                </SelectTrigger>
                <SelectContent>
                  <div className="p-2">
                    <div className="text-xs font-medium text-muted-foreground mb-2">
                      Basic Patterns
                    </div>
                    {javascriptPresets.filter(preset => preset.category === 'basic').map((preset) => (
                      <SelectItem key={preset.id} value={preset.id} className="py-2">
                        <div className="flex items-center gap-2">
                          <span>{preset.name}</span>
                          <Badge variant="outline" className="text-xs">
                            {preset.category}
                          </Badge>
                        </div>
                      </SelectItem>
                    ))}
                    <div className="text-xs font-medium text-muted-foreground mb-2 mt-3">
                      Intermediate Patterns
                    </div>
                    {javascriptPresets.filter(preset => preset.category === 'intermediate').map((preset) => (
                      <SelectItem key={preset.id} value={preset.id} className="py-2">
                        <div className="flex items-center gap-2">
                          <span>{preset.name}</span>
                          <Badge variant="outline" className="text-xs">
                            {preset.category}
                          </Badge>
                        </div>
                      </SelectItem>
                    ))}
                    <div className="text-xs font-medium text-muted-foreground mb-2 mt-3">
                      Advanced Patterns
                    </div>
                    {javascriptPresets.filter(preset => preset.category === 'advanced').map((preset) => (
                      <SelectItem key={preset.id} value={preset.id} className="py-2">
                        <div className="flex items-center gap-2">
                          <span>{preset.name}</span>
                          <Badge variant="outline" className="text-xs bg-orange-100 text-orange-800 dark:bg-orange-900 dark:text-orange-300">
                            {preset.category}
                          </Badge>
                        </div>
                      </SelectItem>
                    ))}
                  </div>
                </SelectContent>
              </Select>
              <Button
                onClick={handleResetCode}
                variant="outline"
                size="sm"
                className="flex items-center gap-1"
              >
                <RotateCcw className="h-3 w-3" />
                Reset
              </Button>
            </div>

            <div className="space-y-2">
              <div className="border border-border rounded-lg overflow-hidden">
                <Editor
                  height={editorHeight}
                  defaultLanguage="javascript"
                  value={simulationData.javascriptCode}
                  onChange={(value) => handleCodeChange(value || "")}
                  theme={monacoTheme}
                  options={{
                    // Performance optimizations
                    minimap: { enabled: false },
                    scrollBeyondLastLine: false,
                    automaticLayout: false,
                    renderValidationDecorations: 'on',
                    hover: { enabled: false },
                    occurrencesHighlight: 'off',
                    codeLens: false,
                    folding: false,
                    lineNumbersMinChars: 2,
                    // Reduced features for performance
                    lineNumbers: 'on',
                    roundedSelection: false,
                    tabSize: 2,
                    insertSpaces: true,
                    wordWrap: 'bounded',
                    fontSize: 14,
                    fontFamily: 'Monaco, Consolas, "Courier New", monospace',
                    suggest: {
                      insertMode: 'replace',
                      filterGraceful: true
                    },
                    quickSuggestions: {
                      other: true,
                      comments: false, // Disabled for performance
                      strings: false   // Disabled for performance
                    },
                    acceptSuggestionOnCommitCharacter: true,
                    acceptSuggestionOnEnter: 'on',
                    snippetSuggestions: 'top',
                    readOnly: simulationData.javascriptExecuting || isExecuting,
                    // Performance-related options
                    smoothScrolling: false,
                    cursorBlinking: 'solid',
                    renderLineHighlight: 'none',
                    bracketPairColorization: { enabled: false }
                  }}
                  onMount={(editor, monaco) => {
                    editorRef.current = editor;

                    // Performance: Reduce TypeScript diagnostics overhead
                    monaco.languages.typescript.javascriptDefaults.setDiagnosticsOptions({
                      noSemanticValidation: true, // Disabled for performance
                      noSyntaxValidation: false,
                    });

                    monaco.languages.typescript.javascriptDefaults.setCompilerOptions({
                      target: monaco.languages.typescript.ScriptTarget.ES2020,
                      allowNonTsExtensions: true,
                      moduleResolution: monaco.languages.typescript.ModuleResolutionKind.NodeJs,
                      module: monaco.languages.typescript.ModuleKind.CommonJS,
                      noEmit: true,
                      esModuleInterop: true,
                      jsx: monaco.languages.typescript.JsxEmit.React,
                      reactNamespace: 'React',
                      allowJs: true,
                      typeRoots: ['node_modules/@types']
                    });

                    // Performance: Only add essential type definitions
                    monaco.languages.typescript.javascriptDefaults.addExtraLib(`
                      declare function set(index: number, state: boolean): void;
                      declare function sleep(ms: number): Promise<void>;
                      declare function on(index: number): void;
                      declare function off(index: number): void;
                      declare const pin_size_: number;
                      declare const delay_time_: number;
                      declare const pin_ptr_: number[];
                      declare const halfSize: number;
                      declare function allOn(): void;
                      declare function allOff(): void;
                      declare function waveLeftToRight(delay: number): Promise<void>;
                      declare function waveRightToLeft(delay: number): Promise<void>;
                      declare function waveCenterOut(delay: number): Promise<void>;
                    `, 'autolight-api.d.ts');

                    editor.focus();
                  }}
                  beforeMount={() => {
                  }}
                />
              </div>

              {showHelp && (
                <Card className="bg-muted/50">
                  <CardContent className="p-4">
                    <div className="space-y-4">
                      <div>
                        <h4 className="text-sm font-semibold mb-2">💡 Editor Features</h4>
                        <ul className="text-xs space-y-1 text-muted-foreground">
                          <li>• <kbd className="px-1 py-0.5 text-xs bg-muted rounded">Ctrl+Space</kbd> - Trigger autocomplete</li>
                          <li>• <kbd className="px-1 py-0.5 text-xs bg-muted rounded">Ctrl+/</kbd> - Toggle comment</li>
                          <li>• <kbd className="px-1 py-0.5 text-xs bg-muted rounded">Shift+Alt+F</kbd> - Format code</li>
                          <li>• <kbd className="px-1 py-0.5 text-xs bg-muted rounded">Ctrl+D</kbd> - Select next occurrence</li>
                          <li>• <kbd className="px-1 py-0.5 text-xs bg-muted rounded">Alt+↑/↓</kbd> - Move line up/down</li>
                        </ul>
                      </div>
                      <div>
                        <h4 className="text-sm font-semibold mb-2">🔧 C++ API Functions</h4>
                        <pre className="text-xs whitespace-pre-wrap font-mono">
                          {getTemplateHelp()}
                        </pre>
                      </div>
                    </div>
                  </CardContent>
                </Card>
              )}
            </div>

            <div className="space-y-3">
              <div className="flex items-center gap-2">
                <Button
                  onClick={handleGenerateFrames}
                  disabled={
                    simulationData.javascriptGenerating ||
                    isGenerating ||
                    !simulationData.javascriptCode.trim() ||
                    isPlaying ||
                    isLooping
                  }
                  className="flex items-center gap-1"
                >
                  <Zap className="h-3 w-3" />
                  {(simulationData.javascriptGenerating || isGenerating) ? 'Generating...' : 'Generate Frames'}
                </Button>

                <Button
                  onClick={handleAutoPlay}
                  disabled={
                    simulationData.currentPattern.frames.length === 0 ||
                    simulationData.javascriptCodeChanged ||
                    isLooping
                  }
                  variant="secondary"
                  className="flex items-center gap-1"
                >
                  <Play className="h-3 w-3" />
                  Play Pattern
                </Button>

                <Button
                  onClick={handleStartLooping}
                  disabled={
                    simulationData.currentPattern.frames.length === 0 ||
                    simulationData.javascriptCodeChanged ||
                    isPlaying ||
                    isLooping
                  }
                  variant="default"
                  className="flex items-center gap-1"
                >
                  <Repeat className="h-3 w-3" />
                  Play Loop
                </Button>

                {(isPlaying || isLooping) && (
                  <Button
                    onClick={() => {
                      handleStopExecution()
                      handleStopLooping()
                    }}
                    variant="destructive"
                    size="sm"
                    className="flex items-center gap-1"
                  >
                    <Square className="h-3 w-3" />
                    Stop
                  </Button>
                )}
              </div>

              {isLooping && (
                <div className="flex items-center gap-2 px-2 py-1.5 bg-muted rounded-md">
                  <Badge variant="default" className="flex items-center gap-1 animate-pulse">
                    <Repeat className="h-3 w-3" />
                    Loop Active
                  </Badge>
                  <span className="text-sm text-muted-foreground">
                    Cycle: {localLoopCountRef.current + 1}/∞
                  </span>
                </div>
              )}

              {simulationData.javascriptCodeChanged && simulationData.currentPattern.frames.length > 0 && (
                <Alert variant="default" className="border-amber-500 bg-amber-50 dark:bg-amber-950/20">
                  <AlertTriangle className="h-4 w-4 text-amber-600 dark:text-amber-500" />
                  <AlertDescription className="text-amber-800 dark:text-amber-200">
                    Code has changed. Click <strong>&quot;Generate Frames&quot;</strong> to recompile before playing.
                  </AlertDescription>
                </Alert>
              )}
            </div>

            {executionStats && (
              <Card>
                <CardContent className="p-4">
                  {renderExecutionStats()}
                  {executionStats.warningMessages.length > 0 && (
                    <div className="mt-3 text-xs text-muted-foreground">
                      <p className="font-medium">Recommendations:</p>
                      <ul className="list-disc list-inside space-y-1">
                        {defaultSafety.getRecommendations(
                          executionStats.frameCount,
                          executionStats.executionTime
                        ).map((rec, idx) => (
                          <li key={idx}>{rec}</li>
                        ))}
                      </ul>
                    </div>
                  )}
                </CardContent>
              </Card>
            )}

            {simulationData.javascriptError && (
              <Alert variant="destructive">
                <AlertTriangle className="h-4 w-4" />
                <AlertDescription>{simulationData.javascriptError}</AlertDescription>
              </Alert>
            )}
          </CardContent>
        </Card>
      </div>

      <div className="space-y-4">
        <Card>
          <CardHeader className="pb-3">
            <CardTitle className="flex items-center gap-2">
              Live Pattern Preview
              <Badge variant="secondary" className="text-xs">
                {simulationData.currentPattern.frames.length} frames
              </Badge>
              {simulationData.playbackSpeed !== 1 && (
                <Badge variant="outline" className="text-xs">
                  {simulationData.playbackSpeed}x speed
                </Badge>
              )}
            </CardTitle>
          </CardHeader>
          <CardContent>
            {simulationData.gridConfig && simulationData.currentPattern.frames.length > 0 ? (
              <DynamicPatternGrid />
            ) : (
              <div className="flex items-center justify-center h-48 text-muted-foreground">
                <div className="text-center">
                  <Code className="h-8 w-8 mx-auto mb-2" />
                  <p>Run JavaScript code to see pattern preview</p>
                </div>
              </div>
            )}
          </CardContent>
        </Card>

        {simulationData.currentPattern.frames.length > 0 && (
          <Card>
            <CardHeader className="pb-3">
              <CardTitle>Frame Timeline</CardTitle>
            </CardHeader>
            <CardContent>
              <FrameTimeline />

              <div className="flex items-center justify-center gap-2 mt-4">
                <Button
                  onClick={() => selectFrame(Math.max(0, simulationData.selectedFrameIndex - 1))}
                  variant="outline"
                  size="sm"
                  disabled={simulationData.selectedFrameIndex === 0}
                >
                  <ChevronLeft className="h-3 w-3" />
                </Button>

                <Button
                  onClick={isPlaying ? handleStopLooping : handleAutoPlay}
                  variant="outline"
                  size="sm"
                  className="flex items-center gap-1"
                >
                  {isPlaying ? (
                    <Pause className="h-3 w-3" />
                  ) : (
                    <Play className="h-3 w-3" />
                  )}
                  {isPlaying ? 'Pause' : 'Play'}
                </Button>

                <Button
                  onClick={() => selectFrame(Math.min(simulationData.currentPattern.frames.length - 1, simulationData.selectedFrameIndex + 1))}
                  variant="outline"
                  size="sm"
                  disabled={simulationData.selectedFrameIndex === simulationData.currentPattern.frames.length - 1}
                >
                  <ChevronRight className="h-3 w-3" />
                </Button>
              </div>

              <div className="border-t border-border my-4"></div>
              <div className="space-y-4 px-4">
                <div className="text-sm font-medium text-center">Precision Timing Controls</div>

                <div className="flex items-center gap-4">
                  <div className="text-sm text-muted-foreground min-w-[4rem]">
                    Speed: {simulationData.playbackSpeed}x
                  </div>
                  <div className="flex-1 max-w-[200px]">
                    <Slider
                      value={[simulationData.playbackSpeed]}
                      onValueChange={([speed]) => setPlaybackSpeed(speed)}
                      min={0.25}
                      max={4}
                      step={0.25}
                      className="w-full"
                    />
                  </div>
                  <div className="flex gap-1">
                    <Button
                      onClick={() => setPlaybackSpeed(0.5)}
                      variant="outline"
                      size="sm"
                      className="text-xs px-2"
                    >
                      0.5x
                    </Button>
                    <Button
                      onClick={() => setPlaybackSpeed(1)}
                      variant="outline"
                      size="sm"
                      className="text-xs px-2"
                    >
                      1x
                    </Button>
                    <Button
                      onClick={() => setPlaybackSpeed(2)}
                      variant="outline"
                      size="sm"
                      className="text-xs px-2"
                    >
                      2x
                    </Button>
                  </div>
                </div>

                {(isPlaying || isLooping) && simulationData.currentPattern.frames.length > 0 && (
                  <div className="text-xs text-muted-foreground text-center">
                    <Badge variant="outline" className="text-xs">
                      ⚡ Local Timer Active - Real-time Playback
                    </Badge>
                  </div>
                )}
              </div>
            </CardContent>
          </Card>
        )}
      </div>
    </div>
  )
}
