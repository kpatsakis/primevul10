static void memory_error_and_abort(char *func) {
   eprintf("%s: not enough memory\n", func);
   exit(EXIT_FAILURE);
}