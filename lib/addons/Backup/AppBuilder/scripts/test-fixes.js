const fs = require('fs');
const path = require('path');

console.log('🔍 Testing Expo Go QR Code Fixes...\n');

const APP_JSON = path.join(__dirname, '../app.json');
const PACKAGE_JSON = path.join(__dirname, '../package.json');

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

console.log('📦 Bundle Size Optimization:');
try {
  const appJson = JSON.parse(fs.readFileSync(APP_JSON, 'utf8'));
  const bundlePatterns = appJson.expo.assetBundlePatterns;
  
  test('Asset bundle excludes web assets', 
    !bundlePatterns.includes('**/*') && bundlePatterns.length <= 4,
    `Patterns: ${bundlePatterns.length} items`);
  
  test('Only essential assets bundled', 
    bundlePatterns.includes('assets/icon.png') && 
    bundlePatterns.includes('assets/adaptive-icon.png'));

  test('EAS project configuration exists', appJson.expo.extra?.eas?.projectId !== undefined);
  test('Updates configuration added', appJson.expo.updates?.enabled === true);
  
} catch (error) {
  test('app.json readable', false, error.message);
}

console.log('\n📱 Dependency Updates:');
try {
  const packageJson = JSON.parse(fs.readFileSync(PACKAGE_JSON, 'utf8'));
  
  test('Expo SDK updated', packageJson.dependencies.expo === '53.0.22');
  test('React Native updated', packageJson.dependencies['react-native'] === '0.79.5');
  test('WebView version fixed', packageJson.dependencies['react-native-webview'] === '13.13.5');
  test('Tunnel script added', packageJson.scripts['start:tunnel'] !== undefined);
  
} catch (error) {
  test('package.json readable', false, error.message);
}

console.log('\n🌐 WebView Configuration:');
try {
  const webViewContent = fs.readFileSync(
    path.join(__dirname, '../components/WebViewContainer.jsx'), 'utf8'
  );
  
  test('Dynamic source loading', webViewContent.includes('setupWebViewSource'));
  test('Development mode detection', webViewContent.includes('__DEV__'));
  test('Constants import added', webViewContent.includes('import Constants'));
  test('Fallback mechanism', webViewContent.includes('fallback'));
  
} catch (error) {
  test('WebViewContainer readable', false, error.message);
}

console.log('\n🎯 Asset Files:');
const ASSETS_DIR = path.join(__dirname, '../assets');
test('Icon assets exist', fs.existsSync(path.join(ASSETS_DIR, 'icon.png')));
test('Adaptive icon exists', fs.existsSync(path.join(ASSETS_DIR, 'adaptive-icon.png')));
test('Web assets preserved', fs.existsSync(path.join(ASSETS_DIR, 'web', 'index.html')));

console.log('\n📊 Test Results:');
console.log(`✅ Passed: ${passed}`);
console.log(`❌ Failed: ${failed}`);
console.log(`📈 Success Rate: ${Math.round((passed / (passed + failed)) * 100)}%`);

if (failed === 0) {
  console.log('\n🎉 All fixes applied successfully!');
  console.log('\n📱 Next Steps:');
  console.log('   1. npm run start:tunnel - Use tunnel mode');
  console.log('   2. Scan QR code with Expo Go');
  console.log('   3. If still fails, try npm run start:local');
  console.log('\n🔧 Alternative Solutions:');
  console.log('   • Use different WiFi network');
  console.log('   • Update Expo Go app to latest version');
  console.log('   • Try USB connection with adb');
  process.exit(0);
} else {
  console.log('\n⚠️  Some tests failed. Check the issues above.');
  process.exit(1);
}