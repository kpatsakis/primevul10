gnutls_priority_string_list(unsigned iter, unsigned int flags)
{
	if (flags & GNUTLS_PRIORITY_LIST_INIT_KEYWORDS) {
		if (iter >= (sizeof(pgroups)/sizeof(pgroups[0]))-1)
			return NULL;
		return pgroups[iter].name;
	} else if (flags & GNUTLS_PRIORITY_LIST_SPECIAL) {
		if (iter >= (sizeof(wordlist)/sizeof(wordlist[0]))-1)
			return NULL;
		return wordlist[iter].name;
	}
	return NULL;
}