KCleanup::cleanTitle() const
{
    // Use the cleanup action's title, if possible.
   
    QString title = _title;

    if ( title.isEmpty() )
    {
	title = _id;
    }

    // Get rid of any "&" characters in the text that denote keyboard
    // shortcuts in menus.
    title.replace( QRegExp( "&" ), "" );

    return title;
}