# Puppeteer MCP Usage Guide

## Installation
```bash
claude mcp add puppeteer -s user -- npx -y @modelcontextprotocol/server-puppeteer
npx puppeteer browsers install chrome
```
Restart Claude Code after installation.

## Available Tools
- `mcp__puppeteer__puppeteer_navigate` - Navigate to URL
- `mcp__puppeteer__puppeteer_screenshot` - Take screenshot
- `mcp__puppeteer__puppeteer_click` - Click element
- `mcp__puppeteer__puppeteer_fill` - Fill input field
- `mcp__puppeteer__puppeteer_select` - Select dropdown
- `mcp__puppeteer__puppeteer_hover` - Hover element
- `mcp__puppeteer__puppeteer_evaluate` - Execute JavaScript

## Basic Usage Examples

### Simple Screenshot
```
"Take a screenshot of https://example.com"
```

### Navigate and Screenshot
```
"Navigate to https://google.com and take a screenshot"
```

### Custom Size Screenshot
```
"Take a screenshot of https://example.com with width 1920 and height 1080"
```

### Form Interaction
```
"Go to https://example.com/login, fill email with 'test@test.com', fill password with 'password123', click submit button"
```

## For This System
✅ Already installed and working
- Chrome binary: `/home/iwandwp/.cache/puppeteer/chrome/linux-139.0.7258.68/chrome-linux64/chrome`
- Use custom executablePath in launchOptions if needed

## Working Pattern
```javascript
// 1. Navigate with custom Chrome path
mcp__puppeteer__puppeteer_navigate({
  "url": "http://localhost:3001",
  "launchOptions": {
    "headless": true,
    "executablePath": "/home/iwandwp/.cache/puppeteer/chrome/linux-139.0.7258.68/chrome-linux64/chrome"
  }
})

// 2. Take screenshot
mcp__puppeteer__puppeteer_screenshot({
  "name": "website_screenshot",
  "width": 1200,
  "height": 800
})
```

## Local Development
- Default URL: `http://localhost:3001` (Next.js dev server)
- Start server: `npm run dev` in WebClient folder
- Test: Screenshot local development server