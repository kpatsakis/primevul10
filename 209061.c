void redirect_to_stderr(void) {
   printf_handler = print_stderr;
   fputs_handler = fputs_stderr;

   current_output_stream = stderr;
}