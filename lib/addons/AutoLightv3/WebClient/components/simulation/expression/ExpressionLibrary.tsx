"use client"

import { useState } from "react"
import { Button } from "@/components/ui/button"
import { Card } from "@/components/ui/card"
import { Badge } from "@/components/ui/badge"
import { Input } from "@/components/ui/input"
import { Label } from "@/components/ui/label"
import { Plus } from "lucide-react"
import { BUILTIN_EXPRESSIONS, type PatternExpression, type ExpressionCategory } from "@/lib/simulation/expressionParser"

interface ExpressionLibraryProps {
  totalChannels: number
  onAddFrame: (expression: PatternExpression, parameters?: Record<string, number | boolean | string>, duration?: number) => void
}

export default function ExpressionLibrary({ totalChannels, onAddFrame }: ExpressionLibraryProps) {
  const [selectedCategory, setSelectedCategory] = useState<ExpressionCategory>('basic')
  const [parameterValues, setParameterValues] = useState<Record<string, Record<string, number | boolean | string>>>({})

  const categories: { value: ExpressionCategory; label: string; color: string }[] = [
    { value: 'basic', label: 'Basic', color: 'bg-blue-100 text-blue-800 dark:bg-blue-900 dark:text-blue-200' },
    { value: 'mathematical', label: 'Math', color: 'bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-200' },
    { value: 'sequential', label: 'Sequential', color: 'bg-purple-100 text-purple-800 dark:bg-purple-900 dark:text-purple-200' },
    { value: 'custom', label: 'Custom', color: 'bg-orange-100 text-orange-800 dark:bg-orange-900 dark:text-orange-200' }
  ]

  const filteredExpressions = BUILTIN_EXPRESSIONS.filter(expr => expr.category === selectedCategory)

  const handleAddExpression = (expression: PatternExpression, duration: number = 100) => {
    const parameters = expression.parameters
      ? parameterValues[expression.id] || getDefaultParameters(expression)
      : {}

    onAddFrame(expression, parameters, duration)
  }

  const handleParameterChange = (expressionId: string, paramName: string, value: number | boolean | string) => {
    setParameterValues(prev => ({
      ...prev,
      [expressionId]: {
        ...prev[expressionId],
        [paramName]: value
      }
    }))
  }

  const getDefaultParameters = (expression: PatternExpression): Record<string, number | boolean | string> => {
    if (!expression.parameters) return {}

    const defaults: Record<string, number | boolean | string> = {}
    expression.parameters.forEach(param => {
      defaults[param.name] = param.defaultValue
    })
    return defaults
  }

  const previewExpression = (expression: PatternExpression): string => {
    let preview = expression.expression

    if (expression.parameters) {
      const params = parameterValues[expression.id] || getDefaultParameters(expression)
      Object.entries(params).forEach(([key, value]) => {
        preview = preview.replace(`\${${key}}`, value.toString())
      })
    }

    return preview.replace(/totalChannels/g, totalChannels.toString())
  }

  const getActiveChannels = (expression: PatternExpression): number[] => {
    try {
      const activeChannels: number[] = []
      const params = expression.parameters
        ? (parameterValues[expression.id] || getDefaultParameters(expression))
        : {}

      for (let i = 0; i < totalChannels; i++) {
        let expr = expression.expression
          .replace(/\bi\b/g, i.toString())
          .replace(/\btotalChannels\b/g, totalChannels.toString())

        Object.entries(params).forEach(([key, value]) => {
          expr = expr.replace(`\${${key}}`, value.toString())
        })

        try {
          if (new Function('return ' + expr)()) {
            activeChannels.push(i)
          }
        } catch {
          // Skip invalid expressions
        }
      }

      return activeChannels
    } catch {
      return []
    }
  }

  return (
    <div className="space-y-4">
      {/* Category Selector */}
      <div className="flex gap-2 flex-wrap">
        {categories.map(category => (
          <Button
            key={category.value}
            variant={selectedCategory === category.value ? 'default' : 'outline'}
            size="sm"
            onClick={() => setSelectedCategory(category.value)}
            className="capitalize"
          >
            {category.label}
          </Button>
        ))}
      </div>

      {/* Expression List */}
      <div className="space-y-3 max-h-96 overflow-y-auto">
        {filteredExpressions.map(expression => {
          const activeChannels = getActiveChannels(expression)
          return (
            <Card key={expression.id} className="p-3">
              <div className="flex justify-between items-start">
                <div className="flex-1 space-y-2">
                  <div className="flex items-start justify-between">
                    <div>
                      <h4 className="font-medium">{expression.name}</h4>
                      <p className="text-sm text-muted-foreground">{expression.description}</p>
                    </div>
                    <Badge
                      className={categories.find(c => c.value === expression.category)?.color}
                      variant="secondary"
                    >
                      {expression.category}
                    </Badge>
                  </div>

                  <code className="text-xs bg-muted px-2 py-1 rounded block">
                    {previewExpression(expression)}
                  </code>

                  <div className="text-xs text-muted-foreground">
                    Active channels: {activeChannels.length > 0 ? (
                      activeChannels.length > 8 ?
                        `${activeChannels.slice(0, 8).join(', ')}... (+${activeChannels.length - 8} more)` :
                        activeChannels.join(', ')
                    ) : 'None'}
                  </div>

                  {/* Parameter Controls */}
                  {expression.parameters && (
                    <div className="space-y-2 pt-2 border-t">
                      <div className="text-xs font-medium text-muted-foreground">Parameters:</div>
                      <div className="grid grid-cols-2 gap-2">
                        {expression.parameters.map(param => (
                          <div key={param.name} className="space-y-1">
                            <Label className="text-xs">{param.name}:</Label>
                            {param.type === 'number' ? (
                              <Input
                                type="number"
                                min={param.min}
                                max={param.max}
                                defaultValue={String(param.defaultValue)}
                                onChange={(e) => handleParameterChange(expression.id, param.name, parseInt(e.target.value))}
                                className="h-7 text-xs"
                              />
                            ) : (
                              <Button
                                variant="outline"
                                size="sm"
                                onClick={() => {
                                  const currentValue = parameterValues[expression.id]?.[param.name] ?? param.defaultValue
                                  handleParameterChange(expression.id, param.name, !currentValue)
                                }}
                                className="h-7 text-xs w-full justify-center"
                              >
                                {(parameterValues[expression.id]?.[param.name] ?? param.defaultValue) ? 'True' : 'False'}
                              </Button>
                            )}
                          </div>
                        ))}
                      </div>
                    </div>
                  )}
                </div>

                <Button
                  size="sm"
                  onClick={() => handleAddExpression(expression)}
                  className="ml-2 flex-shrink-0"
                >
                  <Plus className="h-3 w-3" />
                </Button>
              </div>
            </Card>
          )
        })}

        {filteredExpressions.length === 0 && (
          <div className="text-center py-8 text-muted-foreground">
            <p className="text-sm">No expressions in this category</p>
          </div>
        )}
      </div>

      {/* Category Info */}
      <div className="text-xs text-muted-foreground p-3 bg-muted rounded-lg">
        <div className="font-medium mb-1">
          {categories.find(c => c.value === selectedCategory)?.label} Patterns:
        </div>
        <div>
          {selectedCategory === 'basic' && 'Simple ON/OFF patterns for entire channel groups'}
          {selectedCategory === 'mathematical' && 'Patterns based on mathematical operations and modulo arithmetic'}
          {selectedCategory === 'sequential' && 'Range-based patterns that affect consecutive channel groups'}
          {selectedCategory === 'custom' && 'Configurable patterns with adjustable parameters'}
        </div>
      </div>
    </div>
  )
}