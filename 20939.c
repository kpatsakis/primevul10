get_password_from_file(int file_descript, char *filename,
		       struct parsed_mount_info *parsed_info)
{
	int rc = 0;
	char buf[sizeof(parsed_info->password) + 1];

	if (filename != NULL) {
		rc = toggle_dac_capability(0, 1);
		if (rc)
			return rc;

		rc = access(filename, R_OK);
		if (rc) {
			fprintf(stderr,
				"mount.cifs failed: access check of %s failed: %s\n",
				filename, strerror(errno));
			toggle_dac_capability(0, 0);
			return EX_SYSERR;
		}

		file_descript = open(filename, O_RDONLY);
		if (file_descript < 0) {
			fprintf(stderr,
				"mount.cifs failed. %s attempting to open password file %s\n",
				strerror(errno), filename);
			toggle_dac_capability(0, 0);
			return EX_SYSERR;
		}

		rc = toggle_dac_capability(0, 0);
		if (rc) {
			rc = EX_SYSERR;
			goto get_pw_exit;
		}
	}

	memset(buf, 0, sizeof(buf));
	rc = read(file_descript, buf, sizeof(buf) - 1);
	if (rc < 0) {
		fprintf(stderr,
			"mount.cifs failed. Error %s reading password file\n",
			strerror(errno));
		rc = EX_SYSERR;
		goto get_pw_exit;
	}

	rc = set_password(parsed_info, buf);

get_pw_exit:
	if (filename != NULL)
		close(file_descript);
	return rc;
}