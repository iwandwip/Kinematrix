# Deployment Guide - AutoLight V3 WebClient

## Pre-Deployment Checklist

### ✅ Verification Status
- **Migration**: 100% complete from Vue 3 to Next.js 15
- **Build Status**: ✅ Successful (5.9s build time)
- **Client-Side Ready**: ✅ All components use "use client"
- **TypeScript**: ✅ Strict mode compliance
- **Components**: 11 main + 30+ UI components migrated
- **Features**: 100% feature parity with original Vue application

### ✅ Technical Requirements Met
- **Framework**: Next.js 15 + React 19
- **Build Tool**: Turbopack optimization
- **Bundle Size**: 165 kB (optimized)
- **Browser Support**: Modern browsers with ES2020+
- **Hardware Integration**: AutoLight V3 API compatible

## Production Deployment Options

### Option 1: Vercel Deployment (Recommended)
**Best for**: Quick deployment with zero configuration

```bash
# Install Vercel CLI
npm i -g vercel

# Deploy to Vercel
vercel

# Follow prompts:
# - Link to Git repository
# - Configure project settings
# - Deploy automatically on Git push
```

**Vercel Configuration:**
- **Framework**: Next.js (auto-detected)
- **Build Command**: `npm run build`
- **Output Directory**: `.next` (auto-configured)
- **Install Command**: `npm install`
- **Environment**: Node.js 18+ required

### Option 2: Docker Deployment
**Best for**: Self-hosted production environments

```dockerfile
# Dockerfile
FROM node:18-alpine AS base

# Install dependencies only when needed
FROM base AS deps
RUN apk add --no-cache libc6-compat
WORKDIR /app

COPY package.json package-lock.json ./
RUN npm ci

# Build the app
FROM base AS builder
WORKDIR /app
COPY --from=deps /app/node_modules ./node_modules
COPY . .

RUN npm run build

# Production image
FROM base AS runner
WORKDIR /app

ENV NODE_ENV=production
RUN addgroup --system --gid 1001 nodejs
RUN adduser --system --uid 1001 nextjs

COPY --from=builder /app/public ./public
COPY --from=builder --chown=nextjs:nodejs /app/.next/standalone ./
COPY --from=builder --chown=nextjs:nodejs /app/.next/static ./.next/static

USER nextjs
EXPOSE 3001
ENV PORT=3001

CMD ["node", "server.js"]
```

```bash
# Build Docker image
docker build -t autolight-webclient .

# Run container
docker run -p 3001:3001 autolight-webclient
```

### Option 3: Traditional Server Deployment
**Best for**: VPS, dedicated servers, or local deployment

```bash
# Production build
npm run build

# Start production server
npm start

# Or with PM2 for process management
npm install -g pm2
pm2 start npm --name "autolight-webclient" -- start
pm2 startup
pm2 save
```

## Environment Configuration

### Production Environment Variables
```bash
# .env.production
NODE_ENV=production
PORT=3001
HOSTNAME=0.0.0.0

# AutoLight Hardware API (configure based on your setup)
NEXT_PUBLIC_API_BASE_URL=http://your-autolight-device-ip
NEXT_PUBLIC_API_TIMEOUT=5000
```

### Development Environment Variables
```bash
# .env.local
NODE_ENV=development
PORT=3001

# Local AutoLight Hardware
NEXT_PUBLIC_API_BASE_URL=http://192.168.1.100
NEXT_PUBLIC_API_TIMEOUT=5000
```

## Hardware Integration Setup

### AutoLight V3 Device Configuration
1. **Network Setup**: Ensure AutoLight device is on same network
2. **API Endpoints**: Verify all endpoints are accessible:
   - `/get/device/ch` - Device channels
   - `/get/delay` - Current delay
   - `/get/device/name` - Device name
   - `/get/device/serial` - Serial number
   - `/get/mode` - Current mode
   - `/set/mode?value={mode}` - Set mode
   - `/set/delay?value={delay}` - Set delay
   - `/set/device/name?value={name}` - Set name

3. **Network Access**: Configure CORS if needed for cross-origin requests

### API Configuration Update
Update API base URL in production:

```typescript
// lib/api/device.ts - Update for production
const API_BASE_URL = process.env.NEXT_PUBLIC_API_BASE_URL || 'http://localhost'

export async function setDataDeviceMode(value: string) {
  try {
    const response = await fetch(`${API_BASE_URL}/set/mode?value=${value}`)
    return response
  } catch (e) {
    console.error('Error setting device mode:', e)
    throw e
  }
}
```

## Performance Optimization

### Build Optimization
```bash
# Production build with analysis
npm run build

# Bundle analysis (optional)
npm install -g @next/bundle-analyzer
ANALYZE=true npm run build
```

### Caching Strategy
```javascript
// next.config.ts
export default {
  // Enable static optimization
  output: 'standalone',
  
  // Enable compression
  compress: true,
  
  // Cache static assets
  headers: async () => [
    {
      source: '/_next/static/(.*)',
      headers: [
        {
          key: 'Cache-Control',
          value: 'public, max-age=31536000, immutable',
        },
      ],
    },
  ],
}
```

### Client-Side Performance
- **Code Splitting**: Automatic with Next.js App Router
- **Image Optimization**: Built-in Next.js Image component
- **Bundle Size**: Optimized to 165 kB main bundle
- **Loading States**: Spinner component for better UX
- **Error Boundaries**: Comprehensive error handling

## Security Considerations

### Production Security
```javascript
// next.config.ts - Security headers
export default {
  headers: async () => [
    {
      source: '/(.*)',
      headers: [
        {
          key: 'X-Frame-Options',
          value: 'DENY',
        },
        {
          key: 'X-Content-Type-Options',
          value: 'nosniff',
        },
        {
          key: 'Referrer-Policy',
          value: 'origin-when-cross-origin',
        },
      ],
    },
  ],
}
```

### Network Security
- **HTTPS**: Enable SSL/TLS for production
- **CORS**: Configure properly for hardware API access
- **Firewall**: Restrict access to AutoLight device API
- **Authentication**: Consider adding auth if needed for public deployment

## Monitoring & Maintenance

### Health Checks
```typescript
// app/api/health/route.ts
export async function GET() {
  return Response.json({
    status: 'ok',
    timestamp: new Date().toISOString(),
    version: process.env.npm_package_version,
  })
}
```

### Logging Strategy
```bash
# PM2 logs
pm2 logs autolight-webclient

# Docker logs
docker logs container-name

# Vercel logs
vercel logs
```

### Update Strategy
1. **Git-based**: Push to main branch for automatic deployment (Vercel)
2. **Manual**: Build and replace files for traditional deployment
3. **Docker**: Update image and restart container
4. **Rolling Updates**: Use PM2 for zero-downtime updates

## Troubleshooting

### Common Issues

**Build Errors:**
```bash
# Clear Next.js cache
rm -rf .next
npm run build
```

**Port Conflicts:**
```bash
# Change port in package.json
"dev": "next dev --turbopack -p 3002"
"start": "next start -p 3002"
```

**Hardware API Connection:**
- Verify device IP address
- Check network connectivity
- Ensure API endpoints are accessible
- Verify CORS configuration

### Performance Issues
- **Bundle Size**: Use bundle analyzer to identify large dependencies
- **Memory Usage**: Monitor with PM2 or Docker stats
- **API Latency**: Check network connection to AutoLight device

## Support & Resources

### Documentation Links
- **Next.js 15**: https://nextjs.org/docs
- **shadcn/ui**: https://ui.shadcn.com/docs
- **Tailwind CSS**: https://tailwindcss.com/docs
- **AutoLight V3**: See main project documentation

### Development Resources
- **Migration Guide**: See `MIGRATION_PLAN.md`
- **Progress Report**: See `MIGRATION_PROGRESS.md`
- **Project Structure**: See `CLAUDE.md`

The AutoLight V3 WebClient is now ready for production deployment with any of the above methods. Choose the deployment option that best fits your infrastructure requirements.