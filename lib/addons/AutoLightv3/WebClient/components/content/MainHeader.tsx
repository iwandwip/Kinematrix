"use client"

import TopbarTitle from "@/components/content/TopbarTitle"
import ThemeToggle from "@/components/content/ThemeToggle"
import Sidebar from "@/components/content/Sidebar"
import ConnectionStatus from "@/components/content/ConnectionStatus"
import { useALS } from "@/lib/contexts/ALSContext"

export default function MainHeader() {
  const { settingsData } = useALS()

  return (
    <header className="sticky z-50 top-0 flex h-16 items-center gap-4 border-b bg-background px-4 md:px-6">
      <Sidebar />
      
      <div className="flex w-full items-center text-center gap-4 md:ml-auto md:gap-2 lg:gap-4">
        <TopbarTitle />
        <div className="flex items-center gap-2 ml-auto">
          <ConnectionStatus />
          {settingsData.themes === 'modern' && (
            <ThemeToggle />
          )}
        </div>
      </div>
    </header>
  )
}