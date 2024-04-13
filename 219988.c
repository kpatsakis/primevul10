int headerMod(Header h, rpmtd td)
{
    indexEntry entry;
    rpm_data_t oldData;
    rpm_data_t data;
    int length = 0;

    /* First find the tag */
    entry = findEntry(h, td->tag, td->type);
    if (!entry)
	return 0;

    data = grabData(td->type, td->data, td->count, &length);
    if (data == NULL)
	return 0;

    /* make sure entry points to the first occurrence of this tag */
    while (entry > h->index && (entry - 1)->info.tag == td->tag)  
	entry--;

    /* free after we've grabbed the new data in case the two are intertwined;
       that's a bad idea but at least we won't break */
    oldData = entry->data;

    entry->info.count = td->count;
    entry->info.type = td->type;
    entry->data = data;
    entry->length = length;

    if (ENTRY_IN_REGION(entry)) {
	entry->info.offset = 0;
    } else
	free(oldData);

    return 1;
}