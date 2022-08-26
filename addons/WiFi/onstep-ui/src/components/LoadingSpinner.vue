<template lang="pug">
.flex.flex-col.text-white
  .flex.item-center.justify-center.space-x-2
    .spinner-border.animate-spin.inline-block.w-20.h-20.border-4.rounded-full.border-white

  .flex.justify-center.mt-8(v-if="$slots.default")
    slot
</template>

<style scoped>
.spinner-border {
  vertical-align: -0.125em;
  border: 0.1em solid;
  border-right-color: transparent;
}
</style>
