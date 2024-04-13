Array HHVM_FUNCTION(HH_object_prop_array, const Object& obj) {
  return obj.toArray().toDArray();
}