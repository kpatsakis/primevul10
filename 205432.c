int db_dict_value_iter_init(struct dict_connection *conn,
			    struct auth_request *auth_request,
			    const ARRAY_TYPE(db_dict_field) *fields,
			    const ARRAY_TYPE(db_dict_key_p) *objects,
			    struct db_dict_value_iter **iter_r)
{
	struct db_dict_value_iter *iter;
	struct db_dict_iter_key *iterkey;
	const struct db_dict_key *key;
	pool_t pool;
	int ret;

	pool = pool_alloconly_create(MEMPOOL_GROWING"auth dict lookup", 1024);
	iter = p_new(pool, struct db_dict_value_iter, 1);
	iter->pool = pool;
	iter->conn = conn;
	iter->fields = fields;
	iter->objects = objects;
	iter->tmpstr = str_new(pool, 128);
	iter->auth_request = auth_request;
	iter->var_expand_table = auth_request_get_var_expand_table(auth_request, NULL);

	/* figure out what keys we need to lookup, and lookup them */
	p_array_init(&iter->keys, pool, array_count(&conn->set.keys));
	array_foreach(&conn->set.keys, key) {
		iterkey = array_append_space(&iter->keys);
		struct db_dict_key *new_key = p_new(iter->pool, struct db_dict_key, 1);
		memcpy(new_key, key, sizeof(struct db_dict_key));
		string_t *expanded_key = str_new(iter->pool, strlen(key->key));
		const char *error;
		if (auth_request_var_expand_with_table(expanded_key, key->key, auth_request,
						       iter->var_expand_table,
						       NULL, &error) <= 0) {
			auth_request_log_error(iter->auth_request, AUTH_SUBSYS_DB,
				"Failed to expand key %s: %s", key->key, error);
			pool_unref(&pool);
			return -1;
		}
		new_key->key = str_c(expanded_key);
		iterkey->key = new_key;
	}
	T_BEGIN {
		db_dict_iter_find_used_keys(iter);
		db_dict_iter_find_used_objects(iter);
		ret = db_dict_iter_lookup_key_values(iter);
	} T_END;
	if (ret <= 0) {
		pool_unref(&pool);
		return ret;
	}
	*iter_r = iter;
	return 1;
}