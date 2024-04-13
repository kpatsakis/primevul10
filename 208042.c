KCleanup::selectionChanged( KFileInfo *selection )
{
    bool enabled = false;
    _selection = selection;

    if ( selection )
    {
	enabled = worksFor( selection );

	if ( ! selection->isFinished() )
	{
	    // This subtree isn't finished reading yet

	    switch ( _refreshPolicy )
	    {
		// Refresh policies that would cause this subtree to be deleted
		case refreshThis:
		case refreshParent:
		case assumeDeleted:

		    // Prevent premature deletion of this tree - this would
		    // cause a core dump for sure. 
		    enabled = false;
		    break;
			
		default:
		    break;
	    }
	    
	}
    }
    
    KAction::setEnabled( enabled );
}