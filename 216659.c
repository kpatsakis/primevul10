static int acl_to_byte(const sc_acl_entry_t *e)
{
	if (e != NULL) {
		switch (e->method) {
		case SC_AC_NONE:
			return 0x00;
		case SC_AC_NEVER:
			return 0xFF;
		case SC_AC_CHV:
		case SC_AC_TERM:
		case SC_AC_AUT:
			if (e->key_ref == SC_AC_KEY_REF_NONE)
				return -1;
			if (e->key_ref > 0x7F)
				return -1;
			return e->key_ref;
		}
	}
        return 0x00;
}