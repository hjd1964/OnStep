<template lang="pug">
Columns
  Widget
    template(#heading) General Settings

    .text-lg.text-center Max Slewing Rate
    .flex.justify-center.mt-2
      Toggles(
        :options="maxSlewOptions"
        :value="status.slewing.maxSpeed"
        :onChange="changeSlewingSpeed"
      )


    template(v-if="mountHasCompensation(mount)")
      hr.text-rose-500.my-5
      .flex.flex-col.mt-2
        .text-lg.text-center Compensated Tracking Rate 
        small.text-center Pointing Model/Refraction

        .flex.flex-col.items-center.mt-2
          Toggles.mb-4(
            :options="trackingRateCompOptions"
            :value="mount.rate_comp_type"
            :onChange="v => mountHasCompensation(mount) ? control.setTrackingRateComp(v, mount.rate_comp_axes): invalidMount()"
          )

          Toggles(
            :options=`[{ title: 'Dual axes', value: 'dual' }, { title: 'Single axis', value: 'single' }]`
            :value="mount.rate_comp_axes"
            :disabled="mount.rate_comp_type === 'none'"
            :onChange="v => mountHasCompensation(mount) ? control.setTrackingRateComp(mount.rate_comp_type, v) : invalidMount()"
          )

    hr.text-rose-500.my-5
    .text-lg.text-center Tracking Rate
    .flex.space-x-2.justify-center.mt-2
      ControlButton(
        @click="() => adjustTrackingRate('faster')"
        :loading="adjustingTrackingRate"
      ) + (0.02Hz faster)
      ControlButton(
        @click="() => adjustTrackingRate('reset')"
        :loading="adjustingTrackingRate"
      ) Reset
      ControlButton(
        @click="() => adjustTrackingRate('slower')"
        :loading="adjustingTrackingRate"
      ) - (0.02Hz slower)

    hr.text-rose-500.my-5

    .flex
      .text-lg.grow.flex.flex-col.justify-center Go/To Buzzer:
      Toggle(
        :modelValue="status.features.buzzer"
        :onChange="v => control.setBuzzer(v)"
      )

    template(v-if="status.mount.type === 'gem'")
      .flex.mt-2
        .text-lg.grow.flex.flex-col.justify-center Automatic Meridian Flip at Limit:
        ControlButton.mr-4(
          @click="setMeridianAutoFlipNow"
          :loading="settingMeridianFlipNow"
        ) Now
        .flex.flex-col.justify-center
          Toggle(
            :modelValue="status.mount.auto_meridian"
            :onChange="v => control.setMeridianAutoFlip(v)"
          )

      .flex.mt-2
        .text-lg.grow.flex.flex-col.justify-center Meridian Flip, Pause at Home:
        .flex.justify-center
          Toggle(
            :modelValue="status.mount.pause_at_home"
            :onChange="v => control.setMeridianPauseAtHome(v)"
          )


  Widget
    template(#heading) Limits & Mechanical

    template(v-if="mutableMount")
      .text-lg.text-center Horizon & Overhead Limits
      InputField.mt-2(
        label="Horizon, minimum altitude in degrees +/- 30"
        type="number"
        :min="-30"
        :max="90"
        v-model="mutableMount.min_alt"
      )
      InputField.mt-2(
        label="Overhead, maximum altitude in degrees 60 to 90"
        type="number"
        :min="60"
        :max="90"
        v-model="mutableMount.max_alt"
      )

      hr.text-rose-500.my-5
      .text-lg.text-center Axis1 RA/Azm
      InputField.mt-2(
        label="Backlash, in arc-seconds 0 to 3600"
        type="number"
        v-model="mutableMount.backlash_ra"
        :min="0"
        :max="3600"
      )

      template(v-if="mutableMount.type === 'gem'")
        InputField.mt-2(
          label="Overhead, maximum altitude in degrees 60 to 90"
          type="number"
          :min="-270"
          :max="270"
          v-model="mutableMount.deg_past_mer_e"
        )

        InputField.mt-2(
          label="Overhead, maximum altitude in degrees 60 to 90"
          type="number"
          :min="-270"
          :max="270"
          v-model="mutableMount.deg_past_mer_w"
        )

      hr.text-rose-500.my-5
      .text-lg.text-center Axis2 Dec/Alt
      InputField.mt-2(
        label="Backlash, in arc-seconds 0 to 3600"
        type="number"
        :min="0"
        :max="3600"
        v-model="mutableMount.backlash_dec"
      )

      ControlButton.w-full.mt-8(
        :loading="savingMountSettings"
        :disabled="!mountSettingsChanged"
        @click="saveMountSettings"
      ) Save Settings



</template>

<script setup lang="ts">
import { ref, computed, onMounted, toRaw } from "vue";
import { MountStatusApi } from "../api/control";
import {
  type MaxSlewSpeed,
  type ValidMountStatus,
  mountHasCompensation,
} from "../types";
import Widget from "../components/Widget.vue";
import Toggles from "../components/Toggles.vue";
import Toggle from "../components/Toggle.vue";
import ControlButton from "../components/ControlButton.vue";
import InputField from "../components/InputField.vue";
import { objectsEqual } from "../utils/compareObjects";
import Columns from "../components/Columns.vue";

const props = defineProps<{
  control: MountStatusApi;
  status: ValidMountStatus;
}>();

const mount = computed(() => props.status.mount);
const mutableMount = ref<ValidMountStatus["mount"] | null>(null);
const savingMountSettings = ref(false);
const adjustingTrackingRate = ref(false);
const settingMeridianFlipNow = ref(false);

const mountSettingsChanged = computed(() => {
  if (!mutableMount.value) {
    return false;
  }

  return !objectsEqual(mutableMount.value, props.status.mount);
});

const trackingRateCompOptions = ref([
  {
    title: "Full",
    value: "full",
  },
  {
    title: "Refraction Only",
    value: "refraction_only",
  },
  {
    title: "Off",
    value: "none",
  },
]);

const maxSlewOptions = ref([
  {
    title: "2x",
    value: "vf",
  },
  {
    title: "1.5x",
    value: "f",
  },
  {
    title: "1x",
    value: "n",
  },
  {
    title: "0.75x",
    value: "s",
  },
  {
    title: "0.5x",
    value: "vs",
  },
]);

onMounted(() => {
  mutableMount.value = toRaw(props.status.mount);
});

async function saveMountSettings() {
  if (!mutableMount.value) {
    return;
  }
  savingMountSettings.value = true;
  try {
    await props.control.updateMountSettings(mutableMount.value);
  } finally {
    savingMountSettings.value = false;
  }
}

function invalidMount() {
  return Promise.reject("foo");
}

function changeSlewingSpeed(speed: MaxSlewSpeed) {
  return props.control.changeMaxSlewSpeed(speed);
}

async function adjustTrackingRate(rate: "faster" | "slower" | "reset") {
  adjustingTrackingRate.value = true;
  try {
    await props.control.adjustTrackingRate(rate);
  } finally {
    adjustingTrackingRate.value = false;
  }
}

async function setMeridianAutoFlipNow() {
  settingMeridianFlipNow.value = true;
  try {
    await props.control.setMeridianAutoFlipNow();
  } finally {
    settingMeridianFlipNow.value = false;
  }
}
</script>
