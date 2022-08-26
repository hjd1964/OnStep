<template lang="pug">
.flex.flex-col
  .flex.justify-end.mb-3(
    v-if="wifi"
  )
    .flex.flex-col.justify-center.grow.mr-6(v-if="wifi.restartRequired")
      .bg-rose-300.text-rose-700.p-3.rounded-md A manual restart is required for your changes to take effect.

    ControlButton.w-full(
      class="md_w-1/5"
      @click="logOut"
      :loading="loggingOut"
    ) Log out

  .flex.md_space-x-2.md_space-y-0.space-y-2.flex-wrap.md_flex-nowrap.justify-center(v-if="!loadingConfig")
    Modal(
      :no-icon="true"
      ref="newWifiPasswordModal"
      confirm-text="Done"
      cancel-text="Cancel"
    )
      template(#title) Join {{ newWifi?.name }}
      template(#body)
        InputField(
          v-model="newWifiPassword"
        )
          template(#label)
            .text-black Enter the password for this network.

    template(v-if="loginRequired")
      Widget
        .flex.justify-center.my-6
          .flex.flex-col.mx-0.space-y-4.md_mx-20
            .flex.justify-center
              LockClosedIcon.h-20.w-20.text-orange-300

            Heading Please log in to continue.
            .bg-rose-300.text-center.text-rose-700.p-3.rounded-md(v-if="loginFail") Incorrect credentials. Please try again.
            InputField(
              v-model="login"
              type="password"
              @submit="() => doLogin(true)"
              label="Enter your master password to continue."
            )

            ControlButton(
              @click="() => doLogin(true)"
              :loading="loggingIn"
            ) Log in

    template(v-if="!loginRequired && mutableWifi")
      Widget
        template(#heading) Station mode

        SelectBox.w-full(
          :items=`networks`
          :modelValue="selectedNetwork"
          @update:modelValue="v => chooseNetwork(v)"
        )
          template(#label) 
            .flex
              .grow WiFi SSID
              a(@click="refreshWifi")
                ArrowPathIcon.h-4.w-4.shadow.cursor-pointer(
                  :class="{ 'animate-spin': loadingNetworks }"
                )

        hr.text-rose-500

        .flex
          label.grow.text-sm Mac Address
          code {{ mutableWifi.station.network.mac }}

        .flex
          strong.grow.flex.flex-col.justify-center Enable DHCP
          Toggle(v-model="mutableWifi.station.network.dhcp")

        InputField(
          v-model="mutableWifi.station.network.ip"
          label="IP Address"
          :disabled="mutableWifi.station.network.dhcp"
          :validate="[ipValidation]"
        )

        InputField(
          v-model="mutableWifi.station.network.gw"
          :disabled="mutableWifi.station.network.dhcp"
          label="Internet Gateway"
          :validate="[ipValidation]"
        )
          
        InputField(
          v-model="mutableWifi.station.network.sn"
          :disabled="mutableWifi.station.network.dhcp"
          label="Subnet"
          :validate="[ipValidation]"
        )
        .flex
          strong.grow.flex.flex-col.justify-center Enable Station Mode
          Toggle(v-model="mutableWifi.station.enabled")

        hr.text-rose-500
        ControlButton(
          :disabled="!staChanged"
          @click="saveStation"
          :loading="savingStation"
        ) Save changes to Station


      Widget
        template(#heading) Access point mode

        .flex.space-x-2
          InputField.w-full.grow(
            v-model="mutableWifi.accessPoint.network.ssid"
            label="AccessPoint SSID"
            :max-length="32"
          )

          InputField.w-full.grow(
            v-model="newApPassword"
            label="AccessPoint Password"
            type="password"
          )

        hr.text-rose-500
        .flex
          label.grow.text-sm Mac Address
          code {{ mutableWifi.accessPoint.network.mac }}

        InputField.w-full(
          v-model="mutableWifi.accessPoint.network.ip"
          label="IP Address"
          :validate="[ipValidation]"
        )

        InputField.w-full(
          v-model="mutableWifi.accessPoint.network.gw"
          label="Internet Gateway"
          :validate="[ipValidation]"
        )

        InputField.w-full(
          v-model="mutableWifi.accessPoint.network.sn"
          label="Subnet"
          :validate="[ipValidation]"
        )

        .flex
          strong.grow.flex.flex-col.justify-center Enable Access Point
          Toggle(v-model="mutableWifi.accessPoint.enabled")

        .flex(v-if="!mutableWifi.accessPoint.enabled")
          small Note: If OnStep fails to connect to the station, Access Point will be automatically enabled.


        hr.text-rose-500
        ControlButton.w-full(
          :disabled="!apChanged"
          @click="saveAp"
          :loading="savingAp"
        ) Save changes to Access Point

  LoadingSpinner.mt-2(v-else)
    | Loading WiFi Settings

</template>
<script setup lang="ts">
import { ref, onMounted, computed } from "vue";
import { api } from "../api/api";
import { WifiApi } from "../api/wifi";
import Widget from "../components/Widget.vue";
import Heading from "../components/Heading.vue";
import SelectBox, { type Item } from "../components/SelectBox.vue";
import { ArrowPathIcon } from "@heroicons/vue/24/outline";
import type { WifiConfig } from "../types";
import LoadingSpinner from "../components/LoadingSpinner.vue";
import { LockClosedIcon } from "@heroicons/vue/24/outline";
import Modal from "../components/Modal.vue";
import InputField from "../components/InputField.vue";
import Toggle from "../components/Toggle.vue";
import ControlButton from "../components/ControlButton.vue";
import { AxiosError } from "axios";
import { objectsEqual, clone } from "../utils/compareObjects";

const wifiApi = new WifiApi(api);

const ipValidation = [
  "Must be a valid IP address" as const,
  /^((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\.){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])$/,
] as const;

defineProps<{}>();

const loadingNetworks = ref(false);
const loadingConfig = ref(true);

const loginRequired = ref(false);
const loginFail = ref(false);
const loggingIn = ref(false);
const loggingOut = ref(false);
const login = ref("");

const networks = ref<Item[]>([]);

const wifi = ref<WifiConfig | null>(null);
const mutableWifi = ref<WifiConfig | null>(null);
const newWifi = ref<Item | null>(null);
const newWifiPassword = ref("");
const newApPassword = ref("");
const newWifiPasswordModal = ref<InstanceType<typeof Modal>>(null!);

const selectedNetwork = computed<Item | null>(() => {
  const ssid = mutableWifi.value?.station.network.ssid;

  return ssid
    ? {
        name: ssid,
        checked: true,
        value: ssid,
        icon: mutableWifi.value?.station.network.secured ? "lock" : "unlock",
      }
    : null;
});

const apChanged = computed(
  () =>
    !objectsEqual(
      wifi.value?.accessPoint ?? {},
      mutableWifi.value?.accessPoint ?? {}
    ) || newApPassword.value.length
);

const staChanged = computed(
  () =>
    !objectsEqual(wifi.value?.station ?? {}, mutableWifi.value?.station ?? {})
);

const savingAp = ref(false);
const savingStation = ref(false);
async function saveAp() {
  if (!mutableWifi.value) {
    return;
  }

  try {
    savingAp.value = true;
    wifi.value = await wifiApi.updateAp(
      mutableWifi.value.accessPoint,
      newApPassword.value
    );
    newApPassword.value = "";
  } finally {
    savingAp.value = false;
  }
}

async function saveStation() {
  if (!mutableWifi.value) {
    return;
  }

  try {
    savingStation.value = true;
    wifi.value = await wifiApi.updateStation(
      mutableWifi.value.station,
      newWifiPassword.value
    );
    newWifiPassword.value = "";
  } finally {
    savingStation.value = false;
  }
}

async function doLogin(showError = false) {
  try {
    loggingIn.value = true;
    loginFail.value = false;
    wifi.value = await wifiApi.getWifiSettings(login.value);
    mutableWifi.value = clone(wifi.value);
    loginRequired.value = false;
    login.value = "";
  } catch (e) {
    loginRequired.value = true;

    if (showError) {
      loginFail.value = true;
    }
  } finally {
    loggingIn.value = false;
  }
}

async function loadWifiSettings() {
  try {
    loadingConfig.value = true;
    wifi.value = await wifiApi.getWifiSettings();
    mutableWifi.value = clone(wifi.value);
  } catch (e) {
    if (is401(e)) {
      await doLogin();
    }
  } finally {
    loadingConfig.value = false;
  }
}

onMounted(loadWifiSettings);

async function chooseNetwork(v: Item | null) {
  if (!mutableWifi.value) {
    return;
  }

  newWifiPassword.value = "";
  newWifi.value = null;

  if (!v) {
    return;
  }

  if (v.name === selectedNetwork.value?.name) {
    return;
  }

  newWifi.value = v;

  if (!(await newWifiPasswordModal.value.awaitAnswer())) {
    newWifi.value = null;
    newWifiPassword.value = "";
    return;
  }

  mutableWifi.value.station.network.ssid = v.name;
  newWifi.value = null;
}

function is401(e: unknown) {
  return (e as AxiosError)?.response?.status === 401;
}

async function logOut() {
  try {
    loggingOut.value = true;
    await wifiApi.wifiLogout();
    loginRequired.value = true;
  } catch (e) {
    if (is401(e)) {
      loginRequired.value = true;
      wifi.value = null;
    } else {
      throw e;
    }
  } finally {
    loggingOut.value = false;
  }
}

async function refreshWifi() {
  try {
    loadingNetworks.value = true;

    networks.value = Object.entries(await wifiApi.scanForWifiNetworks()).map(
      ([network, secure]) => ({
        name: network,
        checked: network === selectedNetwork.value?.name,
        value: network,
        icon: secure === "secured" ? ("lock" as const) : ("unlock" as const),
      })
    );
  } finally {
    loadingNetworks.value = false;
  }
}
</script>
