const fs = require('fs');
const path = require('path');

console.log('🔍 Testing Final Expo Go Fix...\n');

const APP_JSON = path.join(__dirname, '../app.json');

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

console.log('🚫 EAS/Updates Removal Tests:');
try {
  const appJson = JSON.parse(fs.readFileSync(APP_JSON, 'utf8'));
  const expo = appJson.expo;
  
  test('EAS project ID removed', !expo.extra?.eas?.projectId);
  test('Updates configuration removed', !expo.updates);
  test('Owner field removed', !expo.owner);
  test('Extra.router preserved', expo.extra?.router !== undefined);
  
  // Essential fields still exist
  test('App name exists', expo.name !== undefined);
  test('Slug exists', expo.slug !== undefined);
  test('Version exists', expo.version !== undefined);
  test('Asset bundle patterns optimized', 
    expo.assetBundlePatterns && expo.assetBundlePatterns.length <= 4);
  
} catch (error) {
  test('app.json readable', false, error.message);
}

console.log('\n📱 App Configuration Verification:');
try {
  const appJson = JSON.parse(fs.readFileSync(APP_JSON, 'utf8'));
  const content = JSON.stringify(appJson, null, 2);
  
  test('No remote update references', !content.includes('updates'));
  test('No EAS references', !content.includes('"eas"'));
  test('No owner references', !content.includes('owner'));
  test('Clean JSON structure', content.includes('"expo"'));
  
} catch (error) {
  test('JSON structure check failed', false, error.message);
}

console.log('\n📊 Test Results:');
console.log(`✅ Passed: ${passed}`);
console.log(`❌ Failed: ${failed}`);
console.log(`📈 Success Rate: ${Math.round((passed / (passed + failed)) * 100)}%`);

if (failed === 0) {
  console.log('\n🎉 Final fix complete! No more remote update errors.');
  console.log('\n📱 Ready to Scan:');
  console.log('   1. npm run start:tunnel');
  console.log('   2. Scan QR code with Expo Go');
  console.log('   3. App loads directly - no remote downloads!');
  console.log('\n✨ Pure local WebView app - no network dependencies!');
  process.exit(0);
} else {
  console.log('\n⚠️  Some tests failed. Check the issues above.');
  process.exit(1);
}