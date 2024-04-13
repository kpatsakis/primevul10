start_debugger ()
{
#if defined (DEBUGGER) && defined (DEBUGGER_START_FILE)
  int old_errexit;
  int r;

  old_errexit = exit_immediately_on_error;
  exit_immediately_on_error = 0;

  r = force_execute_file (DEBUGGER_START_FILE, 1);
  if (r < 0)
    {
      internal_warning (_("cannot start debugger; debugging mode disabled"));
      debugging_mode = 0;
    }
  error_trace_mode = function_trace_mode = debugging_mode;

  set_shellopts ();
  set_bashopts ();

  exit_immediately_on_error += old_errexit;
#endif
}