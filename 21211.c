addentry(struct magic_set *ms, struct magic_entry *me,
   struct magic_entry_set *mset)
{
	size_t i = me->mp->type == FILE_NAME ? 1 : 0;
	if (mset[i].count == mset[i].max) {
		struct magic_entry *mp;

		mset[i].max += ALLOC_INCR;
		if ((mp = CAST(struct magic_entry *,
		    erealloc(mset[i].me, sizeof(*mp) * mset[i].max))) ==
		    NULL) {
			file_oomem(ms, sizeof(*mp) * mset[i].max);
			return -1;
		}
		(void)memset(&mp[mset[i].count], 0, sizeof(*mp) *
		    ALLOC_INCR);
		mset[i].me = mp;
	}
	mset[i].me[mset[i].count++] = *me;
	memset(me, 0, sizeof(*me));
	return 0;
}