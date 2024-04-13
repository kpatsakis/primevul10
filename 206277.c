part_write_bodystructure_siblings(const struct message_part *part,
				  string_t *dest, bool extended)
{
	for (; part != NULL; part = part->next) {
		str_append_c(dest, '(');
		imap_bodystructure_write(part, dest, extended);
		str_append_c(dest, ')');
	}
}