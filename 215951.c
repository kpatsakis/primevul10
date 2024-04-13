void GraphConstructor::DFS(int cur_node, std::vector<int>* cur_branch,
                           std::vector<bool>* is_on_cur_branch,
                           absl::flat_hash_set<int>* unvisited) {
  cur_branch->push_back(cur_node);
  is_on_cur_branch->at(cur_node) = true;
  for (auto next_node : outputs_[cur_node]) {
    if (unvisited->find(next_node) != unvisited->end()) {
      if (is_on_cur_branch->at(next_node)) {
        auto iter =
            std::find(cur_branch->begin(), cur_branch->end(), next_node);
        LOG(WARNING) << "Cycle detected:";
        while (iter != cur_branch->end()) {
          LOG(WARNING) << SummarizeNodeDef(get_node_def(*iter));
          ++iter;
        }
        LOG(WARNING) << "End of cycle";
      } else {
        DFS(next_node, cur_branch, is_on_cur_branch, unvisited);
      }
    }
  }
  cur_branch->pop_back();
  is_on_cur_branch->at(cur_node) = false;
  unvisited->erase(cur_node);
}