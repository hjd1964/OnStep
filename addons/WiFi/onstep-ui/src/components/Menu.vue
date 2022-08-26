<template lang="pug">
Menu.relative.inline-block.text-left(
  as="div"
)
  .flex.flex-col.justify-center
    MenuButton
      slot(name="button")

  transition(
    enter-active-class="transition ease-out duration-100" 
    enter-from-class="transform opacity-0 scale-95" 
    enter-to-class="transform opacity-100 scale-100" 
    leave-active-class="transition ease-in duration-75" 
    leave-from-class="transform opacity-100 scale-100" 
    leave-to-class="transform opacity-0 scale-95"
  )
    MenuItems.origin-top-right.absolute.right-0.mt-2.w-56.rounded-md.shadow-lg.bg-white.ring-1.ring-black.ring-opacity-5.focus_outline-none.py-1
      RouterLink(
        :to="{ name: 'wifi' }"
        custom
        v-slot="{ href, navigate }"
      )
        MenuItem.text-sm.py-2.px-3.text-right(
          :href="href" @click="navigate"
          v-slot="{ active }"
        ) 
          .flex.space-x-5.cursor-pointer(
            :class="[ active ? 'bg-rose-500 text-white' : 'text-gray-700']"
          )
            WifiIcon.h-5.w-5
            a WiFi Settings

      RouterLink(
        :to="{ name: 'settings'}"
        custom
        v-slot="{ href, navigate }"
      )
        MenuItem.text-sm.py-2.px-3.text-right(
          :href="href" @click="navigate"
          v-slot="{ active }"
        )
          .flex.space-x-5.cursor-pointer(
            :class="[ active ? 'bg-rose-500 text-white' : 'text-gray-700']"
          )
            GlobeAmericasIcon.h-5.w-5
            a Mount Settings

      RouterLink(
        :to="{ name: 'terminal' }"
        custom
        v-slot="{ href, navigate }"
      )
        MenuItem.text-sm.py-2.px-3.text-right(
          :href="href" @click="navigate"
          v-slot="{ active }"
        )
          .flex.space-x-5.cursor-pointer(
            :class="[ active ? 'bg-rose-500 text-white' : 'text-gray-700']"
          )
            CommandLineIcon.h-5.w-5
            a Terminal

      MenuItem.text-sm.py-2.px-3.text-right(
        v-slot="{ active }"
      )
        a.flex.space-x-5.cursor-pointer(
          :class="[ active ? 'bg-rose-500 text-white' : 'text-gray-700']"
          href="https://onstep.groups.io/g/main/wiki/5650"
          target="_blank"
          rel="noopener noreferrer"
        )
          QuestionMarkCircleIcon.h-5.w-5.mr-5
          | Help




</template>

<script setup lang="ts">
import {
  WifiIcon,
  GlobeAmericasIcon,
  CommandLineIcon,
  QuestionMarkCircleIcon,
} from "@heroicons/vue/24/outline";
import { Menu, MenuButton, MenuItem, MenuItems } from "@headlessui/vue";
</script>
