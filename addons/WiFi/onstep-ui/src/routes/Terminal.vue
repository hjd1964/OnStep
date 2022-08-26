<template lang="pug">
Columns
  Widget(width="full")
    template(#heading) Send commands to the mount
    .flex.w-full
      InputField.w-full(
        v-model="cmd"
        label="Enter a command to get started."
        @keypress.enter="send"
        @keyup.up="showHistory('up')"
        @keyup.down="showHistory('down')"
      )

        template(#right)
          ControlButton(
            @click="send"
            :disabled="!cmd"
            :loading="loading"
          ) send

    .flex.mt-3.flex-col
      template(v-if="commands.length")
        .text-white.text-sm.mb-2(
          v-for="cmd in commands"
        )
          strong > {{ cmd.command }}
          small.block.ml-4 {{ cmd.response || '[none]' }}
      template(v-else)
        .text-white.text-sm No command history yet.


</template>

<script setup lang="ts">
import { computed, ref } from "vue";
import InputField from "../components/InputField.vue";
import { MountStatusApi } from "../api/control";
import ControlButton from "../components/ControlButton.vue";
import Widget from "../components/Widget.vue";
import Columns from "../components/Columns.vue";

const props = defineProps<{
  control: MountStatusApi;
}>();

const cmd = ref("");
const loading = ref(false);
const upIndex = ref(0);

async function send() {
  if (["cls", "clear"].includes(cmd.value)) {
    props.control.commandLogs = [];
  }

  try {
    loading.value = true;
    await props.control.sendCommand(cmd.value.trim(), true);
    cmd.value = "";
  } finally {
    loading.value = false;
  }
}

const commands = computed(() => {
  if (loading.value) {
    return [
      {
        command: "Loading.....",
        response: null,
      },
      ...props.control.commandLogs,
    ];
  }

  return props.control.commandLogs;
});

function showHistory(direction: "up" | "down") {
  if (direction === "down" && upIndex.value === 0) {
    cmd.value = "";
    return;
  }

  upIndex.value = Math.max(
    0,
    Math.min(
      direction === "up" ? upIndex.value + 1 : upIndex.value - 1,
      props.control.commandLogs.length - 1
    )
  );

  cmd.value = props.control.commandLogs[upIndex.value]?.command;
}
</script>
