const fs = require('fs');
const path = require('path');

const ASSETS_DIR = path.join(__dirname, '../assets/web');
const PACKAGE_JSON = path.join(__dirname, '../package.json');
const COMPONENTS_DIR = path.join(__dirname, '../components');
const SERVICES_DIR = path.join(__dirname, '../services');

console.log('🔍 Testing AutoLight V3 Mobile Integration...\n');

let passed = 0;
let failed = 0;

function test(name, condition, details = '') {
  if (condition) {
    console.log(`✅ ${name}`);
    passed++;
  } else {
    console.log(`❌ ${name}${details ? ` - ${details}` : ''}`);
    failed++;
  }
}

console.log('📁 File Structure Tests:');
test('WebClient assets exist', fs.existsSync(ASSETS_DIR));
test('index.html exists', fs.existsSync(path.join(ASSETS_DIR, 'index.html')));
test('_next assets exist', fs.existsSync(path.join(ASSETS_DIR, '_next')));
test('Package.json exists', fs.existsSync(PACKAGE_JSON));

console.log('\n🧩 Component Tests:');
test('WebViewContainer exists', fs.existsSync(path.join(COMPONENTS_DIR, 'WebViewContainer.jsx')));
test('NativeBridge exists', fs.existsSync(path.join(COMPONENTS_DIR, 'NativeBridge.jsx')));
test('LoadingSpinner exists', fs.existsSync(path.join(COMPONENTS_DIR, 'ui', 'LoadingSpinner.jsx')));
test('DeviceSettings exists', fs.existsSync(path.join(COMPONENTS_DIR, 'DeviceSettings.jsx')));

console.log('\n⚙️ Service Tests:');
test('WebViewBridge exists', fs.existsSync(path.join(SERVICES_DIR, 'WebViewBridge.js')));
test('DeviceAPI exists', fs.existsSync(path.join(SERVICES_DIR, 'DeviceAPI.js')));

console.log('\n📦 Dependency Tests:');
try {
  const packageJson = JSON.parse(fs.readFileSync(PACKAGE_JSON, 'utf8'));
  test('react-native-webview installed', packageJson.dependencies['react-native-webview'] !== undefined);
  test('AsyncStorage installed', packageJson.dependencies['@react-native-async-storage/async-storage'] !== undefined);
  test('expo-router configured', packageJson.dependencies['expo-router'] !== undefined);
  test('Build script exists', packageJson.scripts['build:webclient'] !== undefined);
} catch (error) {
  test('Package.json readable', false, error.message);
}

console.log('\n🔗 Integration Tests:');
try {
  const webViewContent = fs.readFileSync(path.join(COMPONENTS_DIR, 'WebViewContainer.jsx'), 'utf8');
  test('WebView imports bridge', webViewContent.includes('NativeBridge'));
  test('WebView handles messages', webViewContent.includes('handleMessage'));
  test('WebView injects JavaScript', webViewContent.includes('injectedJavaScript'));
  
  const bridgeContent = fs.readFileSync(path.join(COMPONENTS_DIR, 'NativeBridge.jsx'), 'utf8');
  test('Bridge has device methods', bridgeContent.includes('setDeviceMode'));
  test('Bridge has API calls', bridgeContent.includes('callNativeAPI'));
} catch (error) {
  test('Component files readable', false, error.message);
}

console.log('\n📊 Test Results:');
console.log(`✅ Passed: ${passed}`);
console.log(`❌ Failed: ${failed}`);
console.log(`📈 Success Rate: ${Math.round((passed / (passed + failed)) * 100)}%`);

if (failed === 0) {
  console.log('\n🎉 All tests passed! AutoLight V3 Mobile integration is ready.');
  console.log('\n📱 Next Steps:');
  console.log('   1. npm start - Start Expo development server');
  console.log('   2. Test on device/simulator');  
  console.log('   3. eas build --platform android --profile preview - Build APK');
  process.exit(0);
} else {
  console.log('\n⚠️  Some tests failed. Please check the issues above.');
  process.exit(1);
}