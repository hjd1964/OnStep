export function formatDate(date: Date, utc = false) {
  function pad(v: number, length = 2) {
    return String(v).padStart(length, "0");
  }

  return (
    utc
      ? [
          [
            date.getUTCMonth() + 1,
            date.getUTCDate(),
            date.getUTCFullYear(),
          ].join("/"),
          [date.getUTCHours(), date.getUTCMinutes(), date.getUTCSeconds()]
            .map((v) => pad(v))
            .join(":"),
        ]
      : [
          [date.getMonth() + 1, date.getDate(), date.getFullYear()].join("/"),
          [date.getHours(), date.getMinutes(), date.getSeconds()]
            .map((v) => pad(v))
            .join(":"),
        ]
  ).join(" ");
}
