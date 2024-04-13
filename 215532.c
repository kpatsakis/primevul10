int ecall_dispatch_trusted_call(uint64_t selector, void *buffer) {
  return asylo::primitives::asylo_enclave_call(selector, buffer);
}