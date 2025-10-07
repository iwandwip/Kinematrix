import { useState, useEffect } from 'react'

const MONACO_THEME_KEY = 'monaco-editor-theme'
type MonacoTheme = 'vs-dark' | 'vs'

export function useMonacoTheme(defaultTheme: MonacoTheme = 'vs-dark') {
  const [theme, setTheme] = useState<MonacoTheme>(() => {
    if (typeof window !== 'undefined') {
      const saved = localStorage.getItem(MONACO_THEME_KEY)
      if (saved === 'vs-dark' || saved === 'vs') {
        return saved
      }
    }
    return defaultTheme
  })

  useEffect(() => {
    if (typeof window !== 'undefined') {
      localStorage.setItem(MONACO_THEME_KEY, theme)
    }
  }, [theme])

  const toggleTheme = () => {
    setTheme(prev => prev === 'vs-dark' ? 'vs' : 'vs-dark')
  }

  const isDarkMode = theme === 'vs-dark'

  return { theme, setTheme, toggleTheme, isDarkMode } as const
}