"use client"

import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card"
import { Code, Lightbulb } from "lucide-react"
import DynamicPatternGrid from "../grid/DynamicPatternGrid"
import PatternControls from "../PatternControls"
import CodeGenerator from "../CodeGenerator"
import FrameTimeline from "../FrameTimeline"

export default function VisualPatternMode() {
  return (
    <div className="space-y-6">
      {/* Pattern Controls Section - Top */}
      <PatternControls />

      {/* LED Grid + Frame Timeline Section - Side by Side */}
      <div className="grid gap-6 lg:grid-cols-2">
        {/* LED Pattern Grid - Left */}
        <Card>
          <CardHeader>
            <CardTitle className="flex items-center gap-2">
              <Lightbulb className="h-5 w-5" />
              LED Pattern Grid
            </CardTitle>
          </CardHeader>
          <CardContent>
            <DynamicPatternGrid />
          </CardContent>
        </Card>

        {/* Frame Timeline - Right */}
        <Card>
          <CardHeader>
            <CardTitle>Frame Timeline</CardTitle>
          </CardHeader>
          <CardContent>
            <FrameTimeline />
          </CardContent>
        </Card>
      </div>

      {/* Code Generator Section */}
      <Card>
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <Code className="h-5 w-5" />
            Generated C++ Code
          </CardTitle>
        </CardHeader>
        <CardContent>
          <CodeGenerator />
        </CardContent>
      </Card>
    </div>
  )
}