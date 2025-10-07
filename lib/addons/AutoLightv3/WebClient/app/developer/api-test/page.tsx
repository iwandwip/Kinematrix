"use client"

import dynamic from "next/dynamic"

const APITestPageContent = dynamic(() => import("./content"), { ssr: false })

export default function APITestPage() {
  return <APITestPageContent />
}