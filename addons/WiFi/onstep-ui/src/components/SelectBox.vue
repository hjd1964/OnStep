<template lang="pug">
Listbox(as='div' v-model="selected")
  ListboxLabel.block.text-sm.font-medium.text-white
    slot(name="label")

  .mt-1.relative

    ListboxButton.relative.w-full.bg-rose-200.border.border-rose-300.rounded-md.shadow-sm.pl-3.pr-10.py-2.text-left.cursor-default.focus_outline-none.focus_ring-1.focus_ring-rose-500.focus_border-rose-500.sm_text-sm
      span.flex.items-center
        span(
          :class="[selected?.checked ? 'bg-rose-700' : 'bg-rose-400', 'flex-shrink-0 inline-block h-2 w-2 rounded-full']"
        )
        span.ml-3.block.truncate.text-rose-400.flex.w-full
          .grow {{ selected?.name ?? "Select an option" }}
          LockClosedIcon.h-5.w-5.text-rose-400(v-if="selected?.icon === 'lock'")
          LockOpenIcon.h-5.w-5.text-rose-500(v-if="selected?.icon === 'unlock'")
 

      span.absolute.inset-y-0.right-0.flex.items-center.pr-2.pointer-events-none
        ChevronUpDownIcon.h-5.w-5.text-rose-400

    transition(
      leave-active-class='transition ease-in duration-100' 
      leave-from-class='opacity-100' 
      leave-to-class='opacity-0'
    )
      ListboxOptions.absolute.z-10.mt-1.w-full.bg-rose-100.shadow-lg.max-h-60.rounded-md.py-1.text-base.ring-1.ring-black.ring-opacity-5.overflow-auto.focus_outline-none.sm_text-sm
        ListboxOption(
          as="template" 
          v-for="item in items" 
          :key="item.value" 
          :value="item"
          v-slot="{ active, selected }"
        )
          li(:class="[active ? 'text-white bg-rose-400' : 'text-rose-900', 'cursor-default select-none relative py-2 pl-3 pr-9']")
            .flex.items-center
              span(:class="[item.checked ? 'bg-rose-400' : 'bg-rose-200', 'flex-shrink-0 inline-block h-2 w-2 rounded-full']")
              span.flex.w-full(:class="[selected ? 'font-semibold' : 'font-normal', 'ml-3 block truncate']")
                .grow {{ item.name }}

                LockClosedIcon.h-5.w-5.text-rose-400.mr-5(v-if="item.icon === 'lock'")
                LockOpenIcon.h-5.w-5.text-rose-500.mr-5(v-if="item.icon === 'unlock'")
 

            span(
              v-if='item.checked' 
              :class="[active ? 'text-white' : 'text-rose-600', 'absolute inset-y-0 right-0 flex items-center pr-4']"
            )
              CheckIcon.h-5.w-5(aria-hidden='true')

</template>

<script setup lang="ts">
import { computed } from "vue";
import {
  Listbox,
  ListboxButton,
  ListboxLabel,
  ListboxOption,
  ListboxOptions,
} from "@headlessui/vue";
import {
  CheckIcon,
  ChevronUpDownIcon,
  LockClosedIcon,
  LockOpenIcon,
} from "@heroicons/vue/24/solid";

export type Item = {
  name: string;
  checked: boolean;
  value: string;
  icon?: "lock" | "unlock";
};

const emits = defineEmits<{
  (e: "update:modelValue", v: Item | null): void;
}>();

const props = defineProps<{
  modelValue: Item | null;
  items: Item[];
}>();

const selected = computed({
  get() {
    return props.modelValue ?? undefined;
  },
  set(v: Item | undefined) {
    emits("update:modelValue", v ?? null);
  },
});
</script>
