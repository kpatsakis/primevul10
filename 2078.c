Status FunctionCallFrame::GetArg(int index, const Tensor** val) {
  if (index < 0 || static_cast<size_t>(index) >= args_.size()) {
    return errors::InvalidArgument("GetArg ", index, " is not within [0, ",
                                   args_.size(), ")");
  }
  *val = &args_[index];
  return Status::OK();
}