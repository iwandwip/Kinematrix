"use client"

import { useState, useEffect, useMemo, useCallback } from "react"
import { useALS } from "@/lib/contexts/ALSContext"
import { useALSPolling } from "@/lib/hooks/useALSPolling"
import { testConnection, validateHost, validatePort, detectConnectionType } from "@/lib/api/config"
import { Button } from "@/components/ui/button"
import { Input } from "@/components/ui/input"
import { Label } from "@/components/ui/label"
import { Card, CardContent, CardDescription, CardFooter, CardHeader, CardTitle } from "@/components/ui/card"
import { RadioGroup, RadioGroupItem } from "@/components/ui/radio-group"
import { Alert, AlertDescription } from "@/components/ui/alert"
import { Badge } from "@/components/ui/badge"
import Spinner from "@/components/ui/spinner"
import { CheckCircle, XCircle, Wifi, WifiOff } from "lucide-react"

export default function ConnectionPageContent() {
  const { alsData, setApiConfig, resetApiConfig } = useALS()
  useALSPolling()
  const [formData, setFormData] = useState({
    host: '192.168.4.1',
    port: 8000,
    protocol: 'http',
    connectionType: 'ip' as 'ip' | 'dns'
  })
  const [isLoading, setIsLoading] = useState(false)
  const [testResult, setTestResult] = useState<{ success: boolean; error?: string; responseTime?: number } | null>(null)
  const [errors, setErrors] = useState<{ host?: string; port?: string }>({})
  const [isConnected, setIsConnected] = useState<boolean | null>(null)

  useEffect(() => {
    setFormData({
      host: alsData.apiConfig.host || '192.168.4.1',
      port: alsData.apiConfig.port || 8000,
      protocol: alsData.apiConfig.protocol || 'http',
      connectionType: alsData.apiConfig.connectionType || 'ip'
    })
  }, [alsData.apiConfig.host, alsData.apiConfig.port, alsData.apiConfig.protocol, alsData.apiConfig.connectionType])

  const handleInputChange = useCallback((field: keyof typeof formData, value: string | number) => {
    setFormData(prev => {
      const newData = { ...prev, [field]: value }
      if (field === 'host') {
        newData.connectionType = detectConnectionType(value as string)
      }
      return newData
    })
    setErrors(prev => ({ ...prev, [field === 'host' ? 'host' : field]: undefined }))
  }, [])

  const validateForm = useCallback(() => {
    const newErrors: { host?: string; port?: string } = {}
    
    const hostValidation = validateHost(formData.host)
    if (!hostValidation.isValid) {
      newErrors.host = hostValidation.error
    }

    const portValidation = validatePort(formData.port)
    if (!portValidation.isValid) {
      newErrors.port = portValidation.error
    }

    setErrors(newErrors)
    return Object.keys(newErrors).length === 0
  }, [formData.host, formData.port])

  const handleTestConnection = useCallback(async () => {
    if (!validateForm()) return

    setIsLoading(true)
    setTestResult(null)
    
    try {
      const result = await testConnection(formData)
      setTestResult(result)
      setIsConnected(result.success)
    } catch {
      setTestResult({ success: false, error: 'Test connection failed' })
      setIsConnected(false)
    } finally {
      setIsLoading(false)
    }
  }, [formData, validateForm])

  const handleSaveConfig = useCallback(() => {
    if (!validateForm()) return
    
    setApiConfig(formData)
    setIsConnected(null)
    setTestResult(null)
  }, [formData, validateForm, setApiConfig])

  const handleResetConfig = useCallback(() => {
    resetApiConfig()
    setFormData({ host: '192.168.4.1', port: 8000, protocol: 'http', connectionType: 'ip' })
    setIsConnected(null)
    setTestResult(null)
    setErrors({})
  }, [resetApiConfig])

  const isFormValid = useMemo(() => {
    const hostValidation = validateHost(formData.host)
    const portValidation = validatePort(formData.port)
    return hostValidation.isValid && portValidation.isValid
  }, [formData.host, formData.port])

  const hasChanges = useMemo(() => {
    return formData.host !== alsData.apiConfig.host ||
           formData.port !== alsData.apiConfig.port ||
           formData.protocol !== alsData.apiConfig.protocol ||
           formData.connectionType !== alsData.apiConfig.connectionType
  }, [formData.host, formData.port, formData.protocol, formData.connectionType, alsData.apiConfig.host, alsData.apiConfig.port, alsData.apiConfig.protocol, alsData.apiConfig.connectionType])

  const handlePresetHost = useCallback((host: string, type: 'ip' | 'dns') => {
    setFormData(prev => ({ ...prev, host, connectionType: type }))
    setErrors(prev => ({ ...prev, host: undefined }))
  }, [])

  return (
    <div className="grid gap-4 lg:gap-6">
      <Card>
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            {isConnected === true ? (
              <Wifi className="h-5 w-5 text-green-500" />
            ) : isConnected === false ? (
              <WifiOff className="h-5 w-5 text-red-500" />
            ) : (
              <Wifi className="h-5 w-5 text-muted-foreground" />
            )}
            Konfigurasi Koneksi
          </CardTitle>
          <CardDescription>
            Atur alamat IP dan port untuk terhubung ke perangkat AutoLight V3
          </CardDescription>
        </CardHeader>
        
        <CardContent className="space-y-6">
          <div className="grid gap-4">
            <div className="space-y-2">
              <Label htmlFor="protocol">Protokol</Label>
              <RadioGroup
                value={formData.protocol}
                onValueChange={(value) => handleInputChange('protocol', value)}
                className="flex gap-4"
              >
                <div className="flex items-center space-x-2">
                  <RadioGroupItem value="http" id="http" />
                  <Label htmlFor="http">HTTP</Label>
                </div>
                <div className="flex items-center space-x-2">
                  <RadioGroupItem value="https" id="https" />
                  <Label htmlFor="https">HTTPS</Label>
                </div>
              </RadioGroup>
            </div>

            <div className="space-y-4">
              <div className="space-y-2">
                <Label>Jenis Koneksi</Label>
                <RadioGroup
                  value={formData.connectionType}
                  onValueChange={(value) => handleInputChange('connectionType', value as 'ip' | 'dns')}
                  className="flex gap-4"
                >
                  <div className="flex items-center space-x-2">
                    <RadioGroupItem value="ip" id="connection-ip" />
                    <Label htmlFor="connection-ip">Alamat IP</Label>
                  </div>
                  <div className="flex items-center space-x-2">
                    <RadioGroupItem value="dns" id="connection-dns" />
                    <Label htmlFor="connection-dns">DNS/Hostname</Label>
                  </div>
                </RadioGroup>
              </div>
              
              <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
                <div className="space-y-2">
                  <Label htmlFor="host">
                    {formData.connectionType === 'ip' ? 'Alamat IP' : 'DNS/Hostname'}
                  </Label>
                  <div className="space-y-2">
                    <Input
                      id="host"
                      type="text"
                      value={formData.host}
                      onChange={(e) => handleInputChange('host', e.target.value)}
                      placeholder={formData.connectionType === 'ip' ? "192.168.4.1" : "als.local"}
                      className={errors.host ? "border-red-500" : ""}
                    />
                    <div className="flex flex-wrap gap-2">
                      <Button
                        type="button"
                        variant="outline"
                        size="sm"
                        onClick={() => handlePresetHost('192.168.4.1', 'ip')}
                      >
                        Default IP
                      </Button>
                      <Button
                        type="button"
                        variant="outline"
                        size="sm"
                        onClick={() => handlePresetHost('als.local', 'dns')}
                      >
                        mDNS
                      </Button>
                    </div>
                  </div>
                  {errors.host && (
                    <p className="text-sm text-red-500">{errors.host}</p>
                  )}
                </div>

              <div className="space-y-2">
                <Label htmlFor="port">Port</Label>
                <Input
                  id="port"
                  type="number"
                  value={formData.port}
                  onChange={(e) => handleInputChange('port', parseInt(e.target.value) || 0)}
                  placeholder="8000"
                  min="1"
                  max="65535"
                  className={errors.port ? "border-red-500" : ""}
                />
                {errors.port && (
                  <p className="text-sm text-red-500">{errors.port}</p>
                )}
              </div>
            </div>
            </div>

            <div className="bg-muted p-3 rounded-md">
              <p className="text-sm font-medium mb-2">URL Target:</p>
              <p className="text-xs sm:text-sm text-muted-foreground font-mono break-all">
                {formData.protocol}://{formData.host}:{formData.port}/api/v1
              </p>
              <p className="text-xs text-muted-foreground mt-2">
                Jenis: {formData.connectionType === 'ip' ? 'Alamat IP' : 'DNS/Hostname'}
              </p>
            </div>
          </div>

          {testResult && (
            <Alert className={testResult.success ? "border-green-500" : "border-red-500"}>
              <div className="flex items-center gap-2">
                {testResult.success ? (
                  <CheckCircle className="h-4 w-4 text-green-500" />
                ) : (
                  <XCircle className="h-4 w-4 text-red-500" />
                )}
                <AlertDescription>
                  {testResult.success ? (
                    <span>
                      Koneksi berhasil! 
                      {testResult.responseTime && (
                        <span className="ml-1">({testResult.responseTime}ms)</span>
                      )}
                    </span>
                  ) : (
                    <span>Koneksi gagal: {testResult.error}</span>
                  )}
                </AlertDescription>
              </div>
            </Alert>
          )}
        </CardContent>

        <CardFooter className="flex flex-col gap-3 sm:flex-row sm:justify-between">
          <div className="flex flex-wrap gap-2 w-full sm:w-auto justify-center sm:justify-start">
            <Button
              onClick={handleTestConnection}
              disabled={isLoading || !isFormValid}
              variant="outline"
            >
              {isLoading && <Spinner className="mr-2 h-4 w-4" />}
              Test Koneksi
            </Button>
            <Button
              onClick={handleResetConfig}
              variant="outline"
            >
              Reset Default
            </Button>
          </div>
          
          <Button
            onClick={handleSaveConfig}
            disabled={!hasChanges || !isFormValid}
            className="w-full sm:w-auto"
          >
            Simpan Konfigurasi
          </Button>
        </CardFooter>
      </Card>

      <Card>
        <CardHeader>
          <CardTitle>Status Koneksi Saat Ini</CardTitle>
          <CardDescription>
            Informasi konfigurasi yang sedang aktif
          </CardDescription>
        </CardHeader>
        <CardContent>
          <div className="grid gap-3 text-sm">
            <div className="flex flex-col gap-1 sm:flex-row sm:justify-between sm:items-center">
              <span className="font-medium">Protokol:</span>
              <Badge variant="outline">{alsData.apiConfig.protocol.toUpperCase()}</Badge>
            </div>
            <div className="flex flex-col gap-1 sm:flex-row sm:justify-between sm:items-center">
              <span className="font-medium">
                {alsData.apiConfig.connectionType === 'ip' ? 'IP Address:' : 'DNS/Hostname:'}
              </span>
              <Badge variant="outline" className="self-start sm:self-auto break-all">{alsData.apiConfig.host}</Badge>
            </div>
            <div className="flex flex-col gap-1 sm:flex-row sm:justify-between sm:items-center">
              <span className="font-medium">Port:</span>
              <Badge variant="outline">{alsData.apiConfig.port}</Badge>
            </div>
            <div className="flex flex-col gap-1 sm:flex-row sm:justify-between sm:items-center">
              <span className="font-medium">Jenis Koneksi:</span>
              <Badge variant="outline" className="self-start sm:self-auto">
                {alsData.apiConfig.connectionType === 'ip' ? 'Alamat IP' : 'DNS/Hostname'}
              </Badge>
            </div>
            <div className="flex flex-col gap-1 sm:flex-row sm:justify-between sm:items-center">
              <span className="font-medium">Status:</span>
              <div className="self-start sm:self-auto">
                {isConnected === true ? (
                  <Badge className="bg-green-500">Terhubung</Badge>
                ) : isConnected === false ? (
                  <Badge variant="destructive">Terputus</Badge>
                ) : (
                  <Badge variant="outline">Belum ditest</Badge>
                )}
              </div>
            </div>
          </div>
        </CardContent>
      </Card>
    </div>
  )
}