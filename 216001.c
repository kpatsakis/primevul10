Status GraphConstructor::Convert() {
  // Import functions before adding nodes, since imported nodes may refer to
  // functions
  if (library()) {
    // TODO(b/135705010): Add rvalue overloads into the function library, to
    // avoid unnecessarily copying `*library()` here.
    TF_RETURN_IF_ERROR(g_->AddFunctionLibrary(*library()));
  }

  std::vector<InputInfo> inputs;
  int processed = 0;

  std::vector<bool> input_already_exists;

  // Process the NodeDefs in topological order.
  // (InitFromEdges() sets this up by filling in ready_ with nodes that have no
  // inputs, pending_counts_ with the number of inputs for each node and
  // outputs_ with the outputs of each node).
  while (!ready_.empty()) {
    int o = *ready_.begin();
    ready_.erase(ready_.begin());
    ++processed;
    inputs.clear();
    bool has_data_back_edge = false;

    NodeDef node_def = consume_node_def(o);

    // input_already_exists[i] is true iff the i-th input of the node we're
    // importing refers to a preexisting node in g_ (i.e. input[i] existed prior
    // to importing node_defs_).  Conversely, input_already_exists[i] is false
    // iff the input refers to a node in node_defs_.
    input_already_exists.clear();
    input_already_exists.resize(node_def.input_size(), false);

    ssize_t string_intern_table_index = -1;

    if (opts_.importing) {
      // Intern the original node name, so that we can use a StringPiece of the
      // name to index gdef_nodes_.
      string_intern_table_index = string_intern_table_.size();
      string_intern_table_.push_back(node_def.name());

      if (opts_.skip_mapped_nodes) {
        bool is_node_mapped = false;
        TF_RETURN_IF_ERROR(IsNodeFullyMapped(node_def, &is_node_mapped));
        if (is_node_mapped) {
          // Skip this node after updating pending_count_ for outputs
          UpdatePendingCountAndReady(o, IsNextIteration(node_def));
          continue;
        }
      }

      if (!opts_.input_map.empty()) {
        // Note that input_already_exists can shrink here
        RemapNodeDefInputs(&node_def, &input_already_exists);
      }
      if (!opts_.control_dependencies.empty()) {
        // Note that input_already_exists can grow here
        AddControlDependencies(&node_def, &input_already_exists);
      }
      if (!opts_.default_device.empty() && node_def.device().empty()) {
        node_def.set_device(opts_.default_device);
      }
    }

    DCHECK_EQ(node_def.input_size(), input_already_exists.size());
    TF_RETURN_IF_ERROR(ValidateColocationConstraints(node_def));
    for (int i = 0; i < node_def.input_size(); ++i) {
      TensorId tensor_id = ParseTensorName(node_def.input(i));
      Node* src_node;
      int src_index;

      if (!input_already_exists[i]) {
        // Locate input in newly-imported nodes
        auto iter = gdef_nodes_.find(tensor_id.node());
        DCHECK(iter != gdef_nodes_.end()) << tensor_id.node();
        src_node = iter->second.node;
        src_index = tensor_id.index();
        if (src_node == nullptr) has_data_back_edge = true;
      } else {
        // Input refers to preexistng node in graph
        auto iter = existing_nodes_.find(tensor_id.node());
        DCHECK(iter != existing_nodes_.end()) << tensor_id.node();
        src_node = iter->second;
        src_index = tensor_id.index();
      }

      if (src_node != nullptr && src_index >= src_node->num_outputs()) {
        std::ostringstream out;
        out << "Node '" << node_def.name() << "': Connecting to invalid output "
            << tensor_id.index() << " of source node " << tensor_id.node()
            << " which has " << src_node->num_outputs() << " outputs.";

        if (src_node->type_string() == "If" ||
            src_node->type_string() == "StatelessIf" ||
            src_node->type_string() == "While" ||
            src_node->type_string() == "StatelessWhile") {
          out << " Try using "
              << "tf.compat.v1.experimental.output_all_intermediates(True).";
        }
        return errors::InvalidArgument(out.str());
      }

      inputs.emplace_back(string(tensor_id.node()), src_node, src_index);
    }

    if (has_data_back_edge && !IsMerge(node_def)) {
      return errors::InvalidArgument(
          "Node '", node_def.name(),
          "' had a back edge, but only Merge nodes can have back edges.");
    }

    Node* node;
    if (opts_.importing) {
      if (!prefix_.empty()) {
        AddPrefixToNodeDef(input_already_exists, &node_def);
      }
      // Note: no need to uniquify names if the prefix already guarantees
      // uniqueness
      if (opts_.uniquify_names && (prefix_.empty() || !opts_.uniquify_prefix)) {
        UniquifyNames(input_already_exists, &node_def);
      }
    }

    if (opts_.importing) {
      TF_RETURN_IF_ERROR(ModifyNodeDefForImport(&node_def));
    } else {
      const OpDef* op_def;
      TF_RETURN_IF_ERROR(
          g_->op_registry()->LookUpOpDef(node_def.op(), &op_def));
      if (opts_.add_default_attributes) {
        AddDefaultsToNodeDef(*op_def, &node_def);
      }
      if (opts_.validate_nodes) {
        TF_RETURN_IF_ERROR(ValidateNodeDef(node_def, *op_def));
      }
    }

    TF_RETURN_IF_ERROR(MakeNode(std::move(node_def), &node));

    if (opts_.importing) {
      // Use interned original node name so StringPiece remains valid.
      DCHECK_GE(string_intern_table_index, 0);
      gdef_nodes_[string_intern_table_[string_intern_table_index]].node = node;
    } else {
      DCHECK_EQ(string_intern_table_index, -1);
      gdef_nodes_[node->name()].node = node;
    }

    // Remove duplicate control inputs before adding edges to the graph. It
    // will allow us to skip expensive duplicates check in 'AddControlEdge'.
    auto first_control = absl::c_find_if(inputs, &InputInfo::IsControlInput);
    auto first_control_copy = first_control;
    std::sort(first_control, inputs.end(), &InputInfo::CompareName);
    inputs.erase(
        std::unique(first_control_copy, inputs.end(), &InputInfo::IsSameName),
        inputs.end());

    // Add edges from inputs to *node to the graph.
    for (size_t i = 0; i < inputs.size(); ++i) {
      if (inputs[i].node == nullptr) {
        // Record this back edge, which will be added after all nodes
        // are created.
        back_edges_.emplace_back(inputs[i].name, inputs[i].index, node, i);
      } else if (inputs[i].index == Graph::kControlSlot) {
        g_->AddControlEdge(inputs[i].node, node, kDoNotCheckDuplicates);
      } else {
        TF_RETURN_IF_ERROR(MakeEdge(inputs[i].node, inputs[i].index, node, i));
      }
    }

    TF_RETURN_IF_ERROR(ValidateShape(node));

    // Update pending_count_ for outputs.
    UpdatePendingCountAndReady(o, node->IsNextIteration());
  }

  if (processed < node_def_count()) {
    LOG(WARNING) << "IN " << __func__ << " " << (node_def_count() - processed)
                 << " NODES IN A CYCLE";
    for (int64 i = 0; i < node_def_count(); i++) {
      if (pending_count_[i] != 0) {
        LOG(WARNING) << "PENDING: " << SummarizeNodeDef(get_node_def(i))
                     << " WITH PENDING COUNT = " << pending_count_[i];
      }
    }
    PrintCycles();
    return errors::InvalidArgument(node_def_count() - processed,
                                   " nodes in a cycle");
  }

  return Status::OK();
}