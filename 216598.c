Status EagerKernelArgs::GetLocalArg(const FunctionArgIndex& index,
                                    Tensor* val) const {
  if (index.sub_index >= 0) {
    return errors::InvalidArgument("Got unexpected sub_index ", index.sub_index,
                                   " for argument ", index.index);
  }
  Tensor* arg = tensor_args_.at(index.index).tensor;
  if (arg) {
    *val = *arg;
    return Status::OK();
  } else {
    return errors::NotFound("Argument ", index.index, " has no local tensor.");
  }
}