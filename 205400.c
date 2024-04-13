void do_shutdown_server(struct st_command *command)
{
  long timeout=60;
  int pid;
  DYNAMIC_STRING ds_pidfile_name;
  MYSQL* mysql = cur_con->mysql;
  static DYNAMIC_STRING ds_timeout;
  const struct command_arg shutdown_args[] = {
    {"timeout", ARG_STRING, FALSE, &ds_timeout, "Timeout before killing server"}
  };
  DBUG_ENTER("do_shutdown_server");

  check_command_args(command, command->first_argument, shutdown_args,
                     sizeof(shutdown_args)/sizeof(struct command_arg),
                     ' ');

  if (ds_timeout.length)
  {
    char* endptr;
    timeout= strtol(ds_timeout.str, &endptr, 10);
    if (*endptr != '\0')
      die("Illegal argument for timeout: '%s'", ds_timeout.str);
  }
  dynstr_free(&ds_timeout);

  /* Get the servers pid_file name and use it to read pid */
  if (query_get_string(mysql, "SHOW VARIABLES LIKE 'pid_file'", 1,
                       &ds_pidfile_name))
    die("Failed to get pid_file from server");

  /* Read the pid from the file */
  {
    int fd;
    char buff[32];

    if ((fd= my_open(ds_pidfile_name.str, O_RDONLY, MYF(0))) < 0)
      die("Failed to open file '%s'", ds_pidfile_name.str);
    dynstr_free(&ds_pidfile_name);

    if (my_read(fd, (uchar*)&buff,
                sizeof(buff), MYF(0)) <= 0){
      my_close(fd, MYF(0));
      die("pid file was empty");
    }
    my_close(fd, MYF(0));

    pid= atoi(buff);
    if (pid == 0)
      die("Pidfile didn't contain a valid number");
  }
  DBUG_PRINT("info", ("Got pid %d", pid));

  /* Tell server to shutdown if timeout > 0*/
  if (timeout && mysql_shutdown(mysql, SHUTDOWN_DEFAULT))
    die("mysql_shutdown failed");

  /* Check that server dies */
  while(timeout--){
    if (my_kill(pid, 0) < 0){
      DBUG_PRINT("info", ("Process %d does not exist anymore", pid));
      DBUG_VOID_RETURN;
    }
    DBUG_PRINT("info", ("Sleeping, timeout: %ld", timeout));
    my_sleep(1000000L);
  }

  /* Kill the server */
  DBUG_PRINT("info", ("Killing server, pid: %d", pid));
  (void)my_kill(pid, 9);

  DBUG_VOID_RETURN;

}