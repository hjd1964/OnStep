<template lang="pug">
Widget(width="half")
  template(#heading) {{ title }}

  .flex.justify-center.mb-6
    Toggle(
      v-model="status.status.tracking"
      :onChange="setTracking"
    )

  .flex.justify-center
    Toggles(
      :options="trackTypeOptions"
      :onChange="v => changeSpeed(v)"
      :value="status.tracking.type"
    )
</template>

<script setup lang="ts">
import { computed } from "vue";
import Widget from "../components/Widget.vue";
import Toggle from "../components/Toggle.vue";
import { StarIcon, MoonIcon, SunIcon } from "@heroicons/vue/24/outline";
import { type ValidMountStatus, type TrackingModes } from "../types";
import { MountStatusApi } from "../api/control";
import Toggles from "../components/Toggles.vue";

const props = defineProps<{
  control: MountStatusApi;
  status: ValidMountStatus;
}>();

function setTracking(v: boolean) {
  return v ? props.control.startTracking() : props.control.stopTracking();
}

const trackTypeOptions = [
  { icon: StarIcon, value: "sidereal" as const },
  { icon: MoonIcon, value: "lunar" as const },
  { icon: SunIcon, value: "solar" as const },
];

async function changeSpeed(v: TrackingModes) {
  return props.control.changeTrackingType(v);
}

const title = computed(() =>
  props.status.status.tracking ? "Tracking" : "Not Tracking"
);
</script>
