static Status AllocateOutputWithShape(OpKernelContext* ctx, const Tensor& shape,
                                      int index, Tensor** output) {
  TensorShape tensor_shape;
  TF_RETURN_IF_ERROR(tensor::MakeShape(shape, &tensor_shape));
  return ctx->allocate_output(index, tensor_shape, output);
}