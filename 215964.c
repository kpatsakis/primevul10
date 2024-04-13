  GraphConstructor(const Options& opts, Graph* g, ShapeRefiner* refiner,
                   std::vector<std::pair<Node*, int>>* return_tensors,
                   std::vector<Node*>* return_nodes,
                   std::vector<SafeTensorId>* missing_unused_input_map_keys)
      : opts_(opts),
        g_(g),
        original_versions_(g->versions()),
        prefix_(opts.prefix),
        refiner_(refiner),
        return_tensors_(return_tensors),
        return_nodes_(return_nodes),
        missing_unused_input_map_keys_(missing_unused_input_map_keys) {}