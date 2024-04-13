int TrustedPrimitives::CreateThread() {
  MessageWriter input;
  MessageReader output;
  PrimitiveStatus status =
      UntrustedCall(kSelectorCreateThread, &input, &output);
  if (!status.ok()) {
    DebugPuts("CreateThread failed.");
    return -1;
  }
  if (output.size() != 1) {
    TrustedPrimitives::BestEffortAbort(
        "CreateThread error: unexpected output size received.");
  }
  return output.next<int>();
}