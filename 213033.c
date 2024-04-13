PrimitiveStatus FinalizeEnclave(void *context, MessageReader *in,
                                MessageWriter *out) {
  if (in) {
    ASYLO_RETURN_IF_READER_NOT_EMPTY(*in);
  }
  // Delete instance of the global memory pool singleton freeing all memory held
  // by the pool.
  delete UntrustedCacheMalloc::Instance();
  return asylo_enclave_fini();
}