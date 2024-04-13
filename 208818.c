void CL_ConsolePrint( char *txt ) {
	int	y, l;
	unsigned char	c;
	unsigned short	color;

	// for some demos we don't want to ever show anything on the console
	if ( cl_noprint && cl_noprint->integer ) {
		return;
	}

	if ( !con.initialized ) {
		con.color[0] =
			con.color[1] =
				con.color[2] =
					con.color[3] = 1.0f;
		con.linewidth = -1;
		Con_CheckResize();
		con.initialized = qtrue;
	}

	color = ColorIndex( COLNSOLE_COLOR );

	while ( (c = *((unsigned char *) txt)) != 0 ) {
		if ( Q_IsColorString( txt ) ) {
			color = ColorIndex( *( txt + 1 ) );
			txt += 2;
			continue;
		}

		// count word length
		for ( l = 0 ; l < con.linewidth ; l++ ) {
			if ( txt[l] <= ' ' ) {
				break;
			}

		}

		// word wrap
		if ( l != con.linewidth && ( con.x + l >= con.linewidth ) ) {
			Con_Linefeed();

		}

		txt++;

		switch ( c )
		{
		case '\n':
			Con_Linefeed();
			break;
		case '\r':
			con.x = 0;
			break;
		default:    // display character and advance
			y = con.current % con.totallines;
			con.text[y * con.linewidth + con.x] = ( color << 8 ) | c;
			con.x++;
			if(con.x >= con.linewidth)
				Con_Linefeed();
			break;
		}
	}

	// mark time for transparent overlay
	if ( con.current >= 0 ) {
		con.times[con.current % NUM_CON_TIMES] = cls.realtime;
	}
}