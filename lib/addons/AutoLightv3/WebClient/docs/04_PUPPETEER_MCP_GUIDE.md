# Complete Puppeteer MCP Guide for Claude Code

## Overview
Puppeteer MCP adalah server yang menyediakan kemampuan browser automation untuk Claude Code menggunakan Puppeteer. Server ini memungkinkan Claude untuk berinteraksi dengan halaman web, mengambil screenshot, dan menjalankan JavaScript di browser secara programmatic.

**Use Cases:**
- Website screenshot dan visual testing
- Web scraping dan data extraction
- UI automation dan testing
- Form filling dan interaction
- Performance monitoring
- Cross-browser compatibility testing

## System Requirements & Prerequisites

### Required Software:
- **Node.js**: Version 16+ (recommended: 18+)
- **NPM**: Latest version
- **Claude Code**: Latest version
- **Operating System**: Windows, macOS, Linux/WSL

### Check Prerequisites:
```bash
# Check Node.js version
node --version  # Should show v16+ 

# Check NPM version  
npm --version

# Check Claude Code
claude --version
```

### Install Prerequisites (if missing):
```bash
# Install Node.js (Ubuntu/Debian)
curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash -
sudo apt-get install -y nodejs

# Install Node.js (Windows)
# Download from: https://nodejs.org/

# Install Node.js (macOS)
brew install node
```

## Installation Process

### Method 1: Claude Code Command (Recommended)
```bash
# Install Puppeteer MCP server
claude mcp add puppeteer -s user -- npx -y @modelcontextprotocol/server-puppeteer

# Verify installation
claude mcp list
```

### Method 2: JSON Configuration
```bash
claude mcp add-json "puppeteer" '{"command":"npx","args":["-y","@modelcontextprotocol/server-puppeteer"]}'
```

### Method 3: Manual Configuration
Edit Claude configuration file:

**Linux/WSL**: `~/.claude.json`
**Windows**: `%APPDATA%\Claude\claude_desktop_config.json`  
**macOS**: `~/Library/Application Support/Claude/claude_desktop_config.json`

Add this configuration:
```json
{
  "mcpServers": {
    "puppeteer": {
      "command": "npx",
      "args": ["-y", "@modelcontextprotocol/server-puppeteer"],
      "env": {
        "NODE_ENV": "production"
      }
    }
  }
}
```

## Post-Installation Setup

### 1. Install Chrome Browser
```bash
# Install Chrome for Puppeteer
npx puppeteer browsers install chrome

# Install stable version
npx @puppeteer/browsers install chrome@stable

# Install specific version (if needed)
npx @puppeteer/browsers install chrome@131.0.6778.204
```

### 2. Restart Claude Code
**IMPORTANT**: Setelah installation, restart Claude Code completely untuk memuat MCP server.

### 3. Verify Installation
```bash
# Check available Puppeteer browsers
npx puppeteer browsers list

# Check Chrome installation path
find ~/.cache/puppeteer/ -name "chrome" -type f 2>/dev/null

# Verify Claude MCP servers
claude mcp list | grep puppeteer
```

## Available Tools & Functions

Setelah installation berhasil, Claude Code memiliki akses ke 11 tools Puppeteer:

### Core Navigation:
- `mcp__puppeteer__puppeteer_navigate` - Navigate ke URL
- `mcp__puppeteer__puppeteer_screenshot` - Ambil screenshot halaman
- `mcp__puppeteer__puppeteer_evaluate` - Execute JavaScript di browser

### Interaction Tools:
- `mcp__puppeteer__puppeteer_click` - Click elemen di halaman
- `mcp__puppeteer__puppeteer_fill` - Fill input fields
- `mcp__puppeteer__puppeteer_select` - Select dropdown options
- `mcp__puppeteer__puppeteer_hover` - Hover elemen

### Advanced Features:
- Multi-tab management
- Cookie handling
- Network interception
- PDF generation
- Mobile device emulation

## Basic Usage Examples

### 1. Simple Screenshot
```
"Take a screenshot of https://example.com"
```

### 2. Navigate and Screenshot
```
"Navigate to https://google.com and take a screenshot"
```

### 3. Form Interaction
```
"Go to https://example.com/login, fill email with 'test@test.com', fill password with 'password123', click submit button, and take a screenshot"
```

### 4. Custom Size Screenshot
```
"Take a screenshot of https://example.com with width 1920 and height 1080"
```

### 5. Element-specific Screenshot  
```
"Take a screenshot of the header section on https://example.com"
```

## Advanced Usage Patterns

### With Launch Options:
```javascript
mcp__puppeteer__puppeteer_navigate({
  "url": "https://example.com",
  "launchOptions": {
    "headless": true,
    "viewport": {"width": 1280, "height": 720},
    "executablePath": "/path/to/chrome"
  }
})
```

### JavaScript Execution:
```javascript
mcp__puppeteer__puppeteer_evaluate({
  "script": "document.querySelector('h1').textContent"
})
```

### Form Filling Pattern:
```javascript
// 1. Navigate
mcp__puppeteer__puppeteer_navigate({"url": "https://example.com/login"})

// 2. Fill form
mcp__puppeteer__puppeteer_fill({"selector": "#email", "value": "user@example.com"})
mcp__puppeteer__puppeteer_fill({"selector": "#password", "value": "password123"})

// 3. Submit
mcp__puppeteer__puppeteer_click({"selector": "button[type='submit']"})

// 4. Screenshot result
mcp__puppeteer__puppeteer_screenshot({"name": "login_result"})
```

## Installation Status Untuk Sistem Ini
✅ **SUDAH TERINSTALL** - Puppeteer MCP sudah berhasil diinstall pada sistem ini.

### Installation Command yang Digunakan:
```bash
claude mcp add puppeteer -s user -- npx -y @modelcontextprotocol/server-puppeteer
```

### Verifikasi Installation:
- Node.js version: v24.2.0
- NPM version: 11.3.0
- MCP config location: `/home/iwandwp/.claude.json`
- Status: ✅ Successfully added to user config

## Prerequisites
- Node.js 16+ (✅ Current: v24.2.0)
- NPM (✅ Current: 11.3.0)
- Chrome/Chromium browser (automatically downloaded by Puppeteer)

## Available Tools
Setelah installation, Claude Code memiliki akses ke 11 tools Puppeteer:

### Core Navigation Tools:
- `mcp__puppeteer__puppeteer_navigate` - Navigate ke URL
- `mcp__puppeteer__puppeteer_screenshot` - Ambil screenshot halaman
- `mcp__puppeteer__puppeteer_click` - Click elemen di halaman
- `mcp__puppeteer__puppeteer_fill` - Fill input fields
- `mcp__puppeteer__puppeteer_select` - Select dropdown options
- `mcp__puppeteer__puppeteer_hover` - Hover elemen
- `mcp__puppeteer__puppeteer_evaluate` - Execute JavaScript

## Usage Examples

### 1. Mengambil Screenshot Website
```
"Take a screenshot of http://localhost"
```

### 2. Navigate dan Screenshot
```
"Navigate to http://localhost and take a screenshot"
```

### 3. Screenshot dengan Custom Size
```
"Take a screenshot of http://localhost with width 1920 and height 1080"
```

### 4. Navigate dan Interact
```
"Go to http://localhost, click on the login button, and take a screenshot"
```

## Local Development Server Integration
Untuk testing dengan local development server:

### Server Configuration:
- **Default URL**: http://localhost (atau port custom seperti http://localhost:3000, :8080, dsb)
- **Supported Technologies**: Semua web framework (React, Vue, Angular, Flask, Django, Express.js, dll)

### Testing Commands:
```bash
# 1. Start development server (contoh berbagai framework)
npm start          # React/Vue/Angular
python app.py      # Flask
python manage.py runserver  # Django
node server.js     # Express.js
php -S localhost:8080      # PHP

# 2. Then use Puppeteer to screenshot
# "Take a screenshot of http://localhost:PORT"
```

## Comprehensive Troubleshooting Guide

### Pre-Installation Issues:

#### 1. **Node.js Not Found**
```bash
# Error: node: command not found
# Solution: Install Node.js
curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash -
sudo apt-get install -y nodejs
```

#### 2. **NPM Permission Issues** 
```bash
# Error: permission denied
# Solution: Fix NPM permissions
npm config set prefix ~/.npm-global
export PATH=~/.npm-global/bin:$PATH
```

#### 3. **Claude Command Not Found**
```bash
# Error: claude: command not found  
# Solution: Verify Claude Code installation
# Download from: https://claude.ai/code
```

### Installation Issues:

#### 4. **MCP Installation Failed**
```bash
# Error: Failed to add MCP server
# Solution: Try alternative methods

# Method 1: Direct installation
npm install -g @modelcontextprotocol/server-puppeteer

# Method 2: Manual config editing
# Edit ~/.claude.json manually
```

### Post-Installation Issues:

#### 5. **Tools Not Available After Installation**
```bash
# Issue: MCP tools tidak muncul di Claude Code
# Solutions:
1. Restart Claude Code completely (close & reopen)
2. Check MCP configuration: claude mcp list
3. Verify tools: Check if mcp__puppeteer__ functions available
4. Re-install if needed: claude mcp remove puppeteer && claude mcp add puppeteer...
```

#### 6. **Navigation Timeout Errors**
```bash
# Error: Navigation timeout exceeded
# Solutions:
1. Increase timeout: Add "timeout": 30000 to launchOptions
2. Check target server: Ensure URL is accessible
3. Use wait conditions: Wait for specific elements to load
4. Check network: Verify internet connectivity
```

#### 7. **Screenshot Blank/Empty**
```bash
# Issue: Screenshot captures empty/white page
# Solutions:
1. Add wait time: Wait for page load completion
2. Wait for elements: Use specific element selectors
3. Check JavaScript: Ensure JS/CSS loaded properly
4. Use full page: Add "fullPage": true parameter
5. Set proper viewport: Configure width/height
```

#### 8. **❌ Chrome Binary Not Found Error** - SOLVED ✅
   ```
   Error: Could not find Chrome (ver. 131.0.6778.204). This can occur if either
   1. you did not perform an installation before running the script
   2. your cache path is incorrectly configured
   ```
   
   **Root Cause**: MCP server mencari Chrome version 131.0.6778.204 tapi yang terinstall version berbeda (139.0.7258.68)
   
   **Steps to Fix**:
   ```bash
   # 1. Install Chrome untuk Puppeteer
   npx puppeteer browsers install chrome
   
   # 2. Install specific version jika diperlukan
   npx @puppeteer/browsers install chrome@stable
   
   # 3. Check lokasi Chrome binary
   find /home/iwandwp/.cache/puppeteer/ -name "chrome" -type f
   
   # 4. Verify installation
   ls -la /home/iwandwp/.cache/puppeteer/chrome/linux-139.0.7258.68/chrome-linux64/chrome
   ```
   
   **Working Solution**: Gunakan launchOptions dengan executablePath
   ```javascript
   // Navigate dengan custom Chrome path
   mcp__puppeteer__puppeteer_navigate({
     "url": "http://localhost",
     "launchOptions": {
       "headless": true,
       "executablePath": "/home/iwandwp/.cache/puppeteer/chrome/linux-139.0.7258.68/chrome-linux64/chrome"
     }
   })
   ```
   
   **Status**: ✅ **RESOLVED** - Navigation berhasil dengan custom executablePath

#### 9. **Chrome Installation Paths**
- **Cache location**: `~/.cache/puppeteer/` (Linux/macOS) atau `%LOCALAPPDATA%\puppeteer` (Windows)
- **Available versions pada sistem ini**: 
  - `linux-138.0.7204.168`
  - `linux-139.0.7258.68` ✅ (Working)
- **Binary path**: `~/.cache/puppeteer/chrome/linux-139.0.7258.68/chrome-linux64/chrome`

#### 10. **Permission Issues (Linux/macOS)**
```bash
# Error: Permission denied to execute Chrome
# Solutions:
chmod +x ~/.cache/puppeteer/chrome/*/chrome-linux64/chrome
sudo chown -R $USER:$USER ~/.cache/puppeteer/
```

#### 11. **Memory/Performance Issues**
```bash
# Error: Out of memory or slow performance
# Solutions - Add to launchOptions:
{
  "args": [
    "--no-sandbox",
    "--disable-dev-shm-usage",
    "--disable-background-timer-throttling",
    "--disable-backgrounding-occluded-windows",
    "--disable-renderer-backgrounding"
  ]
}
```

#### 12. **Network/Proxy Issues**
```bash
# Error: Cannot connect to websites
# Solutions:
1. Add proxy settings to launchOptions if behind corporate proxy
2. Disable proxy: --no-proxy-server
3. Set user agent: --user-agent="..."
4. Trust certificates: --ignore-certificate-errors
```

### Debug Commands:
```bash
# Check Chrome installations
npx puppeteer browsers list

# Test Chrome manually
~/.cache/puppeteer/chrome/linux-139.0.7258.68/chrome-linux64/chrome --version

# Check MCP status
claude mcp list
claude mcp status puppeteer

# Clean reinstall
claude mcp remove puppeteer
rm -rf ~/.cache/puppeteer
npx puppeteer browsers install chrome
claude mcp add puppeteer -s user -- npx -y @modelcontextprotocol/server-puppeteer
```

### Configuration Location:
- Linux/WSL: `/home/iwandwp/.claude.json`
- Windows: `%APPDATA%\Claude\claude_desktop_config.json`
- macOS: `~/Library/Application Support/Claude/claude_desktop_config.json`

## Quick Reference Commands

### When User Wants Screenshots:
- Read this file: `PUPPETEER_MCP_GUIDE.md`
- Use tools: `mcp__puppeteer__puppeteer_navigate` and `mcp__puppeteer__puppeteer_screenshot`  
- Target URL: User-specified URL (http://localhost, https://example.com, dll)

### Working Commands (TESTED ✅):
```javascript
// 1. Navigate dengan custom Chrome path
mcp__puppeteer__puppeteer_navigate({
  "url": "https://example.com",  // User-specified URL
  "launchOptions": {
    "headless": true,
    "executablePath": "/home/iwandwp/.cache/puppeteer/chrome/linux-139.0.7258.68/chrome-linux64/chrome"
  }
})

// 2. Screenshot setelah navigate
mcp__puppeteer__puppeteer_screenshot({
  "name": "website_screenshot",
  "width": 1200,
  "height": 800
})
```

### Alternative Installation Methods:
```bash
# Method 1 (Used):
claude mcp add puppeteer -s user -- npx -y @modelcontextprotocol/server-puppeteer

# Method 2:
claude mcp add-json "puppeteer" '{"command":"npx","args":["-y","@modelcontextprotocol/server-puppeteer"]}'

# Method 3 (Manual):
# Add to claude_desktop_config.json:
{
  "mcpServers": {
    "puppeteer": {
      "command": "npx",
      "args": ["-y", "@modelcontextprotocol/server-puppeteer"]
    }
  }
}
```

## Best Practices & Tips

### Performance Optimization:
```javascript
// Optimal launchOptions for most use cases
{
  "headless": true,
  "args": [
    "--no-sandbox",
    "--disable-dev-shm-usage",
    "--disable-extensions",
    "--disable-gpu",
    "--disable-background-timer-throttling"
  ],
  "viewport": {"width": 1280, "height": 720}
}
```

### Security Considerations:
- Always use `headless: true` for server environments
- Never expose Chrome with `--remote-debugging-port` in production
- Validate all user inputs before passing to Puppeteer
- Use `--no-sandbox` only when necessary

### Common Patterns:
```javascript
// Wait for page load
await page.waitForLoadState('networkidle');

// Wait for specific element
await page.waitForSelector('#content', {timeout: 10000});

// Handle popups
page.on('dialog', dialog => dialog.accept());

// Mobile emulation
"launchOptions": {
  "args": ["--window-size=375,667"],
  "viewport": {"width": 375, "height": 667, "isMobile": true}
}
```

### Resource Management:
- Always close browser instances when done
- Use `--disable-dev-shm-usage` for low-memory environments
- Limit concurrent browser instances
- Clean up temp files regularly

## Configuration Templates

### Standard Website Testing:
```json
{
  "url": "https://example.com",
  "launchOptions": {
    "headless": true,
    "viewport": {"width": 1920, "height": 1080},
    "timeout": 30000
  }
}
```

### Mobile Testing:
```json
{
  "url": "https://example.com",
  "launchOptions": {
    "headless": true,
    "viewport": {"width": 375, "height": 667, "isMobile": true},
    "args": ["--user-agent=Mozilla/5.0 (iPhone; CPU iPhone OS 14_0 like Mac OS X)"]
  }
}
```

### Performance Testing:
```json
{
  "url": "https://example.com",
  "launchOptions": {
    "headless": true,
    "args": [
      "--no-sandbox",
      "--disable-dev-shm-usage",
      "--disable-background-timer-throttling",
      "--disable-backgrounding-occluded-windows",
      "--disable-renderer-backgrounding",
      "--disable-extensions",
      "--disable-plugins",
      "--disable-default-apps"
    ]
  }
}
```

## Environment-Specific Notes

### Linux/WSL:
- Chrome cache: `~/.cache/puppeteer/`
- May need `--no-sandbox` flag
- Check permissions: `chmod +x chrome-binary`

### Windows:  
- Chrome cache: `%LOCALAPPDATA%\puppeteer\`
- PowerShell may require different syntax
- Use forward slashes in paths

### macOS:
- Chrome cache: `~/Library/Caches/puppeteer/`
- May need Rosetta for M1 Macs
- Check Gatekeeper permissions

### Docker/Containers:
```dockerfile
RUN apt-get update && apt-get install -y \
    chromium-browser \
    --no-install-recommends \
    && rm -rf /var/lib/apt/lists/*

ENV PUPPETEER_EXECUTABLE_PATH=/usr/bin/chromium-browser
```

## Support & Resources

### Official Documentation:
- Puppeteer: https://pptr.dev/
- MCP Protocol: https://spec.modelcontextprotocol.io/
- Claude Code: https://docs.anthropic.com/en/docs/claude-code

### Community Resources:
- GitHub Issues: https://github.com/puppeteer/puppeteer/issues
- Stack Overflow: [puppeteer] tag
- Discord: Puppeteer community

### Version Compatibility:
- Puppeteer: 21.0.0+
- Node.js: 16.14.0+
- Chrome: Auto-installed
- Claude Code: Latest version

---

## Notes for This System
- ✅ Installation completed successfully  
- ✅ All 11 Puppeteer tools available
- ✅ Chrome binary working: `linux-139.0.7258.68`
- ✅ Ready for website screenshots and automation
- 🎯 Compatible with all web technologies and frameworks
- 📝 Created: 2025-08-15 by Claude Code assistant
- 🔄 Updated: Complete guide for universal use

---

## Quick Start Guide
1. **Prerequisites**: Node.js 16+, Claude Code installed
2. **Install**: `claude mcp add puppeteer -s user -- npx -y @modelcontextprotocol/server-puppeteer`
3. **Setup Chrome**: `npx puppeteer browsers install chrome`
4. **Restart**: Claude Code completely
5. **Test**: `"Take a screenshot of https://example.com"`

**For any project**: Read this guide → Install Puppeteer MCP → Start screenshotting! 🚀