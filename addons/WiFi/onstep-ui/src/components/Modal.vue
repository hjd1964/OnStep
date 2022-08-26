<template lang="pug">

TransitionRoot(as="template", :show="isOpen")
  Dialog.relative.z-10(
    as="div", 
    @close="setIsOpen(false)"
    :open="isOpen"
  )
    .fixed.inset-0.bg-gray-500.bg-opacity-75.transition-opacity(
      @click="setIsOpen(false)"
      @touchend="setIsOpen(false)"
    )

    TransitionChild(
      as="template", 
      enter="ease-out duration-300", 
      enter-from="opacity-0", 
      enter-to="opacity-100", 
      leave="ease-in duration-200", 
      leave-from="opacity-100", 
      leave-to="opacity-0"
    )
     .fixed.z-10.inset-0.overflow-y-auto
        .flex.items-end.sm_items-center.justify-center.min-h-full.p-4.text-center.sm_p-0
          TransitionChild(
            as="template", 
            enter="ease-out duration-300", 
            enter-from="opacity-0 translate-y-4 sm_translate-y-0 sm_scale-95", 
            enter-to="opacity-100 translate-y-0 sm_scale-100", 
            leave="ease-in duration-200", 
            leave-from="opacity-100 translate-y-0 sm_scale-100", 
            leave-to="opacity-0 translate-y-4 sm:translate-y-0 sm_scale-95"
          )

            DialogPanel.relative.bg-rose-200.rounded-lg.px-4.pt-5.pb-4.text-left.overflow-hidden.shadow-xl.transform.transition-all.sm_my-8.sm_max-w-lg.sm_w-full.sm_p-6
              .hidden.sm_block.absolute.top-0.right-0.pt-4.pr-4
                button.bg-rose-200.rounded-md.text-rose-700.hover_text-gray-500.focus_outline-none.focus_ring-none.focus_ring-indigo-500(type="button", @click="setIsOpen(false)")
                  span.sr-only Close
                  XMarkIcon.h-6.w-6(aria-hidden="true").
                    
                    
              .sm_flex.sm_items-start.w-full
                slot(name="icon" v-if="!noIcon")
                  .mx-auto.flex-shrink-0.flex.items-center.justify-center.h-12.w-12.rounded-full.bg-red-100.sm_mx-0.sm_h-10.sm_w-10
                    ExclamationTriangleIcon.h-6.w-6.text-red-600(aria-hidden="true").
                    
                    
                .mt-3.text-center.sm_mt-0.sm_ml-4.sm_text-left.w-full
                  DialogTitle.text-lg.leading-6.font-medium.text-gray-900(as="h3")
                    slot(name="title")

                  .mt-2
                    p.text-sm.text-gray-900 
                      slot(name="body" :item="confirmingI")

              .mt-5.sm_mt-4.sm_flex.sm_flex-row-reverse
                slot(name="buttons")
                  button.w-full.inline-flex.justify-center.rounded-md.border.border-transparent.shadow-sm.px-4.py-2.bg-red-600.text-base.font-medium.text-white.hover_bg-red-700.focus_outline-none.focus_ring-2.focus_ring-offset-2.focus_ring-red-500.sm_ml-3.sm_w-auto.sm_text-sm(
                    type="button", 
                    @click="confirmPromise?.(true)"
                  ) {{ confirmText }}

                  button.mt-3.w-full.inline-flex.justify-center.rounded-md.border.border-rose-300.shadow-sm.px-4.py-2.bg-rose-100.text-base.font-medium.text-gray-700.hover_text-gray-500.focus_outline-none.focus_ring-2.focus_ring-offset-2.focus_ring-indigo-500.sm_mt-0.sm_w-auto.sm_text-sm(
                    type="button", 
                    @click="setIsOpen(false)"
                  ) {{ cancelText }}

</template>

<script setup lang="ts">
import { ref, watchEffect } from "vue";
import {
  Dialog,
  DialogTitle,
  TransitionChild,
  TransitionRoot,
  DialogPanel,
} from "@headlessui/vue";
import { ExclamationTriangleIcon, XMarkIcon } from "@heroicons/vue/24/outline";

const props = withDefaults(
  defineProps<{
    confirmText?: string;
    cancelText?: string;
    open?: boolean;
    noIcon?: boolean;
  }>(),
  {
    cancelText: () => "Cancel",
    confirmText: () => `I'm sure`,
    open: () => false,
    noIcon: () => false,
  }
);

const confirmingI = ref<any>(null);
const emits = defineEmits<{
  (e: "update:open", v: boolean): void;
}>();

const isOpen = ref(false);

watchEffect(() => {
  setIsOpen(props.open);
});

function setIsOpen(v: boolean) {
  isOpen.value = v;
  emits("update:open", v);
}

defineExpose({
  setIsOpen,
  awaitAnswer,
});

const confirmPromise = ref<((v: boolean) => void) | null>(null);

function awaitAnswer(i?: any) {
  confirmingI.value = i;
  setIsOpen(true);

  return new Promise((resolve) => {
    confirmPromise.value = (v: boolean) => {
      setIsOpen(false);
      resolve(v);
      confirmPromise.value = null;
    };
  });
}
</script>
