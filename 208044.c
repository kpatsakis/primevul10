KCleanup::saveConfig() const
{
    KConfigGroup config = KGlobal::config()->group(_id);

    config.writeEntry( "valid",		true			);
    config.writeEntry( "command",		_command		);
    config.writeEntry( "title",		_title			);
    config.writeEntry( "enabled",		_enabled		);
    config.writeEntry( "worksForDir",		_worksForDir		);
    config.writeEntry( "worksForFile",		_worksForFile		);
    config.writeEntry( "worksForDotEntry",	_worksForDotEntry	);
    config.writeEntry( "worksLocalOnly",	_worksLocalOnly		);
    config.writeEntry( "recurse",		_recurse		);
    config.writeEntry( "askForConfirmation",	_askForConfirmation	);
    config.writeEntry( "refreshPolicy",	(int) _refreshPolicy	);
}