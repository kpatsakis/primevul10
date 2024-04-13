std::unique_ptr<OutputWriter> MakeOutputWriter(const FeatureReaders& features,
                                               int64 num_buckets,
                                               uint64 hash_key,
                                               const Tensor* splits_out,
                                               Tensor* values_out) {
  if (values_out->dtype() == DT_INT64) {
    if (splits_out->dtype() == DT_INT64) {
      return std::make_unique<OutputWriterImpl<int64, int64>>(
          features, num_buckets, hash_key, splits_out, values_out);
    } else {
      return std::make_unique<OutputWriterImpl<int64, int32>>(
          features, num_buckets, hash_key, splits_out, values_out);
    }
  } else {
    if (splits_out->dtype() == DT_INT64) {
      return std::make_unique<OutputWriterImpl<tstring, int64>>(
          features, num_buckets, hash_key, splits_out, values_out);
    } else {
      return std::make_unique<OutputWriterImpl<tstring, int32>>(
          features, num_buckets, hash_key, splits_out, values_out);
    }
  }
}