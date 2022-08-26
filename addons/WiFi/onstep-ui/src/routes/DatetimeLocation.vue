<template lang="pug">
Columns
  Widget
    template(#heading) Date/Time
      ExclamationTriangleIcon.h-6.h-6.text-rose-400.ml-2(v-if="dateHasDrifted")
    template(#subheading v-if="dateHasDrifted") 
      | The current browser time we detected is far off from the current mount time.
      | Unless your browser doens't have access to the current time, you should click
      | "sync time" below.

    .flex.flex-col
      .w-full
        .flex
          label.grow.font-bold UTC Offset
          div {{ status.dateTime.utcOffset }}

        .flex
          label.grow.font-bold UTC time
          div {{ formatDate(status.dateTime.utc, true) }}

        .flex
          label.grow.font-bold Browser time 
          div {{ formatDate(status.dateTime.browser, false) }}


      ControlButton.w-full.my-4(
        :loading="syncingTime"
        @click="sync"
      ) Sync time with Browser

    hr.text-rose-500.my-5
    Heading Set a manual timezone offset
      template(#subheading)
        .flex.flex-col.text-center
          | UTC Offset, in hours and min. -14 to +12
          | Opposite of a time-zone value, this is for Standard Time (not Daylight Time.)

    .flex.mt-4
      InputField.grow(
        label="Change timezone offset manually (enter hours, for example, -6.5)"
        type="number"
        :min="-14"
        :max="12"
        v-model="manualTimezone"
      )
        template(#right)
          ControlButton.ml-2(
            :loading="syncingTime"
            :disabled="!manualTimezone"
            @click="setManualTimezone"
          ) Set

  Widget
    template(#heading) Site Location

    .flex.flex-col.space-y-8(v-if="currentLocation")
      .flex.flex-col
        Heading Longitude
          template(#subheading)
            | Longitude is deg, min, sec. +/- 180, W is +

        .flex.space-x-4.mt-2
          InputField.grow(
            type="number"
            :max="180"
            :min="-180"
            v-model="currentLocation.long.deg" label="Degrees")
          InputField.grow(
            :max="60"
            :min="0"
            type="number"
            v-model="currentLocation.long.min" label="Minutes")
          InputField.grow(
            type="number"
            :max="60"
            :min="0"
            v-model="currentLocation.long.sec" label="Seconds")

      hr.text-rose-500
      .flex.flex-col
        Heading Latitude
          template(#subheading) Latitude is deg. min, sec. +/- 90, N is +
        .flex.space-x-4.mt-2
          InputField.grow(
            type="number"
            :max="90"
            :min="-90"
            v-model="currentLocation.lat.deg" label="Degrees")
          InputField.grow(
            :max="60"
            :min="0"
            type="number"
            v-model="currentLocation.lat.min" label="Minutes")
          InputField.grow(
            type="number"
            :max="60"
            :min="0"
            v-model="currentLocation.lat.sec" label="Seconds")


      ControlButton.w-full.mt-8(
        :loading="savingLocation"
        :disabled="!locationChanged"
        @click="saveLocation"
      ) Save Location




</template>

<script setup lang="ts">
import { computed, onMounted, ref, toRaw } from "vue";
import { MountStatusApi } from "../api/control";
import ControlButton from "../components/ControlButton.vue";
import type { ValidMountStatus } from "../types";
import { formatDate } from "../utils/formatDate";
import Widget from "../components/Widget.vue";
import { ExclamationTriangleIcon } from "@heroicons/vue/24/outline";
import InputField from "../components/InputField.vue";
import { objectsEqual } from "../utils/compareObjects";
import Columns from "../components/Columns.vue";
import Heading from "../components/Heading.vue";

const props = defineProps<{
  control: MountStatusApi;
  status: ValidMountStatus;
}>();

const syncingTime = ref(false);
const savingLocation = ref(false);
const currentLocation = ref<ValidMountStatus["location"]>(null!);
const manualTimezone = ref("");

const locationChanged = computed(
  () => !objectsEqual(props.status.location, currentLocation.value)
);

const dateHasDrifted = computed(() => {
  return props.status.dateTime.datesAreOutOfSync;
});

async function saveLocation() {
  savingLocation.value = true;
  try {
    await props.control.setLocation(currentLocation.value);
  } finally {
    savingLocation.value = false;
  }
}

async function setManualTimezone() {
  if (manualTimezone.value) {
    await sync(manualTimezone.value);
    manualTimezone.value = "";
  }
}

async function sync(offset?: string) {
  syncingTime.value = true;

  try {
    await props.control.setDateTime(offset);
  } finally {
    syncingTime.value = false;
  }
}

onMounted(() => {
  currentLocation.value = toRaw(props.status.location);
});
</script>
