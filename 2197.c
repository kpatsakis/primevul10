std::vector<ShapeHandle> ReplaceUnknownDimFromConstWithUnknownDim(
    InferenceContext* ic, const std::vector<ShapeHandle>& shapes) {
  std::vector<ShapeHandle> converted_shapes(shapes.size());
  for (int i = 0, shapes_size = shapes.size(); i < shapes_size; i++) {
    const auto& shape = shapes[i];
    if (!ic->RankKnown(shape)) {
      converted_shapes[i] = shape;
      continue;
    }
    bool just_copy = true;
    std::vector<DimensionHandle> dims;
    for (int32_t i = 0; i < ic->Rank(shape); ++i) {
      DimensionHandle dim = ic->Dim(shape, i);
      if (ic->ValueKnown(dim) && ic->Value(dim) == kUnknownDimFromConst) {
        just_copy = false;
        dims.push_back(ic->UnknownDim());
      } else {
        dims.push_back(dim);
      }
    }
    if (just_copy) {
      converted_shapes[i] = shape;
      continue;
    }
    converted_shapes[i] = ic->MakeShape(dims);
  }
  return converted_shapes;
}