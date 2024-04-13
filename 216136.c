R_API ut8 r_str_contains_macro(const char *input_value) {
	char *has_tilde = input_value ? HASCH('~') : NULL,
		 *has_bang = input_value ? HASCH('!') : NULL,
		 *has_brace = input_value ? CAST(HASCH('[') || HASCH(']')) : NULL,
		 *has_paren = input_value ? CAST(HASCH('(') || HASCH(')')) : NULL,
		 *has_cbrace = input_value ? CAST(HASCH('{') || HASCH('}')) : NULL,
		 *has_qmark = input_value ? HASCH('?') : NULL,
		 *has_colon = input_value ? HASCH(':') : NULL,
		 *has_at = input_value ? strchr (input_value, '@') : NULL;

	return has_tilde || has_bang || has_brace || has_cbrace || has_qmark \
		|| has_paren || has_colon || has_at;
}