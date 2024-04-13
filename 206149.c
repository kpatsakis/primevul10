imap_parser_create(struct istream *input, struct ostream *output,
		   size_t max_line_size)
{
	struct imap_parser *parser;

	parser = i_new(struct imap_parser, 1);
	parser->refcount = 1;
	parser->pool = pool_alloconly_create(MEMPOOL_GROWING"IMAP parser",
					     1024);
	parser->input = input;
	parser->output = output;
	parser->max_line_size = max_line_size;

	p_array_init(&parser->root_list, parser->pool, LIST_INIT_COUNT);
	parser->cur_list = &parser->root_list;
	return parser;
}