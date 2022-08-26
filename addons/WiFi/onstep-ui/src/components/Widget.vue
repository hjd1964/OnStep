<template lang="pug">
.border-red-400.border-2.rounded-md.p-4.flex.justify-center(
  :class="widthClasses"
)
  .flex.flex-col.w-full.text-white
    Heading
      slot(name="heading")

      template(#subheading)
        slot(name="subheading")

    .flex.flex-col.grow.space-y-4.mx-0.xl_mx-10
      slot
</template>

<script setup lang="ts">
import Heading from "./Heading.vue";
import { computed } from "vue";

const props = withDefaults(
  defineProps<{
    width?: "full" | "half";
  }>(),
  {
    width: () => "half",
  }
);
const widthClasses = computed(() => {
  return props.width === "full" ? "w-full" : "w-full lg_w-1/2";
});
</script>
