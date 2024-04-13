inline bool ShouldHandleFormatError(const ErrorOptions& error_options,
                                    UChar32 ch, bool format_error) {
  return ((error_options.replace_control_chars && ch <= 0x1F) || format_error);
}