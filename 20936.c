parse_options(const char *data, struct parsed_mount_info *parsed_info)
{
	char *value = NULL;
	char *equals = NULL;
	char *next_keyword = NULL;
	char *out = parsed_info->options;
	unsigned long *filesys_flags = &parsed_info->flags;
	int out_len = 0;
	int word_len;
	int rc = 0;
	int got_uid = 0;
	int got_cruid = 0;
	int got_gid = 0;
	uid_t uid, cruid = 0;
	gid_t gid;
	char *ep;
	struct passwd *pw;
	struct group *gr;
	/*
	 * max 32-bit uint in decimal is 4294967295 which is 10 chars wide
	 * +1 for NULL, and +1 for good measure
	 */
	char txtbuf[12];

	/* make sure we're starting from beginning */
	out[0] = '\0';

	/* BB fixme check for separator override BB */
	uid = getuid();
	if (uid != 0)
		got_uid = 1;

	gid = getgid();
	if (gid != 0)
		got_gid = 1;

	if (!data)
		return EX_USAGE;

	/*
	 * format is keyword,keyword2=value2,keyword3=value3... 
	 * data  = next keyword
	 * value = next value ie stuff after equal sign
	 */
	while (data && *data) {
		next_keyword = strchr(data, ',');	/* BB handle sep= */

		/* temporarily null terminate end of keyword=value pair */
		if (next_keyword)
			*next_keyword++ = 0;

		/* temporarily null terminate keyword if there's a value */
		value = NULL;
		if ((equals = strchr(data, '=')) != NULL) {
			*equals = '\0';
			value = equals + 1;
		}

		switch(parse_opt_token(data)) {
		case OPT_USERS:
			if (!value || !*value) {
				*filesys_flags |= MS_USERS;
				goto nocopy;
			}
			break;

		case OPT_USER:
			if (!value || !*value) {
				if (data[4] == '\0') {
					*filesys_flags |= MS_USER;
					goto nocopy;
				} else {
					fprintf(stderr,
						"username specified with no parameter\n");
					return EX_USAGE;
				}
			} else {
				/* domain/username%password */
				const int max = MAX_DOMAIN_SIZE +
						MAX_USERNAME_SIZE +
						MOUNT_PASSWD_SIZE + 2;
				if (strnlen(value, max + 1) >= max + 1) {
					fprintf(stderr, "username too long\n");
					return EX_USAGE;
				}
				rc = parse_username(value, parsed_info);
				if (rc) {
					fprintf(stderr,
						"problem parsing username\n");
					return rc;
				}
				goto nocopy;
			}

		case OPT_PASS:
			if (parsed_info->got_password) {
				fprintf(stderr,
					"password specified twice, ignoring second\n");
				goto nocopy;
			}
			if (!value || !*value) {
				parsed_info->got_password = 1;
				goto nocopy;
			}
			rc = set_password(parsed_info, value);
			if (rc)
				return rc;
			goto nocopy;

		case OPT_SEC:
			if (value) {
				if (!strncmp(value, "none", 4) ||
				    !strncmp(value, "krb5", 4))
					parsed_info->got_password = 1;
			}
			break;

		case OPT_IP:
			if (!value || !*value) {
				fprintf(stderr,
					"target ip address argument missing\n");
			} else if (strnlen(value, MAX_ADDRESS_LEN) <=
				MAX_ADDRESS_LEN) {
				strcpy(parsed_info->addrlist, value);
				if (parsed_info->verboseflag)
					fprintf(stderr,
						"ip address %s override specified\n",
						value);
				goto nocopy;
			} else {
				fprintf(stderr, "ip address too long\n");
				return EX_USAGE;

			}
			break;

		/* unc || target || path */
		case OPT_UNC:
			if (!value || !*value) {
				fprintf(stderr,
					"invalid path to network resource\n");
				return EX_USAGE;
			}
			rc = parse_unc(value, parsed_info);
			if (rc)
				return rc;
			break;

		/* dom || workgroup */
		case OPT_DOM:
			if (!value || !*value) {
				fprintf(stderr, "CIFS: invalid domain name\n");
				return EX_USAGE;
			}
			if (strnlen(value, sizeof(parsed_info->domain)) >=
			    sizeof(parsed_info->domain)) {
				fprintf(stderr, "domain name too long\n");
				return EX_USAGE;
			}
			strlcpy(parsed_info->domain, value,
				sizeof(parsed_info->domain));
			goto nocopy;

		case OPT_CRED:
			if (!value || !*value) {
				fprintf(stderr,
					"invalid credential file name specified\n");
				return EX_USAGE;
			}
			rc = open_cred_file(value, parsed_info);
			if (rc) {
				fprintf(stderr,
					"error %d (%s) opening credential file %s\n",
					rc, strerror(rc), value);
				return rc;
			}
			break;

		case OPT_UID:
			if (!value || !*value)
				goto nocopy;

			got_uid = 1;
			errno = 0;
			uid = strtoul(value, &ep, 10);
			if (errno == 0)
				goto nocopy;

			pw = getpwnam(value);
			if (pw == NULL) {
				fprintf(stderr, "bad user name \"%s\"\n", value);
				return EX_USAGE;
			}

			uid = pw->pw_uid;
			goto nocopy;

		case OPT_CRUID:
			if (!value || !*value)
				goto nocopy;

			got_cruid = 1;
			errno = 0;
			cruid = strtoul(value, &ep, 10);
			if (errno == 0)
				goto nocopy;

			pw = getpwnam(value);
			if (pw == NULL) {
				fprintf(stderr, "bad user name \"%s\"\n", value);
				return EX_USAGE;
			}
			cruid = pw->pw_uid;
			goto nocopy;

		case OPT_GID:
			if (!value || !*value)
				goto nocopy;

			got_gid = 1;
			errno = 0;
			gid = strtoul(value, &ep, 10);
			if (errno == 0)
				goto nocopy;

			gr = getgrnam(value);
			if (gr == NULL) {
				fprintf(stderr, "bad group name \"%s\"\n", value);
				return EX_USAGE;
			}

			gid = gr->gr_gid;
			goto nocopy;

		/* fmask fall through to file_mode */
		case OPT_FMASK:
			fprintf(stderr,
				"WARNING: CIFS mount option 'fmask' is\
				 deprecated. Use 'file_mode' instead.\n");
			data = "file_mode";	/* BB fix this */
		case OPT_FILE_MODE:
			if (!value || !*value) {
				fprintf(stderr,
					"Option '%s' requires a numerical argument\n",
					data);
				return EX_USAGE;
			}

			if (value[0] != '0')
				fprintf(stderr,
					"WARNING: '%s' not expressed in octal.\n",
					data);
			break;

		/* dmask falls through to dir_mode */
		case OPT_DMASK:
			fprintf(stderr,
				"WARNING: CIFS mount option 'dmask' is\
				 deprecated. Use 'dir_mode' instead.\n");
			data = "dir_mode";
		case OPT_DIR_MODE:
			if (!value || !*value) {
				fprintf(stderr,
					"Option '%s' requires a numerical argument\n",
					data);
				return EX_USAGE;
			}

			if (value[0] != '0')
				fprintf(stderr,
					"WARNING: '%s' not expressed in octal.\n",
					data);
			break;

		/* the following mount options should be
		   stripped out from what is passed into the kernel
		   since these options are best passed as the
		   mount flags rather than redundantly to the kernel
		   and could generate spurious warnings depending on the
		   level of the corresponding cifs vfs kernel code */
		case OPT_NO_SUID:
			*filesys_flags |= MS_NOSUID;
			break;
		case OPT_SUID:
			*filesys_flags &= ~MS_NOSUID;
			break;
		case OPT_NO_DEV:
			*filesys_flags |= MS_NODEV;
			break;
		/* nolock || nobrl */
		case OPT_NO_LOCK:
			*filesys_flags &= ~MS_MANDLOCK;
			break;
		case OPT_MAND:
			*filesys_flags |= MS_MANDLOCK;
			goto nocopy;
		case OPT_NOMAND:
			*filesys_flags &= ~MS_MANDLOCK;
			goto nocopy;
		case OPT_DEV:
			*filesys_flags &= ~MS_NODEV;
			break;
		case OPT_NO_EXEC:
			*filesys_flags |= MS_NOEXEC;
			break;
		case OPT_EXEC:
			*filesys_flags &= ~MS_NOEXEC;
			break;
		case OPT_GUEST:
			parsed_info->got_user = 1;
			parsed_info->got_password = 1;
			break;
		case OPT_RO:
			*filesys_flags |= MS_RDONLY;
			goto nocopy;
		case OPT_RW:
			*filesys_flags &= ~MS_RDONLY;
			goto nocopy;
		case OPT_REMOUNT:
			*filesys_flags |= MS_REMOUNT;
			break;
		case OPT_IGNORE:
			goto nocopy;
		}

		/* check size before copying option to buffer */
		word_len = strlen(data);
		if (value)
			word_len += 1 + strlen(value);

		/* need 2 extra bytes for comma and null byte */
		if (out_len + word_len + 2 > MAX_OPTIONS_LEN) {
			fprintf(stderr, "Options string too long\n");
			return EX_USAGE;
		}

		/* put back equals sign, if any */
		if (equals)
			*equals = '=';

		/* go ahead and copy */
		if (out_len)
			strlcat(out, ",", MAX_OPTIONS_LEN);

		strlcat(out, data, MAX_OPTIONS_LEN);
		out_len = strlen(out);
nocopy:
		data = next_keyword;
	}

	/* special-case the uid and gid */
	if (got_uid) {
		word_len = snprintf(txtbuf, sizeof(txtbuf), "%u", uid);

		/* comma + "uid=" + terminating NULL == 6 */
		if (out_len + word_len + 6 > MAX_OPTIONS_LEN) {
			fprintf(stderr, "Options string too long\n");
			return EX_USAGE;
		}

		if (out_len) {
			strlcat(out, ",", MAX_OPTIONS_LEN);
			out_len++;
		}
		snprintf(out + out_len, word_len + 5, "uid=%s", txtbuf);
		out_len = strlen(out);
	}
	if (got_cruid) {
		word_len = snprintf(txtbuf, sizeof(txtbuf), "%u", cruid);

		/* comma + "cruid=" + terminating NULL == 6 */
		if (out_len + word_len + 8 > MAX_OPTIONS_LEN) {
			fprintf(stderr, "Options string too long\n");
			return EX_USAGE;
		}

		if (out_len) {
			strlcat(out, ",", MAX_OPTIONS_LEN);
			out_len++;
		}
		snprintf(out + out_len, word_len + 7, "cruid=%s", txtbuf);
		out_len = strlen(out);
	}
	if (got_gid) {
		word_len = snprintf(txtbuf, sizeof(txtbuf), "%u", gid);

		/* comma + "gid=" + terminating NULL == 6 */
		if (out_len + word_len + 6 > MAX_OPTIONS_LEN) {
			fprintf(stderr, "Options string too long\n");
			return EX_USAGE;
		}

		if (out_len) {
			strlcat(out, ",", MAX_OPTIONS_LEN);
			out_len++;
		}
		snprintf(out + out_len, word_len + 5, "gid=%s", txtbuf);
	}
	return 0;
}