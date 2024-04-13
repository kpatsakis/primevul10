  int64 CrossCountByBatchIndex(const FeatureReaders& features,
                               int batch_index) {
    int64 cross_count = 1;
    for (int i = 0; i < features.size(); ++i) {
      const auto feature_count = features[i]->FeatureCount(batch_index);
      if (feature_count == 0) return 0;
      cross_count *= feature_count;
    }
    return cross_count;
  }