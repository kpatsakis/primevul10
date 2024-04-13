ZoneScope::~ZoneScope() {
  if (ShouldDeleteOnExit()) zone_->DeleteAll();
  zone_->scope_nesting_--;
}