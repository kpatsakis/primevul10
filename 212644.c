  void WriteCombination(int64 batch_index, const std::vector<int>& combination,
                        tstring* out) {
    static const auto k_feature_separator = "_X_";
    gtl::InlinedVector<tstring, 6> cross_vec(features_.size());
    for (int i = 0; i < combination.size(); ++i) {
      features_[i]->ReadValue(batch_index, combination[i], &cross_vec[i]);
    }
    *out = absl::StrJoin(cross_vec, k_feature_separator);
  }