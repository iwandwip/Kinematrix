<script setup lang="ts">

import ModernTheme from "@/components/ModernTheme.vue";
import ClassicTheme from "@/components/ClassicTheme.vue";
// import HelloWorld from "@/components/HelloWorld.vue";
import Spinner from "@/components/ui/spinner/spinner.vue";

import {settingsData} from "@/state/state.ts";
import SettingsTema from "@/components/SettingsTema.vue";
import SettingsUpdate from "@/components/SettingsUpdate.vue";

</script>

<template>

  <div v-if="settingsData.loader.value == true">
    <Spinner></Spinner>
  </div>
  <div v-else>
    <!-- Page Inddex -->
    <div v-if="settingsData.page.value == 'index'">
      <div v-if="settingsData.themes.value == 'classic'">
        <ClassicTheme></ClassicTheme>
      </div>
      <div v-else-if="settingsData.themes.value == 'modern'">
        <ModernTheme></ModernTheme>
      </div>
    </div>


    <!-- Page Settings  -->
    <div v-else>
      <div v-if="settingsData.page.value == 'settings-tema'">
        <SettingsTema></SettingsTema>
      </div>
      <div v-if="settingsData.page.value == 'settings-update'">
        <SettingsUpdate></SettingsUpdate>
      </div>
    </div>
  </div>


</template>

<style scoped>

</style>