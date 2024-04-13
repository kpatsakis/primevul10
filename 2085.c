Status ValidateSignatureWithAttrs(const OpDef& sig, AttrSlice attr_values) {
  // attr_values should specify all attrs defined in fdef, except for those
  // which have a default value
  for (const auto& attr : sig.attr()) {
    const AttrValue* attr_value = attr_values.Find(attr.name());
    if (attr_value) {
      Status status = AttrValueHasType(*attr_value, attr.type());
      if (!status.ok()) {
        errors::AppendToMessage(&status, "for attr '", attr.name(), "'");
        return status;
      }
    } else if (!attr.has_default_value()) {
      return errors::NotFound("Attr ", attr.name(), " is not found from ",
                              SummarizeOpDef(sig));
    }
  }

// TODO(josh11b): Enable this code once it works with function gradients.
// Right now the C++ function gradient code assumes it can pass
// all the attrs of the function to the gradient, and any attrs that
// the gradient doesn't care about will be ignored.
#if 0
  if (attr_values.size() != sig.attr_size()) {
    for (const auto& a : attr_values) {
      // TODO(josh11b): Possibly should ignore attrs that start with "_" here?
      bool found = false;
      for (const auto& s : sig.attr()) {
        if (a.first == s.name()) {
          found = true;
          break;
        }
      }
      if (!found) {
        return errors::NotFound("Attr ", a.first, " is not found in ",
                                SummarizeOpDef(sig));
      }
    }
  }
#endif

  return Status::OK();
}