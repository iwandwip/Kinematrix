"use client"

import { useState, useEffect } from "react"
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card"
import { Button } from "@/components/ui/button"
import { Input } from "@/components/ui/input"
import { Label } from "@/components/ui/label"
import { Textarea } from "@/components/ui/textarea"
import { Badge } from "@/components/ui/badge"
import { Alert, AlertDescription } from "@/components/ui/alert"
import { Table, TableBody, TableCell, TableHead, TableHeader, TableRow } from "@/components/ui/table"
import { 
  Play, 
  Square, 
  RefreshCw, 
  CheckCircle2, 
  XCircle, 
  Clock, 
  Download,
  Zap,
  Eye
} from "lucide-react"
import { 
  getDataDeviceChannels,
  getDataDeviceName,
  getDataDeviceSerial,
  getDataDeviceMode,
  getDataDeviceDelay,
  setDataDeviceMode,
  setDataDeviceName,
  setDataDeviceDelay
} from "@/lib/api/device"

interface TestResult {
  endpoint: string
  method: string
  status: 'success' | 'error' | 'pending'
  responseTime: number
  response?: unknown
  error?: string
  timestamp: Date
}

interface APIConfig {
  host: string
  port: number
  protocol: string
}

export default function APITestContent() {
  const [apiConfig, setApiConfig] = useState<APIConfig>({ host: '192.168.4.1', port: 8000, protocol: 'http' })
  const [testResults, setTestResults] = useState<TestResult[]>([])
  const [isRunning, setIsRunning] = useState(false)
  const [connectionStatus, setConnectionStatus] = useState<'connected' | 'disconnected' | 'testing'>('disconnected')

  const [testValues, setTestValues] = useState({
    mode: '1',
    delay: '100',
    deviceName: 'AutoLight V3'
  })

  useEffect(() => {
    if (typeof window !== 'undefined') {
      const savedConfig = localStorage.getItem('autolight-api-config')
      if (savedConfig) {
        try {
          const config = JSON.parse(savedConfig)
          setApiConfig({
            host: config.host || config.ip || '192.168.4.1',
            port: config.port || 8000,
            protocol: config.protocol || 'http'
          })
        } catch (error) {
          console.error('Failed to parse saved API config:', error)
        }
      }
    }
  }, [])

  const addTestResult = (result: Omit<TestResult, 'timestamp'>) => {
    setTestResults(prev => [{ ...result, timestamp: new Date() }, ...prev.slice(0, 49)])
  }

  const testSingleEndpoint = async (
    name: string,
    method: string,
    testFunction: () => Promise<Response>
  ) => {
    const startTime = Date.now()
    
    try {
      addTestResult({
        endpoint: name,
        method: method,
        status: 'pending',
        responseTime: 0
      })

      const response = await testFunction()
      const responseTime = Date.now() - startTime
      const responseData = await response.text()

      if (response.ok) {
        addTestResult({
          endpoint: name,
          method: method,
          status: 'success',
          responseTime,
          response: responseData
        })
      } else {
        addTestResult({
          endpoint: name,
          method: method,
          status: 'error',
          responseTime,
          error: `HTTP ${response.status}: ${response.statusText}`
        })
      }
    } catch (error) {
      const responseTime = Date.now() - startTime
      addTestResult({
        endpoint: name,
        method: method,
        status: 'error',
        responseTime,
        error: error instanceof Error ? error.message : 'Unknown error'
      })
    }
  }

  const testConnection = async () => {
    setConnectionStatus('testing')
    await testSingleEndpoint(
      'Connection Test',
      'GET',
      () => getDataDeviceMode()
    )
    setConnectionStatus(testResults[0]?.status === 'success' ? 'connected' : 'disconnected')
  }

  const testAllGetEndpoints = async () => {
    setIsRunning(true)
    
    const getTests = [
      { name: 'Get Device Mode', fn: getDataDeviceMode },
      { name: 'Get Device Delay', fn: getDataDeviceDelay },
      { name: 'Get Device Name', fn: getDataDeviceName },
      { name: 'Get Device Serial', fn: getDataDeviceSerial },
      { name: 'Get Device Channels', fn: getDataDeviceChannels }
    ]

    for (const test of getTests) {
      await testSingleEndpoint(test.name, 'GET', test.fn)
      await new Promise(resolve => setTimeout(resolve, 500))
    }
    
    setIsRunning(false)
  }

  const testAllSetEndpoints = async () => {
    setIsRunning(true)
    
    const setTests = [
      { 
        name: `Set Device Mode (${testValues.mode})`, 
        fn: () => setDataDeviceMode(testValues.mode) 
      },
      { 
        name: `Set Device Delay (${testValues.delay})`, 
        fn: () => setDataDeviceDelay(parseInt(testValues.delay)) 
      },
      { 
        name: `Set Device Name (${testValues.deviceName})`, 
        fn: () => setDataDeviceName(testValues.deviceName) 
      }
    ]

    for (const test of setTests) {
      await testSingleEndpoint(test.name, 'POST', test.fn)
      await new Promise(resolve => setTimeout(resolve, 500))
    }
    
    setIsRunning(false)
  }

  const testAllEndpoints = async () => {
    setIsRunning(true)
    await testConnection()
    await new Promise(resolve => setTimeout(resolve, 1000))
    await testAllGetEndpoints()
    await new Promise(resolve => setTimeout(resolve, 1000))
    await testAllSetEndpoints()
    setIsRunning(false)
  }

  const clearResults = () => {
    setTestResults([])
  }

  const exportResults = () => {
    const dataStr = JSON.stringify(testResults, null, 2)
    const dataBlob = new Blob([dataStr], { type: 'application/json' })
    const url = URL.createObjectURL(dataBlob)
    const link = document.createElement('a')
    link.href = url
    link.download = `autolight-api-test-results-${new Date().toISOString().slice(0, 19).replace(/:/g, '-')}.json`
    link.click()
    URL.revokeObjectURL(url)
  }

  const getStatusBadge = (status: TestResult['status']) => {
    switch (status) {
      case 'success':
        return <Badge variant="default" className="bg-green-500"><CheckCircle2 className="h-3 w-3 mr-1" />Success</Badge>
      case 'error':
        return <Badge variant="destructive"><XCircle className="h-3 w-3 mr-1" />Error</Badge>
      case 'pending':
        return <Badge variant="secondary"><Clock className="h-3 w-3 mr-1" />Running...</Badge>
    }
  }

  const formatResponseTime = (ms: number) => {
    if (ms < 1000) return `${ms}ms`
    return `${(ms / 1000).toFixed(1)}s`
  }

  return (
    <div className="space-y-4 p-4 sm:space-y-6 sm:p-6">
      <div className="flex items-center space-x-2">
        <Zap className="h-5 w-5" />
        <h1 className="text-2xl font-bold">API Test</h1>
      </div>

      <Alert>
        <AlertDescription>
          Halaman ini memungkinkan Anda untuk menguji semua endpoint API AutoLight V3 secara manual atau otomatis.
          Pastikan device AutoLight V3 terhubung dan dapat diakses.
        </AlertDescription>
      </Alert>

      <div className="grid gap-4 lg:gap-6 lg:grid-cols-2">
        <Card>
          <CardHeader>
            <CardTitle>Connection Test</CardTitle>
            <CardDescription>Test koneksi ke AutoLight V3 device</CardDescription>
          </CardHeader>
          <CardContent className="space-y-4">
            <div className="flex items-center space-x-2">
              <Badge 
                variant={connectionStatus === 'connected' ? 'default' : 'destructive'}
                className={connectionStatus === 'connected' ? 'bg-green-500' : ''}
              >
                {connectionStatus === 'connected' && <CheckCircle2 className="h-3 w-3 mr-1" />}
                {connectionStatus === 'disconnected' && <XCircle className="h-3 w-3 mr-1" />}
                {connectionStatus === 'testing' && <RefreshCw className="h-3 w-3 mr-1 animate-spin" />}
                {connectionStatus === 'connected' ? 'Connected' : connectionStatus === 'disconnected' ? 'Disconnected' : 'Testing...'}
              </Badge>
              <span className="text-sm text-muted-foreground">
                {apiConfig.protocol}://{apiConfig.host}:{apiConfig.port}
              </span>
            </div>
            <Button onClick={testConnection} disabled={connectionStatus === 'testing'}>
              <RefreshCw className="h-4 w-4 mr-2" />
              Test Connection
            </Button>
          </CardContent>
        </Card>

        <Card>
          <CardHeader>
            <CardTitle>Bulk Test</CardTitle>
            <CardDescription>Test semua endpoint sekaligus</CardDescription>
          </CardHeader>
          <CardContent className="space-y-4">
            <div className="grid grid-cols-1 sm:grid-cols-2 gap-2">
              <Button onClick={testAllGetEndpoints} disabled={isRunning}>
                <Play className="h-4 w-4 mr-2" />
                Test GET APIs
              </Button>
              <Button onClick={testAllSetEndpoints} disabled={isRunning}>
                <Play className="h-4 w-4 mr-2" />
                Test SET APIs
              </Button>
            </div>
            <Button onClick={testAllEndpoints} disabled={isRunning} className="w-full">
              {isRunning ? (
                <RefreshCw className="h-4 w-4 mr-2 animate-spin" />
              ) : (
                <Play className="h-4 w-4 mr-2" />
              )}
              Test All Endpoints
            </Button>
          </CardContent>
        </Card>
      </div>

      <Card>
        <CardHeader>
          <CardTitle>GET Endpoint Testing</CardTitle>
          <CardDescription>Test endpoint untuk mengambil data dari device</CardDescription>
        </CardHeader>
        <CardContent className="space-y-4">
          <div className="grid grid-cols-1 sm:grid-cols-2 md:grid-cols-3 gap-4">
            <Button 
              variant="outline"
              onClick={() => testSingleEndpoint('Get Device Mode', 'GET', getDataDeviceMode)}
              disabled={isRunning}
              className="flex items-center justify-center space-x-2"
            >
              <Eye className="h-4 w-4" />
              <span>Get Mode</span>
            </Button>
            <Button 
              variant="outline"
              onClick={() => testSingleEndpoint('Get Device Delay', 'GET', getDataDeviceDelay)}
              disabled={isRunning}
              className="flex items-center justify-center space-x-2"
            >
              <Eye className="h-4 w-4" />
              <span>Get Delay</span>
            </Button>
            <Button 
              variant="outline"
              onClick={() => testSingleEndpoint('Get Device Name', 'GET', getDataDeviceName)}
              disabled={isRunning}
              className="flex items-center justify-center space-x-2"
            >
              <Eye className="h-4 w-4" />
              <span>Get Name</span>
            </Button>
            <Button 
              variant="outline"
              onClick={() => testSingleEndpoint('Get Device Serial', 'GET', getDataDeviceSerial)}
              disabled={isRunning}
              className="flex items-center justify-center space-x-2"
            >
              <Eye className="h-4 w-4" />
              <span>Get Serial</span>
            </Button>
            <Button 
              variant="outline"
              onClick={() => testSingleEndpoint('Get Device Channels', 'GET', getDataDeviceChannels)}
              disabled={isRunning}
              className="flex items-center justify-center space-x-2"
            >
              <Eye className="h-4 w-4" />
              <span>Get Channels</span>
            </Button>
          </div>
        </CardContent>
      </Card>

      <Card>
        <CardHeader>
          <CardTitle>SET Endpoint Testing</CardTitle>
          <CardDescription>Test endpoint untuk mengubah konfigurasi device</CardDescription>
        </CardHeader>
        <CardContent className="space-y-4">
          <div className="grid grid-cols-1 gap-4">
            <div className="space-y-2">
              <Label htmlFor="mode">Device Mode (0-15)</Label>
              <Input
                id="mode"
                value={testValues.mode}
                onChange={(e) => setTestValues(prev => ({ ...prev, mode: e.target.value }))}
                placeholder="0-15"
              />
              <Button 
                size="sm" 
                onClick={() => testSingleEndpoint(`Set Mode (${testValues.mode})`, 'POST', () => setDataDeviceMode(testValues.mode))}
                disabled={isRunning}
                className="w-full"
              >
                Test Set Mode
              </Button>
            </div>
            <div className="space-y-2">
              <Label htmlFor="delay">Delay (ms)</Label>
              <Input
                id="delay"
                value={testValues.delay}
                onChange={(e) => setTestValues(prev => ({ ...prev, delay: e.target.value }))}
                placeholder="30-300"
              />
              <Button 
                size="sm" 
                onClick={() => testSingleEndpoint(`Set Delay (${testValues.delay})`, 'POST', () => setDataDeviceDelay(parseInt(testValues.delay)))}
                disabled={isRunning}
                className="w-full"
              >
                Test Set Delay
              </Button>
            </div>
            <div className="space-y-2">
              <Label htmlFor="deviceName">Device Name</Label>
              <Input
                id="deviceName"
                value={testValues.deviceName}
                onChange={(e) => setTestValues(prev => ({ ...prev, deviceName: e.target.value }))}
                placeholder="AutoLight V3"
              />
              <Button 
                size="sm" 
                onClick={() => testSingleEndpoint(`Set Name (${testValues.deviceName})`, 'POST', () => setDataDeviceName(testValues.deviceName))}
                disabled={isRunning}
                className="w-full"
              >
                Test Set Name
              </Button>
            </div>
          </div>
        </CardContent>
      </Card>

      <Card>
        <CardHeader>
          <CardTitle className="flex items-center justify-between">
            <span>Test Results</span>
            <div className="flex space-x-2">
              <Button variant="outline" size="sm" onClick={exportResults} disabled={testResults.length === 0}>
                <Download className="h-4 w-4 mr-2" />
                Export
              </Button>
              <Button variant="outline" size="sm" onClick={clearResults} disabled={testResults.length === 0}>
                <Square className="h-4 w-4 mr-2" />
                Clear
              </Button>
            </div>
          </CardTitle>
          <CardDescription>Hasil test API endpoints (max 50 results)</CardDescription>
        </CardHeader>
        <CardContent>
          {testResults.length === 0 ? (
            <div className="text-center text-muted-foreground py-8">
              Belum ada hasil test. Jalankan test untuk melihat hasilnya.
            </div>
          ) : (
            <div className="space-y-4">
              <Table>
                <TableHeader>
                  <TableRow>
                    <TableHead>Endpoint</TableHead>
                    <TableHead>Method</TableHead>
                    <TableHead>Status</TableHead>
                    <TableHead>Response Time</TableHead>
                    <TableHead>Time</TableHead>
                  </TableRow>
                </TableHeader>
                <TableBody>
                  {testResults.slice(0, 10).map((result, index) => (
                    <TableRow key={index}>
                      <TableCell className="font-medium">{result.endpoint}</TableCell>
                      <TableCell>{result.method}</TableCell>
                      <TableCell>{getStatusBadge(result.status)}</TableCell>
                      <TableCell>{formatResponseTime(result.responseTime)}</TableCell>
                      <TableCell>{result.timestamp.toLocaleTimeString()}</TableCell>
                    </TableRow>
                  ))}
                </TableBody>
              </Table>
              
              {testResults.length > 10 && (
                <div className="text-center text-sm text-muted-foreground">
                  Showing 10 of {testResults.length} results. Export to see all.
                </div>
              )}

              {testResults[0] && (
                <Card>
                  <CardHeader>
                    <CardTitle className="text-sm">Latest Response</CardTitle>
                  </CardHeader>
                  <CardContent>
                    <div className="space-y-2">
                      <div className="flex items-center space-x-2">
                        <Badge>{testResults[0].endpoint}</Badge>
                        {getStatusBadge(testResults[0].status)}
                        <span className="text-sm text-muted-foreground">
                          {formatResponseTime(testResults[0].responseTime)}
                        </span>
                      </div>
                      <Textarea
                        readOnly
                        value={testResults[0].error || String(testResults[0].response) || 'No response data'}
                        className="font-mono text-xs"
                        rows={4}
                      />
                    </div>
                  </CardContent>
                </Card>
              )}
            </div>
          )}
        </CardContent>
      </Card>
    </div>
  )
}