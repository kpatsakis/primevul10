  ShapeHandle GetMergedShape(InferenceContext* ic, ShapeHandle s) {
    const auto& actual_shape = shapes_.GetMergedValue(s);
    if (!InferenceContext::RankKnown(actual_shape)) {
      return ic->UnknownShape();
    } else {
      std::vector<DimensionHandle> dims;
      for (int j = 0; j < InferenceContext::Rank(actual_shape); ++j) {
        shape_inference::DimensionHandle dim =
            InferenceContext::DimKnownRank(actual_shape, j);
        int64_t d = dims_.GetMergedValue(dim);
        // Symbolic shape manager may made some dims < -1, which causes errors
        // in creating Dimension.
        if (d < -1) {
          d = -1;
        }
        dims.push_back(ic->MakeDim(d));
      }
      return ic->MakeShape(dims);
    }
  }