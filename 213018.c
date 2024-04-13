void RegisterInternalHandlers() {
  // Register the enclave donate thread entry handler.
  if (!TrustedPrimitives::RegisterEntryHandler(kSelectorAsyloDonateThread,
                                               EntryHandler{DonateThread})
           .ok()) {
    TrustedPrimitives::BestEffortAbort(
        "Could not register entry handler: DonateThread.");
  }

  // Register the enclave finalization entry handler.
  if (!TrustedPrimitives::RegisterEntryHandler(kSelectorAsyloFini,
                                               EntryHandler{FinalizeEnclave})
           .ok()) {
    TrustedPrimitives::BestEffortAbort(
        "Could not register entry handler: FinalizeEnclave");
  }
}