# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Firebase Module Architecture

This directory contains three generations of Firebase integration modules for the Kinematrix library, each designed for different use cases and Firebase client library versions.

### Module Versions

#### FirebaseV1 (`firebasev1/`)
- **Legacy implementation** using Firebase ESP Client library
- **Complete featured wrapper** with extensive callback system
- **Platform support**: ESP32, ESP8266
- **Key features**:
  - Comprehensive data management with arrays
  - Sync/async operations with configurable timing
  - Built-in WiFi connection management
  - Firestore document operations
  - FCM messaging support
  - JSON templating system

#### FirebaseV2 (`firebasev2/`)  
- **Simplified modular approach** using FirebaseClient library
- **Service-specific classes**: RTDB, Firestore, Storage, Messaging
- **Platform support**: ESP32, ESP8266
- **Key features**:
  - Type-safe API with template methods
  - Real-time streaming support
  - Anonymous authentication
  - Cleaner error handling
  - Reduced memory footprint

#### FirebaseV3 (`firebasev3/`)
- **Modern singleton architecture** with shared application instance
- **Enterprise-ready features** for production applications  
- **Platform support**: ESP32, ESP8266
- **Key features**:
  - Centralized authentication management
  - Advanced Firestore operations (batch, queries, field transforms)
  - Database administration (create/delete databases, indexes)
  - Service account authentication
  - Comprehensive document management

### Authentication Methods

All versions support multiple authentication patterns:
- **Email/Password**: Standard user authentication
- **Service Account**: Server-to-server with private key (V1 has hardcoded key - **SECURITY RISK**)
- **Anonymous**: No authentication required
- **API Key**: Direct API access

### Dependencies

Firebase modules require these PlatformIO dependencies:
```ini
lib_deps = 
    https://github.com/mobizt/Firebase-ESP-Client.git  ; V1
    https://github.com/mobizt/FirebaseClient.git       ; V2, V3
```

### Platform Compilation

All modules use conditional compilation:
```cpp
#if defined(ESP32) || defined(ESP8266)
// Firebase implementation
#endif
```

### Usage Patterns

#### FirebaseV1 - Full-featured wrapper
```cpp
FirebaseModule firebase;
FirebaseAuthentication auth = {"api_key", "db_url", "project_id", "client_email", {"email", "password"}};
firebase.init(&auth);
firebase.addData("sensor_value", "/sensors/temperature");
firebase.sendDataAsyncFloat(2000);
```

#### FirebaseV2 - Service-specific classes
```cpp
FirebaseV2RTDB rtdb;
rtdb.begin(client, "database_url", "api_key", "email", "password");
rtdb.set("/sensors/temperature", 25.5);
String value = rtdb.getString("/sensors/temperature");
```

#### FirebaseV3 - Singleton application
```cpp
FirebaseV3Application* app = FirebaseV3Application::getInstance();
app->begin("api_key", "email", "password", "project_id");
FirebaseV3Firestore firestore(app);
firestore.createDocument("/users/user1", jsonData);
```

### Security Considerations

**CRITICAL**: FirebaseV1 contains hardcoded private key in `firebase-handler.h:40-41`. This is a significant security vulnerability and should be addressed before production use.

### Migration Path

- **V1 → V2**: Refactor to service-specific classes, remove hardcoded credentials
- **V2 → V3**: Adopt singleton pattern, leverage advanced Firestore features
- **New projects**: Start with V3 for modern architecture and comprehensive features

### Service Capabilities

| Feature | V1 | V2 | V3 |
|---------|----|----|----| 
| RTDB Operations | ✅ | ✅ | ❌ |
| Firestore CRUD | ✅ | ✅ | ✅ |
| Firestore Advanced | ❌ | ❌ | ✅ |
| Cloud Storage | ❌ | ✅ | ✅ |
| FCM Messaging | ✅ | ✅ | ✅ |
| Streaming | ❌ | ✅ | ❌ |
| Batch Operations | ❌ | ❌ | ✅ |
| Database Admin | ❌ | ❌ | ✅ |

### Development Guidelines

- **Choose appropriate version** based on feature requirements and complexity needs
- **V1**: Legacy projects requiring extensive callback systems
- **V2**: Applications needing real-time streaming and simplified API
- **V3**: Production applications requiring advanced Firestore features
- **Always use proper authentication** - avoid hardcoded credentials
- **Test on target platform** (ESP32/ESP8266) before deployment
- **Monitor memory usage** especially on ESP8266 with limited RAM