void PropertyAccessorBuildMeta(const GCCell *cell, Metadata::Builder &mb) {
  const auto *self = static_cast<const PropertyAccessor *>(cell);
  mb.addField("getter", &self->getter);
  mb.addField("setter", &self->setter);
}