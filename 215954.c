  NodeDefCopyingGraphConstructor(
      const Options& opts, NodeDefSlice node_defs, const VersionDef* versions,
      const FunctionDefLibrary* library, Graph* g, ShapeRefiner* refiner,
      std::vector<std::pair<Node*, int>>* return_tensors,
      std::vector<Node*>* return_nodes,
      std::vector<SafeTensorId>* missing_unused_input_map_keys)
      : GraphConstructor(opts, g, refiner, return_tensors, return_nodes,
                         missing_unused_input_map_keys),
        node_defs_(node_defs),
        versions_(versions),
        library_(library) {}