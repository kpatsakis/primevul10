PrimitiveStatus DonateThread(void *context, MessageReader *in,
                             MessageWriter *out) {
  if (in) {
    ASYLO_RETURN_IF_INCORRECT_READER_ARGUMENTS(*in, 1);
  }
  int result = 0;
  try {
    ThreadManager *thread_manager = ThreadManager::GetInstance();
    result = thread_manager->StartThread(in->next<pid_t>());
  } catch (...) {
    TrustedPrimitives::BestEffortAbort(
        "Uncaught exception in enclave entry handler: DonateThread. Failed to "
        "get ThreadManager instance or start the thread.");
  }
  return PrimitiveStatus(result);
}