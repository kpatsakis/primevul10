fr_archive_libarchive_init (FrArchiveLibarchive *self)
{
	FrArchive *base = FR_ARCHIVE (self);

	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, FR_TYPE_ARCHIVE_LIBARCHIVE, FrArchiveLibarchivePrivate);

	base->propAddCanReplace = TRUE;
	base->propAddCanUpdate = TRUE;
	base->propAddCanStoreFolders = TRUE;
	base->propAddCanStoreLinks = TRUE;
	base->propExtractCanAvoidOverwrite = TRUE;
	base->propExtractCanSkipOlder = TRUE;
	base->propExtractCanJunkPaths = TRUE;
	base->propCanExtractAll = TRUE;
	base->propCanDeleteNonEmptyFolders = TRUE;
	base->propCanExtractNonEmptyFolders = TRUE;
}