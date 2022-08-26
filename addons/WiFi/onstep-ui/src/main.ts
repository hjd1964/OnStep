import { createApp } from "vue";
import { createRouter, createWebHashHistory } from "vue-router";
import Control from "./routes/Control.vue";
import Settings from "./routes/Settings.vue";
import Terminal from "./routes/Terminal.vue";
import Library from "./routes/Library.vue";
import DatetimeLocation from "./routes/DatetimeLocation.vue";
import WifiSettings from "./routes/WifiSettings.vue";
import "./style.css";
import App from "./App.vue";

const routes = [
  {
    name: "root",
    path: "",
    redirect: { name: "control" },
  },
  {
    name: "settings",
    path: "/mount-settings",
    component: Settings,
  },
  {
    name: "control",
    path: "/control",
    component: Control,
  },
  {
    name: "library",
    path: "/library",
    component: Library,
  },
  {
    name: "terminal",
    path: "/terminal",
    component: Terminal,
  },
  {
    name: "wifi",
    path: "/wifi",
    component: WifiSettings,
  },
  {
    name: "datetime",
    path: "/dates-locations",
    component: DatetimeLocation,
  },
  {
    name: "mountSettings",
    path: "/mount-settings",
    component: DatetimeLocation,
  },
];

const router = createRouter({
  routes,
  history: createWebHashHistory(),
  scrollBehavior() {
    return { top: 0 };
  },
});

createApp(App).use(router).mount("#app");
