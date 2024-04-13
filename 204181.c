void cgit_clone_info(void)
{
	if (!ctx.qry.path || strcmp(ctx.qry.path, "refs")) {
		cgit_print_error_page(400, "Bad request", "Bad request");
		return;
	}

	ctx.page.mimetype = "text/plain";
	ctx.page.filename = "info/refs";
	cgit_print_http_headers();
	for_each_ref(print_ref_info, NULL);
}