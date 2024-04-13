bool HHVM_FUNCTION(HH_global_key_exists, StringArg key) {
  return g_context->m_globalNVTable->lookup(key.get()) != nullptr;
}