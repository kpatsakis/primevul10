R_API int r_sys_cmd_str_full(const char *cmd, const char *input, char **output, int *len, char **sterr) {
	char *mysterr = NULL;
	if (!sterr) {
		sterr = &mysterr;
	}
	char buffer[1024], *outputptr = NULL;
	char *inputptr = (char *)input;
	int pid, bytes = 0, status;
	int sh_in[2], sh_out[2], sh_err[2];

	if (len) {
		*len = 0;
	}
	if (pipe (sh_in)) {
		return false;
	}
	if (output) {
		if (pipe (sh_out)) {
			close (sh_in[0]);
			close (sh_in[1]);
			close (sh_out[0]);
			close (sh_out[1]);
			return false;
		}
	}
	if (pipe (sh_err)) {
		close (sh_in[0]);
		close (sh_in[1]);
		return false;
	}

	switch ((pid = r_sys_fork ())) {
	case -1:
		return false;
	case 0:
		dup2 (sh_in[0], 0);
		close (sh_in[0]);
		close (sh_in[1]);
		if (output) {
			dup2 (sh_out[1], 1);
			close (sh_out[0]);
			close (sh_out[1]);
		}
		if (sterr) {
			dup2 (sh_err[1], 2);
		} else {
			close (2);
		}
		close (sh_err[0]);
		close (sh_err[1]);
		exit (r_sandbox_system (cmd, 0));
	default:
		outputptr = strdup ("");
		if (!outputptr) {
			return false;
		}
		if (sterr) {
			*sterr = strdup ("");
			if (!*sterr) {
				free (outputptr);
				return false;
			}
		}
		if (output) {
			close (sh_out[1]);
		}
		close (sh_err[1]);
		close (sh_in[0]);
		if (!inputptr || !*inputptr) {
			close (sh_in[1]);
		}
		// we should handle broken pipes somehow better
		r_sys_signal (SIGPIPE, SIG_IGN);
		size_t err_len = 0, out_len = 0;
		for (;;) {
			fd_set rfds, wfds;
			int nfd;
			FD_ZERO (&rfds);
			FD_ZERO (&wfds);
			if (output) {
				FD_SET (sh_out[0], &rfds);
			}
			if (sterr) {
				FD_SET (sh_err[0], &rfds);
			}
			if (inputptr && *inputptr) {
				FD_SET (sh_in[1], &wfds);
			}
			memset (buffer, 0, sizeof (buffer));
			nfd = select (sh_err[0] + 1, &rfds, &wfds, NULL, NULL);
			if (nfd < 0) {
				break;
			}
			if (output && FD_ISSET (sh_out[0], &rfds)) {
				if ((bytes = read (sh_out[0], buffer, sizeof (buffer))) < 1) {
					break;
				}
				char *tmp = realloc (outputptr, out_len + bytes + 1);
				if (!tmp) {
					R_FREE (outputptr);
					break;
				}
				outputptr = tmp;
				memcpy (outputptr + out_len, buffer, bytes);
				out_len += bytes;
			} else if (FD_ISSET (sh_err[0], &rfds) && sterr) {
				if ((bytes = read (sh_err[0], buffer, sizeof (buffer))) < 1) {
					break;
				}
				char *tmp = realloc (*sterr, err_len + bytes + 1);
				if (!tmp) {
					R_FREE (*sterr);
					break;
				}
				*sterr = tmp;
				memcpy (*sterr + err_len, buffer, bytes);
				err_len += bytes;
			} else if (FD_ISSET (sh_in[1], &wfds) && inputptr && *inputptr) {
				int inputptr_len = strlen (inputptr);
				bytes = write (sh_in[1], inputptr, inputptr_len);
				if (bytes != inputptr_len) {
					break;
				}
				inputptr += bytes;
				if (!*inputptr) {
					close (sh_in[1]);
					/* If neither stdout nor stderr should be captured,
					 * abort now - nothing more to do for select(). */
					if (!output && !sterr) {
						break;
					}
				}
			}
		}
		if (output) {
			close (sh_out[0]);
		}
		close (sh_err[0]);
		close (sh_in[1]);
		waitpid (pid, &status, 0);
		bool ret = true;
		if (status) {
			// char *escmd = r_str_escape (cmd);
			// eprintf ("error code %d (%s): %s\n", WEXITSTATUS (status), escmd, *sterr);
			// eprintf ("(%s)\n", output);
			// eprintf ("%s: failed command '%s'\n", __func__, escmd);
			// free (escmd);
			ret = false;
		}

		if (len) {
			*len = out_len;
		}
		if (*sterr) {
			(*sterr)[err_len] = 0;
		}
		if (outputptr) {
			outputptr[out_len] = 0;
		}
		if (output) {
			*output = outputptr;
		} else {
			free (outputptr);
		}
		return ret;
	}
	return false;
}