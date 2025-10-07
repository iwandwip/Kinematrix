"use client"

import { Sheet, SheetContent, SheetTrigger, SheetHeader, SheetTitle } from "@/components/ui/sheet";
import { Button } from "@/components/ui/button";
import { Menu, ShoppingCart, Package, Wifi, Activity, Code } from "lucide-react";
import Link from "next/link";

export default function Sidebar() {
  return (
    <Sheet>
      <SheetTrigger asChild>
        <Button
          variant="outline"
          size="icon"
          className="shrink-0"
        >
          <Menu className="h-5 w-5" />
          <span className="sr-only">Toggle navigation menu</span>
        </Button>
      </SheetTrigger>
      <SheetContent side="left" className="flex flex-col">
        <SheetHeader>
          <SheetTitle>Menu Navigasi</SheetTitle>
        </SheetHeader>
        <nav className="grid gap-2 text-lg font-medium pl-2">
          <p className="flex items-center gap-4 rounded-xl px-4 py-2 text-foreground ml-2">
            Home
          </p>
          <Link 
            href="/"
            className="flex items-center gap-4 rounded-xl px-4 py-2 text-foreground hover:text-foreground hover:bg-muted/50 ml-2"
          >
            <ShoppingCart className="h-5 w-5" />
            Kontrol
          </Link>
          <p className="flex items-center gap-4 rounded-xl px-4 py-2 text-muted-foreground ml-2">
            Pengaturan
          </p>
          <Link 
            href="/settings/theme"
            className="flex items-center gap-4 rounded-xl px-4 py-2 text-foreground hover:text-foreground hover:bg-muted/50 ml-2"
          >
            <ShoppingCart className="h-5 w-5" />
            Tema
          </Link>
          <p className="flex items-center gap-4 rounded-xl px-4 py-2 text-muted-foreground ml-2 mt-4">
            Developer Settings
          </p>
          <Link 
            href="/developer/connection"
            className="flex items-center gap-4 rounded-xl px-4 py-2 text-muted-foreground hover:text-foreground hover:bg-muted/50 ml-2"
          >
            <Wifi className="h-5 w-5" />
            Koneksi
          </Link>
          <Link 
            href="/developer/update"
            className="flex items-center gap-4 rounded-xl px-4 py-2 text-muted-foreground hover:text-foreground hover:bg-muted/50 ml-2"
          >
            <Package className="h-5 w-5" />
            Update
          </Link>
          <Link
            href="/developer/api-test"
            className="flex items-center gap-4 rounded-xl px-4 py-2 text-muted-foreground hover:text-foreground hover:bg-muted/50 ml-2"
          >
            <Activity className="h-5 w-5" />
            API Test
          </Link>
          <Link
            href="/simulation"
            className="hidden lg:flex items-center gap-4 rounded-xl px-4 py-2 text-muted-foreground hover:text-foreground hover:bg-muted/50 ml-2"
          >
            <Code className="h-5 w-5" />
            Pattern Builder
          </Link>
        </nav>
      </SheetContent>
    </Sheet>
  );
}