R_API bool r_core_serve(RCore *core, RIODesc *file) {
	ut8 cmd, flg, *ptr = NULL, buf[1024];
	int i, pipefd = -1;
	ut64 x;

	RIORap *rior = (RIORap *)file->data;
	if (!rior|| !rior->fd) {
		eprintf ("rap: cannot listen.\n");
		return false;
	}
	RSocket *fd = rior->fd;
	eprintf ("RAP Server started (rap.loop=%s)\n",
			r_config_get (core->config, "rap.loop"));
	r_cons_break_push (rap_break, rior);
reaccept:
	while (!r_cons_is_breaked ()) {
		RSocket *c = r_socket_accept (fd);
		if (!c) {
			break;
		}
		if (r_cons_is_breaked ()) {
			goto out_of_function;
		}
		if (!c) {
			eprintf ("rap: cannot accept\n");
			r_socket_free (c);
			goto out_of_function;
		}
		eprintf ("rap: client connected\n");
		for (;!r_cons_is_breaked ();) {
			if (!r_socket_read (c, &cmd, 1)) {
				eprintf ("rap: connection closed\n");
				if (r_config_get_i (core->config, "rap.loop")) {
					eprintf ("rap: waiting for new connection\n");
					r_socket_free (c);
					goto reaccept;
				}
				goto out_of_function;
			}
			switch ((ut8)cmd) {
			case RMT_OPEN:
				r_socket_read_block (c, &flg, 1); // flags
				eprintf ("open (%d): ", cmd);
				r_socket_read_block (c, &cmd, 1); // len
				pipefd = -1;
				ptr = malloc (cmd + 1);
				//XXX cmd is ut8..so <256 if (cmd<RMT_MAX)
				if (!ptr) {
					eprintf ("Cannot malloc in rmt-open len = %d\n", cmd);
				} else {
					ut64 baddr = r_config_get_i (core->config, "bin.laddr");
					r_socket_read_block (c, ptr, cmd);
					ptr[cmd] = 0;
					ut32 perm = R_PERM_R;
					if (flg & R_PERM_W) {
						perm |= R_PERM_W;
					}
					if (r_core_file_open (core, (const char *)ptr, perm, 0)) {
						int fd = r_io_fd_get_current (core->io);
						r_core_bin_load (core, NULL, baddr);
						r_io_map_add (core->io, fd, perm, 0, 0, r_io_fd_size (core->io, fd));
						if (core->file) {
							pipefd = fd;
						} else {
							pipefd = -1;
						}
						eprintf ("(flags: %d) len: %d filename: '%s'\n",
							flg, cmd, ptr); //config.file);
					} else {
						pipefd = -1;
						eprintf ("Cannot open file (%s)\n", ptr);
						r_socket_close (c);
						if (r_config_get_i (core->config, "rap.loop")) {
							eprintf ("rap: waiting for new connection\n");
							r_socket_free (c);
							goto reaccept;
						}
						goto out_of_function; //XXX: Close connection and goto accept
					}
				}
				buf[0] = RMT_OPEN | RMT_REPLY;
				r_write_be32 (buf + 1, pipefd);
				r_socket_write (c, buf, 5);
				r_socket_flush (c);
				R_FREE (ptr);
				break;
			case RMT_READ:
				r_socket_read_block (c, (ut8*)&buf, 4);
				i = r_read_be32 (buf);
				ptr = (ut8 *)malloc (i + core->blocksize + 5);
				if (ptr) {
					r_core_block_read (core);
					ptr[0] = RMT_READ | RMT_REPLY;
					if (i > RMT_MAX) {
						i = RMT_MAX;
					}
					if (i > core->blocksize) {
						r_core_block_size (core, i);
					}
					if (i + 128 < core->blocksize) {
						r_core_block_size (core, i);
					}
					r_write_be32 (ptr + 1, i);
					memcpy (ptr + 5, core->block, i); //core->blocksize);
					r_socket_write (c, ptr, i + 5);
					r_socket_flush (c);
					R_FREE (ptr);
				} else {
					eprintf ("Cannot read %d byte(s)\n", i);
					r_socket_free (c);
					// TODO: reply error here
					goto out_of_function;
				}
				break;
			case RMT_CMD:
				{
				char *cmd = NULL, *cmd_output = NULL;
				char bufr[8], *bufw = NULL;
				ut32 cmd_len = 0;
				int i;

				/* read */
				r_socket_read_block (c, (ut8*)&bufr, 4);
				i = r_read_be32 (bufr);
				if (i > 0 && i < RMT_MAX) {
					if ((cmd = malloc (i + 1))) {
						r_socket_read_block (c, (ut8*)cmd, i);
						cmd[i] = '\0';
						int scr_interactive = r_config_get_i (core->config, "scr.interactive");
						r_config_set_i (core->config, "scr.interactive", 0);
						cmd_output = r_core_cmd_str (core, cmd);
						r_config_set_i (core->config, "scr.interactive", scr_interactive);
						free (cmd);
					} else {
						eprintf ("rap: cannot malloc\n");
					}
				} else {
					eprintf ("rap: invalid length '%d'\n", i);
				}
				/* write */
				if (cmd_output) {
					cmd_len = strlen (cmd_output) + 1;
				} else {
					cmd_output = strdup ("");
					cmd_len = 0;
				}
#if DEMO_SERVER_SENDS_CMD_TO_CLIENT
				static bool once = true;
				/* TODO: server can reply a command request to the client only here */
				if (once) {
					const char *cmd = "pd 4";
					int cmd_len = strlen (cmd) + 1;
					ut8 *b = malloc (cmd_len + 5);
					b[0] = RMT_CMD;
					r_write_be32 (b + 1, cmd_len);
					strcpy ((char *)b+ 5, cmd);
					r_socket_write (c, b, 5 + cmd_len);
					r_socket_flush (c);

					/* read response */
					r_socket_read (c, b, 5);
					if (b[0] == (RMT_CMD | RMT_REPLY)) {
						ut32 n = r_read_be32 (b + 1);
						eprintf ("REPLY %d\n", n);
						if (n > 0) {
							ut8 *res = calloc (1, n);
							r_socket_read (c, res, n);
							eprintf ("RESPONSE(%s)\n", (const char *)res);
							free (res);
						}
					}
					r_socket_flush (c);
					free (b);
					once = false;
				}
#endif
				bufw = malloc (cmd_len + 5);
				bufw[0] = (ut8) (RMT_CMD | RMT_REPLY);
				r_write_be32 (bufw + 1, cmd_len);
				memcpy (bufw + 5, cmd_output, cmd_len);
				r_socket_write (c, bufw, cmd_len+5);
				r_socket_flush (c);
				free (bufw);
				free (cmd_output);
				break;
				}
			case RMT_WRITE:
				r_socket_read (c, buf, 4);
				x = r_read_at_be32 (buf, 0);
				ptr = malloc (x);
				r_socket_read (c, ptr, x);
				int ret = r_core_write_at (core, core->offset, ptr, x);
				buf[0] = RMT_WRITE | RMT_REPLY;
				r_write_be32 (buf + 1, ret);
				r_socket_write (c, buf, 5);
				r_socket_flush (c);
				R_FREE (ptr);
				break;
			case RMT_SEEK:
				r_socket_read_block (c, buf, 9);
				x = r_read_at_be64 (buf, 1);
				if (buf[0] == 2) {
					if (core->file) {
						x = r_io_fd_size (core->io, core->file->fd);
					} else {
						x = 0;
					}
				} else {
					if (buf[0] == 0) {
						r_core_seek (core, x, 1); //buf[0]);
					}
					x = core->offset;
				}
				buf[0] = RMT_SEEK | RMT_REPLY;
				r_write_be64 (buf + 1, x);
				r_socket_write (c, buf, 9);
				r_socket_flush (c);
				break;
			case RMT_CLOSE:
				// XXX : proper shutdown
				r_socket_read_block (c, buf, 4);
				i = r_read_be32 (buf);
				{
				//FIXME: Use r_socket_close
				int ret = close (i);
				r_write_be32 (buf + 1, ret);
				buf[0] = RMT_CLOSE | RMT_REPLY;
				r_socket_write (c, buf, 5);
				r_socket_flush (c);
				}
				break;
			default:
				if (cmd == 'G') {
					// silly http emulation over rap://
					char line[256] = {0};
					char *cmd = line;
					r_socket_read (c, (ut8*)line, sizeof (line));
					if (!strncmp (line, "ET /cmd/", 8)) {
						cmd = line + 8;
						char *http = strstr (cmd, "HTTP");
						if (http) {
							*http = 0;
							http--;
							if (*http == ' ') {
								*http = 0;
							}
						}
						r_str_uri_decode (cmd);
						char *res = r_core_cmd_str (core, cmd);
						if (res) {
							r_socket_printf (c, "HTTP/1.0 %d %s\r\n%s"
									"Connection: close\r\nContent-Length: %d\r\n\r\n",
									200, "OK", "", -1); // strlen (res));
							r_socket_write (c, res, strlen (res));
							free (res);
						}
						r_socket_flush (c);
						r_socket_close (c);
					}
				} else {
					eprintf ("[r2p] unknown command 0x%02x\n", cmd);
					r_socket_close (c);
					R_FREE (ptr);
				}
				if (r_config_get_i (core->config, "rap.loop")) {
					eprintf ("rap: waiting for new connection\n");
					r_socket_free (c);
					goto reaccept;
				}
				goto out_of_function;
			}
		}
		eprintf ("client: disconnected\n");
		r_socket_free (c);
	}
out_of_function:
	r_cons_break_pop ();
	return false;
}