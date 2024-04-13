static void attach_zval(json_parser *json,
                        const String& key,
                        int assoc,
                        JSONContainerType container_type) {
  if (json->top < 1) {
    return;
  }

  auto& root = json->stack[json->top - 1].val;
  auto& child =  json->stack[json->top].val;
  auto up_mode = json->stack[json->top - 1].mode;

  if (up_mode == Mode::ARRAY) {
    if (container_type == JSONContainerType::COLLECTIONS) {
      collections::append(root.getObjectData(), child.asTypedValue());
    } else {
      root.asArrRef().append(child);
    }
  } else if (up_mode == Mode::OBJECT) {
    object_set(json, root, key, child, assoc, container_type);
  }
}