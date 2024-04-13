void GraphConstructor::PrintCycles() {
  int num_nodes = outputs_.size();
  absl::flat_hash_set<int> unvisited;
  for (int i = 0; i < num_nodes; i++) {
    unvisited.insert(i);
  }
  while (!unvisited.empty()) {
    int cur_node = *unvisited.begin();
    // Nodes on the current branch of DFS in traversal order. This is used for
    // printing the nodes in the cycle.
    std::vector<int> cur_branch;
    // This is just to make lookups O(1).
    // is_on_cur_branch[i] ==
    //   (std::find(cur_branch.start(),
    //              cur_branch.end(), i) != cur_branch.end())
    std::vector<bool> is_on_cur_branch(num_nodes, false);
    DFS(cur_node, &cur_branch, &is_on_cur_branch, &unvisited);
  }
}