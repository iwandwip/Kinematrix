export type SaveBehaviorMode = 'browser-default' | 'editor-only' | 'page-wide';

export interface SaveBehaviorOption {
  value: SaveBehaviorMode;
  label: string;
  description: string;
  icon: string;
}

export const SAVE_BEHAVIOR_OPTIONS: SaveBehaviorOption[] = [
  {
    value: 'browser-default',
    label: 'Browser Default',
    description: 'Allow browser save dialog (Ctrl+S saves page as HTML)',
    icon: 'Download'
  },
  {
    value: 'editor-only',
    label: 'Prevent in Editor Only',
    description: 'Block save dialog when focused in code editor',
    icon: 'FileCode'
  },
  {
    value: 'page-wide',
    label: 'Prevent Entire Page',
    description: 'Block save dialog anywhere in simulation page',
    icon: 'Shield'
  }
];