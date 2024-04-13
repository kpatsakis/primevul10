  void NextCombination(int64 batch_index, std::vector<int>* combination) const {
    bool carry = true;
    for (int i = combination->size() - 1; i >= 0; i--) {
      if (carry) {
        (*combination)[i] = (*combination)[i] + 1;
      }
      if ((*combination)[i] == features_[i]->FeatureCount(batch_index)) {
        (*combination)[i] = 0;
      } else {
        carry = false;
        break;
      }
    }
  }