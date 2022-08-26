<template lang="pug">
button.text-center.inline-flex.items-center.text-center.justify-center.px-3.py-2.border.border-transparent.text-base.font-medium.shadow-sm.text-white.bg-rose-400.hover_bg-rose-600.focus_outline-none.transition.place-content-center.relative(
  @click="click"
  @mousedown="e => down(e)"
  @mouseup="e => up(e)"
  @touchstart="e => down(e)"
  @touchend="e => up(e)"
  @touchcancel="e => up(e)"
  :class=`[
    square ? '' : 'rounded-md',
    active && !mouseIsDown ? 'bg-rose-300 border-rose-400' : '',
    !active && mouseIsDown ? 'bg-rose-700 border-rose-400' : '',
    disabled ? 'cursor-not-allowed bg-rose-200 text-rose-300' : ''
  ]`
)
  .absolute.inset-0.flex.justify-center(v-show="loading")
    .flex.flex-col.justify-center
      SmallSpinner
  .flex.items-center.text-center.justify-center(
    :class="{ 'text-transparent': loading }"
  )
    slot

</template>

<script setup lang="ts">
import SmallSpinner from "./SmallSpinner.vue";
import { ref } from "vue";

const props = defineProps<{
  square?: boolean;
  active?: boolean;
  disabled?: boolean;
  loading?: boolean;
}>();

const emits = defineEmits<{
  (e: "click"): void;
  (e: "mousedown"): void;
  (e: "mouseup"): void;
}>();

function click() {
  if (!props.disabled) {
    emits("click");
  }
}

const mouseIsDown = ref(false);

function down(e: unknown) {
  if (mouseIsDown.value) {
    return;
  }

  mouseIsDown.value = true;
  emits("mousedown");
}

function up(e: unknown) {
  mouseIsDown.value = false;
  emits("mouseup");
}
</script>
