  AttrKeyAndValue(absl::string_view key_name, int key_suffix, string value,
                  ValueRepresentationOp value_op = kRaw)
      : key_name_(key_name),
        key_suffix_(key_suffix),
        value_op_(value_op),
        value_(std::move(value)) {}