<script setup lang="ts">

import TopbarTittle from "@/components/content/TopbarTittle.vue";
import ThemeToogle from "@/components/content/ThemeToogle.vue";
import Sidebar from "@/components/content/Sidebar.vue";

import {settingsData} from "@/state/state.ts";

</script>

<template>
  <header class="sticky z-50 top-0 flex h-16 items-center gap-4 border-b bg-background px-4 md:px-6">
    <Topbar></Topbar>
    <Sidebar></Sidebar>

    <div class="flex w-full items-center text-center gap-4 md:ml-auto md:gap-2 lg:gap-4">
      <TopbarTittle></TopbarTittle>
      <div v-if="settingsData.themes.value == 'modern'">
        <ThemeToogle></ThemeToogle>
      </div>

    </div>
  </header>
</template>

<style scoped>

</style>