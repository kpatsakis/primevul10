PCREglobals::~PCREglobals() {
  pcre_jit_stack_free(jit_stack);
}