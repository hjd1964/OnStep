import _ from "lodash";
import { gzipSync } from "zlib";
import { writeFileSync } from "fs";
import { PluginOption, HtmlTagDescriptor } from "vite";

export const inlineAssets: PluginOption = {
  name: "inline-assets",
  enforce: "post",
  transformIndexHtml(html, ctx) {
    const tags: HtmlTagDescriptor[] = [];

    Object.entries(ctx.bundle ?? {}).forEach((entry) => {
      const [file, bundle] = entry;

      if (bundle.type === "chunk" && file.endsWith(".js")) {
        tags.push({
          tag: "script",
          children: bundle.code,
          injectTo: "body",
        });
      } else if (bundle.type === "asset" && file.endsWith(".css")) {
        tags.push({
          tag: "style",
          children: bundle.source.toString(),
          attrs: { type: "text/css" },
          injectTo: "head",
        });
      } else {
        console.error("Unsupported asset", file);
      }
    });

    return {
      tags,
      html: html
        .replace(/<script[^>]+><\/script>/, "")
        .replace(/<link rel="stylesheet"[^>]+>/, ""),
    };
  },
};

export const generateHeaderFile: PluginOption = {
  name: "generate-gzip",
  enforce: "post",
  generateBundle(__, bundle) {
    Object.entries(bundle).forEach(([file, value]) => {
      if (file === "index.html" && value.type === "asset") {
        const index = gzipSync(value.source);

        console.log(
          `\n\n-----\nWriting ${index.length} bytes to output.h\n-------\n`,
        );

        const output = `
#ifndef HTML_H
#define HTML_H
#define html_len ${index.length}

const char new_ui[] PROGMEM = {
  ${_.chunk(
    Array.from(index).map((item) => {
      return `0x${item.toString(16).padStart(2, "0")}`;
    }),
    50,
  )
    .map((line) => line.join(","))
    .join(",\n  ")}
};

#endif
`;

        writeFileSync("./dist/output.h", output);
      }
    });
  },
};
