int hfs_cat_keycmp(const btree_key *key1, const btree_key *key2)
{
	int retval;

	retval = be32_to_cpu(key1->cat.ParID) - be32_to_cpu(key2->cat.ParID);
	if (!retval)
		retval = hfs_strcmp(key1->cat.CName.name, key1->cat.CName.len,
				    key2->cat.CName.name, key2->cat.CName.len);

	return retval;
}