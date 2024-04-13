  void AppendTo(bool first, string* s) const {
    absl::string_view v;
    bool add_escaped = false;
    if ((value_op_ == kCEscape) && NeedsEscaping(value_)) {
      // Use CEscape call below
      add_escaped = true;
    } else {
      // Add raw value contents directly
      v = value_;
    }
    if (key_suffix_ >= 0) {
      strings::StrAppend(s, first ? "" : ",", key_name_, key_suffix_, "=", v);
    } else {
      strings::StrAppend(s, first ? "" : ",", key_name_, "=", v);
    }
    if (add_escaped) {
      strings::StrAppend(s, absl::CEscape(value_));
    }
  }