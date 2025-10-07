"use client"

import React, { Component, ErrorInfo, ReactNode } from 'react'

interface Props {
  children?: ReactNode
}

interface State {
  hasError: boolean
  error?: Error
}

export class ErrorBoundary extends Component<Props, State> {
  public state: State = {
    hasError: false,
  }

  public static getDerivedStateFromError(error: Error): State {
    return { hasError: true, error }
  }

  public componentDidCatch(error: Error, errorInfo: ErrorInfo) {
    console.error('Uncaught error:', error, errorInfo)
  }

  public render() {
    if (this.state.hasError) {
      return (
        <div className="min-h-screen flex items-center justify-center bg-background">
          <div className="max-w-md w-full mx-4">
            <div className="bg-card border border-border rounded-lg p-6">
              <div className="text-center">
                <h1 className="text-2xl font-bold text-destructive mb-4">
                  Something went wrong
                </h1>
                <p className="text-muted-foreground mb-6">
                  The AutoLight application encountered an unexpected error.
                </p>
                <div className="space-y-3">
                  <button
                    onClick={() => window.location.reload()}
                    className="w-full bg-primary text-primary-foreground hover:bg-primary/90 px-4 py-2 rounded-md transition-colors"
                  >
                    Reload Application
                  </button>
                  <button
                    onClick={() => this.setState({ hasError: false, error: undefined })}
                    className="w-full bg-secondary text-secondary-foreground hover:bg-secondary/90 px-4 py-2 rounded-md transition-colors"
                  >
                    Try Again
                  </button>
                </div>
                {process.env.NODE_ENV === 'development' && this.state.error && (
                  <details className="mt-6 text-left">
                    <summary className="cursor-pointer text-sm text-muted-foreground hover:text-foreground">
                      Error Details (Development)
                    </summary>
                    <pre className="mt-2 text-xs bg-muted p-3 rounded overflow-auto max-h-40">
                      {this.state.error.stack}
                    </pre>
                  </details>
                )}
              </div>
            </div>
          </div>
        </div>
      )
    }

    return this.props.children
  }
}