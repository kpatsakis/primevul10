  void ComputeAsync(OpKernelContext* context, DoneCallback done) override {
    SparseFillEmptyRowsOpImpl<GPUDevice, T, Tindex>(context, done);
  }