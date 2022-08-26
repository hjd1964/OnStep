//@ts-ignore
import Toastify from "toastify-js";

export function toast(text: string, type: "error" | "success" = "success") {
  Toastify({
    text,
    duration: 3000,
    gravity: "top",
    className: [
      type === "error" ? "text-white bg-rose-500" : "text-white bg-blue-500",
      "w-full",
      "md_w-1/2",
    ].join(" "),
    position: "right",
    style: {
      backgroundImage: "unset",
    },
  }).showToast();
}
