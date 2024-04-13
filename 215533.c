int ecall_deliver_signal(int signum, int sigcode) {
  int result = 0;
  try {
    result = asylo::primitives::DeliverSignal(signum, sigcode);
  } catch (...) {
    LOG(FATAL) << "Uncaught exception in enclave";
  }
  return result;
}