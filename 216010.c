  NodeDefMovingGraphConstructor(
      const Options& opts, GraphDef&& graph_def, Graph* g,
      ShapeRefiner* refiner, std::vector<std::pair<Node*, int>>* return_tensors,
      std::vector<Node*>* return_nodes,
      std::vector<SafeTensorId>* missing_unused_input_map_keys)
      : GraphConstructor(opts, g, refiner, return_tensors, return_nodes,
                         missing_unused_input_map_keys),
        graph_def_(std::move(graph_def)),
        is_consumed_(graph_def_.node_size(), false) {}