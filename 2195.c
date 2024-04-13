Status GraphProperties::UpdateEnter(SymbolicShapeRefiner* shape_refiner,
                                    const NodeDef* node, bool* new_shapes) {
  InferenceContext* ic = shape_refiner->GetContext(node);
  if (!ic) {
    TF_RETURN_IF_ERROR(shape_refiner->UpdateNode(node, new_shapes));
    ic = shape_refiner->GetContext(node);
  }

  GraphView::InputPort port(node, 0);
  GraphView::OutputPort fanin = shape_refiner->graph().GetRegularFanin(port);

  InferenceContext* src_ic = shape_refiner->GetContext(fanin.node);
  ShapeHandle input = src_ic->output(fanin.port_id);
  if (!ic->output(0).SameHandle(input)) {
    ic->SetInput(0, input);
    ic->set_output(0, input);
    *new_shapes = true;
  }
  auto* outputs = src_ic->output_handle_shapes_and_types(fanin.port_id);
  if (outputs) {
    ic->set_input_handle_shapes_and_types(0, *outputs);
    ic->set_output_handle_shapes_and_types(0, *outputs);
    *new_shapes = true;
  }
  return Status::OK();
}