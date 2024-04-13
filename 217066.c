inline void SparseTensor::Reorder(const VarDimArray& order) {
  DCHECK_EQ(DataTypeToEnum<T>::v(), dtype())
      << "Reorder requested with the wrong datatype";
  DCHECK_EQ(order.size(), dims_) << "Order length must be SparseTensor rank";
  auto ix_t = ix_.matrix<int64>();
  auto vals_t = vals_.vec<T>();

  std::vector<int64> reorder(num_entries());
  std::iota(reorder.begin(), reorder.end(), 0);

  // Sort to get order of indices
  switch (order.size()) {
#define CASE_SORT(ORDER_SIZE)                                    \
  case ORDER_SIZE: {                                             \
    FixedDimComparator<ORDER_SIZE> sorter(ix_t, order, shape()); \
    std::sort(reorder.begin(), reorder.end(), sorter);           \
    break;                                                       \
  }
    CASE_SORT(0);
    CASE_SORT(1);
    CASE_SORT(2);
    CASE_SORT(3);
    CASE_SORT(4);
    CASE_SORT(5);
#undef CASE_SORT
    default: {
      DimComparator sorter(ix_t, order, shape());
      std::sort(reorder.begin(), reorder.end(), sorter);
    }
  }

  // We have a forward reordering, but what we'll need is a
  // permutation (the inverse).  This can be calculated with O(1)
  // additional
  // and O(n) time (INVPERM) but we just do the simple thing here.
  std::vector<size_t> permutation(reorder.size());
  for (std::size_t n = 0; n < reorder.size(); ++n) {
    permutation[reorder[n]] = n;
  }

  // Update indices & values by converting the permutations to
  // a product of transpositions.  Iterate over the cycles in the
  // permutation, and convert each of those into a product of
  // transpositions (swaps):
  //   https://en.wikipedia.org/wiki/Cyclic_permutation
  // This is N swaps, 2*N comparisons.
  for (std::size_t n = 0; n + 1 < permutation.size(); ++n) {
    while (n != permutation[n]) {
      std::size_t r = permutation[n];
      std::swap_ranges(&(ix_t(n, 0)), &(ix_t(n + 1, 0)), &(ix_t(r, 0)));
      std::swap(vals_t(n), vals_t(r));
      std::swap(permutation[n], permutation[r]);
    }
  }

  order_ = ShapeArray(order.begin(), order.end());
}