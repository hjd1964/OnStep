<template lang="pug">
.flex.flex-col.space-y-2
  Columns
    Homing(
      :control="control"
      :status="status"
    )

    Parking(
      :control="control"
      :status="status"
    )

  Columns
    Tracking(
      :control="control"
      :status="status"
    )

    Alignment(
      :status="status"
      :control="control"
    )

  Slewing(
    :control="control"
    :status="status"
  )

</template>

<script setup lang="ts">
import { MountStatusApi } from "../api/control";
import { type ValidMountStatus } from "../types";
import Tracking from "../widgets/Tracking.vue";
import Parking from "../widgets/Parking.vue";
import Homing from "../widgets/Homing.vue";
import Slewing from "../widgets/Slewing.vue";
import Alignment from "../widgets/Alignment.vue";
import Columns from "../components/Columns.vue";

defineProps<{
  control: MountStatusApi;
  status: ValidMountStatus;
}>();
</script>
