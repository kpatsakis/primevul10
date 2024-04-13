void eflush(void) {
   /* Do this only for non-NULL streams (uninitiated stream or a syslog) */
   if (current_output_stream != NULL)
      fflush(current_output_stream);
}