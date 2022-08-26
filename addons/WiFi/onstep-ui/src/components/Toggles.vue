<template lang="pug">
span.relative.z-0.inline-flex.shadow-sm.rounded-md
  ControlButton(
    v-for="(option, i) in options"
    @click="() => change(option.value)"
    :square="true"
    :disabled="disabled"
    :active="value === option.value"
    :loading="loading && loadingValue === option.value"
    :class=`[
      i === 0 
        ? 'rounded-l-md border-r-1 border-y-0 border-l-0 border-red-300'
        : i === options.length - 1
          ? 'rounded-r-md border-l-1 border-y-0 border-r-0 border-red-300'
          : 'border-l-1 border-r-1 border-y-0 border-red-300'
    ]`
  ) 
    component.h-5.w-5(
      :is="option.icon"
      v-if="option.icon"
    )

    template(v-else) {{ option.title }}
</template>

<script setup lang="ts">
import { type FunctionalComponent, ref } from "vue";
import ControlButton from "./ControlButton.vue";

const loadingValue = ref<string | null | number>(null);
const loading = ref(false);

const props = defineProps<{
  onChange: (v: any) => Promise<void>;
  value: string | number;
  disabled?: boolean;
  options: {
    title?: string;
    icon?: FunctionalComponent;
    value: string | number;
  }[];
}>();

async function change(v: string | number) {
  try {
    loading.value = true;
    loadingValue.value = v;
    await props.onChange(v);
  } finally {
    loading.value = false;
    loadingValue.value = null;
  }
}
</script>
