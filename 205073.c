bool ZoneScope::ShouldDeleteOnExit() {
  return zone_->scope_nesting_ == 1 && mode_ == DELETE_ON_EXIT;
}