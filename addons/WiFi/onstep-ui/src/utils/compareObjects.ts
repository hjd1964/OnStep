export function objectsEqual(a: object, b: object): boolean {
  const keysA = Object.keys(a);
  const keysB = Object.keys(b);
  if (keysA.length !== keysB.length) {
    return false;
  }

  for (const key of keysA) {
    const valA = a[key as keyof typeof a];
    const valB = b[key as keyof typeof b];

    const areObjects = isObject(valA) && isObject(valB);

    if (
      (areObjects && !objectsEqual(valA, valB)) ||
      (!areObjects && valA !== valB)
    ) {
      return false;
    }
  }

  return true;
}
function isObject(object: object) {
  return object != null && typeof object === "object";
}

export function clone(object: object) {
  return JSON.parse(JSON.stringify(object));
}
