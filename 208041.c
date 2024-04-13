KCleanup::execute( KFileInfo *item )
{
    if ( worksFor( item ) )
    {
	if ( _askForConfirmation && ! confirmation( item ) )
	    return;
	    
	KDirTree  * tree = item->tree();
	
	executeRecursive( item );
	
	switch ( _refreshPolicy )
	{
	    case noRefresh:
		// Do nothing.
		break;


	    case refreshThis:
		tree->refresh( item );
		break;


	    case refreshParent:
		tree->refresh( item->parent() );
		break;


	    case assumeDeleted:

		// Assume the cleanup action has deleted the item.
		// Modify the KDirTree accordingly.

		tree->deleteSubtree( item );

		// Don't try to figure out a reasonable next selection - the
		// views have to do that while handling the subtree
		// deletion. Only the views have any knowledge about a
		// reasonable strategy for choosing a next selection. Unlike
		// the view items, the KFileInfo items don't have an order that
		// makes any sense to the user.

		break;
	}
    }

    emit executed();
}