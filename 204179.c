static void print_pack_info(void)
{
	struct packed_git *pack;
	char *offset;

	ctx.page.mimetype = "text/plain";
	ctx.page.filename = "objects/info/packs";
	cgit_print_http_headers();
	reprepare_packed_git(the_repository);
	for (pack = get_packed_git(the_repository); pack; pack = pack->next) {
		if (pack->pack_local) {
			offset = strrchr(pack->pack_name, '/');
			if (offset && offset[1] != '\0')
				++offset;
			else
				offset = pack->pack_name;
			htmlf("P %s\n", offset);
		}
	}
}