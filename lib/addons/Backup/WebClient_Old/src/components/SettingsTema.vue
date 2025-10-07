<script setup lang="ts">
import { Button } from '@/components/ui/button'
import { Label } from '@/components/ui/label'
import { RadioGroup, RadioGroupItem } from '@/components/ui/radio-group'
import {Card, CardDescription, CardContent, CardFooter, CardHeader, CardTitle} from '@/components/ui/card'
import MainHeader from "@/components/content/MainHeader.vue";
import { settingsData} from "@/state/state.ts";
import {ref} from "vue";
import {LoaderCircle} from "lucide-vue-next";
import {useRouter} from "vue-router";

const route = useRouter();

// console.log(settingsData.themes.value)

const temaSelection = ref(settingsData.themes.value);

function gantiTema() {
  try {
    settingsData.startLoading()
    settingsData.themes.value = temaSelection.value;
    settingsData.stopLoading()

    settingsData.page.value = "index"
    route.push('/')
  }
  catch (e) {
    settingsData.page.value = "settings-tema"
  }
}

</script>

<template>
  <div class="flex min-h-screen w-full flex-col">
    <MainHeader></MainHeader>
    <main class="flex min-h-[calc(100vh_-_theme(spacing.16))] flex-1 flex-col gap-4 bg-muted/40 p-4 md:gap-8 md:p-10">
      <div class="mx-auto grid w-full max-w-6xl gap-2">
        <h1 class="text-3xl font-semibold">
          Pengaturan: Tema {{  settingsData.themes.value}}

        </h1>
      </div>
      <div class="mx-auto grid w-full max-w-6xl items-start gap-6 md:grid-cols-[180px_1fr] lg:grid-cols-[250px_1fr]">
        <nav class="grid gap-4 text-sm text-muted-foreground">
          <a @click="settingsData.page.value = 'settings-tema'" class="font-semibold text-primary" >Tema</a>
          <a @click="settingsData.page.value = 'settings-update'" class="">Update</a>

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
              <RadioGroup v-model="temaSelection">
                <div class="flex items-center space-x-2">
                  <RadioGroupItem id="r1" value="classic" />
                  <Label for="r1">Klasik</Label>
                </div>
                <div class="flex items-center space-x-2">
                  <RadioGroupItem id="r2" value="modern" />
                  <Label for="r2">Modern</Label>
                </div>
              </RadioGroup>
            </CardContent>
            <CardFooter class="border-t px-6 py-4">
              <Button @click="gantiTema()">
                Ganti <LoaderCircle class="ml-2 animate-spin h-6 w-6 " v-if="settingsData.loader.value == true" />

              </Button>

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