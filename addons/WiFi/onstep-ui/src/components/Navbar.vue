<template lang="pug">
disclosure.bg-gray-800.sticky.top-0.z-50(as="nav", v-slot="{ open }")
  .max-w-7xl.mx-auto.px-2.sm_px-6.lg_px-8
    .relative.flex.items-center.justify-between.h-16
      .absolute.inset-y-0.left-0.flex.items-center.sm_hidden
        disclosure-button.inline-flex.items-center.justify-center.p-2.rounded-md.text-gray-400.hover_text-white.hover_text-white.hover_bg-gray-700.focus_outline-none.focus_ring-0.focus_ring-inset.focus_ring-0
          template(v-if="!open")
            Bars3Icon.block.h-6.w-6
            template(v-if="status.dateTime.datesAreOutOfSync")
              ExclamationTriangleIcon.h-5.w-5.text-rose-400.ml-2

            template(v-if="status.status.waitingAtHome")
              ClockIcon.h-5.w-5.text-rose-400.ml-2

          XMarkIcon.block.h-6.w-6(v-else)

      .flex-1.flex.items-center.justify-center.sm_items-stretch.sm_justify-start
        .flex-shrink-0.flex.items-center
          .w-4
          img.block.h-8.w-auto.lg_hidden(:src="logo")
          img.hidden.h-8.w-auto.lg_block(:src="logo")
          small.text-white.flex.flex-col.justify-center.w-4.h-8.ml-2
            ArrowTrendingUpIcon.h-4.w-4.text-rose-300(v-if="status.status.slewing && !status.status.waitingAtHome")

        .hidden.sm_block.sm_ml-6
          .flex.space-x-4
            template(
              v-for="item in navigation"
            )
              router-link(
                :custom="true"
                v-slot="{ isActive, navigate }"
                :key="item.name"
                :to="{ name: item.route }", 
                v-if="!item.onlyMenu"
              ) 
                a.cursor-pointer.px-3.py-2.rounded-md.text-sm.font-medium.whitespace-nowrap.flex(
                  @click="navigate"
                  :class="[isActive ? 'bg-gray-900 text-white' : 'text-gray-300 hover_bg-gray-700 hover_text-white', '']"
                  :aria-current="isActive ? 'page' : undefined"
                ) {{ item.name }}
                  template(v-if="item.route === 'control'")
                    ClockIcon.h-5.h-5.text-rose-400.ml-2(
                      v-if="status.status.waitingAtHome"
                    )

                  template(v-if="item.route === 'datetime'")
                    ExclamationTriangleIcon.h-5.h-5.text-rose-400.ml-2(
                      v-if="status.dateTime.datesAreOutOfSync"
                    )


      .absolute.inset-y-0.right-0.flex.items-center.pr-2.text-white.text-lg.sm_static.sm_inset-auto.sm_ml-6.sm_pr-0
        .flex.space-x-4
          h1.flex.flex-col.justify-center.invisible.md_visible SmartWebController
            small.text-xs.flex.flex-row-reverse
              template(v-if="status.version") {{ version }}
              template(v-else) Loading..

          a.flex.flex-col.justify-center
            .flex.space-x-2
              .flex.flex-col.justify-center.hidden.md_flex
                Menu
                  template(#button)
                    Cog6ToothIcon.h-8.h-8.text-white

              .flex.flex-col.justify-center
                ExclamationCircleIcon.h-10.w-10.text-red-300.hover_text-red-600.cursor-pointer.transition(
                  @click="() => control.estop()"
                )

  disclosure-panel.sm_hidden(
    v-slot="{ close }"
  )
    .px-2.pt-2.pb-3.space-y-1
      router-link(
        v-for="item in navigation"
        :key="item.name"
        :to="{ name: item.route }"
        :custom="true"
        v-slot="{ isActive, navigate, href }"
      ) 
        a.block.px-3.py-2.rounded-md.text-base.font-medium.transition.flex(
          @click="()=> { close(null); navigate(); }"
          :href="href"
          :class="[isActive ? 'bg-gray-900 text-white' : 'text-gray-300 hover_bg-gray-700 hover_text-white']"
        ) {{ item.longName ?? item.name }}
          template(v-if="item.route === 'datetime'")
            ExclamationTriangleIcon.h-5.h-5.text-rose-400.ml-2(v-if="status.dateTime.datesAreOutOfSync")

          template(v-if="item.route === 'control'")
            ClockIcon.h-5.h-5.text-rose-400.ml-2(v-if="status.status.waitingAtHome")

      a.block.px-3.py-2.rounded-md.text-base.font-medium.transition.flex(
        class="text-gray-300 hover_bg-gray-700 hover_text-white"
        @click="()=> { close(null); }"
        href="https://onstep.groups.io/g/main/wiki/5650"
      ) Help
 

      hr
      .text-white.text-center.w-full.pt-2.my-2 {{ version }}


</template>

<script setup lang="ts">
import { computed } from "vue";
import { Disclosure, DisclosureButton, DisclosurePanel } from "@headlessui/vue";
import {
  ClockIcon,
  ExclamationTriangleIcon,
  XMarkIcon,
  Cog6ToothIcon,
  Bars3Icon,
  ArrowTrendingUpIcon,
} from "@heroicons/vue/24/outline";
import { ExclamationCircleIcon } from "@heroicons/vue/24/solid";
import { RouterLink } from "vue-router";
import type { ValidMountStatus } from "../types";
import logo from "../assets/onstep.png";
import { MountStatusApi } from "../api/control";
import Menu from "./Menu.vue";

const props = defineProps<{
  status: ValidMountStatus;
  control: MountStatusApi;
}>();

const navigation: {
  name: string;
  route: string;
  onlyMenu?: boolean;
  longName?: string;
}[] = [
  { name: "Control", route: "control" },
  { name: "Library", route: "library", longName: "Library & GoTo" },
  { name: "Datetime & Location", route: "datetime" },
  { name: "Terminal", route: "terminal", onlyMenu: true },
  { name: "WiFi", route: "wifi", onlyMenu: true },
  { name: "Mount Settings", route: "mountSettings", onlyMenu: true },
];

const version = computed(
  () =>
    `OnStep: ${props.status.version.onstep} / Wifi: ${props.status.version.wifi}`
);
</script>
