void CheckErrors<GPUDevice, int64>(OpKernelContext* context, int batch_dim,
                                   int seq_dim) {
  CheckErrorsGPU(context, batch_dim, seq_dim);
}