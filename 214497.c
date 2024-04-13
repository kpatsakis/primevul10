void UnionSparseIndicesAndValues(
    typename TTypes<int64>::ConstMatrix a_indices_mat,
    typename TTypes<T>::ConstFlat a_values, int64 a_nnz,
    typename TTypes<int64>::ConstMatrix b_indices_mat,
    typename TTypes<T>::ConstFlat b_values, int64 b_nnz, int num_dims,
    std::vector<T> *a_augmented_values, std::vector<T> *b_augmented_values,
    std::vector<std::pair<bool, int64>> *entries_to_copy) {
  entries_to_copy->reserve(a_nnz + b_nnz);
  a_augmented_values->reserve(a_nnz);
  b_augmented_values->reserve(b_nnz);

  int64 i = 0, j = 0;
  const T kZero = T(0);
  while (i < a_nnz && j < b_nnz) {
    switch (sparse::DimComparator::cmp(a_indices_mat, b_indices_mat, i, j,
                                       num_dims)) {
      case -1:
        entries_to_copy->emplace_back(true, i);
        a_augmented_values->push_back(a_values(i));
        b_augmented_values->push_back(kZero);
        ++i;
        break;
      case 0:
        entries_to_copy->emplace_back(true, i);
        a_augmented_values->push_back(a_values(i));
        b_augmented_values->push_back(b_values(j));
        ++i;
        ++j;
        break;
      case 1:
        entries_to_copy->emplace_back(false, j);
        a_augmented_values->push_back(kZero);
        b_augmented_values->push_back(b_values(j));
        ++j;
        break;
    }
  }
  // Handles leftovers; at most one loop runs.
  while (i < a_nnz) {
    entries_to_copy->emplace_back(/* is_a */ true, i);
    a_augmented_values->push_back(a_values(i++));
    b_augmented_values->push_back(kZero);
  }
  while (j < b_nnz) {
    entries_to_copy->emplace_back(/* is_a */ false, j);
    a_augmented_values->push_back(kZero);
    b_augmented_values->push_back(b_values(j++));
  }
}