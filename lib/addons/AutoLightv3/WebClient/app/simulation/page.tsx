"use client"

import { SimulationProvider } from "@/lib/contexts/SimulationContext"
import { PatternBuilderHydrated } from "@/components/simulation/PatternBuilderHydrated"
import MainHeader from "@/components/content/MainHeader"
import SimulationContent from "@/components/simulation/SimulationContent"
import { Alert, AlertTitle, AlertDescription } from "@/components/ui/alert"
import { Monitor } from "lucide-react"

export default function SimulationPage() {
  return (
    <SimulationProvider>
      <PatternBuilderHydrated>
        <MainHeader />

        {/* Desktop Content */}
        <div className="hidden lg:block">
          <SimulationContent />
        </div>

        {/* Mobile Warning */}
        <div className="block lg:hidden p-4 md:p-6">
          <div className="max-w-2xl mx-auto">
            <Alert className="border-amber-200 bg-amber-50 dark:border-amber-800 dark:bg-amber-950">
              <Monitor className="h-4 w-4 text-amber-600" />
              <AlertTitle className="text-amber-800 dark:text-amber-200">
                Desktop Required
              </AlertTitle>
              <AlertDescription className="text-amber-700 dark:text-amber-300">
                <p>Pattern Builder hanya tersedia di desktop untuk pengalaman terbaik.</p>
                <p className="mt-2">
                  Silahkan akses menggunakan desktop atau laptop dengan layar minimal 1024px untuk menggunakan fitur ini.
                </p>
              </AlertDescription>
            </Alert>

            <div className="mt-6 text-center">
              <h2 className="text-xl font-semibold text-muted-foreground">Pattern Builder</h2>
              <p className="text-sm text-muted-foreground mt-2">
                Visual LED pattern creator dengan custom grid layouts
              </p>
            </div>
          </div>
        </div>
      </PatternBuilderHydrated>
    </SimulationProvider>
  )
}