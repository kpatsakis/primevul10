  bool CompatibleShapes(ShapeHandle inferred_shape,
                        ShapeHandle annotated_shape) const {
    if (inferred_shape.SameHandle(annotated_shape)) {
      return true;
    }
    if (!InferenceContext::RankKnown(inferred_shape)) {
      return true;
    }
    if (InferenceContext::Rank(inferred_shape) !=
        InferenceContext::Rank(annotated_shape)) {
      return false;
    }
    const int rank = InferenceContext::Rank(inferred_shape);
    for (int i = 0; i < rank; ++i) {
      if (!InferenceContext::DimKnownRank(inferred_shape, i)
               .SameHandle(
                   InferenceContext::DimKnownRank(annotated_shape, i))) {
        int64_t val1 = InferenceContext::Value(
            InferenceContext::DimKnownRank(inferred_shape, i));
        int64_t val2 = InferenceContext::Value(
            InferenceContext::DimKnownRank(annotated_shape, i));
        if (val1 >= 0 && val1 != val2) {
          return false;
        }
      }
    }
    return true;
  }