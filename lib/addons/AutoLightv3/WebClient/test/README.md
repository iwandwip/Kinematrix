# AutoLight V3 C++ Parser Testing Framework

Enhanced C++ parser testing framework untuk validasi sequence BaseChannelSequence.cpp dengan output format yang mudah dianalisis.

## Setup

```bash
# Install dependencies
npm install

# Build TypeScript
npm run build
```

## Usage

### Test Individual Sequence
```bash
# Test sequence 2
npm run test -- --sequence 2

# Test sequence 3
npm run test -- --sequence 3
```

### Test All Sequences
```bash
npm run test -- --all
```

### Compare Results
```bash
npm run test -- --compare
```

### Validate Output
```bash
npm run test -- --validate 2
```

## Output Format

Frame format: `{frame_number} {channel_states_semicolon_separated}`

Contoh:
```
1 1;1;1;1;1;1;1;1;1;1;1;1;0;0;0;0;0;0;0;0;0;0;0;0;
2 0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;
3 1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;
```

- **Format**: Frame number diikuti spasi, lalu channel states dipisah semicolon
- **Channels**: 24 channels (0-23), bisa dikonfigurasi untuk angka genap lainnya
- **States**: 1=HIGH/ON, 0=LOW/OFF
- **Purpose**: Format mudah untuk analisis dan validasi

## Directory Structure

```
test/
├── package.json              # Dependencies & scripts
├── tsconfig.json            # TypeScript config
├── index.ts                 # Main test runner
├── lib/                     # Core libraries
│   ├── enhanced-cpp-parser.ts  # Enhanced C++ parser
│   ├── cpp-simulator.ts       # Execution engine
│   ├── frame-logger.ts        # Output formatter
│   └── types.ts              # TypeScript interfaces
├── input/                   # C++ sequence files
│   ├── sequence-2.cpp        # taskSequence2BlinkAll()
│   └── sequence-3.cpp        # taskSequence3FillTwoPoint()
├── output/                  # Generated results (auto-created)
└── README.md               # This documentation
```

## Parser Features

### Supported C++ Patterns
- **Nested for loops**: `for (int i = 0; i < config_data_ptr_->header.pin_size_; i++)`
- **Set commands**: `set(config_data_ptr_->header.pin_ptr_[expression], HIGH/LOW)`
- **Sleep commands**: `sleep(channel_data_.delay_time_)` dan `sleep(fixed_numbers)`
- **Mathematical expressions**: `pin_size_/2`, `i+j`, complex array access
- **Utility functions**: `off()`, `on()`

### Enhanced Features vs Existing Parsers
- **More accurate** than simpleCppParser.ts (83 lines)
- **Less complex** than cppParser.ts (1018+ lines)
- **Focused optimization** for BaseChannelSequence.cpp patterns
- **Better error handling** dan debugging output

## Console Output Example

```
=== Testing Sequence 2: taskSequence2BlinkAll ===
✓ Parsed 45 commands
✓ Generated 78 frames
✓ Simulation completed in 12ms

1 1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;
  -> Active channels: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23], Delay: 50ms
2 0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;
  -> Active channels: [], Delay: 50ms
3 1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;
  -> Active channels: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23], Delay: 50ms
  ... (75 more frames) ...

✓ Generated 78 frames successfully
✓ Total simulation time: 3900ms
✓ Output saved to: ./output/sequence-2.txt
```

## Integration to WebClient

### Phase 1: Standalone Testing ✅
1. Framework struktur selesai
2. Sequence 2 & 3 sudah diekstrak
3. Enhanced parser implementation
4. Output validation tools

### Phase 2: WebClient Integration
1. Copy `enhanced-cpp-parser.ts` ke `WebClient/lib/simulation/`
2. Update imports di WebClient components:
```typescript
// Replace:
import { parseAndSimulateCppCode } from './cppParser'

// With:
import { parseAndSimulateCppCode } from './enhanced-cpp-parser'
```

## Configuration

Default config di `index.ts`:
```typescript
{
  totalChannels: 24,     // LED channels
  defaultDelay: 50,      // Default sleep time (ms)
  pinSize: 24,           // Pin size for calculations
  outputFormat: 'both',  // 'console' | 'file' | 'both'
  outputPath: './output' // Output directory
}
```

## Troubleshooting

### Build Issues
```bash
npm run build
# Check for TypeScript errors
```

### Missing Input Files
- Ensure `input/sequence-2.cpp` dan `input/sequence-3.cpp` exist
- Check file content format matches BaseChannelSequence.cpp

### Output Validation
```bash
npm run test -- --validate 2
# Validates frame format dan channel count
```