int create_pipe(char *command)
{

    int pipe1[2];
    int pipe2[2];

    ci_debug_printf(1, "DEBUG create_pipe: Open pipe to squidGuard %s!\n", command);

    if (command != NULL) {
	if ( pipe(pipe1) < 0  ||  pipe(pipe2) < 0 ) {
		ci_debug_printf(0, "ERROR create_pipe: unable to open pipe, disabling call to %s.\n", command);
		perror("pipe");
		usepipe = 0;
	} else {
		if ( (pid = fork()) == -1) {
			ci_debug_printf(0, "ERROR create_pipe: unable to fork, disabling call to %s.\n", command);
			usepipe = 0;
		} else {
			if(pid == 0) {
				close(pipe1[1]);
				dup2(pipe1[0],0);
				close(pipe2[0]);
				dup2(pipe2[1],1);
				setsid();
				/* Running chained program */
				execlp(command,(char *)basename(command),(char  *)0);
				exit(EXIT_SUCCESS);
				return(0);
			} else {
				close(pipe1[0]);
				sgfpw = fdopen(pipe1[1], "w");
				if (!sgfpw) {
				   ci_debug_printf(0, "ERROR create_pipe: unable to fopen command's child stdin, disabling it.\n");
					usepipe = 0;
				} else {
					/* make pipe line buffered */
					if (setvbuf (sgfpw, (char *)NULL, _IOLBF, 0)  != 0)
						ci_debug_printf(1, "DEBUG create_pipe: unable to line buffering pipe.\n");
					sgfpr = fdopen(pipe2[0], "r");
					if(!sgfpr) {
						ci_debug_printf(0, "ERROR create_pipe: unable to fopen command's child stdout, disabling it.\n");
						usepipe = 0;
					} else {
						ci_debug_printf(1, "DEBUG create_pipe: bidirectional pipe to %s childs ready...\n", command);
						usepipe = 1;
					}
				}
			}
		}
	}
    }

    return 1;
}