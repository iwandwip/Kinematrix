export interface SimpleLEDFrame {
  id: string
  channels: boolean[]
  delay: number
  label?: string
}

export function parseSimpleCppCode(code: string, totalChannels: number): SimpleLEDFrame[] {
  const frames: SimpleLEDFrame[] = []
  const lines = code.split('\n')
  const currentChannels = new Array(totalChannels).fill(false)
  let frameCounter = 0

  console.log('Parsing simple C++ code with', totalChannels, 'channels')

  for (const line of lines) {
    const trimmed = line.trim()

    // Parse set(pin_ptr_[index], HIGH/LOW)
    const setMatch = trimmed.match(/set\(pin_ptr_\[(\d+)\],\s*(HIGH|LOW)\)/)
    if (setMatch) {
      const channelIndex = parseInt(setMatch[1])
      const state = setMatch[2] === 'HIGH'

      if (channelIndex >= 0 && channelIndex < totalChannels) {
        currentChannels[channelIndex] = state
        console.log(`Set channel ${channelIndex} to ${state}`)
      }
    }

    // Parse sleep(delay)
    const sleepMatch = trimmed.match(/sleep\((\d+)\)/)
    if (sleepMatch) {
      const delay = parseInt(sleepMatch[1])

      // Create frame with current LED state
      frames.push({
        id: `frame-${frameCounter++}`,
        channels: [...currentChannels],
        delay,
        label: `Frame ${frameCounter}`
      })

      console.log(`Created frame with delay ${delay}ms, active channels:`,
        currentChannels.map((state, i) => state ? i : null).filter(x => x !== null))
    }

    // Parse off() - turn all off
    if (trimmed.includes('off()')) {
      currentChannels.fill(false)
      frames.push({
        id: `frame-${frameCounter++}`,
        channels: [...currentChannels],
        delay: 50,
        label: 'All OFF'
      })
    }

    // Parse on() - turn all on
    if (trimmed.includes('on()')) {
      currentChannels.fill(true)
      frames.push({
        id: `frame-${frameCounter++}`,
        channels: [...currentChannels],
        delay: 50,
        label: 'All ON'
      })
    }
  }

  // If no frames created, create one default frame
  if (frames.length === 0) {
    frames.push({
      id: 'frame-0',
      channels: new Array(totalChannels).fill(false),
      delay: 100,
      label: 'Default Frame'
    })
  }

  console.log(`Generated ${frames.length} frames`)
  return frames
}