void Con_DrawNotify( void ) {
	int x, v;
	short   *text;
	int i;
	int time;
	int skip;
	int currentColor;

	// NERVE - SMF - we dont want draw notify in limbo mode
	if ( Cvar_VariableIntegerValue( "ui_limboMode" ) ) {
		return;
	}

	currentColor = 7;
	re.SetColor( g_color_table[currentColor] );

	v = 0;
	for ( i = con.current - NUM_CON_TIMES + 1 ; i <= con.current ; i++ )
	{
		if ( i < 0 ) {
			continue;
		}
		time = con.times[i % NUM_CON_TIMES];
		if ( time == 0 ) {
			continue;
		}
		time = cls.realtime - time;
		if ( time > con_notifytime->value * 1000 ) {
			continue;
		}
		text = con.text + ( i % con.totallines ) * con.linewidth;

		if (cl.snap.ps.pm_type != PM_INTERMISSION && Key_GetCatcher( ) & (KEYCATCH_UI | KEYCATCH_CGAME) ) {
			continue;
		}

		for ( x = 0 ; x < con.linewidth ; x++ ) {
			if ( ( text[x] & 0xff ) == ' ' ) {
				continue;
			}
			if ( ColorIndexForNumber( text[x] >> 8 ) != currentColor ) {
				currentColor = ColorIndexForNumber( text[x] >> 8 );
				re.SetColor( g_color_table[currentColor] );
			}
			SCR_DrawSmallChar( cl_conXOffset->integer + con.xadjust + ( x + 1 ) * SMALLCHAR_WIDTH, v, text[x] & 0xff );
		}

		v += SMALLCHAR_HEIGHT;
	}

	re.SetColor( NULL );

	if (Key_GetCatcher( ) & (KEYCATCH_UI | KEYCATCH_CGAME) ) {
		return;
	}

	// draw the chat line
	if ( Key_GetCatcher( ) & KEYCATCH_MESSAGE ) {
		if ( chat_team ) {
			char buf[128];
			CL_TranslateString( "say_team:", buf );
			SCR_DrawBigString( 8, v, buf, 1.0f, qfalse );
			skip = strlen( buf ) + 2;
		} else
		{
			char buf[128];
			CL_TranslateString( "say:", buf );
			SCR_DrawBigString( 8, v, buf, 1.0f, qfalse );
			skip = strlen( buf ) + 1;
		}

		Field_BigDraw( &chatField, skip * BIGCHAR_WIDTH, v, SCREEN_WIDTH - ( skip + 1 ) * BIGCHAR_WIDTH, qtrue, qtrue );

	}

}