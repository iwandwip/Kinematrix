<script setup lang="ts">
import {Card, CardDescription, CardContent, CardFooter, CardHeader, CardTitle} from '@/components/ui/card'
import MainHeader from "@/components/content/MainHeader.vue";

</script>

<template>
  <div class="flex min-h-screen w-full flex-col">
    <MainHeader></MainHeader>
    <main class="flex min-h-[calc(100vh_-_theme(spacing.16))] flex-1 flex-col gap-4 bg-muted/40 p-4 md:gap-8 md:p-10">

    <div class="mx-auto grid w-full max-w-6xl items-start gap-6 md:grid-cols-[180px_1fr] lg:grid-cols-[250px_1fr]">
      <nav class="grid gap-4 text-sm text-muted-foreground">
        <router-link to="/settings/tema"  class="font-semibold text-primary">Tema</router-link>
        <router-link to="/settings/update"  class="">Update</router-link>

      </nav>
      <div class="grid gap-6">
        <Card>
          <CardHeader>
            <CardTitle>Pilih Tema</CardTitle>
            <CardDescription>
              Pilih tema aplikasi sesuai dengan selera anda
            </CardDescription>
          </CardHeader>
          <CardContent>
            <form>
              <Input placeholder="Store Name" />
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
            class="underline underline-offset-4 font-bold decoration-foreground"> workalogi </a></span>
          <br>

        </div>
      </div>
    </footer>
  </div>
</template>

<style scoped>

</style>