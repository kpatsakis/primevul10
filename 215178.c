  Status StoreAllDecodedSequences(
      const std::vector<std::vector<std::vector<int> > >& sequences,
      OpOutputList* decoded_indices, OpOutputList* decoded_values,
      OpOutputList* decoded_shape) const {
    // Calculate the total number of entries for each path
    const int64 batch_size = sequences.size();
    std::vector<int64> num_entries(top_paths_, 0);

    // Calculate num_entries per path
    for (const auto& batch_s : sequences) {
      CHECK_EQ(batch_s.size(), top_paths_);
      for (int p = 0; p < top_paths_; ++p) {
        num_entries[p] += batch_s[p].size();
      }
    }

    for (int p = 0; p < top_paths_; ++p) {
      Tensor* p_indices = nullptr;
      Tensor* p_values = nullptr;
      Tensor* p_shape = nullptr;

      const int64 p_num = num_entries[p];

      Status s =
          decoded_indices->allocate(p, TensorShape({p_num, 2}), &p_indices);
      if (!s.ok()) return s;
      s = decoded_values->allocate(p, TensorShape({p_num}), &p_values);
      if (!s.ok()) return s;
      s = decoded_shape->allocate(p, TensorShape({2}), &p_shape);
      if (!s.ok()) return s;

      auto indices_t = p_indices->matrix<int64>();
      auto values_t = p_values->vec<int64>();
      auto shape_t = p_shape->vec<int64>();

      int64 max_decoded = 0;
      int64 offset = 0;

      for (int64 b = 0; b < batch_size; ++b) {
        auto& p_batch = sequences[b][p];
        int64 num_decoded = p_batch.size();
        max_decoded = std::max(max_decoded, num_decoded);
        if (num_decoded > 0) {
          DCHECK_NE(values_t.data(), nullptr)
              << "values_t should not be nullptr: p_num=" << p_num
              << " num_decoded=" << num_decoded;
          DCHECK_LT(offset, values_t.size())
              << "offset should be smaller than values_t.size()";
          std::copy_n(p_batch.begin(), num_decoded, &values_t(offset));
        }
        for (int64 t = 0; t < num_decoded; ++t, ++offset) {
          indices_t(offset, 0) = b;
          indices_t(offset, 1) = t;
        }
      }

      shape_t(0) = batch_size;
      shape_t(1) = max_decoded;
    }
    return Status::OK();
  }