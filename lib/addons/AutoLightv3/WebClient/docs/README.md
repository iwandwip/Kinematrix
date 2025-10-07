# AutoLight V3 WebClient Documentation

Central documentation hub for the AutoLight V3 WebClient - a professional-grade Next.js 15 web interface for LED matrix control systems.

## Table of Contents

### Latest Checkpoints

- [2025-09-30 15:03 - JavaScript Simulator: Loop Mode Fix & Compile-First Workflow](./CHECKPOINT_20250930_1503.md) ⭐ **LATEST**
- [2025-09-30 14:17 - Channel Mapper v4 Bug Fix](./CHECKPOINT_20250930_1417.md)
- [2025-09-30 10:03 - Monaco Editor Integration](./CHECKPOINT_20250930_1003.md)
- [2025-09-29 20:46 - High-Precision Timing System](./CHECKPOINT_20250929_2046.md)
- [2025-09-29 18:25 - JavaScript Simulator Production Ready](./CHECKPOINT_20250929_1825.md)

### Core Documentation

#### Development Guides
1. [Version Comparison](./01_PERBANDINGAN_VERSI.md) - Classic vs Modern theme comparison
2. [TaskSequence Guide](./02_PANDUAN_TASKSEQUENCE.md) - Task management patterns
3. [Migration Plan](./03_MIGRATION_PLAN.md) - Upgrade strategies
4. [Puppeteer MCP Guide](./04_PUPPETEER_MCP_GUIDE.md) - Browser automation testing
5. [Git Commands](./14_GIT_COMMAND.md) - Common Git operations

#### Feature Documentation
7. [Real-time Visualizer Complete Guide](./07_REALTIME_VISUALIZER_COMPLETE_GUIDE.md)
8. [Puppeteer Usage](./08_PUPPETEER_USAGE.md)
9. [API Polling System](./09_SISTEM_POLLING_API.md)
10. [Universal Pattern Generator Plan](./10_UNIVERSAL_PATTERN_GENERATOR_PLAN.md)
11. [Pattern Builder Dual Mode](./11_PATTERN_BUILDER_DUAL_MODE.md)
12. [Preset Implementation Progress](./12_PRESET_IMPLEMENTATION_PROGRESS.md)
13. [Testing Plan](./13_TESTING_PLAN.md)
15. [Dynamic Channel Code Generator Enhancement](./15_DYNAMIC_CHANNEL_CODE_GENERATOR_ENHANCEMENT.md)

#### Advanced Features
16. [Expression Builder Complete Design](./16_EXPRESSION_BUILDER_COMPLETE_DESIGN.md)
17. [Expression Builder UI Design](./17_EXPRESSION_BUILDER_UI_DESIGN.md)
18. [JavaScript Simulator Mode Plan](./18_JAVASCRIPT_SIMULATOR_MODE_PLAN.md)
19. [BaseChannelSequence vs JavaScript Analysis](./19_BASECHANNELSEQUENCE_VS_JAVASCRIPT_ANALYSIS.md)

#### Testing Documentation
5. [Puppeteer Test Results](./05_PUPPETEER_TEST_RESULTS.md)
6. [Next.js Test Results](./06_NEXTJS_TEST_RESULTS.md)

### Change History

See [CHANGELOG.md](./CHANGELOG.md) for user-facing changes and feature releases.

## Quick Links

- [Project CLAUDE.md](../CLAUDE.md) - AutoLight V3 architecture guide
- [WebClient CLAUDE.md](./CLAUDE.md) - WebClient-specific development guide
- [Main Repository CLAUDE.md](../../../../../CLAUDE.md) - Kinematrix framework overview

## Documentation Standards

- **Checkpoint Files**: UPPER_SNAKE_CASE.md format (e.g., CHECKPOINT_YYYYMMDD_HHMM.md)
- **Feature Docs**: Numbered prefix for ordered reading (e.g., 01_FEATURE_NAME.md)
- **Links**: Relative paths for cross-document references
- **Code Blocks**: Language-tagged fences for syntax highlighting
- **Headings**: Sentence case with single H1 per document

## Project Overview

**Technology Stack**:
- Frontend: Next.js 15 + React 19 + TypeScript
- UI Framework: shadcn/ui (21 components) + Tailwind CSS 4
- Editor: Monaco Editor with custom themes
- Testing: TypeScript C++ parser + Puppeteer automation
- Mobile: Capacitor for Android/iOS deployment

**Core Features**:
- 4 Pattern Development Modes: Visual, C++, Expression, JavaScript
- Real-time Hardware Control via REST API
- Pattern Builder with dynamic grid system
- High-precision timing system (±1-4% accuracy)
- Professional IDE-like development workflow

**Architecture**:
- Client-side rendering with App Router
- Global state management via React Context
- 2-second polling for real-time updates
- Dynamic theme system (Classic/Modern)
- Desktop-focused Pattern Builder (≥1024px)

## Getting Started

```bash
# Install dependencies
npm install

# Start development server (port 3001)
npm run dev

# Build for production
npm run build

# Run linter
npm run lint

# Kill port 3001 if in use
npm run kill
```

## Contributing

When adding new features:
1. Follow existing code patterns and conventions
2. Update relevant documentation (CLAUDE.md, feature docs)
3. Create checkpoint document for significant changes
4. Add entry to CHANGELOG.md with checkpoint link
5. Update this README.md table of contents

---

**Last Updated**: 2025-09-30
**Version**: AutoLight V3 WebClient
**Maintainer**: Kinematrix Development Team