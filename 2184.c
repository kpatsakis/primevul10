NodeDef MakeConstNodeDefFromShape(InferenceContext* ic,
                                  const ShapeHandle& shape,
                                  const ShapeHandle& tensor_as_shape,
                                  const DataType& dtype) {
  return MakeConstNodeDefFromTensorProto(
      ic, MakeTensorProtoFromShape(ic, shape, tensor_as_shape, dtype), dtype);
}