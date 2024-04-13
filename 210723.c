bool IsFormSubmit(const NavigationEntry* entry) {
  return (ui::PageTransitionStripQualifier(entry->GetTransitionType()) ==
          ui::PAGE_TRANSITION_FORM_SUBMIT);
}
