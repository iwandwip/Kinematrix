<script setup lang="ts">
import {useColorMode} from '@vueuse/core'
import {Icon} from '@iconify/vue'
const mode = useColorMode()
</script>

<template>
  <Button variant="outline" v-if="mode == 'light'" @click="mode = 'dark'">
    <Icon icon="radix-icons:moon"
          class="h-[1.2rem] w-[1.2rem] rotate-0 scale-100 transition-all dark:-rotate-90 dark:scale-0"/>
  </Button>
  <Button variant="outline" v-if="mode == 'dark'" @click="mode = 'light'">
    <Icon icon="radix-icons:sun"
          class=" h-[1.2rem] w-[1.2rem] rotate-90 scale-0 transition-all dark:rotate-0 dark:scale-100"/>
  </Button>
</template>

<style scoped>

</style>