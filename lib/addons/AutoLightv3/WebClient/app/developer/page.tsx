"use client"

import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card"
import { Button } from "@/components/ui/button"
import { Wifi, Package, Activity, Code, Settings, Database, Lightbulb } from "lucide-react"
import Link from "next/link"

export default function DeveloperPage() {
  return (
    <div className="space-y-6">
      <div className="grid gap-6 md:grid-cols-2 lg:grid-cols-4">
        <Card>
          <CardHeader className="flex flex-row items-center space-y-0 pb-2">
            <div className="flex h-8 w-8 items-center justify-center rounded-lg bg-primary text-primary-foreground">
              <Wifi className="h-4 w-4" />
            </div>
            <div className="ml-4">
              <CardTitle className="text-sm font-medium">
                Koneksi
              </CardTitle>
              <CardDescription>
                Konfigurasi koneksi device
              </CardDescription>
            </div>
          </CardHeader>
          <CardContent>
            <div className="text-xs text-muted-foreground mb-4">
              Atur IP address, port, dan protokol komunikasi dengan AutoLight V3
            </div>
            <Link href="/developer/connection">
              <Button variant="outline" size="sm" className="w-full">
                Kelola Koneksi
              </Button>
            </Link>
          </CardContent>
        </Card>

        <Card>
          <CardHeader className="flex flex-row items-center space-y-0 pb-2">
            <div className="flex h-8 w-8 items-center justify-center rounded-lg bg-secondary text-secondary-foreground">
              <Package className="h-4 w-4" />
            </div>
            <div className="ml-4">
              <CardTitle className="text-sm font-medium">
                Update
              </CardTitle>
              <CardDescription>
                System update management
              </CardDescription>
            </div>
          </CardHeader>
          <CardContent>
            <div className="text-xs text-muted-foreground mb-4">
              Kelola update firmware dan konfigurasi sistem
            </div>
            <Link href="/developer/update">
              <Button variant="outline" size="sm" className="w-full">
                Manage Updates
              </Button>
            </Link>
          </CardContent>
        </Card>

        <Card>
          <CardHeader className="flex flex-row items-center space-y-0 pb-2">
            <div className="flex h-8 w-8 items-center justify-center rounded-lg bg-destructive text-destructive-foreground">
              <Activity className="h-4 w-4" />
            </div>
            <div className="ml-4">
              <CardTitle className="text-sm font-medium">
                API Test
              </CardTitle>
              <CardDescription>
                Test API endpoints
              </CardDescription>
            </div>
          </CardHeader>
          <CardContent>
            <div className="text-xs text-muted-foreground mb-4">
              Test dan debug REST API endpoints untuk development
            </div>
            <Link href="/developer/api-test">
              <Button variant="outline" size="sm" className="w-full">
                Test APIs
              </Button>
            </Link>
          </CardContent>
        </Card>

        <Card>
          <CardHeader className="flex flex-row items-center space-y-0 pb-2">
            <div className="flex h-8 w-8 items-center justify-center rounded-lg bg-amber-500 text-white">
              <Lightbulb className="h-4 w-4" />
            </div>
            <div className="ml-4">
              <CardTitle className="text-sm font-medium">
                Pattern Builder
              </CardTitle>
              <CardDescription>
                Visual LED pattern creator
              </CardDescription>
            </div>
          </CardHeader>
          <CardContent>
            <div className="text-xs text-muted-foreground mb-4">
              Create custom LED sequences dengan visual editor dan generate C++ code
            </div>
            <Link href="/simulation">
              <Button variant="outline" size="sm" className="w-full">
                Open Builder
              </Button>
            </Link>
          </CardContent>
        </Card>
      </div>

      <Card>
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <Code className="h-5 w-5" />
            Developer Tools
          </CardTitle>
          <CardDescription>
            Advanced tools untuk development dan debugging AutoLight V3 system
          </CardDescription>
        </CardHeader>
        <CardContent>
          <div className="grid gap-4 md:grid-cols-2">
            <div className="space-y-2">
              <h4 className="text-sm font-medium">System Information</h4>
              <p className="text-xs text-muted-foreground">
                AutoLight V3 WebClient - Next.js 15 + React 19
              </p>
              <p className="text-xs text-muted-foreground">
                API Version: v1
              </p>
            </div>
            <div className="space-y-2">
              <h4 className="text-sm font-medium">Quick Actions</h4>
              <div className="flex gap-2">
                <Button variant="outline" size="sm">
                  <Database className="h-3 w-3 mr-1" />
                  Clear Cache
                </Button>
                <Button variant="outline" size="sm">
                  <Settings className="h-3 w-3 mr-1" />
                  Reset Config
                </Button>
              </div>
            </div>
          </div>
        </CardContent>
      </Card>
    </div>
  )
}