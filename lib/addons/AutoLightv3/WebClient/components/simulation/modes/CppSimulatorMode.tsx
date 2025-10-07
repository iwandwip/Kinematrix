"use client"

import { useCallback, useEffect, useRef } from "react"
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card"
import { Button } from "@/components/ui/button"
import { Badge } from "@/components/ui/badge"
import { useSimulation } from "@/lib/contexts/SimulationContext"
import { Code, Play, RotateCcw, ChevronLeft, ChevronRight, Pause, Square } from "lucide-react"
import DynamicPatternGrid from "../grid/DynamicPatternGrid"
import FrameTimeline from "../FrameTimeline"
import PresetSelector from "../PresetSelector"

export default function CppSimulatorMode() {
  const {
    simulationData,
    setCppCode,
    generateFramesFromCpp,
    selectFrame,
    startPlayback,
    stopPlayback,
    startLooping,
    stopLooping
  } = useSimulation()

  const autoPlayTimerRef = useRef<NodeJS.Timeout | null>(null)

  const handleCodeChange = useCallback((value: string | undefined) => {
    const code = value || ''
    setCppCode(code)
  }, [setCppCode])

  const handleRunSimulation = () => {
    generateFramesFromCpp(simulationData.cppCode)
  }

  const handleInsertTemplate = (templateCode: string) => {
    setCppCode(templateCode)
    generateFramesFromCpp(templateCode)
  }

  const handleAppendTemplate = (templateCode: string) => {
    const currentCode = simulationData.cppCode
    const newCode = currentCode.trim() + '\n\n' + templateCode
    setCppCode(newCode)
    generateFramesFromCpp(newCode)
  }

  const handleResetCode = () => {
    const defaultCode = `void BaseChannel::taskSequenceAdvanced() {
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
    setCppCode(defaultCode)
    generateFramesFromCpp(defaultCode)
  }

  const handlePreviousFrame = () => {
    if (simulationData.selectedFrameIndex > 0) {
      selectFrame(simulationData.selectedFrameIndex - 1)
    }
  }

  const handleNextFrame = () => {
    if (simulationData.selectedFrameIndex < simulationData.currentPattern.frames.length - 1) {
      selectFrame(simulationData.selectedFrameIndex + 1)
    }
  }

  const handleResetToFirstFrame = () => {
    // Stop auto-play dan looping jika sedang berjalan
    if (simulationData.isLooping) {
      stopLooping()
    } else if (simulationData.isPlaying) {
      stopPlayback()
    }
    if (autoPlayTimerRef.current) {
      clearTimeout(autoPlayTimerRef.current)
      autoPlayTimerRef.current = null
    }
    // Reset ke frame pertama (index 0)
    selectFrame(0)
  }

  const handleStartLooping = () => {
    startLooping()
    scheduleNextFrame()
  }

  const handleStopLooping = () => {
    stopLooping()
    if (autoPlayTimerRef.current) {
      clearTimeout(autoPlayTimerRef.current)
      autoPlayTimerRef.current = null
    }
  }

  const scheduleNextFrame = useCallback(() => {
    // Clear existing timer
    if (autoPlayTimerRef.current) {
      clearTimeout(autoPlayTimerRef.current)
      autoPlayTimerRef.current = null
    }

    const currentFrame = simulationData.currentPattern.frames[simulationData.selectedFrameIndex]
    const delay = currentFrame?.delay || 50
    const adjustedDelay = delay / simulationData.playbackSpeed

    const timer = setTimeout(() => {
      const nextIndex = simulationData.selectedFrameIndex + 1
      if (nextIndex < simulationData.currentPattern.frames.length) {
        selectFrame(nextIndex)
      } else {
        // Reached end - check if looping
        if (simulationData.isLooping) {
          selectFrame(0)  // Restart from frame 1
        } else {
          // Normal play - stop
          stopPlayback()
          autoPlayTimerRef.current = null
        }
      }
    }, adjustedDelay)

    autoPlayTimerRef.current = timer
  }, [simulationData.selectedFrameIndex, simulationData.currentPattern.frames, simulationData.playbackSpeed, simulationData.isLooping, selectFrame, stopPlayback])

  const handleAutoPlay = () => {
    if (simulationData.isPlaying) {
      // Stop auto-play
      stopPlayback()
      if (autoPlayTimerRef.current) {
        clearTimeout(autoPlayTimerRef.current)
        autoPlayTimerRef.current = null
      }
    } else {
      // Start auto-play
      startPlayback()
      scheduleNextFrame()
    }
  }

  // Cleanup timer on unmount
  useEffect(() => {
    return () => {
      if (autoPlayTimerRef.current) {
        clearTimeout(autoPlayTimerRef.current)
        autoPlayTimerRef.current = null
      }
    }
  }, [])

  // Auto-play management
  useEffect(() => {
    if (simulationData.isPlaying) {
      scheduleNextFrame()
    } else {
      if (autoPlayTimerRef.current) {
        clearTimeout(autoPlayTimerRef.current)
        autoPlayTimerRef.current = null
      }
    }
  }, [simulationData.isPlaying, scheduleNextFrame])

  // Handle frame changes during auto-play
  useEffect(() => {
    if (simulationData.isPlaying) {
      // Clear existing timer and schedule next frame when selectedFrameIndex changes
      if (autoPlayTimerRef.current) {
        clearTimeout(autoPlayTimerRef.current)
        autoPlayTimerRef.current = null
      }
      scheduleNextFrame()
    }
  }, [simulationData.selectedFrameIndex, simulationData.isPlaying, scheduleNextFrame])

  useEffect(() => {
    if (simulationData.cppCode) {
      generateFramesFromCpp(simulationData.cppCode)
    }
  }, [simulationData.cppCode, generateFramesFromCpp])

  return (
    <div className="space-y-6">
      {/* Preset Templates Section */}
      <PresetSelector
        onInsertTemplate={handleInsertTemplate}
        onAppendTemplate={handleAppendTemplate}
        totalChannels={simulationData.gridConfig?.totalChannels || 24}
      />

      {/* C++ Code Editor Section */}
      <div className="grid gap-6 lg:grid-cols-2">
        {/* Code Editor - Left */}
        <Card>
          <CardHeader className="pb-3">
            <div className="flex items-center justify-between">
              <CardTitle className="flex items-center gap-2">
                <Code className="h-5 w-5" />
                C++ Pattern Code
              </CardTitle>
              <div className="flex gap-2">
                <Button
                  onClick={handleRunSimulation}
                  size="sm"
                  className="flex items-center gap-1"
                >
                  <Play className="h-3 w-3" />
                  Run
                </Button>
                <Button
                  onClick={handleResetCode}
                  size="sm"
                  variant="outline"
                  className="flex items-center gap-1"
                >
                  <RotateCcw className="h-3 w-3" />
                  Reset
                </Button>
              </div>
            </div>
          </CardHeader>
          <CardContent>
            {/* Monaco Editor Placeholder */}
            <div className="border rounded-lg bg-slate-950 text-green-400 font-mono text-sm p-4 min-h-[400px] overflow-auto">
              <textarea
                value={simulationData.cppCode}
                onChange={(e) => handleCodeChange(e.target.value)}
                className="w-full h-full bg-transparent border-0 outline-0 resize-none text-green-400 font-mono text-sm"
                style={{ minHeight: '360px' }}
                placeholder="Write your C++ AutoLight pattern code here..."
                spellCheck={false}
              />
            </div>

            {/* Code Info */}
            <div className="mt-3 flex flex-wrap gap-2">
              <Badge variant="secondary" className="text-xs">
                LED Control: set(), sleep(), off(), on()
              </Badge>
              <Badge variant="outline" className="text-xs">
                Loops: for, do-while
              </Badge>
              <Badge variant="secondary" className="text-xs">
                Conditionals: if (condition), else
              </Badge>
              <Badge variant="outline" className="text-xs">
                Variables: int var = value;
              </Badge>
              <Badge variant="secondary" className="text-xs">
                Math: map(), ceil(), floor(), abs(), random()
              </Badge>
              <Badge variant="outline" className="text-xs">
                Arrays: int arr[size];
              </Badge>
              <Badge variant="secondary" className="text-xs">
                Custom: resetArray(), findMax()
              </Badge>
              <Badge variant="outline" className="text-xs">
                Lambda: auto func = [&](int) {'->'} void
              </Badge>
            </div>
          </CardContent>
        </Card>

        {/* Live LED Simulation - Right */}
        <Card>
          <CardHeader className="pb-3">
            <CardTitle className="flex items-center gap-2">
              <div className="w-2 h-2 bg-green-500 rounded-full animate-pulse"></div>
              Live LED Simulation
            </CardTitle>
          </CardHeader>
          <CardContent>
            {/* Frame Navigation Controls */}
            <div className="flex items-center justify-center gap-2 mb-4">
              <Button
                onClick={handleResetToFirstFrame}
                disabled={simulationData.selectedFrameIndex === 0 || simulationData.currentPattern.frames.length === 0}
                size="sm"
                variant="outline"
                className="flex items-center gap-1"
              >
                <RotateCcw className="h-3 w-3" />
                Reset
              </Button>

              <Button
                onClick={handlePreviousFrame}
                disabled={simulationData.selectedFrameIndex === 0 || simulationData.isPlaying}
                size="sm"
                variant="outline"
                className="flex items-center gap-1"
              >
                <ChevronLeft className="h-3 w-3" />
                Previous
              </Button>

              <Button
                onClick={handleAutoPlay}
                size="sm"
                variant={simulationData.isPlaying && !simulationData.isLooping ? "destructive" : "default"}
                className="flex items-center gap-1"
                disabled={simulationData.isLooping}
              >
                {simulationData.isPlaying && !simulationData.isLooping ? (
                  <>
                    <Pause className="h-3 w-3" />
                    Pause
                  </>
                ) : (
                  <>
                    <Play className="h-3 w-3" />
                    Play
                  </>
                )}
              </Button>

              <Button
                onClick={handleStartLooping}
                disabled={simulationData.isLooping || simulationData.currentPattern.frames.length === 0}
                size="sm"
                variant="default"
                className="flex items-center gap-1 bg-green-600 hover:bg-green-700 text-white"
              >
                <Play className="h-3 w-3" />
                Start
              </Button>

              <Button
                onClick={handleStopLooping}
                disabled={!simulationData.isLooping}
                size="sm"
                variant="destructive"
                className="flex items-center gap-1"
              >
                <Square className="h-3 w-3" />
                Stop
              </Button>

              <div className="px-4 py-1 bg-muted rounded text-sm font-mono">
                Frame {simulationData.selectedFrameIndex + 1} of {simulationData.currentPattern.frames.length}
                {simulationData.isPlaying && !simulationData.isLooping && (
                  <span className="ml-2 text-blue-600">▶ Playing</span>
                )}
                {simulationData.isLooping && (
                  <span className="ml-2 text-green-600">🔄 Looping</span>
                )}
              </div>

              <Button
                onClick={handleNextFrame}
                disabled={simulationData.selectedFrameIndex >= simulationData.currentPattern.frames.length - 1 || simulationData.isPlaying}
                size="sm"
                variant="outline"
                className="flex items-center gap-1"
              >
                Next
                <ChevronRight className="h-3 w-3" />
              </Button>
            </div>

            <DynamicPatternGrid disabled={false} />

            {/* Simulation Stats */}
            <div className="mt-4 space-y-2">
              <div className="flex justify-between text-xs text-muted-foreground">
                <span>Generated Frames:</span>
                <span>{simulationData.currentPattern.frames.length}</span>
              </div>
              <div className="flex justify-between text-xs text-muted-foreground">
                <span>Total Channels:</span>
                <span>{simulationData.gridConfig?.totalChannels || 0}</span>
              </div>
              <div className="flex justify-between text-xs text-muted-foreground">
                <span>Estimated Duration:</span>
                <span>
                  ~{Math.round(
                    simulationData.currentPattern.frames.reduce((total, frame) => total + frame.delay, 0) / 1000 * 100
                  ) / 100}s
                </span>
              </div>
            </div>
          </CardContent>
        </Card>
      </div>

      {/* Frame Timeline Section */}
      <Card>
        <CardHeader className="pb-3">
          <CardTitle>Generated Frame Sequence</CardTitle>
        </CardHeader>
        <CardContent>
          <FrameTimeline />
        </CardContent>
      </Card>

      {/* C++ Syntax Help */}
      <Card>
        <CardHeader className="pb-3">
          <CardTitle className="text-sm">Enhanced C++ Syntax Support</CardTitle>
        </CardHeader>
        <CardContent className="text-xs space-y-4">
          <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
            <div>
              <h4 className="font-medium mb-2 text-green-600">LED Control Commands:</h4>
              <code className="block bg-muted p-2 rounded text-xs">
                set(config_data_ptr_{'->'} header.pin_ptr_[index], HIGH);<br/>
                set(config_data_ptr_{'->'} header.pin_ptr_[index], LOW);<br/>
                sleep(channel_data_.delay_time_);<br/>
                sleep(300);<br/>
                off(); // Turn all OFF<br/>
                on();  // Turn all ON
              </code>
            </div>

            <div>
              <h4 className="font-medium mb-2 text-blue-600">Variables & Arrays:</h4>
              <code className="block bg-muted p-2 rounded text-xs">
                int halfSize = config_data_ptr_{'->'} header.pin_size_ / 2;<br/>
                int time = map(value, 1, 24, 100, 50);<br/>
                int data[config_data_ptr_{'->'} header.pin_size_];<br/>
                const int pattern[9][8] = {'{'}{'{'} 3,4,4,1 {'}'}{'}'};
              </code>
            </div>

            <div>
              <h4 className="font-medium mb-2 text-purple-600">Control Flow:</h4>
              <code className="block bg-muted p-2 rounded text-xs">
                for (int i = 0; i &lt; pin_size_; i++) {'{'}<br/>
                &nbsp;&nbsp;// Loop body<br/>
                {'}'}<br/>
                if (i &lt; halfSize) {'{'}<br/>
                &nbsp;&nbsp;// Conditional code<br/>
                {'}'} else {'{'}<br/>
                &nbsp;&nbsp;// Else block<br/>
                {'}'}
              </code>
            </div>

            <div>
              <h4 className="font-medium mb-2 text-orange-600">Mathematical Functions:</h4>
              <code className="block bg-muted p-2 rounded text-xs">
                map(value, fromLow, fromHigh, toLow, toHigh)<br/>
                ceil(value) // Ceiling function<br/>
                floor(value) // Floor function<br/>
                abs(value) // Absolute value
              </code>
            </div>

            <div>
              <h4 className="font-medium mb-2 text-red-600">Complex Expressions:</h4>
              <code className="block bg-muted p-2 rounded text-xs">
                pin_ptr_[(pin_size_ - 1) - i]<br/>
                channel_data_.delay_time_ * 2<br/>
                (pin_size_ / 2) % 2 == 1<br/>
                i &gt; 1 && i &lt; pin_size_ + 2
              </code>
            </div>

            <div>
              <h4 className="font-medium mb-2 text-teal-600">Built-in Variables:</h4>
              <code className="block bg-muted p-2 rounded text-xs">
                config_data_ptr_{'->'} header.pin_size_<br/>
                {/* Total number of channels */}<br/>
                channel_data_.delay_time_<br/>
                {/* Default timing delay (50ms) */}
              </code>
            </div>
          </div>

          <div className="mt-4 p-3 bg-blue-50 dark:bg-blue-950 rounded-lg">
            <h4 className="font-medium mb-2 text-blue-700 dark:text-blue-300">✨ New in Enhanced C++ Simulator:</h4>
            <div className="text-xs text-blue-600 dark:text-blue-400 space-y-1">
              <p>• <strong>Conditional Logic:</strong> Full if/else statement support with complex conditions</p>
              <p>• <strong>Variable Declarations:</strong> int, const int, and dynamic arrays with pin_size_</p>
              <p>• <strong>Mathematical Functions:</strong> Arduino-compatible map(), ceil(), floor(), abs()</p>
              <p>• <strong>Complex Expressions:</strong> Arithmetic, modulo, logical operators (&&, ||, %)</p>
              <p>• <strong>Multi-dimensional Arrays:</strong> Pattern matrices like BaseChannelSequence.cpp</p>
              <p>• <strong>Enhanced Variables:</strong> User-defined variables with expression evaluation</p>
            </div>
          </div>
        </CardContent>
      </Card>
    </div>
  )
}