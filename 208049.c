KCleanup::copy( const KCleanup &src )
{
    setTitle( src.title() );
    _selection		= src.selection();
    _id			= src.id();
    _command		= src.command();
    _enabled		= src.enabled();
    _worksForDir	= src.worksForDir();
    _worksForFile	= src.worksForFile();
    _worksForDotEntry	= src.worksForDotEntry();
    _worksLocalOnly	= src.worksLocalOnly();
    _recurse		= src.recurse();
    _askForConfirmation	= src.askForConfirmation();
    _refreshPolicy	= src.refreshPolicy();
}