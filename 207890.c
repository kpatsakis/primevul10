void KPasswordDlg::showStars()
{
	QString s;
	
	s.fill( '*', password.length() );
	if( blink )
		s += "_";
		
	entry->setText( s );	
}