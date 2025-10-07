const fs = require('fs');
const path = require('path');
const { execSync } = require('child_process');

const WEBCLIENT_DIR = path.join(__dirname, '../../WebClient');
const ASSETS_DIR = path.join(__dirname, '../assets');
const WEB_ASSETS_DIR = path.join(ASSETS_DIR, 'web');

console.log('🔨 Building WebClient...');

try {
  process.chdir(WEBCLIENT_DIR);
  execSync('npm run build:mobile', { stdio: 'inherit' });
  console.log('✅ WebClient build completed');

  if (fs.existsSync(WEB_ASSETS_DIR)) {
    fs.rmSync(WEB_ASSETS_DIR, { recursive: true, force: true });
    console.log('🗑️  Cleared old web assets');
  }

  const outDir = path.join(WEBCLIENT_DIR, 'out');
  if (fs.existsSync(outDir)) {
    fs.cpSync(outDir, WEB_ASSETS_DIR, { recursive: true });
    console.log('📁 Copied new web assets');
  } else {
    throw new Error('WebClient build output not found');
  }

  console.log('🎉 WebClient integration completed successfully!');
  console.log(`📂 Assets location: ${WEB_ASSETS_DIR}`);
  
} catch (error) {
  console.error('❌ Build failed:', error.message);
  process.exit(1);
}