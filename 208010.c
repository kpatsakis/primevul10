load_patterns()
{
  char *buf = NULL;
  FILE *fp  = NULL;

  if (isPathExists(CONFIG_FILE) == 0) {
    fp = fopen(CONFIG_FILE, "rt");
    if (debug > 0)
       ci_debug_printf(0, "LOG load_patterns: Reading configuration from %s\n", CONFIG_FILE);
  }
  

  if (fp == NULL) {
	ci_debug_printf(0, "FATAL load_patterns: unable to open configuration file: %s\n", CONFIG_FILE);
    return 0;
  }

  buf = (char *)malloc(sizeof(char)*LOW_BUFF*2);
  if (buf == NULL) {
	ci_debug_printf(0, "FATAL load_patterns: unable to allocate memory in load_patterns()\n");
	fclose(fp);
	return 0;
  }
  while ((fgets(buf, LOW_BUFF, fp) != NULL)) {
      /* chop newline */
      chomp(buf);
      /* add to regex patterns array */
     if (add_pattern(buf) == 0) {
	xfree(buf);
	fclose(fp);
	return 0;
     }
  }
  xfree(buf);
  if (redirect_url == NULL) {
    ci_debug_printf(0, "FATAL load_patterns: No redirection URL set, going to BRIDGE mode\n");
    return 0;
  }
   if (squidguard != NULL) {
    ci_debug_printf(0, "LOG load_patterns: Chaining with %s\n", squidguard);
  }
  if (fclose(fp) != 0)
	ci_debug_printf(0, "ERROR load_patterns: Can't close configuration file\n");

  /* Set default values */
  if (clamd_local == NULL) {
	  if (clamd_ip == NULL) {
		clamd_ip = (char *) malloc (sizeof (char) * SMALL_CHAR);
		if(clamd_ip == NULL) {
			ci_debug_printf(0, "FATAL load_patterns: unable to allocate memory in load_patterns()\n");
			return 0;
		}
		xstrncpy(clamd_ip, CLAMD_SERVER, SMALL_CHAR);
	  }

	  if (clamd_port == NULL) {
		clamd_port = (char *) malloc (sizeof (char) * LOW_CHAR);
		if(clamd_port == NULL) {
			ci_debug_printf(0, "FATAL load_patterns: unable to allocate memory in load_patterns()\n");
			return 0;
		}
		xstrncpy(clamd_port, CLAMD_PORT, LOW_CHAR);
	  }
  }

  return 1;
}