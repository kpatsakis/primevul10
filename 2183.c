  bool AllOutputShapesKnown(NodeContext* c) {
    InferenceContext* ic = c->inference_context.get();
    // Checks if all the output shapes are fully defined.
    for (int i = 0; i < ic->num_outputs(); i++) {
      if (!ic->FullyDefined(ic->output(i))) {
        return false;
      }
    }
    return true;
  }