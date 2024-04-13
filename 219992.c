static int copyI18NEntry(Header h, indexEntry entry, rpmtd td, 
						headerGetFlags flags)
{
    const char *lang, *l, *le;
    indexEntry table;

    td->type = RPM_STRING_TYPE;
    td->count = 1;
    /* if no match, just return the first string */
    td->data = entry->data;

    /* XXX Drepper sez' this is the order. */
    if ((lang = getenv("LANGUAGE")) == NULL &&
	(lang = getenv("LC_ALL")) == NULL &&
	(lang = getenv("LC_MESSAGES")) == NULL &&
	(lang = getenv("LANG")) == NULL)
	    goto exit;
    
    if ((table = findEntry(h, RPMTAG_HEADERI18NTABLE, RPM_STRING_ARRAY_TYPE)) == NULL)
	goto exit;

    for (l = lang; *l != '\0'; l = le) {
	const char *t;
	char *ed, *ed_weak = NULL;
	int langNum;

	while (*l && *l == ':')			/* skip leading colons */
	    l++;
	if (*l == '\0')
	    break;
	for (le = l; *le && *le != ':'; le++)	/* find end of this locale */
	    {};

	/* For each entry in the header ... */
	for (langNum = 0, t = table->data, ed = entry->data;
	     langNum < entry->info.count;
	     langNum++, t += strlen(t) + 1, ed += strlen(ed) + 1) {

	    int match = headerMatchLocale(t, l, le);
	    if (match == 1) {
		td->data = ed;
		goto exit;
	    } else if (match == 2) { 
		ed_weak = ed;
	    }
	}
	if (ed_weak) {
	    td->data = ed_weak;
	    goto exit;
	}
    }

exit:
    if (flags & HEADERGET_ALLOC) {
	td->data = xstrdup(td->data);
	td->flags |= RPMTD_ALLOCED;
    }

    return 1;
}