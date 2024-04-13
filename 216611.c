  Status AddSparseTensor(OpKernelContext* ctx, const SparseTensor& sp,
                         int64* handle) {
    PersistentTensor persistent_ix;
    Tensor* ix;
    TF_RETURN_IF_ERROR(ctx->allocate_persistent(
        sp.indices().dtype(), sp.indices().shape(), &persistent_ix, &ix));
    *ix = sp.indices();

    PersistentTensor persistent_values;
    Tensor* values;
    TF_RETURN_IF_ERROR(ctx->allocate_persistent(sp.indices().dtype(),
                                                sp.indices().shape(),
                                                &persistent_values, &values));
    *values = sp.values();
    {
      mutex_lock l(mu_);
      int64 unique_st_handle = counter_++;  // increment is guarded on purpose
      sp_tensors_[unique_st_handle] = PersistentSparseTensor{
          persistent_ix, persistent_values,
          gtl::InlinedVector<int64, 8>(sp.shape().begin(), sp.shape().end())};
      *handle = unique_st_handle;
    }
    return Status::OK();
  }