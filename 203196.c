process_cmd_password(CMD_Request *msg, char *line)
{
  char *p;
  struct timeval now;
  int i, len;

  /* Blank and free the old password */
  if (password) {
    for (i = 0; i < password_length; i++)
      password[i] = 0;
    free(password);
    password = NULL;
  }

  p = line;

  if (!*p) {
    /* blank line, prompt for password */
    p = getpass("Password: ");
  }

  if (!*p)
    return 0;

  len = strlen(p);
  password_length = UTI_DecodePasswordFromText(p);

  if (password_length > 0) {
    password = malloc(password_length);
    memcpy(password, p, password_length);
  }

  /* Erase the password from the input or getpass buffer */
  for (i = 0; i < len; i++)
    p[i] = 0;

  if (password_length <= 0) {
      fprintf(stderr, "Could not decode password\n");
      return 0;
  }

  if (gettimeofday(&now, NULL) < 0) {
    printf("500 - Could not read time of day\n");
    return 0;
  } else {
    msg->command = htons(REQ_LOGON); /* Just force a round trip so that we get tokens etc */
    UTI_TimevalHostToNetwork(&now, &msg->data.logon.ts);
    return 1;
  }
}