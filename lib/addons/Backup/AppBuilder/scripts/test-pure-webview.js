const fs = require('fs');
const path = require('path');

console.log('🔍 Testing Pure WebView Integration...\n');

const ASSETS_DIR = path.join(__dirname, '../assets/web');
const PACKAGE_JSON = path.join(__dirname, '../package.json');
const COMPONENTS_DIR = path.join(__dirname, '../components');

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

console.log('📁 WebClient Assets Tests:');
test('WebClient assets exist', fs.existsSync(ASSETS_DIR));
test('index.html exists', fs.existsSync(path.join(ASSETS_DIR, 'index.html')));
test('_next assets exist', fs.existsSync(path.join(ASSETS_DIR, '_next')));
test('CSS files exist', fs.existsSync(path.join(ASSETS_DIR, '_next', 'static', 'css')));
test('JS chunks exist', fs.existsSync(path.join(ASSETS_DIR, '_next', 'static', 'chunks')));

console.log('\n🧩 Component Tests (Pure WebView):');
test('WebViewContainer exists', fs.existsSync(path.join(COMPONENTS_DIR, 'WebViewContainer.jsx')));
test('LoadingSpinner exists', fs.existsSync(path.join(COMPONENTS_DIR, 'ui', 'LoadingSpinner.jsx')));
test('No bridge components (removed)', !fs.existsSync(path.join(COMPONENTS_DIR, 'NativeBridge.jsx')));
test('No device settings (removed)', !fs.existsSync(path.join(COMPONENTS_DIR, 'DeviceSettings.jsx')));

console.log('\n📦 Dependency Tests:');
try {
  const packageJson = JSON.parse(fs.readFileSync(PACKAGE_JSON, 'utf8'));
  test('react-native-webview installed', packageJson.dependencies['react-native-webview'] !== undefined);
  test('expo-router configured', packageJson.dependencies['expo-router'] !== undefined);
  test('AsyncStorage removed (not needed)', packageJson.dependencies['@react-native-async-storage/async-storage'] === undefined);
  test('Build script exists', packageJson.scripts['build:webclient'] !== undefined);
} catch (error) {
  test('Package.json readable', false, error.message);
}

console.log('\n🌐 Pure WebView Tests:');
try {
  const webViewContent = fs.readFileSync(path.join(COMPONENTS_DIR, 'WebViewContainer.jsx'), 'utf8');
  test('No bridge imports', !webViewContent.includes('NativeBridge'));
  test('No device API imports', !webViewContent.includes('DeviceAPI'));
  test('No message handling', !webViewContent.includes('handleMessage'));
  test('Simple WebView source', webViewContent.includes('file:///android_asset/web/index.html'));
  test('Essential props only', webViewContent.includes('javaScriptEnabled={true}'));
  
} catch (error) {
  test('WebViewContainer readable', false, error.message);
}

console.log('\n🎯 File Structure Verification:');
test('Services directory removed', !fs.existsSync(path.join(__dirname, '../services')));
test('Assets directory exists', fs.existsSync(path.join(__dirname, '../assets')));

console.log('\n📊 Test Results:');
console.log(`✅ Passed: ${passed}`);
console.log(`❌ Failed: ${failed}`);
console.log(`📈 Success Rate: ${Math.round((passed / (passed + failed)) * 100)}%`);

if (failed === 0) {
  console.log('\n🎉 Pure WebView setup complete!');
  console.log('\n📱 Ready to Test:');
  console.log('   1. npm run start:tunnel - Start Expo server');
  console.log('   2. Scan QR code with Expo Go');
  console.log('   3. WebView will load WebClient HTML directly');
  console.log('\n✨ No mobile logic - pure HTML viewer only!');
  process.exit(0);
} else {
  console.log('\n⚠️  Some tests failed. Check the issues above.');
  process.exit(1);
}