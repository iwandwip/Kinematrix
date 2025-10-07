"use client"

import { LoaderCircle } from "lucide-react"
import { cn } from "@/lib/utils"

interface SpinnerProps {
  className?: string
}

export default function Spinner({ className }: SpinnerProps) {
  return <LoaderCircle className={cn("animate-spin h-4 w-4", className)} />
}

export function FullScreenSpinner({ className }: SpinnerProps) {
  return (
    <div className="fixed inset-0 flex justify-center items-center">
      <LoaderCircle className={cn("animate-spin h-24 w-24 text-green-700", className)} />
    </div>
  )
}