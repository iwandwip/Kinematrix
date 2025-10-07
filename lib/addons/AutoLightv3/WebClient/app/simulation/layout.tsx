import { Metadata } from "next"

export const metadata: Metadata = {
  title: "Pattern Builder - AutoLight System",
  description: "Visual LED pattern creator and C++ code generator",
}

export default function SimulationLayout({
  children,
}: {
  children: React.ReactNode
}) {
  return (
    <div className="flex min-h-screen w-full flex-col">
      {children}
    </div>
  )
}