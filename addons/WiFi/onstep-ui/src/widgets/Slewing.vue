<template lang="pug">
Widget(width="full")
  template(#heading) Guiding
  template(#subheading) 
    p Use these tools to manually slew your scope (must be tracking)
    p Use the center button to sync on a star if you've adjusted the location.

  .flex.justify-center
    .grid.grid-cols-3.gap-4.select-none.mb-8.w-full(class="md_w-1/2 xl_w-1/3")
      div
      ControlButton(
        @mousedown="slew('n')"
        @mouseup="stop('n')"
        :disabled="!canSlew"
      ) N
      div

      ControlButton(
        @mousedown="slew('e')"
        @mouseup="stop('e')"
        :disabled="!canSlew"
      ) E
      ControlButton(
        @click="() => control.syncWith()"
      ) 
        ArrowPathIcon.w-5.h-5
      ControlButton(
        @mousedown="slew('w')"
        @mouseup="stop('w')"
        :disabled="!canSlew"
      ) W

      div
      ControlButton(
        @mousedown="slew('s')"
        @mouseup="stop('s')"
        :disabled="!canSlew"
      ) S
      div

  Heading Speed
    template(#subheading) More speed options are available in the menu -> Mount Settings.
  .flex.justify-center.space-x-2
    Toggles(
      :options="speedOptions"
      :value="status.slewing.speed"
      :onChange="speedChange"
    )


</template>

<script setup lang="ts">
import Widget from "../components/Widget.vue";
import Heading from "../components/Heading.vue";
import ControlButton from "../components/ControlButton.vue";
import { ArrowPathIcon } from "@heroicons/vue/24/outline";
import Toggles from "../components/Toggles.vue";
import { type Direction, type ValidMountStatus } from "../types";
import { MountStatusApi } from "../api/control";
import { computed } from "vue";

const props = defineProps<{
  control: MountStatusApi;
  status: ValidMountStatus;
}>();

const canSlew = computed(() => {
  return props.status.status.tracking || props.status.status.aligning;
});

const speedOptions = ["V. Slow", "Slow", "Normal", "Fast", "V. Fast"].map(
  (opt, i) => ({
    title: opt,
    value: [0, 2, 4, 6, 8][i],
  })
);

function slew(dir: Direction) {
  return props.control.slew(dir, true);
}
function stop(dir: Direction) {
  return props.control.slew(dir, false);
}

function speedChange(speed: number) {
  return props.control.changeSpeed(speed);
}
</script>
