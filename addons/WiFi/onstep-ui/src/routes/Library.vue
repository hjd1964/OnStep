<template lang="pug">
Columns
  Modal(ref="confirmRemove")
    template(#title) Are you sure?
    template(#body="{ item }") This will remove {{ item.name }} from your catalog.

  Widget
    template(#heading) Find an object
    template(#subheading) Premade library & recent GoTo's

    InputField.mb-2(
      v-model="searchValue"
      label="Filter stars"
      placeholder="Betelgeuse"
    )

    .overflow-auto.max-h-96
      .flex.text-white.my-2.space-x-3(
        v-for="(star) in filteredStars"
        :key="`${star.ra}${star.dec}`"
      ) 
        .grow.mr-4.flex.flex-col.justify-center 
          .flex {{ star.name }} 
            small.flex.flex-col.justify-center(v-if="star.source === 'manual'")
              a.ml-2.cursor-pointer(
                @click="removeFromLib(star)"
              ) (
                template(v-if="removing === star.name") removing...
                template(v-else) remove
                | )

          small.mt-1.flex ({{ star.ra }}, {{ star.dec }})

        ControlButton(
          class="w-1/3"
          @click="goTo(star)"
          :loading="loading == star.name"
        ) GoTo {{ star.name }}

  Widget
    template(#heading) Enter coordinates manually
    template(#subheading) Manual GoTo's will be saved to the library.
    .flex
      InputField.my-2(
        class="w-1/2"
        label="Name of this object"
        v-model="manualGoToName"
        placeholder="Any name you'd like!"
        :max-length="11"
      )

      .flex.flex-col.justify-end.h-full.pb-2.ml-2(
        class="w-1/2"
      )
        SelectBox(
          :items=`starSelectTypes`
          v-model="manualStarSelectType"
        )
          template(#label) Select type of object

    .flex.space-x-2.my-2
      InputField.grow(
        v-model="manualGoTo.ra"
        placeholder="00:08:23"
      )
        template(#label)
          .flex.flex-col
            | Right Ascension
            .text-sm (format: 00:08:23)


      InputField.grow(
        v-model="manualGoTo.dec"
        placeholder="+29*05:25"
      )
        template(#label)
          .flex.flex-col
            | Declination
            .text-sm (format: +29*05:25)

    ControlButton.my-4(
      @click="goTo(manualGoTo)"
      :loading="loading === manualGoTo.name"
    ) GoTo coordinates




</template>

<script setup lang="ts">
import { api } from "../api/api";
import { stars, type Star, starTypes, type StarType } from "../utils/stars";
import Widget from "../components/Widget.vue";
import InputField from "../components/InputField.vue";
import { computed, ref, onMounted, reactive } from "vue";
import { MountStatusApi } from "../api/control";
import ControlButton from "../components/ControlButton.vue";
import Fuse from "fuse.js";
import { RecentStars } from "../utils/recentStars";
import SelectBox from "../components/SelectBox.vue";
import Modal from "../components/Modal.vue";
import Columns from "../components/Columns.vue";

const starSelectTypes = Object.entries(starTypes).map(([value, name]) => ({
  name,
  value,
  checked: false,
}));

const libraryStars = ref<Star[]>([]);

function makeSearchableStars(additionalStars: Star[]) {
  libraryStars.value = additionalStars;

  return new Fuse([...additionalStars, ...stars], {
    threshold: 0.3,
    keys: ["name"],
  });
}

const searchableStars = ref(makeSearchableStars([]));

const manualStarSelectType = ref<{
  value: StarType;
  name: string;
  checked: false;
}>({
  name: "Star",
  value: "STR",
  checked: false,
});

const recentStars = new RecentStars(api, (stars) => {
  searchableStars.value = makeSearchableStars(
    stars.map((s) => ({ ...s, source: "manual" }))
  );
});

const props = defineProps<{
  control: MountStatusApi;
}>();

const manualGoToName = ref("");
const manualGoTo = reactive<Star>({
  name: Symbol("manual"),
  ra: "",
  dec: "",
  type: "STR",
  source: "lib",
});

const filteredStars = computed<Star[]>(() => {
  return searchValue.value
    ? searchableStars.value.search(searchValue.value).map((i) => i.item)
    : [...libraryStars.value, ...stars];
});

const searchValue = ref("");
const loading = ref<string | null | symbol>(null);

const confirmRemove = ref<InstanceType<typeof Modal>>(null!);
const removing = ref<string | null>(null);

async function removeFromLib(star: Star) {
  if (!(await confirmRemove.value.awaitAnswer(star))) {
    return;
  }

  try {
    removing.value = star.name.toString();
    await recentStars.removeStar((s) => {
      return s.dec === star.dec && s.ra === star.ra && s.name === star.name;
    });

    await recentStars.refreshList();
  } finally {
    removing.value = null;
  }
}

async function goTo(star: Star) {
  loading.value = star.name;

  if (star.name === manualGoTo.name) {
    recentStars.addStar({
      ...manualGoTo,
      type: manualStarSelectType.value.value,
      name: manualGoToName.value,
    });

    recentStars.storeList().then(() => recentStars.refreshList());
  }

  try {
    await props.control.goToStar(star);
  } finally {
    loading.value = null;
  }
}

onMounted(() => {
  recentStars.refreshList();
});
</script>
