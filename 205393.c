static void cleanup_and_exit(int exit_code)
{
  free_used_memory();

  /* Only call mysql_server_end if mysql_server_init has been called */
  if (server_initialized)
    mysql_server_end();

  /*
    mysqltest is fundamentally written in a way that makes impossible
    to free all memory before exit (consider memory allocated
    for frame local DYNAMIC_STRING's and die() invoked down the stack.

    We close stderr here to stop unavoidable safemalloc reports
    from polluting the output.
  */
  fclose(stderr);

  my_end(my_end_arg);

  if (!silent) {
    switch (exit_code) {
    case 1:
      printf("not ok\n");
      break;
    case 0:
      printf("ok\n");
      break;
    case 62:
      printf("skipped\n");
    break;
    default:
      printf("unknown exit code: %d\n", exit_code);
      DBUG_ASSERT(0);
    }
  }

  sf_leaking_memory= 0; /* all memory should be freed by now */
  exit(exit_code);
}