KCleanup::KCleanup( QString		id,
		    QString		command,
		    QString		title,
		    KActionCollection *	parent	)
    
    : KAction( title,
	       parent)
    
    , _id	( id	  )
    , _command	( command )
    , _title	( title	  )
{
    _selection		= 0;
    _enabled		= true;
    _worksForDir	= true;
    _worksForFile	= false;
    _worksForDotEntry	= false;
    _worksLocalOnly	= true;
    _recurse		= false;
    _askForConfirmation	= false;
    _refreshPolicy	= noRefresh;
    
    KAction::setEnabled( false );
    parent->addAction(id,this);

    connect(this, SIGNAL(triggered()),
            this, SLOT(executeWithSelection()));
}