  void Decode(OpKernelContext* ctx, std::vector<UChar32>* char_values,
              std::vector<SPLITS_TYPE>* offset_values, int* current_offset,
              SPLITS_TYPE* next_row_split, UChar32 char_value, int char_length,
              bool found_any_format_error) {
    if (error_options_.error_on_malformatting && found_any_format_error) {
      ctx->CtxFailure(
          errors::InvalidArgument("Invalid formatting on input string"));
    }
    UChar32 decoded_value = char_value;
    if (ShouldHandleFormatError(error_options_, char_value,
                                found_any_format_error)) {
      if (error_options_.elide_replacement && (offset_values != nullptr)) {
        *current_offset += char_length;
        return;
      } else {
        decoded_value = error_options_.subst;
      }
    }

    // Emit the char value.
    char_values->push_back(decoded_value);

    // Emit the byte offset
    if (offset_values != nullptr) {
      offset_values->push_back(*current_offset);
      *current_offset += char_length;
    }
    *next_row_split += 1;
  }