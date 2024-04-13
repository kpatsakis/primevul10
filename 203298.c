authenticate_from_config(const char *filename)
{
  CMD_Request tx_message;
  CMD_Reply rx_message;
  char line[2048], keyfile[2048], *command, *arg, *password;
  const char *hashname;
  unsigned long key_id = 0, key_id2 = -1;
  int ret;
  FILE *in;

  in = fopen(filename, "r");
  if (!in) {
    fprintf(stderr, "Could not open file %s\n", filename);
    return 0;
  }

  *keyfile = '\0';
  while (fgets(line, sizeof (line), in)) {
    CPS_NormalizeLine(line);
    command = line;
    arg = CPS_SplitWord(line);
    if (!strcasecmp(command, "keyfile")) {
      snprintf(keyfile, sizeof (keyfile), "%s", arg);
    } else if (!strcasecmp(command, "commandkey")) {
      if (sscanf(arg, "%lu", &key_id) != 1)
        key_id = -1;
    }
  }
  fclose(in);

  if (!*keyfile || key_id == -1) {
    fprintf(stderr, "Could not read keyfile or commandkey in file %s\n", filename);
    return 0;
  }

  in = fopen(keyfile, "r");
  if (!in) {
    fprintf(stderr, "Could not open keyfile %s\n", filename);
    return 0;
  }

  while (fgets(line, sizeof (line), in)) {
    CPS_NormalizeLine(line);
    if (!*line || !CPS_ParseKey(line, &key_id2, &hashname, &password))
      continue;
    if (key_id == key_id2)
      break;
  }
  fclose(in);

  if (key_id == key_id2) {
    if (process_cmd_authhash(hashname) &&
        process_cmd_password(&tx_message, password)) {
      ret = request_reply(&tx_message, &rx_message, RPY_NULL, 1);
    } else {
      ret = 0;
    }
  } else {
    fprintf(stderr, "Could not find key %lu in keyfile %s\n", key_id, keyfile);
    ret = 0;
  }

  /* Erase password from stack */
  memset(line, 0, sizeof (line));

  return ret;
}