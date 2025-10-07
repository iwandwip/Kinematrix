"use client"

import dynamic from "next/dynamic"

const ConnectionPageContent = dynamic(() => import("./content"), { ssr: false })

export default function ConnectionPage() {
  return <ConnectionPageContent />
}