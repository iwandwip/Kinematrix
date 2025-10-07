"use client"

import { ReactNode } from "react"
import MainHeader from "@/components/content/MainHeader"
import Link from "next/link"
import { usePathname } from "next/navigation"

interface SettingsLayoutProps {
  children: ReactNode
}

export default function SettingsLayout({ children }: SettingsLayoutProps) {
  const pathname = usePathname()

  return (
    <div className="flex min-h-screen w-full flex-col">
      <MainHeader />
      <main className="flex min-h-[calc(100vh_-_theme(spacing.16))] flex-1 flex-col gap-4 bg-muted/40 p-4 md:gap-8 md:p-10">
        <div className="mx-auto grid w-full max-w-6xl gap-2">
          <h1 className="text-3xl font-semibold">
            Pengaturan
          </h1>
        </div>
        
        <div className="mx-auto grid w-full max-w-6xl items-start gap-6 md:grid-cols-[180px_1fr] lg:grid-cols-[250px_1fr]">
          <nav className="grid gap-4 text-sm text-muted-foreground">
            <Link 
              href="/settings/theme" 
              className={`${pathname === '/settings/theme' ? 'font-semibold text-primary' : ''}`}
            >
              Tema
            </Link>
          </nav>
          
          <div className="grid gap-6">
            {children}
          </div>
        </div>
      </main>
      
      <footer className="py-6 md:px-8 md:py-0">
        <div className="container flex flex-col items-center justify-between gap-4 md:h-24 md:flex-row">
          <div className="text-center text-sm leading-loose text-muted-foreground md:text-left">
            <span className="inline-block">
              Built and designed by <a
                href="#"
                className="underline underline-offset-4 font-bold decoration-foreground"
              >
                workalogi
              </a>
            </span>
            <br />
          </div>
        </div>
      </footer>
    </div>
  )
}