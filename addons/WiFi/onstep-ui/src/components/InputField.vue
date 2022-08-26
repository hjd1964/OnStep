<template lang="pug">
div
  label.block.text-sm.font-medium.text-white.mb-2
    slot(name="label") {{ label }}
    .text-rose-500(v-if="valid.result !== 'valid'") {{ valid.message }}

  .mt-1.flex
    input.shadow-sm.focus_ring-0.focus_border-rose-0.block.w-full.sm_text-sm.rounded-md.placeholder_text-rose-400.bg-rose-200.text-rose-700.disabled_opacity-75.disabled_cursor-not-allowed.disabled_text-gray-500(
      v-model="value"
      :min="min"
      :class=`[
        valid.result !== 'valid' 
          ? 'border-rose-700'
          : 'border-rose-300'
      ]`
      autocomplete="off"
      :max="max"
      :maxlength="maxLength"
      @keypress.enter="emits('submit')"
      :type="type ?? 'text'"
      :disabled="disabled"
      :placeholder="placeholder"
    )
    slot(name="right")

</template>

<script setup lang="ts">
import { computed } from "vue";

type ValidationRule = [string, RegExp];

const props = defineProps<{
  modelValue: string;
  label?: string;
  placeholder?: string;
  type?: string;
  min?: number;
  max?: number;
  maxLength?: number;
  validate?: Readonly<ValidationRule>[];
  disabled?: boolean;
}>();

const emits = defineEmits<{
  (e: "update:modelValue", v: string): void;
  (e: "submit"): void;
}>();

const valid = computed(() => {
  if (!props.validate) {
    return { result: "valid" };
  }

  for (const [message, rule] of props.validate) {
    if (!rule.test(props.modelValue)) {
      return {
        result: "invalid",
        message,
      };
    }
  }

  return { result: "valid" };
});

const value = computed({
  get() {
    return props.modelValue;
  },
  set(v: string) {
    emits("update:modelValue", String(v));
  },
});
</script>
