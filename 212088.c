static pcre_jit_stack* alloc_jit_stack(void* /*data*/) {
  return tl_pcre_globals->jit_stack;
}