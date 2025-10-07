import { getBaseURL } from './config'

function getAPIConfig() {
  if (typeof window !== 'undefined') {
    const savedConfig = localStorage.getItem('autolight-api-config')
    if (savedConfig) {
      try {
        const config = JSON.parse(savedConfig)
        return {
          host: config.host || config.ip || '192.168.4.1',
          port: config.port || 8000,
          protocol: config.protocol || 'http',
          connectionType: config.connectionType || (config.ip ? 'ip' : 'ip')
        }
      } catch (error) {
        console.error('Failed to parse saved API config:', error)
      }
    }
  }
  return { host: '192.168.4.1', port: 8000, protocol: 'http', connectionType: 'ip' as const }
}

export async function setDataDeviceMode(value: string) {
  try {
    const apiConfig = getAPIConfig()
    const baseURL = getBaseURL(apiConfig)
    const response = await fetch(`${baseURL}/data/set/mode?value=${value}`)
    return response
  } catch (e) {
    console.error('Error setting device mode:', e)
    throw e
  }
}

export async function setDataDeviceName(name: string) {
  try {
    const apiConfig = getAPIConfig()
    const baseURL = getBaseURL(apiConfig)
    const response = await fetch(`${baseURL}/data/set/device/name?value=${name}`)
    return response
  } catch (e) {
    console.error('Error setting device name:', e)
    throw e
  }
}

export async function setDataDeviceDelay(delay: number) {
  try {
    const apiConfig = getAPIConfig()
    const baseURL = getBaseURL(apiConfig)
    const response = await fetch(`${baseURL}/data/set/delay?value=${delay}`)
    return response
  } catch (e) {
    console.error('Error setting device delay:', e)
    throw e
  }
}

export async function getDataDeviceChannels() {
  try {
    const apiConfig = getAPIConfig()
    const baseURL = getBaseURL(apiConfig)
    const response = await fetch(`${baseURL}/data/get/device/ch`)
    return response
  } catch (e) {
    console.error('Error getting device channels:', e)
    throw e
  }
}

export async function getDataDeviceName() {
  try {
    const apiConfig = getAPIConfig()
    const baseURL = getBaseURL(apiConfig)
    const response = await fetch(`${baseURL}/data/get/device/name`)
    return response
  } catch (e) {
    console.error('Error getting device name:', e)
    throw e
  }
}

export async function getDataDeviceSerial() {
  try {
    const apiConfig = getAPIConfig()
    const baseURL = getBaseURL(apiConfig)
    const response = await fetch(`${baseURL}/data/get/device/serial`)
    return response
  } catch (e) {
    console.error('Error getting device serial:', e)
    throw e
  }
}

export async function getDataDeviceMode() {
  try {
    const apiConfig = getAPIConfig()
    const baseURL = getBaseURL(apiConfig)
    const response = await fetch(`${baseURL}/data/get/mode`)
    return response
  } catch (e) {
    console.error('Error getting device mode:', e)
    throw e
  }
}

export async function getDataDeviceDelay() {
  try {
    const apiConfig = getAPIConfig()
    const baseURL = getBaseURL(apiConfig)
    const response = await fetch(`${baseURL}/data/get/delay`)
    return response
  } catch (e) {
    console.error('Error getting device delay:', e)
    throw e
  }
}