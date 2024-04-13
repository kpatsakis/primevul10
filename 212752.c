void ObjectBuildMeta(const GCCell *cell, Metadata::Builder &mb) {
  // This call is just for debugging and consistency purposes.
  mb.addJSObjectOverlapSlots(JSObject::numOverlapSlots<JSObject>());

  const auto *self = static_cast<const JSObject *>(cell);
  mb.addField("parent", &self->parent_);
  mb.addField("class", &self->clazz_);
  mb.addField("propStorage", &self->propStorage_);

  // Declare the direct properties.
  static const char *directPropName[JSObject::DIRECT_PROPERTY_SLOTS] = {
      "directProp0", "directProp1", "directProp2", "directProp3"};
  for (unsigned i = mb.getJSObjectOverlapSlots();
       i < JSObject::DIRECT_PROPERTY_SLOTS;
       ++i) {
    mb.addField(directPropName[i], self->directProps() + i);
  }
}