<script setup lang="ts">
import {Sheet, SheetContent, SheetTrigger} from "@/components/ui/sheet";
import {Button} from "@/components/ui/button";
import {Menu} from "lucide-vue-next";
import { settingsData} from "@/state/state.ts";
</script>

<template>
  <Sheet>
    <SheetTrigger as-child>
      <Button
          variant="outline"
          size="icon"
          class="shrink-0 md:hidden"
      >
        <Menu class="h-5 w-5"/>
        <span class="sr-only">Toggle navigation menu</span>
      </Button>
    </SheetTrigger>
    <SheetContent side="left" class="flex flex-col">
      <nav class="grid gap-2 text-lg font-medium">
        Home
        <a @click="settingsData.page.value = 'index'"
           class="mx-[-0.65rem] flex items-center gap-4 rounded-xl  py-2 text-foreground hover:text-foreground hover:bg-muted/50"
        >
          <ShoppingCart class="h-5 w-5" />
          Kontrol
        </a>
        <p

            class="mx-[-0.65rem] flex items-center gap-4 rounded-xl px-3 py-2 text-muted-foreground "
        >
         Pengaturan
        </p>
        <a @click="settingsData.page.value = 'settings-tema'"
           class="mx-[-0.65rem] flex items-center gap-4 rounded-xl  px-3 py-2 text-foreground  hover:text-foreground hover:bg-muted/50"
        >
          <ShoppingCart class="h-5 w-5" />
          Tema
        </a>
        <a @click="settingsData.page.value = 'settings-update'"
           class="mx-[-0.65rem] flex items-center gap-4 rounded-xl px-3 py-2 text-muted-foreground hover:text-foreground hover:bg-muted/50"
        >
          <Package class="h-5 w-5" />
          Update
        </a>

      </nav>



    </SheetContent>
  </Sheet>
</template>

<style scoped>

</style>