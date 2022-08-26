<template lang="pug">
SwitchGroup
  .flex.items-center

  Switch.relative.inline-flex.h-9.w-16.items-center.rounded-full(
    :modelValue="modelValue"
    @update:modelValue="v => changeValue(v)"
    :class=`loading ? 'bg-rose-300' : modelValue ? 'bg-rose-500' : 'bg-rose-200'`
  )
    span.inline-block.h-7.w-7.transform.rounded-full.transition(
      :class=`[
        loading 
          ? modelValue ? 'translate-x-4 bg-rose-400' : 'translate-x-5 bg-rose-400'
          : modelValue 
            ? 'translate-x-8 bg-rose-200'
            : 'translate-x-1 bg-rose-700'
      ]`
    )
      SmallSpinner.ml-1.mt-1(v-if="loading")
</template>

<script setup lang="ts">
import { Switch, SwitchGroup } from "@headlessui/vue";
import { ref } from "vue";
import SmallSpinner from "./SmallSpinner.vue";

const emits = defineEmits<{
  (e: "update:modelValue", v: boolean): void;
}>();

const props = defineProps<{
  modelValue: boolean;
  onChange?: (v: boolean) => Promise<void>;
}>();

async function changeValue(v: boolean) {
  loading.value = true;
  try {
    if (!props.onChange) {
      emits("update:modelValue", v);
    } else {
      await props.onChange?.(v);
    }
  } finally {
    loading.value = false;
  }
}

const loading = ref(false);
</script>
