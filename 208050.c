KCleanup::readConfig()
{
    KConfigGroup config = KGlobal::config()->group(_id);

    bool valid = config.readEntry( "valid", false	);

    // If the config section requested exists, it should contain a
    // "valid" field with a true value. If not, there is no such
    // section within the config file. In this case, just leave this
    // cleanup action undisturbed - we'd rather have a good default
    // value (as provided - hopefully - by our application upon
    // startup) than a generic empty cleanup action.
   
    if ( valid )
    {
	_command		= config.readEntry ( "command"		);
	_enabled		= config.readEntry ( "enabled"		, false );
	_worksForDir		= config.readEntry ( "worksForDir"	, true  );
	_worksForFile		= config.readEntry ( "worksForFile"	, true  );
	_worksForDotEntry	= config.readEntry ( "worksForDotEntry"	, true  );
	_worksLocalOnly		= config.readEntry ( "worksLocalOnly"	, true  );
	_recurse		= config.readEntry ( "recurse"		, false	);
	_askForConfirmation	= config.readEntry ( "askForConfirmation"	, false	);
	_refreshPolicy		= (KCleanup::RefreshPolicy) config.readEntry( "refreshPolicy", 0);
	setTitle( config.readEntry( "title" ) );
    }
}