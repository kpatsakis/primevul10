add_file_list_columns (FrWindow    *window,
		       GtkTreeView *treeview)
{
	static char       *titles[] = {NC_("File", "Size"),
				       NC_("File", "Type"),
				       NC_("File", "Modified"),
				       NC_("File", "Location")};
	GtkCellRenderer   *renderer;
	GtkTreeViewColumn *column;
	GValue             value = { 0, };
	int                i, j, w;

	/* First column. */

	window->priv->filename_column = column = gtk_tree_view_column_new ();
	gtk_tree_view_column_set_title (column, C_("File", "Name"));

	/* emblem */

	renderer = gtk_cell_renderer_pixbuf_new ();
	gtk_tree_view_column_pack_end (column, renderer, FALSE);
	gtk_tree_view_column_set_attributes (column, renderer,
					     "pixbuf", COLUMN_EMBLEM,
					     NULL);

	/* icon */

	renderer = gtk_cell_renderer_pixbuf_new ();
	gtk_tree_view_column_pack_start (column, renderer, FALSE);
	gtk_tree_view_column_set_attributes (column, renderer,
					     "pixbuf", COLUMN_ICON,
					     NULL);

	/* name */

	window->priv->single_click = is_single_click_policy (window);

	renderer = gtk_cell_renderer_text_new ();

	g_value_init (&value, PANGO_TYPE_ELLIPSIZE_MODE);
	g_value_set_enum (&value, PANGO_ELLIPSIZE_END);
	g_object_set_property (G_OBJECT (renderer), "ellipsize", &value);
	g_value_unset (&value);

	gtk_tree_view_column_pack_start (column,
					 renderer,
					 TRUE);
	gtk_tree_view_column_set_attributes (column, renderer,
					     "text", COLUMN_NAME,
					     NULL);

	gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_FIXED);
	w = g_settings_get_int (window->priv->settings_listing, PREF_LISTING_NAME_COLUMN_WIDTH);
	if (w <= 0)
		w = DEFAULT_NAME_COLUMN_WIDTH;
	gtk_tree_view_column_set_fixed_width (column, w);
	gtk_tree_view_column_set_resizable (column, TRUE);
	gtk_tree_view_column_set_sort_column_id (column, FR_WINDOW_SORT_BY_NAME);
	gtk_tree_view_column_set_cell_data_func (column, renderer,
						 (GtkTreeCellDataFunc) filename_cell_data_func,
						 window, NULL);

	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

	/* Other columns */

	for (j = 0, i = COLUMN_SIZE; i < NUMBER_OF_COLUMNS; i++, j++) {
		GValue  value = { 0, };

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes (g_dpgettext2 (NULL, "File", titles[j]),
								   renderer,
								   "text", i,
								   NULL);

		gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_FIXED);
		gtk_tree_view_column_set_fixed_width (column, OTHER_COLUMNS_WIDTH);
		gtk_tree_view_column_set_resizable (column, TRUE);

		gtk_tree_view_column_set_sort_column_id (column, FR_WINDOW_SORT_BY_NAME + 1 + j);

		g_value_init (&value, PANGO_TYPE_ELLIPSIZE_MODE);
		g_value_set_enum (&value, PANGO_ELLIPSIZE_END);
		g_object_set_property (G_OBJECT (renderer), "ellipsize", &value);
		g_value_unset (&value);

		gtk_tree_view_append_column (treeview, column);
	}
}