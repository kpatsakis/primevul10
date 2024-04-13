void cgit_clone_objects(void)
{
	char *p;

	if (!ctx.qry.path)
		goto err;

	if (!strcmp(ctx.qry.path, "info/packs")) {
		print_pack_info();
		return;
	}

	/* Avoid directory traversal by forbidding "..", but also work around
	 * other funny business by just specifying a fairly strict format. For
	 * example, now we don't have to stress out about the Cygwin port.
	 */
	for (p = ctx.qry.path; *p; ++p) {
		if (*p == '.' && *(p + 1) == '.')
			goto err;
		if (!isalnum(*p) && *p != '/' && *p != '.' && *p != '-')
			goto err;
	}

	send_file(git_path("objects/%s", ctx.qry.path));
	return;

err:
	cgit_print_error_page(400, "Bad request", "Bad request");
}