  OutputWriterImpl(const FeatureReaders& features, int64 num_buckets,
                   uint64 hash_key, const Tensor* splits_out,
                   Tensor* values_out)
      : features_(features),
        num_buckets_(num_buckets),
        hash_key_(hash_key),
        splits_out_(splits_out->flat<SplitsType>()),
        values_out_(values_out->flat<ValuesType>()) {}