static bool canNewPropertyBeIndexed(DefinePropertyFlags dpf) {
  return dpf.setEnumerable && dpf.enumerable && dpf.setWritable &&
      dpf.writable && dpf.setConfigurable && dpf.configurable &&
      !dpf.setSetter && !dpf.setGetter;
}