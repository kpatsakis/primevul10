PCREglobals::PCREglobals() {
  jit_stack = pcre_jit_stack_alloc(32768, 524288);
  // Set these to handle uses of pcre prior to PcreExtension::threadInit
  // In particular, for matching tier overrides during RuntimeOption::Load
  preg_backtrace_limit = RuntimeOption::PregBacktraceLimit;
  preg_recursion_limit = RuntimeOption::PregRecursionLimit;
}