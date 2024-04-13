ZoneSplayTree<Config>::~ZoneSplayTree() {
  // Reset the root to avoid unneeded iteration over all tree nodes
  // in the destructor.  For a zone-allocated tree, nodes will be
  // freed by the Zone.
  SplayTree<Config, ZoneAllocationPolicy>::ResetRoot();
}