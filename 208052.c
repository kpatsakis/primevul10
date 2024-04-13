KCleanup::expandVariables( const KFileInfo *	item,
			   const QString &	unexpanded ) const
{
    QString expanded = unexpanded;
    QString url = QString::fromLocal8Bit( item->url() ).replace("'", "'\\''");
    expanded.replace( QRegExp( "%p" ), "'" + url + "'" );
    QString name = QString::fromLocal8Bit( item->name() ).replace("'", "'\\''");
    expanded.replace( QRegExp( "%n" ), "'" + name + "'" );

   // if ( KDE::versionMajor() >= 3 && KDE::versionMinor() >= 4 )
	expanded.replace( QRegExp( "%t" ), "trash:/" );
    //else
	//expanded.replace( QRegExp( "%t" ), KGlobalSettings::trashPath() );

    return expanded;
}