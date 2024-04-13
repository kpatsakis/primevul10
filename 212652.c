  void WriteOutputSlice(int64 begin, int64 end) override {
    std::vector<int> combination(features_.size(), 0);
    for (int64 b = begin; b < end; ++b) {
      auto row_start = splits_out_(b);
      auto row_limit = splits_out_(b + 1);
      for (auto i = row_start; i < row_limit; ++i) {
        WriteCombination(b, combination, &values_out_(i));
        NextCombination(b, &combination);
      }
      combination.assign(features_.size(), 0);  // reset for next batch.
    }
  }