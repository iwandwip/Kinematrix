# AutoLight V3 - Enhanced Credential Management API

## Overview
The AutoLight V3 library now provides a flexible credential management system with support for different operational modes. This enhancement maintains 100% backward compatibility while adding powerful new capabilities for development, testing, and production deployment.

# WebManager initCredentials() - Usage Examples

## Overview
The enhanced `initCredentials()` method now supports flexible credential management with mode-based operations.

## Function Signature
```cpp
void initCredentials(CredentialMode mode = READ_WRITE, 
                    const String& ssid = "", 
                    const String& password = "");
```

## Available Modes

### 1. READ_ONLY Mode
Only reads existing credentials from EEPROM without writing.
```cpp
// Read existing credentials only
webManager.initCredentials(READ_ONLY);
```

**Use Cases:**
- Production deployment where credentials are pre-configured
- Avoiding unnecessary EEPROM writes
- Debugging existing stored credentials

### 2. WRITE_ONLY Mode  
Only writes credentials to EEPROM without reading.
```cpp
// Write default credentials
webManager.initCredentials(WRITE_ONLY);

// Write custom credentials
webManager.initCredentials(WRITE_ONLY, "MyWiFi-Network", "mypassword123");
```

**Use Cases:**
- Factory reset scenarios
- Initial device setup
- Credential provisioning during manufacturing

### 3. READ_WRITE Mode (Default)
Traditional behavior - write then read credentials.
```cpp
// Default behavior (backward compatible)
webManager.initCredentials();

// Same as above
webManager.initCredentials(READ_WRITE);

// Custom credentials with EEPROM persistence
webManager.initCredentials(READ_WRITE, "ProductionWiFi", "securepass");
```

**Use Cases:**
- Standard operation mode
- Development and testing
- Default deployment scenario

### 4. OVERRIDE Mode
Directly sets credentials in memory without EEPROM operations.
```cpp
// Override for testing
webManager.initCredentials(OVERRIDE, "TestNetwork", "testpass");

// Override SSID only
webManager.initCredentials(OVERRIDE, "DevNetwork", "");
```

**Use Cases:**
- Unit testing with mock credentials
- Development with temporary networks
- Fast credential switching without EEPROM wear
- Factory testing scenarios

## Advanced Usage Examples

### Development Workflow
```cpp
void setupDevelopment() {
    #ifdef DEVELOPMENT_MODE
        // Fast development with override
        webManager.initCredentials(OVERRIDE, "DevWiFi", "devpass");
    #else
        // Production with persistent storage
        webManager.initCredentials(READ_WRITE, "ProdWiFi", "prodpass");
    #endif
}
```

### Conditional Initialization
```cpp
void smartCredentialInit(bool isFirstBoot, bool hasStoredCredentials) {
    if (isFirstBoot) {
        // First boot - write default credentials
        webManager.initCredentials(WRITE_ONLY, "SETUP-" + deviceId, "");
    } else if (hasStoredCredentials) {
        // Subsequent boots - read existing
        webManager.initCredentials(READ_ONLY);
    } else {
        // Fallback - full read/write cycle
        webManager.initCredentials(READ_WRITE);
    }
}
```

### Testing Scenarios
```cpp
void runTests() {
    // Test 1: Override with test credentials
    webManager.initCredentials(OVERRIDE, "TestAP", "test123");
    assert(webManager.getCredentials().ssid == "TestAP");
    
    // Test 2: Write and verify persistence
    webManager.initCredentials(WRITE_ONLY, "PersistTest", "persist123");
    webManager.initCredentials(READ_ONLY);
    assert(webManager.getCredentials().ssid == "PersistTest");
    
    // Test 3: Default behavior
    webManager.initCredentials();
    // Should work with device_name_ based SSID
}
```

## Implementation Benefits

### 1. **Backward Compatibility**
- Existing code continues to work unchanged
- Default parameters maintain original behavior
- No breaking changes to current implementations

### 2. **Testing Friendly**
- OVERRIDE mode enables unit testing without EEPROM
- Fast credential switching for different test scenarios
- No side effects on persistent storage during testing

### 3. **Performance Optimization**
- READ_ONLY mode avoids unnecessary EEPROM writes
- OVERRIDE mode eliminates EEPROM operations entirely
- Reduced EEPROM wear in production scenarios

### 4. **Deployment Flexibility**
- Different modes for development vs production
- Factory provisioning with WRITE_ONLY
- Dynamic credential management based on runtime conditions

### 5. **Debug and Monitoring**
- Clear mode indication in serial output
- Detailed credential logging for troubleshooting
- Separate handling for each operation mode

## Migration Guide

### From Old API
```cpp
// Old way
webManager.initCredentials();

// New way (identical behavior)
webManager.initCredentials();  // Uses READ_WRITE default
```

### Enhanced Usage
```cpp
// Old: Fixed behavior
webManager.initCredentials();

// New: Flexible modes
webManager.initCredentials(READ_ONLY);              // Read existing
webManager.initCredentials(WRITE_ONLY, "WiFi", ""); // Write custom
webManager.initCredentials(OVERRIDE, "Test", "123"); // Memory only
```

## BaseChannel Integration

### New Method: setCredentialMode()
The `BaseChannel` class now provides a convenient method to configure credentials before enabling the web server:

```cpp
// Set credential mode before enabling web server
led.setCredentialMode(OVERRIDE, "DevNetwork", "devpass123");
led.enableWebServer("AUTOLIGHT-DEMO", true);
```

### Complete Usage Examples

#### Development Workflow
```cpp
void setupDevelopment() {
    BaseChannel led(true);  // Enable debug
    
    // Configure for development
    led.setCredentialMode(OVERRIDE, "DEV-WIFI", "dev123");
    led.enableWebServer("DEV-AUTOLIGHT", true);
    
    // Web server will use override credentials
    // No EEPROM operations - fast startup
}
```

#### Production Deployment
```cpp
void setupProduction() {
    BaseChannel led(false);  // Disable debug
    
    // Use existing stored credentials
    led.setCredentialMode(READ_ONLY);
    led.enableWebServer("PROD-AUTOLIGHT", true);
    
    // Optimized for performance - no EEPROM writes
}
```

#### Factory Setup
```cpp
void setupFactory() {
    BaseChannel led(true);
    
    // Provision with factory credentials
    led.setCredentialMode(WRITE_ONLY, "FACTORY-WIFI", "factory2024");
    led.enableWebServer("FACTORY-SETUP", true);
    
    // Credentials written to EEPROM for future use
}
```

#### Conditional Configuration
```cpp
void smartSetup() {
    BaseChannel led(true);
    
    #ifdef DEVELOPMENT_MODE
        // Development mode - fast switching
        led.setCredentialMode(OVERRIDE, "DEV-NET", "devpass");
    #elif defined(FACTORY_MODE)
        // Factory provisioning
        led.setCredentialMode(WRITE_ONLY, "PROD-WIFI", "prodpass");
    #else
        // Production - read existing
        led.setCredentialMode(READ_ONLY);
    #endif
    
    led.enableWebServer("SMART-DEVICE", true);
}
```

## API Reference

### BaseChannel Methods
```cpp
// Configure credential mode (call before enableWebServer)
void setCredentialMode(CredentialMode mode, 
                      const String& ssid = "", 
                      const String& password = "");

// Existing method - now enhanced with credential support
void enableWebServer(const char* device_name = "AutoLight", 
                    bool auto_task = true);
```

### WebManager Methods
```cpp
// Internal method - used by BaseChannel integration
void setCredentialConfig(CredentialMode mode, 
                        const String& ssid = "", 
                        const String& password = "");

// Enhanced initCredentials with mode support
void initCredentials(CredentialMode mode = READ_WRITE, 
                    const String& ssid = "", 
                    const String& password = "");
```

## Migration Guide

### Existing Code (No Changes Required)
```cpp
// This continues to work exactly as before
led.enableWebServer("MY-DEVICE", true);
```

### Enhanced Code (New Capabilities)
```cpp
// Add credential configuration for enhanced control
led.setCredentialMode(OVERRIDE, "TestNetwork", "test123");
led.enableWebServer("MY-DEVICE", true);
```

## Integration Benefits

### 1. **Seamless API Design**
- Single method call to configure credentials
- Automatic passing to WebManager
- Clean separation of concerns

### 2. **Development Acceleration**
- No EEPROM operations in OVERRIDE mode
- Fast credential switching
- Perfect for unit testing

### 3. **Production Optimization**
- READ_ONLY mode reduces EEPROM wear
- Faster startup times
- Better reliability

### 4. **Deployment Flexibility**
- Different modes for different environments
- Conditional compilation support
- Factory provisioning capabilities

This enhancement provides powerful credential management while maintaining complete backward compatibility and adding significant flexibility for different deployment and testing scenarios.