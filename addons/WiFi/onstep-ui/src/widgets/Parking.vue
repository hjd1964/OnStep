<template lang="pug">
Widget(width="half")
  template(#heading) {{ title }}

  Modal(ref="confirm")
    template(#title) Are you sure?
    template(#body)
      p This will save the current telescope location as the new park location.
      p This cannot be undone.

  .flex.justify-center.mb-6
    Toggle(
      :modelValue="status.status.parked"
      :onChange="setParking"
    )

  .flex.justify-center
    ControlButton(
      @click="setParkLocation"
      :disabled="status.status.parked"
      :loading="settingParkLocation"
    ) Set park location
</template>

<script setup lang="ts">
import { computed, ref } from "vue";
import Widget from "../components/Widget.vue";
import Toggle from "../components/Toggle.vue";
import { type ValidMountStatus } from "../types";
import { MountStatusApi } from "../api/control";
import ControlButton from "../components/ControlButton.vue";
import Modal from "../components/Modal.vue";

const props = defineProps<{
  control: MountStatusApi;
  status: ValidMountStatus;
}>();

const settingParkLocation = ref(false);
const confirm = ref<InstanceType<typeof Modal> | null>(null);

function setParking(v: boolean) {
  return v ? props.control.park() : props.control.unPark();
}

const title = computed(() =>
  props.status.status.parked ? "Parked" : "Not Parked"
);

async function setParkLocation() {
  if (!(await confirm.value?.awaitAnswer())) {
    return;
  }

  try {
    settingParkLocation.value = true;
    await props.control.setParkingLocation();
  } finally {
    settingParkLocation.value = false;
  }
}
</script>
