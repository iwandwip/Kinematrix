<script setup lang="ts">


import { Button } from '@/components/ui/button'
import { settingsData} from "@/state/state.ts";
import {Card, CardDescription, CardContent, CardFooter, CardHeader, CardTitle} from "@/components/ui/card";
import MainHeader from "@/components/content/MainHeader.vue";
</script>

<template>
  <div class="flex min-h-screen w-full flex-col">
    <MainHeader></MainHeader>
    <main class="flex min-h-[calc(100vh_-_theme(spacing.16))] flex-1 flex-col gap-4 bg-muted/40 p-4 md:gap-8 md:p-10">
      <div class="mx-auto grid w-full max-w-6xl gap-2">
        <h1 class="text-3xl font-semibold">
          Pengaturan: Update
        </h1>
      </div>
      <div class="mx-auto grid w-full max-w-6xl items-start gap-6 md:grid-cols-[180px_1fr] lg:grid-cols-[250px_1fr]">
        <nav class="grid gap-4 text-sm text-muted-foreground">
          <a @click="settingsData.page.value = 'settings-tema'"  class="">Tema</a>
          <a @click="settingsData.page.value = 'settings-update'"  class="font-semibold text-primary">Update</a>

        </nav>
        <div class="grid gap-6">
          <Card>
            <CardHeader>
              <CardTitle>Update Firmware ALS</CardTitle>
              <CardDescription>
                Update Firmware Auto Light System
              </CardDescription>
            </CardHeader>
            <CardContent>
            </CardContent>
            <CardFooter class="border-t px-6 py-4">
              <Button>Save</Button>
            </CardFooter>
          </Card>
          <Card>
            <CardHeader>
              <CardTitle>Update App ALS</CardTitle>
              <CardDescription>
                Update Aplikasi  Auto Light System
              </CardDescription>
            </CardHeader>
            <CardContent>
              <form class="flex flex-col gap-4">

              </form>
            </CardContent>
            <CardFooter class="border-t px-6 py-4">
              <Button>Save</Button>
            </CardFooter>
          </Card>
        </div>
      </div>
    </main>
    <footer class="py-6 md:px-8 md:py-0">
      <div class="container flex flex-col items-center justify-between gap-4 md:h-24 md:flex-row">
        <div class="text-center text-sm leading-loose text-muted-foreground md:text-left"><span class="inline-block">
          Built and designed by <a
            href="#"
            class="underline underline-offset-4 font-bold decoration-foreground"> workalogi </a></span>
          <br>

        </div>
      </div>
    </footer>
  </div>
</template>

<style scoped>

</style>