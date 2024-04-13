KCleanup::worksFor( KFileInfo *item ) const
{
    if ( ! _enabled || ! item )
	return false;

    if ( worksLocalOnly() && ! item->tree()->isFileProtocol() )
	return false;

    if	( item->isDotEntry() )	return worksForDotEntry();
    if	( item->isDir() )	return worksForDir();
    
    return worksForFile();
}