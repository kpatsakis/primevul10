fr_window_attach (FrWindow      *window,
		  GtkWidget     *child,
		  FrWindowArea   area)
{
	int position;

	g_return_if_fail (window != NULL);
	g_return_if_fail (FR_IS_WINDOW (window));
	g_return_if_fail (child != NULL);
	g_return_if_fail (GTK_IS_WIDGET (child));

	switch (area) {
	case FR_WINDOW_AREA_MENUBAR:
		position = 0;
		break;
	case FR_WINDOW_AREA_TOOLBAR:
		position = 1;
		break;
	case FR_WINDOW_AREA_LOCATIONBAR:
		position = 2;
		break;
	case FR_WINDOW_AREA_FILTERBAR:
		position = 3;
		break;
	case FR_WINDOW_AREA_CONTENTS:
		position = 4;
		if (window->priv->contents != NULL)
			gtk_widget_destroy (window->priv->contents);
		window->priv->contents = child;
		gtk_widget_set_vexpand (child, TRUE);
		break;
	case FR_WINDOW_AREA_STATUSBAR:
		position = 5;
		break;
	default:
		g_critical ("%s: area not recognized!", G_STRFUNC);
		return;
		break;
	}

	gtk_widget_set_hexpand (child, TRUE);
	gtk_grid_attach (GTK_GRID (window->priv->layout),
			 child,
			 0, position,
			 1, 1);
}