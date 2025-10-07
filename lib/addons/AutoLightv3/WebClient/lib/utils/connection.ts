import { testConnection } from '@/lib/api/config'

export interface ConnectionStatus {
  isConnected: boolean
  lastChecked: Date
  responseTime?: number
  error?: string
}

export class ConnectionMonitor {
  private status: ConnectionStatus = {
    isConnected: false,
    lastChecked: new Date()
  }
  
  private listeners: ((status: ConnectionStatus) => void)[] = []
  private intervalId: NodeJS.Timeout | null = null

  constructor(private apiConfig: { host: string; port: number; protocol: string }) {}

  public getStatus(): ConnectionStatus {
    return { ...this.status }
  }

  public addListener(callback: (status: ConnectionStatus) => void): void {
    this.listeners.push(callback)
  }

  public removeListener(callback: (status: ConnectionStatus) => void): void {
    const index = this.listeners.indexOf(callback)
    if (index > -1) {
      this.listeners.splice(index, 1)
    }
  }

  public async checkConnection(): Promise<ConnectionStatus> {
    try {
      const result = await testConnection(this.apiConfig)
      this.status = {
        isConnected: result.success,
        lastChecked: new Date(),
        responseTime: result.responseTime,
        error: result.error
      }
    } catch (error) {
      this.status = {
        isConnected: false,
        lastChecked: new Date(),
        error: error instanceof Error ? error.message : 'Connection check failed'
      }
    }

    this.notifyListeners()
    return this.getStatus()
  }

  public startMonitoring(intervalMs: number = 30000): void {
    this.stopMonitoring()
    
    this.checkConnection()
    this.intervalId = setInterval(() => {
      this.checkConnection()
    }, intervalMs)
  }

  public stopMonitoring(): void {
    if (this.intervalId) {
      clearInterval(this.intervalId)
      this.intervalId = null
    }
  }

  public updateConfig(newConfig: { host: string; port: number; protocol: string }): void {
    this.apiConfig = newConfig
    this.checkConnection()
  }

  private notifyListeners(): void {
    this.listeners.forEach(callback => callback(this.getStatus()))
  }
}

export function formatResponseTime(ms?: number): string {
  if (ms === undefined) return 'N/A'
  if (ms < 1000) return `${ms}ms`
  return `${(ms / 1000).toFixed(1)}s`
}

export function getConnectionStatusText(status: ConnectionStatus): string {
  if (status.isConnected) {
    return `Terhubung (${formatResponseTime(status.responseTime)})`
  } else {
    return status.error || 'Terputus'
  }
}

export function getConnectionStatusColor(status: ConnectionStatus): 'success' | 'error' | 'warning' {
  if (status.isConnected) {
    return status.responseTime && status.responseTime > 5000 ? 'warning' : 'success'
  }
  return 'error'
}