process_line(char *line, int *quit)
{
  char *command;
  int do_normal_submit;
  int ret;
  CMD_Request tx_message;
  CMD_Reply rx_message;

  *quit = 0;
  ret = 0;

  do_normal_submit = 1;

  CPS_NormalizeLine(line);

  if (!*line) {
    fflush(stderr);
    fflush(stdout);
    return 1;
  };

  command = line;
  line = CPS_SplitWord(line);

  if (!strcmp(command, "accheck")) {
    do_normal_submit = process_cmd_accheck(&tx_message, line);
  } else if (!strcmp(command, "activity")) {
    do_normal_submit = 0;
    ret = process_cmd_activity(line);
  } else if (!strcmp(command, "add") && !strncmp(line, "peer", 4)) {
    do_normal_submit = process_cmd_add_peer(&tx_message, CPS_SplitWord(line));
  } else if (!strcmp(command, "add") && !strncmp(line, "server", 6)) {
    do_normal_submit = process_cmd_add_server(&tx_message, CPS_SplitWord(line));
  } else if (!strcmp(command, "allow")) {
    if (!strncmp(line, "all", 3)) {
      do_normal_submit = process_cmd_allowall(&tx_message, CPS_SplitWord(line));
    } else {
      do_normal_submit = process_cmd_allow(&tx_message, line);
    }
  } else if (!strcmp(command, "authhash")) {
    ret = process_cmd_authhash(line);
    do_normal_submit = 0;
  } else if (!strcmp(command, "burst")) {
    do_normal_submit = process_cmd_burst(&tx_message, line);
  } else if (!strcmp(command, "clients")) {
    ret = process_cmd_clients(line);
    do_normal_submit = 0;
  } else if (!strcmp(command, "cmdaccheck")) {
    do_normal_submit = process_cmd_cmdaccheck(&tx_message, line);
  } else if (!strcmp(command, "cmdallow")) {
    if (!strncmp(line, "all", 3)) {
      do_normal_submit = process_cmd_cmdallowall(&tx_message, CPS_SplitWord(line));
    } else {
      do_normal_submit = process_cmd_cmdallow(&tx_message, line);
    }
  } else if (!strcmp(command, "cmddeny")) {
    if (!strncmp(line, "all", 3)) {
      line = CPS_SplitWord(line);
      do_normal_submit = process_cmd_cmddenyall(&tx_message, line);
    } else {
      do_normal_submit = process_cmd_cmddeny(&tx_message, line);
    }
  } else if (!strcmp(command, "cyclelogs")) {
    process_cmd_cyclelogs(&tx_message, line);
  } else if (!strcmp(command, "delete")) {
    do_normal_submit = process_cmd_delete(&tx_message, line);
  } else if (!strcmp(command, "deny")) {
    if (!strncmp(line, "all", 3)) {
      do_normal_submit = process_cmd_denyall(&tx_message, CPS_SplitWord(line));
    } else {
      do_normal_submit = process_cmd_deny(&tx_message, line);
    }
  } else if (!strcmp(command, "dfreq")) {
    process_cmd_dfreq(&tx_message, line);
  } else if (!strcmp(command, "dns")) {
    ret = process_cmd_dns(line);
    do_normal_submit = 0;
  } else if (!strcmp(command, "doffset")) {
    process_cmd_doffset(&tx_message, line);
  } else if (!strcmp(command, "dump")) {
    process_cmd_dump(&tx_message, line);
  } else if (!strcmp(command, "exit")) {
    do_normal_submit = 0;
    *quit = 1;
    ret = 1;
  } else if (!strcmp(command, "help")) {
    do_normal_submit = 0;
    give_help();
    ret = 1;
  } else if (!strcmp(command, "local")) {
    do_normal_submit = process_cmd_local(&tx_message, line);
  } else if (!strcmp(command, "makestep")) {
    process_cmd_makestep(&tx_message, line);
  } else if (!strcmp(command, "manual")) {
    if (!strncmp(line, "list", 4)) {
      do_normal_submit = 0;
      ret = process_cmd_manual_list(CPS_SplitWord(line));
    } else if (!strncmp(line, "delete", 6)) {
      do_normal_submit = process_cmd_manual_delete(&tx_message, CPS_SplitWord(line));
    } else {
      do_normal_submit = process_cmd_manual(&tx_message, line);
    }
  } else if (!strcmp(command, "maxdelay")) {
    do_normal_submit = process_cmd_maxdelay(&tx_message, line);
  } else if (!strcmp(command, "maxdelaydevratio")) {
    do_normal_submit = process_cmd_maxdelaydevratio(&tx_message, line);
  } else if (!strcmp(command, "maxdelayratio")) {
    do_normal_submit = process_cmd_maxdelayratio(&tx_message, line);
  } else if (!strcmp(command, "maxpoll")) {
    do_normal_submit = process_cmd_maxpoll(&tx_message, line);
  } else if (!strcmp(command, "maxupdateskew")) {
    do_normal_submit = process_cmd_maxupdateskew(&tx_message, line);
  } else if (!strcmp(command, "minpoll")) {
    do_normal_submit = process_cmd_minpoll(&tx_message, line);
  } else if (!strcmp(command, "minstratum")) {
    do_normal_submit = process_cmd_minstratum(&tx_message, line);
  } else if (!strcmp(command, "offline")) {
    do_normal_submit = process_cmd_offline(&tx_message, line);
  } else if (!strcmp(command, "online")) {
    do_normal_submit = process_cmd_online(&tx_message, line);
  } else if (!strcmp(command, "password")) {
    do_normal_submit = process_cmd_password(&tx_message, line);
  } else if (!strcmp(command, "polltarget")) {
    do_normal_submit = process_cmd_polltarget(&tx_message, line);
  } else if (!strcmp(command, "quit")) {
    do_normal_submit = 0;
    *quit = 1;
    ret = 1;
  } else if (!strcmp(command, "rekey")) {
    process_cmd_rekey(&tx_message, line);
  } else if (!strcmp(command, "reselect")) {
    process_cmd_reselect(&tx_message, line);
  } else if (!strcmp(command, "reselectdist")) {
    do_normal_submit = process_cmd_reselectdist(&tx_message, line);
  } else if (!strcmp(command, "retries")) {
    ret = process_cmd_retries(line);
    do_normal_submit = 0;
  } else if (!strcmp(command, "rtcdata")) {
    do_normal_submit = 0;
    ret = process_cmd_rtcreport(line);
  } else if (!strcmp(command, "settime")) {
    do_normal_submit = 0;
    ret = process_cmd_settime(line);
  } else if (!strcmp(command, "sources")) {
    do_normal_submit = 0;
    ret = process_cmd_sources(line);
  } else if (!strcmp(command, "sourcestats")) {
    do_normal_submit = 0;
    ret = process_cmd_sourcestats(line);
  } else if (!strcmp(command, "timeout")) {
    ret = process_cmd_timeout(line);
    do_normal_submit = 0;
  } else if (!strcmp(command, "tracking")) {
    ret = process_cmd_tracking(line);
    do_normal_submit = 0;
  } else if (!strcmp(command, "trimrtc")) {
    process_cmd_trimrtc(&tx_message, line);
  } else if (!strcmp(command, "waitsync")) {
    ret = process_cmd_waitsync(line);
    do_normal_submit = 0;
  } else if (!strcmp(command, "writertc")) {
    process_cmd_writertc(&tx_message, line);
  } else {
    fprintf(stderr, "Unrecognized command\n");
    do_normal_submit = 0;
  }
    
  if (do_normal_submit) {
    ret = request_reply(&tx_message, &rx_message, RPY_NULL, 1);
  }
  fflush(stderr);
  fflush(stdout);
  return ret;
}