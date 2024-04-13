static void send_file(const char *path)
{
	struct stat st;

	if (stat(path, &st)) {
		switch (errno) {
		case ENOENT:
			cgit_print_error_page(404, "Not found", "Not found");
			break;
		case EACCES:
			cgit_print_error_page(403, "Forbidden", "Forbidden");
			break;
		default:
			cgit_print_error_page(400, "Bad request", "Bad request");
		}
		return;
	}
	ctx.page.mimetype = "application/octet-stream";
	ctx.page.filename = path;
	skip_prefix(path, ctx.repo->path, &ctx.page.filename);
	skip_prefix(ctx.page.filename, "/", &ctx.page.filename);
	cgit_print_http_headers();
	html_include(path);
}