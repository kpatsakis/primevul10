static int run_command(char* cmd,
                       DYNAMIC_STRING *ds_res)
{
  char buf[512]= {0};
  FILE *res_file;
  int error;
  DBUG_ENTER("run_command");
  DBUG_PRINT("enter", ("cmd: %s", cmd));

  if (!(res_file= popen(cmd, "r")))
  {
    report_or_die("popen(\"%s\", \"r\") failed", cmd);
    return -1;
  }

  while (fgets(buf, sizeof(buf), res_file))
  {
    DBUG_PRINT("info", ("buf: %s", buf));
    if(ds_res)
    {
      /* Save the output of this command in the supplied string */
      dynstr_append(ds_res, buf);
    }
    else
    {
      /* Print it directly on screen */
      fprintf(stdout, "%s", buf);
    }
  }

  error= pclose(res_file);
  DBUG_RETURN(WEXITSTATUS(error));
}