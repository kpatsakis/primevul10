NodeDef MakeConstNodeDefFromTensorProto(InferenceContext* ic,
                                        const TensorProto& tensor_proto,
                                        const DataType& dtype) {
  NodeDef const_node;
  const_node.set_name("const_from_shape");
  const_node.set_op("Const");
  auto* attr = const_node.mutable_attr();
  (*attr)["dtype"].set_type(dtype);
  auto* tensor = (*attr)["value"].mutable_tensor();
  *tensor = tensor_proto;
  return const_node;
}