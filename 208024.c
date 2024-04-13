void cfgreload_command(char *name, int type, char **argv)
{
    ci_debug_printf(1, "DEBUG cfgreload_command: reload configuration command received\n");

    free_global();
    free_pipe();
    debug = 0;
    statit = 0;

    pattc = 0;
    current_pattern_size = 0;
    maxsize = 0;
    logredir = 0;
    dnslookup = 1;
    clamd_curr_ip = (char *) malloc (sizeof (char) * 128);
    memset(clamd_curr_ip, 0, sizeof(clamd_curr_ip));
    if (load_patterns() == 0)
       ci_debug_printf(0, "FATAL cfgreload_command: reload configuration command failed!\n");
    if (squidclamav_xdata)
       set_istag(squidclamav_xdata);

    if (squidguard != NULL) {
	ci_debug_printf(1, "DEBUG cfgreload_command: reopening pipe to %s\n", squidguard);
	create_pipe(squidguard);
    }

}