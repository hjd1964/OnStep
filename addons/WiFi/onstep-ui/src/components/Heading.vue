<template lang="pug">
h1.flex.justify-center.text-xl.text-white.mb-4
  .flex.flex-col.text-center.flex-start
    .flex.w-full.justify-center.mb-1
      slot

    small.text-xs(v-if="$slots.subheading")
      slot(name="subheading")
  
</template>
