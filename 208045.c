KCleanup::confirmation( KFileInfo * item )
{
    QString msg;

    if ( item->isDir() || item->isDotEntry() )
    {
	msg = i18n( "%1\nin directory %2" ).arg( cleanTitle() ).arg( item->url() );
    }
    else
    {
	msg = i18n( "%1\nfor file %2" ).arg( cleanTitle() ).arg( item->url() );
    }

    if ( KMessageBox::warningContinueCancel( 0,				// parentWidget
					     msg,			// message
					     i18n( "Please Confirm" ),	// caption
					     KGuiItem(i18n( "Confirm" ))		// confirmButtonLabel
					     ) == KMessageBox::Continue )
	return true;
    else
	return false;
}