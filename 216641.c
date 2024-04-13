static int cardos_set_file_attributes(sc_card_t *card, sc_file_t *file)
{
	int r;

	if (file->type_attr_len == 0) {
		u8 type[3];

		memset(type, 0, sizeof(type));
		type[0] = 0x00;
		switch (file->type) {
		case SC_FILE_TYPE_WORKING_EF:
			break;
		case SC_FILE_TYPE_DF:
			type[0] = 0x38;
			break;
		default:
			return SC_ERROR_NOT_SUPPORTED;
		}
		if (file->type != SC_FILE_TYPE_DF) {
			switch (file->ef_structure) {
			case SC_FILE_EF_LINEAR_FIXED_TLV:
			case SC_FILE_EF_LINEAR_VARIABLE:
			case SC_FILE_EF_CYCLIC_TLV:
				return SC_ERROR_NOT_SUPPORTED;
				/* No idea what this means, but it
				 * seems to be required for key
				 * generation. */
			case SC_FILE_EF_LINEAR_VARIABLE_TLV:
				type[1] = 0xff;
				/* fall through */
			default:
				type[0] |= file->ef_structure & 7;
				break;
			}
		}
		r = sc_file_set_type_attr(file, type, sizeof(type));
		if (r != SC_SUCCESS)
			return r;
	}
	if (file->prop_attr_len == 0) {
		u8 status[3];

		status[0] = 0x01;
		if (file->type == SC_FILE_TYPE_DF) {
			status[1] = (file->size >> 8) & 0xFF;
			status[2] = file->size & 0xFF;
		} else {
			status[1] = status[2] = 0x00; /* not used */
		}
		r = sc_file_set_prop_attr(file, status, sizeof(status));
		if (r != SC_SUCCESS)
			return r;
	}
	if (file->sec_attr_len == 0) {
		u8     acl[9];
		size_t blen = sizeof(acl);

		r = cardos_acl_to_bytes(card, file, acl, &blen);
		if (r != SC_SUCCESS)
			return r;
		r = sc_file_set_sec_attr(file, acl, blen);
		if (r != SC_SUCCESS)
			return r;
	}
	return SC_SUCCESS;
}