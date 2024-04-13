struct dict_connection *db_dict_init(const char *config_path)
{
	struct dict_settings dict_set;
	struct dict_settings_parser_ctx ctx;
	struct dict_connection *conn;
	const char *error;
	pool_t pool;

	conn = dict_conn_find(config_path);
	if (conn != NULL) {
		conn->refcount++;
		return conn;
	}

	if (*config_path == '\0')
		i_fatal("dict: Configuration file path not given");

	pool = pool_alloconly_create("dict_connection", 1024);
	conn = p_new(pool, struct dict_connection, 1);
	conn->pool = pool;

	conn->refcount = 1;

	conn->config_path = p_strdup(pool, config_path);
	conn->set = default_dict_settings;
	p_array_init(&conn->set.keys, pool, 8);
	p_array_init(&conn->set.passdb_fields, pool, 8);
	p_array_init(&conn->set.userdb_fields, pool, 8);
	p_array_init(&conn->set.parsed_passdb_objects, pool, 2);
	p_array_init(&conn->set.parsed_userdb_objects, pool, 2);

	i_zero(&ctx);
	ctx.conn = conn;
	if (!settings_read(config_path, NULL, parse_setting,
			   parse_section, &ctx, &error))
		i_fatal("dict %s: %s", config_path, error);
	db_dict_settings_parse(&conn->set);

	if (conn->set.uri == NULL)
		i_fatal("dict %s: Empty uri setting", config_path);

	i_zero(&dict_set);
	dict_set.username = "";
	dict_set.base_dir = global_auth_settings->base_dir;
	if (dict_init(conn->set.uri, &dict_set, &conn->dict, &error) < 0)
		i_fatal("dict %s: Failed to init dict: %s", config_path, error);

	conn->next = connections;
	connections = conn;
	return conn;
}