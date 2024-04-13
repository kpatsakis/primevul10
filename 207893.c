KPasswordDlg::KPasswordDlg( QWidget *parent, bool s ) : QWidget( parent )
{
	setCursor( arrowCursor );

	stars = s;
	password = "";

	QFont font( "helvetica", 18 );
	QFontMetrics fm(font);
	QString query = passwordQuery(TRUE); /* Two lines of text */

	int qwidth = fm.width((const char *)query);
	int qheight = fm.height();

	QFrame *frame = new QFrame( this );
	frame->setFrameStyle( QFrame::Panel | QFrame::Raised );
	frame->setLineWidth( 2 );
	frame->setGeometry( 0, 0, qwidth + 2*20, qheight * 3 + 2 * 20 );

	KApplication::getKApplication()->getCharsets()->setQFont(font);
	label = new QLabel( query, frame );
	label->setFont( font );
	label->setGeometry( 20, 20, qwidth, qheight * 2 );
	label->setAlignment( AlignCenter );
	
	font.setPointSize( 16 );
	KApplication::getKApplication()->getCharsets()->setQFont(font);
	entry = new QLabel( "", frame );
	entry->setFont( font );	
	entry->setGeometry( 20, 20 + qheight * 2, qwidth, qheight );

	resize( qwidth + 2*20, qheight * 3 + 2*20 );

	connect( &timer, SIGNAL( timeout() ), SLOT( timeout() ) );

	timerMode = 0;
	timer.start( 10000, TRUE );
	
	if( stars )
	{
		blinkTimer = new QTimer( this, "blink" );
		connect( blinkTimer, SIGNAL( timeout() ), SLOT( blinkTimeout() ) );
		blinkTimer->start( 300 );
		blink = false;
	}
}