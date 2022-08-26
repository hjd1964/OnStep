<script setup lang="ts">
import { ref, onBeforeUnmount } from "vue";
import { api } from "./api/api";
import { MountStatusApi } from "./api/control";
import { type MountStatus } from "./types";
import Navbar from "./components/Navbar.vue";
import LoadingSpinner from "./components/LoadingSpinner.vue";
import FullScreen from "./components/FullScreen.vue";
import { ExclamationCircleIcon } from "@heroicons/vue/24/outline";

const status = ref<null | MountStatus>(null);

const loading = ref(true);

const control = new MountStatusApi(api).startHeartbeat(
  (s) => (status.value = s),
  (l) => (loading.value = l)
);

onBeforeUnmount(() => control.stopHeartbeat());
</script>

<template lang="pug">
FullScreen(v-if="!status")
  LoadingSpinner
    | connecting..

FullScreen(v-else-if="status.type === 'invalid'")
  .flex.justify-center
    .h-20.w-20.text-red-300.hover_text-red-600.cursor-pointer.transition
      ExclamationCircleIcon

  .flex.justify-center.text-white
    .mt-8 There was an issue connecting to your mount.

.min-h-screen.flex.flex-col(v-else-if="status")
  Navbar.mb-5(
    :version="status.version"
    :control="control"
    :status="status"
  )
  RouterView.mx-2.2xl_mx-60.xl_mx-40.md_mx-20.sm_mx-10.grow(
    v-slot="{ Component }"
  )
    transition(name="fade")
      component(
        :status="status"
        :control="control"
        :is="Component"
      )

</template>

<style scoped></style>
