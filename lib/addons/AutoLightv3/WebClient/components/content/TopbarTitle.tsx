"use client"

import { Badge } from "@/components/ui/badge";

export default function TopbarTitle() {
  return (
    <div className="ml-auto flex-1 sm:flex-initial">
      <h2 className="scroll-m-20 pb-2 text-xl font-semibold tracking-tight transition-colors first:mt-0">
        Auto Light System
        <Badge className="ml-2">v2.0</Badge>
      </h2>
    </div>
  );
}