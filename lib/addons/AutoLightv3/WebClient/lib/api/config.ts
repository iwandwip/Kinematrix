export function getBaseURL(apiConfig: { host: string; port: number; protocol: string }): string {
  return `${apiConfig.protocol}://${apiConfig.host}:${apiConfig.port}/api/v1`
}

export async function testConnection(apiConfig: { host: string; port: number; protocol: string }): Promise<{ success: boolean; error?: string; responseTime?: number }> {
  const startTime = Date.now()
  const baseURL = getBaseURL(apiConfig)
  
  try {
    const controller = new AbortController()
    const timeoutId = setTimeout(() => controller.abort(), 5000)
    
    const response = await fetch(`${baseURL}/data/get/mode`, {
      signal: controller.signal,
      method: 'GET'
    })
    
    clearTimeout(timeoutId)
    const responseTime = Date.now() - startTime
    
    if (response.ok) {
      return { success: true, responseTime }
    } else {
      return { success: false, error: `HTTP ${response.status}: ${response.statusText}` }
    }
  } catch (error) {
    const responseTime = Date.now() - startTime
    if (error instanceof Error) {
      if (error.name === 'AbortError') {
        return { success: false, error: 'Connection timeout (5s)', responseTime }
      }
      return { success: false, error: error.message, responseTime }
    }
    return { success: false, error: 'Unknown connection error', responseTime }
  }
}

export function validateIP(ip: string): { isValid: boolean; error?: string } {
  const ipRegex = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/
  
  if (!ip.trim()) {
    return { isValid: false, error: 'IP address is required' }
  }
  
  if (!ipRegex.test(ip)) {
    return { isValid: false, error: 'Invalid IP address format' }
  }
  
  return { isValid: true }
}

export function validatePort(port: number | string): { isValid: boolean; error?: string } {
  const portNum = typeof port === 'string' ? parseInt(port, 10) : port
  
  if (isNaN(portNum)) {
    return { isValid: false, error: 'Port must be a number' }
  }
  
  if (portNum < 1 || portNum > 65535) {
    return { isValid: false, error: 'Port must be between 1 and 65535' }
  }
  
  return { isValid: true }
}

export function validateHostname(hostname: string): { isValid: boolean; error?: string } {
  const hostnameRegex = /^(?:[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?\.)*[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?$/
  
  if (!hostname.trim()) {
    return { isValid: false, error: 'Hostname is required' }
  }
  
  if (hostname.length > 253) {
    return { isValid: false, error: 'Hostname is too long (max 253 characters)' }
  }
  
  if (!hostnameRegex.test(hostname)) {
    return { isValid: false, error: 'Invalid hostname format' }
  }
  
  return { isValid: true }
}

export function validateHost(host: string): { isValid: boolean; error?: string; type: 'ip' | 'dns' } {
  if (!host.trim()) {
    return { isValid: false, error: 'Host is required', type: 'ip' }
  }
  
  const ipValidation = validateIP(host)
  if (ipValidation.isValid) {
    return { isValid: true, type: 'ip' }
  }
  
  const hostnameValidation = validateHostname(host)
  if (hostnameValidation.isValid) {
    return { isValid: true, type: 'dns' }
  }
  
  return { 
    isValid: false, 
    error: 'Invalid host format (must be IP address or hostname)',
    type: 'ip'
  }
}

export function detectConnectionType(host: string): 'ip' | 'dns' {
  const ipRegex = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/
  return ipRegex.test(host) ? 'ip' : 'dns'
}