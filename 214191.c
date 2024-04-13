Status GetErrorOptions(OpKernelConstruction* ctx, ErrorOptions* out) {
  *out = ErrorOptions();

  string error_policy;
  TF_RETURN_IF_ERROR(ctx->GetAttr("errors", &error_policy));

  if (error_policy == "replace") {
    out->elide_replacement = false;
  } else if (error_policy == "ignore") {
    out->elide_replacement = true;
  } else if (error_policy == "strict") {
    out->error_on_malformatting = true;
  } else {
    return errors::InvalidArgument(
        "errors policy must be one of 'strict', 'replace', or 'ignore'");
  }

  int32 replacement_char;
  TF_RETURN_IF_ERROR(ctx->GetAttr("replacement_char", &replacement_char));

  if (replacement_char >= UCHAR_MIN_VALUE &&
      replacement_char <= UCHAR_MAX_VALUE) {
    out->subst = replacement_char;
  } else {
    return errors::InvalidArgument(
        "replacement_char out of unicode codepoint range");
  }

  if (ctx->HasAttr("replace_control_characters")) {
    TF_RETURN_IF_ERROR(ctx->GetAttr("replace_control_characters",
                                    &(out->replace_control_chars)));
  }

  return Status::OK();
}