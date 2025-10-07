"use client"

import { useState, useEffect, useRef } from "react"
import { useALS } from "@/lib/contexts/ALSContext"
import { ConnectionMonitor, ConnectionStatus as ConnectionStatusType } from "@/lib/utils/connection"
import { Badge } from "@/components/ui/badge"
import { Wifi, WifiOff, AlertTriangle } from "lucide-react"

export default function ConnectionStatus() {
  const { alsData } = useALS()
  const [isClient, setIsClient] = useState(false)
  const [connectionStatus, setConnectionStatus] = useState<ConnectionStatusType>({
    isConnected: false,
    lastChecked: new Date(0)
  })
  const connectionMonitorRef = useRef<ConnectionMonitor | null>(null)

  useEffect(() => {
    setIsClient(true)
  }, [])

  useEffect(() => {
    if (!connectionMonitorRef.current) {
      connectionMonitorRef.current = new ConnectionMonitor(alsData.apiConfig)
    } else {
      connectionMonitorRef.current.updateConfig(alsData.apiConfig)
    }

    const handleStatusUpdate = (status: ConnectionStatusType) => {
      setConnectionStatus(status)
    }

    connectionMonitorRef.current.addListener(handleStatusUpdate)
    connectionMonitorRef.current.startMonitoring(30000)

    return () => {
      if (connectionMonitorRef.current) {
        connectionMonitorRef.current.removeListener(handleStatusUpdate)
        connectionMonitorRef.current.stopMonitoring()
      }
    }
  }, [alsData.apiConfig])

  const getStatusIcon = () => {
    if (connectionStatus.isConnected) {
      if (connectionStatus.responseTime && connectionStatus.responseTime > 5000) {
        return <AlertTriangle className="h-3 w-3" />
      }
      return <Wifi className="h-3 w-3" />
    }
    return <WifiOff className="h-3 w-3" />
  }

  const getStatusVariant = () => {
    if (connectionStatus.isConnected) {
      if (connectionStatus.responseTime && connectionStatus.responseTime > 5000) {
        return "outline" as const
      }
      return "default" as const
    }
    return "destructive" as const
  }

  const getStatusText = () => {
    if (connectionStatus.isConnected) {
      return "Online"
    }
    return "Offline"
  }

  const getTitle = () => {
    if (!isClient) {
      return "Connection Status"
    }
    
    if (connectionStatus.error) {
      return connectionStatus.error
    }
    
    if (connectionStatus.lastChecked.getTime() === 0) {
      return "Checking connection..."
    }
    
    return `Last checked: ${connectionStatus.lastChecked.toLocaleTimeString()}`
  }

  return (
    <Badge 
      variant={getStatusVariant()} 
      className="flex items-center gap-1 text-xs"
      title={getTitle()}
    >
      {getStatusIcon()}
      {getStatusText()}
    </Badge>
  )
}