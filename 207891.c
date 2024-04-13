void KPasswordDlg::timeout()
{
	if ( timerMode )
	{
		label->setText( passwordQuery(TRUE) );
		if( stars )
			showStars();
		timerMode = 0;
		timer.start( 5000, TRUE );
	}
	else
		emit passCancel();
}