"use client"

import { Badge } from "@/components/ui/badge"
import { Button } from "@/components/ui/button"
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select"
import { Code, Grid3X3, ArrowLeft, Monitor, Cpu, Calculator, Zap, RotateCcw } from "lucide-react"
import { useSimulation, GridConfig } from "@/lib/contexts/SimulationContext"
import { usePatternBuilderStore } from "@/lib/store/patternBuilderStore"
import GridConfigurator from "./config/GridConfigurator"
import ChannelMapper from "./config/ChannelMapper"
import VisualPatternMode from "./modes/VisualPatternMode"
import CppSimulatorMode from "./modes/CppSimulatorMode"
import ExpressionBuilderMode from "./modes/ExpressionBuilderMode"
import JavaScriptSimulatorMode from "./modes/JavaScriptSimulatorMode"

export default function SimulationContent() {
  const { simulationData, setConfigurationStep, setGridConfig, updateChannelMapping, setPatternMode } = useSimulation()
  const zustandStore = usePatternBuilderStore()

  const handleReset = () => {
    if (confirm('Reset Pattern Builder?\n\nThis will clear all saved configurations, grid settings, and pattern code. This action cannot be undone.')) {
      zustandStore.reset()
      localStorage.removeItem('pattern-builder-storage')
      window.location.reload()
    }
  }

  const handleGridConfigured = (config: Omit<GridConfig, 'channelMapping'>) => {
    const gridConfig = {
      ...config,
      channelMapping: new Map()
    }
    setGridConfig(gridConfig)
    setConfigurationStep('mapping')
  }

  const handleMappingComplete = (mapping: Map<string, number | null>) => {
    updateChannelMapping(mapping)
    setConfigurationStep('pattern')
  }

  const handleBackToGrid = () => {
    setConfigurationStep('grid')
  }

  const handleBackToMapping = () => {
    setConfigurationStep('mapping')
  }

  const renderConfigurationStep = () => {
    switch (simulationData.configurationStep) {
      case 'grid':
        return (
          <div className="space-y-6">
            <div className="text-center space-y-2">
              <h2 className="text-2xl font-bold">Step 1: Configure Grid Layout</h2>
              <p className="text-muted-foreground">
                Set up your LED grid dimensions and layout type
              </p>
            </div>
            <GridConfigurator onGridConfigured={handleGridConfigured} />
          </div>
        )

      case 'mapping':
        return (
          <div className="space-y-6">
            <div className="text-center space-y-2">
              <h2 className="text-2xl font-bold">Step 2: Map Channels</h2>
              <p className="text-muted-foreground">
                Assign channel numbers to grid positions
              </p>
            </div>
            {simulationData.gridConfig && (
              <ChannelMapper
                gridConfig={simulationData.gridConfig}
                onMappingComplete={handleMappingComplete}
                onBack={handleBackToGrid}
              />
            )}
          </div>
        )

      case 'pattern':
        return (
          <div className="space-y-6">
            {/* Pattern Builder Header with Mode Switcher */}
            <div className="flex items-center justify-between">
              <div className="flex items-center gap-4">
                <div>
                  <h2 className="text-2xl font-bold">Step 3: Create Pattern</h2>
                  <p className="text-muted-foreground">
                    Choose your pattern creation method
                  </p>
                </div>
                <Button
                  onClick={handleBackToMapping}
                  variant="outline"
                  size="sm"
                  className="flex items-center gap-1"
                >
                  <ArrowLeft className="h-3 w-3" />
                  Reconfigure
                </Button>
              </div>
              <div className="flex items-center gap-3">
                <Badge variant="secondary" className="flex items-center gap-1">
                  <Grid3X3 className="h-3 w-3" />
                  {simulationData.gridConfig && `${simulationData.gridConfig.rows}×${simulationData.gridConfig.cols} ${simulationData.gridConfig.layoutType}`}
                </Badge>

                <Button
                  onClick={handleReset}
                  variant="outline"
                  size="sm"
                  className="flex items-center gap-1 text-destructive hover:text-destructive"
                  title="Reset Pattern Builder"
                >
                  <RotateCcw className="h-3 w-3" />
                  Reset
                </Button>

                {/* Mode Switcher */}
                <Select value={simulationData.patternMode} onValueChange={setPatternMode}>
                  <SelectTrigger className="w-[180px]">
                    <SelectValue />
                  </SelectTrigger>
                  <SelectContent>
                    <SelectItem value="visual">
                      <div className="flex items-center gap-2">
                        <Monitor className="h-3 w-3" />
                        Visual Builder
                      </div>
                    </SelectItem>
                    <SelectItem value="cpp">
                      <div className="flex items-center gap-2">
                        <Cpu className="h-3 w-3" />
                        C++ Simulator
                      </div>
                    </SelectItem>
                    <SelectItem value="expression">
                      <div className="flex items-center gap-2">
                        <Calculator className="h-3 w-3" />
                        Expression Builder
                      </div>
                    </SelectItem>
                    <SelectItem value="javascript">
                      <div className="flex items-center gap-2">
                        <Zap className="h-3 w-3" />
                        JavaScript Simulator
                      </div>
                    </SelectItem>
                  </SelectContent>
                </Select>
              </div>
            </div>

            {/* Conditional Mode Content */}
            {simulationData.patternMode === 'visual' ? (
              <VisualPatternMode />
            ) : simulationData.patternMode === 'cpp' ? (
              <CppSimulatorMode />
            ) : simulationData.patternMode === 'expression' ? (
              <ExpressionBuilderMode />
            ) : (
              <JavaScriptSimulatorMode />
            )}
          </div>
        )

      default:
        return null
    }
  }

  return (
    <main className="flex flex-1 flex-col gap-4 p-4 md:gap-6 md:p-8">
      <div className="flex items-center justify-between">
        <div>
          <h1 className="text-3xl font-bold tracking-tight">Pattern Builder</h1>
          <p className="text-muted-foreground">
            Dynamic LED pattern creator with custom grid layouts
          </p>
        </div>
        <Badge variant="secondary" className="flex items-center gap-1">
          <Code className="h-3 w-3" />
          Developer Tool
        </Badge>
      </div>

      {renderConfigurationStep()}
    </main>
  )
}