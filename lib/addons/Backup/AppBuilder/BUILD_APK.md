# BUILD_APK.md - Panduan Build APK dengan EAS

## Prerequisites

### 1. Install EAS CLI
```bash
npm install -g eas-cli
```

### 2. Login ke Expo Account
```bash
eas login
```
Jika belum punya akun, buat di https://expo.dev/

### 3. Konfigurasi Project
```bash
eas build:configure
```
Ini akan membuat file `eas.json` jika belum ada.

## Konfigurasi eas.json

File `eas.json` standar untuk project ini:
```json
{
  "cli": {
    "version": ">= 3.0.0"
  },
  "build": {
    "development": {
      "developmentClient": true,
      "distribution": "internal",
      "android": {
        "buildType": "apk"
      }
    },
    "preview": {
      "distribution": "internal",
      "android": {
        "buildType": "apk"
      }
    },
    "production": {
      "android": {
        "buildType": "apk"
      }
    }
  },
  "submit": {
    "production": {}
  }
}
```

## Build APK untuk Testing

### 1. Build APK Preview (Recommended untuk testing)
```bash
eas build --platform android --profile preview
```

### 2. Build APK Preview dengan Clear Cache
```bash
eas build --platform android --profile preview --clear-cache
```

### 3. Build APK Development 
```bash
eas build --platform android --profile development
```

### 4. Build APK Production
```bash
eas build --platform android --profile production
```

### 5. Build APK Production dengan Clear Cache
```bash
eas build --platform android --profile production --clear-cache
```

## Build APK Lokal (Tanpa EAS Cloud)

### Prerequisites Lokal Build
1. Install Android Studio dan Android SDK
2. Set environment variables:
   ```bash
   export ANDROID_HOME=$HOME/Android/Sdk
   export PATH=$PATH:$ANDROID_HOME/emulator
   export PATH=$PATH:$ANDROID_HOME/tools
   export PATH=$PATH:$ANDROID_HOME/tools/bin
   export PATH=$PATH:$ANDROID_HOME/platform-tools
   ```

### Build Lokal
```bash
eas build --platform android --profile preview --local
```

## Monitoring Build Progress

1. Setelah menjalankan perintah build, akan muncul link ke Expo dashboard
2. Buka link tersebut untuk monitor progress build
3. Build biasanya memakan waktu 10-30 menit tergantung antrian

## Download APK

1. Setelah build selesai, akan ada link download di dashboard
2. Atau gunakan command:
   ```bash
   eas build:list --platform android
   ```
3. Download APK dari link yang tersedia

## Troubleshooting

### Error: "Gradle build failed"
```bash
# Clear cache
npm run clear
# Atau
npx expo start -c
# Atau gunakan --clear-cache flag
eas build --platform android --profile preview --clear-cache
```

### Build Stuck atau Gagal
```bash
# Cancel ongoing build
eas build:cancel

# List semua builds
eas build:list --platform android

# Inspect specific build
eas build:inspect [build-id]
```

### Clear EAS Build Cache
```bash
# Clear cache untuk semua builds
eas build --platform android --profile preview --clear-cache

# Atau hapus cache manual
rm -rf node_modules
rm package-lock.json
npm install
```

### Error: "SDK version mismatch"
Pastikan versi SDK di `app.json` sesuai:
```json
{
  "expo": {
    "sdkVersion": "53.0.0"
  }
}
```

### Error: "Bundle identifier missing"
Tambahkan di `app.json`:
```json
{
  "expo": {
    "android": {
      "package": "com.yourcompany.intanapp"
    }
  }
}
```

### Build Gagal Karena Memory
Untuk build lokal dengan memory terbatas:
```bash
eas build --platform android --profile preview --local --clear-cache
```

## Tips Optimasi

### 1. Kurangi Ukuran APK
Di `eas.json`, tambahkan:
```json
{
  "build": {
    "production": {
      "android": {
        "buildType": "apk",
        "enableProguardInReleaseBuilds": true,
        "enableShrinkResourcesInReleaseBuilds": true
      }
    }
  }
}
```

### 2. Split APK by ABI
```json
{
  "build": {
    "production": {
      "android": {
        "buildType": "apk",
        "enableDangerousExperimentalLeanBuilds": true
      }
    }
  }
}
```

### 3. Custom Gradle Properties
Buat file `android/gradle.properties`:
```
org.gradle.jvmargs=-Xmx4096m -XX:MaxPermSize=512m
org.gradle.parallel=true
android.useAndroidX=true
android.enableJetifier=true
```

## Build untuk Google Play Store

### 1. Generate Upload Key
```bash
keytool -genkeypair -v -keystore upload-keystore.jks -keyalg RSA -keysize 2048 -validity 10000 -alias upload
```

### 2. Konfigurasi untuk AAB (Android App Bundle)
```json
{
  "build": {
    "production": {
      "android": {
        "buildType": "app-bundle"
      }
    }
  }
}
```

### 3. Build AAB
```bash
eas build --platform android --profile production
```

### 4. Submit ke Play Store
```bash
eas submit --platform android
```

## Script Shortcuts

Tambahkan ke `package.json`:
```json
{
  "scripts": {
    "build:android:preview": "eas build --platform android --profile preview",
    "build:android:preview:clear": "eas build --platform android --profile preview --clear-cache",
    "build:android:prod": "eas build --platform android --profile production",
    "build:android:prod:clear": "eas build --platform android --profile production --clear-cache",
    "build:android:local": "eas build --platform android --profile preview --local",
    "build:android:local:clear": "eas build --platform android --profile preview --local --clear-cache",
    "build:list": "eas build:list --platform android",
    "build:cancel": "eas build:cancel",
    "build:inspect": "eas build:inspect"
  }
}
```

## Checklist Sebelum Build Production

- [ ] Update version number di `app.json`
- [ ] Test di development build
- [ ] Clear cache: `npm run clear`
- [ ] Update dependencies: `npm update`
- [ ] Check Firebase configuration
- [ ] Verify app permissions di `app.json`
- [ ] Test deep linking configuration
- [ ] Optimize images dan assets
- [ ] Remove console.log statements
- [ ] Update app icon dan splash screen

## Resources

- [EAS Build Documentation](https://docs.expo.dev/build/introduction/)
- [Android Build Configuration](https://docs.expo.dev/build-reference/android-builds/)
- [App Signing](https://docs.expo.dev/app-signing/introduction/)
- [Submitting to Stores](https://docs.expo.dev/submit/introduction/)