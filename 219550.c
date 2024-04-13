copy_comp_methods(gnutls_session_t session,
			  gnutls_buffer_st * cdata)
{
	int ret;
	uint8_t compression_methods[MAX_ALGOS], comp_num;
	size_t init_length = cdata->length;

	ret =
	    _gnutls_supported_compression_methods(session,
						  compression_methods,
						  MAX_ALGOS);
	if (ret < 0)
		return gnutls_assert_val(ret);

	comp_num = ret;

	/* put the number of compression methods */
	ret = _gnutls_buffer_append_prefix(cdata, 8, comp_num);
	if (ret < 0)
		return gnutls_assert_val(ret);

	ret =
	    _gnutls_buffer_append_data(cdata, compression_methods,
				       comp_num);
	if (ret < 0)
		return gnutls_assert_val(ret);

	ret = cdata->length - init_length;

	return ret;
}