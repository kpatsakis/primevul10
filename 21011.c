fr_window_construct (FrWindow *window)
{
	GtkWidget          *toolbar;
	GtkWidget          *list_scrolled_window;
	GtkWidget          *location_box;
	GtkStatusbar       *statusbar;
	GtkWidget          *statusbar_box;
	GtkWidget          *filter_box;
	GtkWidget          *tree_scrolled_window;
	GtkTreeSelection   *selection;
	GtkActionGroup     *actions;
	GtkAction          *action;
	GtkAction          *other_actions_action;
	GtkUIManager       *ui;
	GtkSizeGroup       *toolbar_size_group;
	GError             *error = NULL;
	const char * const *schemas;

	/* Create the settings objects */

	window->priv->settings_listing = g_settings_new (FILE_ROLLER_SCHEMA_LISTING);
	window->priv->settings_ui = g_settings_new (FILE_ROLLER_SCHEMA_UI);
	window->priv->settings_general = g_settings_new (FILE_ROLLER_SCHEMA_GENERAL);
	window->priv->settings_dialogs = g_settings_new (FILE_ROLLER_SCHEMA_DIALOGS);

	/* Only use the nautilus schema if it's installed */
	for (schemas = g_settings_list_schemas ();
	     *schemas != NULL;
	     schemas++)
	{
		if (g_strcmp0 (*schemas, NAUTILUS_SCHEMA) == 0) {
			window->priv->settings_nautilus = g_settings_new (NAUTILUS_SCHEMA);
			break;
		}
	}

	/* Create the application. */

	window->priv->layout = gtk_grid_new ();
	gtk_container_add (GTK_CONTAINER (window), window->priv->layout);
	gtk_widget_show (window->priv->layout);

	gtk_window_set_title (GTK_WINDOW (window), _("Archive Manager"));
	gtk_window_set_has_resize_grip (GTK_WINDOW (window), TRUE);

	g_signal_connect (G_OBJECT (window),
			  "delete_event",
			  G_CALLBACK (fr_window_delete_event_cb),
			  window);

	g_signal_connect (G_OBJECT (window),
			  "show",
			  G_CALLBACK (fr_window_show_cb),
			  window);

	window->priv->theme_changed_handler_id =
		g_signal_connect (gtk_icon_theme_get_default (),
				  "changed",
				  G_CALLBACK (theme_changed_cb),
				  window);

	gtk_window_set_default_size (GTK_WINDOW (window),
				     g_settings_get_int (window->priv->settings_ui, PREF_UI_WINDOW_WIDTH),
				     g_settings_get_int (window->priv->settings_ui, PREF_UI_WINDOW_HEIGHT));

	gtk_drag_dest_set (GTK_WIDGET (window),
			   GTK_DEST_DEFAULT_ALL,
			   target_table, G_N_ELEMENTS (target_table),
			   GDK_ACTION_COPY);

	g_signal_connect (G_OBJECT (window),
			  "drag_data_received",
			  G_CALLBACK (fr_window_drag_data_received),
			  window);
	g_signal_connect (G_OBJECT (window),
			  "drag_motion",
			  G_CALLBACK (fr_window_drag_motion),
			  window);

	g_signal_connect (G_OBJECT (window),
			  "key_press_event",
			  G_CALLBACK (key_press_cb),
			  window);

	/* Initialize Data. */

	window->priv->list_mode = window->priv->last_list_mode = g_settings_get_enum (window->priv->settings_listing, PREF_LISTING_LIST_MODE);
	g_settings_set_boolean (window->priv->settings_listing, PREF_LISTING_SHOW_PATH, (window->priv->list_mode == FR_WINDOW_LIST_MODE_FLAT));

	window->priv->history = NULL;
	window->priv->history_current = NULL;

	window->priv->action = FR_ACTION_NONE;

	window->priv->open_default_dir = g_object_ref (_g_file_get_home ());
	window->priv->add_default_dir = NULL;
	window->priv->extract_default_dir = g_object_ref (_g_file_get_home ());

	window->priv->give_focus_to_the_list = FALSE;

	window->priv->activity_ref = 0;
	window->priv->activity_timeout_handle = 0;

	window->priv->update_timeout_handle = 0;

	window->priv->archive_present = FALSE;
	window->priv->archive_new = FALSE;
	window->priv->reload_archive = FALSE;
	window->priv->archive_file = NULL;

	window->priv->drag_destination_folder = NULL;
	window->priv->drag_base_dir = NULL;
	window->priv->drag_error = NULL;
	window->priv->drag_file_list = NULL;

	window->priv->batch_mode = FALSE;
	window->priv->batch_action_list = NULL;
	window->priv->batch_action = NULL;
	window->priv->extract_interact_use_default_dir = FALSE;

	window->priv->password = NULL;
	window->priv->compression = g_settings_get_enum (window->priv->settings_general, PREF_GENERAL_COMPRESSION_LEVEL);
	window->priv->encrypt_header = g_settings_get_boolean (window->priv->settings_general, PREF_GENERAL_ENCRYPT_HEADER);
	window->priv->volume_size = 0;

	window->priv->stoppable = TRUE;

	window->priv->batch_adding_one_file = FALSE;

	window->priv->path_clicked = NULL;

	window->priv->current_view_length = 0;

	window->priv->current_batch_action.type = FR_BATCH_ACTION_NONE;
	window->priv->current_batch_action.data = NULL;
	window->priv->current_batch_action.free_func = NULL;

	window->priv->pd_last_archive = NULL;
	window->priv->pd_last_message = NULL;
	window->priv->pd_last_fraction = 0.0;

	/* Create the widgets. */

	/* * File list. */

	window->priv->list_store = fr_list_model_new (NUMBER_OF_COLUMNS,
						      G_TYPE_POINTER,
						      GDK_TYPE_PIXBUF,
						      G_TYPE_STRING,
						      GDK_TYPE_PIXBUF,
						      G_TYPE_STRING,
						      G_TYPE_STRING,
						      G_TYPE_STRING,
						      G_TYPE_STRING);
	g_object_set_data (G_OBJECT (window->priv->list_store), "FrWindow", window);
	window->priv->list_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (window->priv->list_store));

	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (window->priv->list_view), TRUE);
	add_file_list_columns (window, GTK_TREE_VIEW (window->priv->list_view));
	gtk_tree_view_set_enable_search (GTK_TREE_VIEW (window->priv->list_view),
					 TRUE);
	gtk_tree_view_set_search_column (GTK_TREE_VIEW (window->priv->list_view),
					 COLUMN_NAME);

	gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE (window->priv->list_store),
					 FR_WINDOW_SORT_BY_NAME, name_column_sort_func,
					 NULL, NULL);
	gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE (window->priv->list_store),
					 FR_WINDOW_SORT_BY_SIZE, size_column_sort_func,
					 NULL, NULL);
	gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE (window->priv->list_store),
					 FR_WINDOW_SORT_BY_TYPE, type_column_sort_func,
					 NULL, NULL);
	gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE (window->priv->list_store),
					 FR_WINDOW_SORT_BY_TIME, time_column_sort_func,
					 NULL, NULL);
	gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE (window->priv->list_store),
					 FR_WINDOW_SORT_BY_PATH, path_column_sort_func,
					 NULL, NULL);

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (window->priv->list_view));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_MULTIPLE);

	g_signal_connect (selection,
			  "changed",
			  G_CALLBACK (selection_changed_cb),
			  window);
	g_signal_connect (G_OBJECT (window->priv->list_view),
			  "row_activated",
			  G_CALLBACK (row_activated_cb),
			  window);
	g_signal_connect (G_OBJECT (window->priv->list_view),
			  "button_press_event",
			  G_CALLBACK (file_button_press_cb),
			  window);
	g_signal_connect (G_OBJECT (window->priv->list_view),
			  "button_release_event",
			  G_CALLBACK (file_button_release_cb),
			  window);
	g_signal_connect (G_OBJECT (window->priv->list_view),
			  "motion_notify_event",
			  G_CALLBACK (file_motion_notify_callback),
			  window);
	g_signal_connect (G_OBJECT (window->priv->list_view),
			  "leave_notify_event",
			  G_CALLBACK (file_leave_notify_callback),
			  window);
	g_signal_connect (G_OBJECT (window->priv->list_view),
			  "drag_begin",
			  G_CALLBACK (file_list_drag_begin),
			  window);
	g_signal_connect (G_OBJECT (window->priv->list_view),
			  "drag_end",
			  G_CALLBACK (file_list_drag_end),
			  window);
	egg_tree_multi_drag_add_drag_support (GTK_TREE_VIEW (window->priv->list_view));

	list_scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (list_scrolled_window),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (list_scrolled_window), window->priv->list_view);

	/* filter bar */

	window->priv->filter_bar = filter_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
	g_object_set (window->priv->filter_bar,
		      "halign", GTK_ALIGN_CENTER,
		      "margin-left", 6,
		      "border-width", 3,
		      NULL);
	fr_window_attach (FR_WINDOW (window), window->priv->filter_bar, FR_WINDOW_AREA_FILTERBAR);

	/* * filter entry */

	window->priv->filter_entry = GTK_WIDGET (gtk_entry_new ());
	gtk_entry_set_icon_from_icon_name (GTK_ENTRY (window->priv->filter_entry),
					   GTK_ENTRY_ICON_SECONDARY,
					   "edit-find-symbolic");
	gtk_entry_set_icon_activatable (GTK_ENTRY (window->priv->filter_entry),
					GTK_ENTRY_ICON_SECONDARY,
					FALSE);
	gtk_entry_set_width_chars (GTK_ENTRY (window->priv->filter_entry), 40);
	gtk_box_pack_start (GTK_BOX (filter_box),
			    window->priv->filter_entry, FALSE, FALSE, 6);

	g_signal_connect (G_OBJECT (window->priv->filter_entry),
			  "activate",
			  G_CALLBACK (filter_entry_activate_cb),
			  window);

	gtk_widget_show_all (filter_box);

	/* tree view */

	window->priv->tree_store = gtk_tree_store_new (TREE_NUMBER_OF_COLUMNS,
						       G_TYPE_STRING,
						       GDK_TYPE_PIXBUF,
						       G_TYPE_STRING,
						       PANGO_TYPE_WEIGHT);
	window->priv->tree_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (window->priv->tree_store));
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (window->priv->tree_view), FALSE);
	add_dir_tree_columns (window, GTK_TREE_VIEW (window->priv->tree_view));

	g_signal_connect (G_OBJECT (window->priv->tree_view),
			  "button_press_event",
			  G_CALLBACK (dir_tree_button_press_cb),
			  window);

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (window->priv->tree_view));
	g_signal_connect (selection,
			  "changed",
			  G_CALLBACK (dir_tree_selection_changed_cb),
			  window);

	g_signal_connect (G_OBJECT (window->priv->tree_view),
			  "drag_begin",
			  G_CALLBACK (file_list_drag_begin),
			  window);
	g_signal_connect (G_OBJECT (window->priv->tree_view),
			  "drag_end",
			  G_CALLBACK (file_list_drag_end),
			  window);
	g_signal_connect (G_OBJECT (window->priv->tree_view),
			  "drag_data_get",
			  G_CALLBACK (fr_window_folder_tree_drag_data_get),
			  window);
	gtk_drag_source_set (window->priv->tree_view,
			     GDK_BUTTON1_MASK,
			     folder_tree_targets, G_N_ELEMENTS (folder_tree_targets),
			     GDK_ACTION_COPY);

	tree_scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (tree_scrolled_window),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (tree_scrolled_window), window->priv->tree_view);

	/* side pane */

	window->priv->sidepane = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start (GTK_BOX (window->priv->sidepane), tree_scrolled_window, TRUE, TRUE, 0);

	/* main content */

	window->priv->paned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_paned_pack1 (GTK_PANED (window->priv->paned), window->priv->sidepane, FALSE, TRUE);
	gtk_paned_pack2 (GTK_PANED (window->priv->paned), list_scrolled_window, TRUE, TRUE);
	gtk_paned_set_position (GTK_PANED (window->priv->paned), g_settings_get_int (window->priv->settings_ui, PREF_UI_SIDEBAR_WIDTH));

	fr_window_attach (FR_WINDOW (window), window->priv->paned, FR_WINDOW_AREA_CONTENTS);
	gtk_widget_show_all (window->priv->paned);

	/* Build the menu and the toolbar. */

	ui = gtk_ui_manager_new ();

	window->priv->actions = actions = gtk_action_group_new ("Actions");

	/* open recent menu item action  */

	action = g_object_new (GTK_TYPE_RECENT_ACTION,
			       "name", "OpenRecent",
			       /* Translators: this is the label for the "open recent file" sub-menu. */
			       "label", _("Open _Recent"),
			       "tooltip", _("Open a recently used archive"),
			       "stock-id", GTK_STOCK_OPEN,
			       NULL);
	fr_window_init_recent_chooser (window, GTK_RECENT_CHOOSER (action));
	gtk_action_group_add_action (actions, action);
	g_object_unref (action);

	/* open recent toolbar item action  */

	action = g_object_new (GTK_TYPE_RECENT_ACTION,
			       "name", "OpenRecent_Toolbar",
			       "label", _("Open"),
			       "tooltip", _("Open a recently used archive"),
			       "stock-id", GTK_STOCK_OPEN,
			       "is-important", TRUE,
			       NULL);
	fr_window_init_recent_chooser (window, GTK_RECENT_CHOOSER (action));
	g_signal_connect (action,
			  "activate",
			  G_CALLBACK (activate_action_open),
			  window);
	gtk_action_group_add_action (actions, action);
	g_object_unref (action);

	/* menu actions */

	other_actions_action = action = g_object_new (GTH_TYPE_TOGGLE_MENU_ACTION,
			       "name", "OtherActions",
			       "label", _("_Other Actions"),
			       "tooltip", _("Other actions"),
			       "menu-halign", GTK_ALIGN_CENTER,
			       "show-arrow", TRUE,
			       NULL);
	gtk_action_group_add_action (actions, action);

	/* other actions */

	gtk_action_group_set_translation_domain (actions, NULL);
	gtk_action_group_add_actions (actions,
				      action_entries,
				      n_action_entries,
				      window);
	gtk_action_group_add_toggle_actions (actions,
					     action_toggle_entries,
					     n_action_toggle_entries,
					     window);
	gtk_action_group_add_radio_actions (actions,
					    view_as_entries,
					    n_view_as_entries,
					    window->priv->list_mode,
					    G_CALLBACK (view_as_radio_action),
					    window);

	g_signal_connect (ui, "connect_proxy",
			  G_CALLBACK (connect_proxy_cb), window);
	g_signal_connect (ui, "disconnect_proxy",
			  G_CALLBACK (disconnect_proxy_cb), window);

	gtk_ui_manager_insert_action_group (ui, actions, 0);
	gtk_window_add_accel_group (GTK_WINDOW (window),
				    gtk_ui_manager_get_accel_group (ui));

	/* Add a hidden short cut Ctrl-Q for power users */
	gtk_accel_group_connect (gtk_ui_manager_get_accel_group (ui),
				 GDK_KEY_q, GDK_CONTROL_MASK, 0,
				 g_cclosure_new_swap (G_CALLBACK (fr_window_close), window, NULL));

	if (! gtk_ui_manager_add_ui_from_resource (ui, "/org/gnome/FileRoller/ui/menus-toolbars.ui", &error)) {
		g_message ("building menus failed: %s", error->message);
		g_error_free (error);
	}

	g_object_set (other_actions_action, "menu", gtk_ui_manager_get_widget (ui, "/OtherActionsMenu"), NULL);
	g_object_unref (other_actions_action);

	window->priv->toolbar = toolbar = gtk_ui_manager_get_widget (ui, "/ToolBar");
	gtk_toolbar_set_show_arrow (GTK_TOOLBAR (toolbar), TRUE);
	gtk_style_context_add_class (gtk_widget_get_style_context (toolbar), GTK_STYLE_CLASS_PRIMARY_TOOLBAR);
	set_action_important (ui, "/ToolBar/Extract_Toolbar");
	set_action_important (ui, "/ToolBar/Add_Toolbar");

	/* location bar */

	window->priv->location_bar = gtk_ui_manager_get_widget (ui, "/LocationBar");
	gtk_toolbar_set_show_arrow (GTK_TOOLBAR (window->priv->location_bar), FALSE);
	gtk_toolbar_set_style (GTK_TOOLBAR (window->priv->location_bar), GTK_TOOLBAR_BOTH_HORIZ);
	gtk_style_context_add_class (gtk_widget_get_style_context (window->priv->location_bar), GTK_STYLE_CLASS_TOOLBAR);
	set_action_important (ui, "/LocationBar/GoBack");

	/* current location */

	location_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
	/* Translators: after the colon there is a folder name. */
	window->priv->location_label = gtk_label_new_with_mnemonic (_("_Location:"));
	gtk_box_pack_start (GTK_BOX (location_box),
			    window->priv->location_label, FALSE, FALSE, 5);

	window->priv->location_entry = gtk_entry_new ();
	gtk_entry_set_icon_from_stock (GTK_ENTRY (window->priv->location_entry),
				       GTK_ENTRY_ICON_PRIMARY,
				       GTK_STOCK_DIRECTORY);

	gtk_box_pack_start (GTK_BOX (location_box),
			    window->priv->location_entry, TRUE, TRUE, 5);

	g_signal_connect (G_OBJECT (window->priv->location_entry),
			  "key_press_event",
			  G_CALLBACK (location_entry_key_press_event_cb),
			  window);

	{
		GtkToolItem *tool_item;

		tool_item = gtk_separator_tool_item_new ();
		gtk_widget_show_all (GTK_WIDGET (tool_item));
		gtk_toolbar_insert (GTK_TOOLBAR (window->priv->location_bar), tool_item, -1);

		tool_item = gtk_tool_item_new ();
		gtk_tool_item_set_expand (tool_item, TRUE);
		gtk_container_add (GTK_CONTAINER (tool_item), location_box);
		gtk_widget_show_all (GTK_WIDGET (tool_item));
		gtk_toolbar_insert (GTK_TOOLBAR (window->priv->location_bar), tool_item, -1);
	}

	fr_window_attach (FR_WINDOW (window), window->priv->location_bar, FR_WINDOW_AREA_LOCATIONBAR);
	if (window->priv->list_mode == FR_WINDOW_LIST_MODE_FLAT)
		gtk_widget_hide (window->priv->location_bar);
	else
		gtk_widget_show (window->priv->location_bar);

	/**/

	fr_window_attach (FR_WINDOW (window), window->priv->toolbar, FR_WINDOW_AREA_TOOLBAR);
	gtk_widget_show (toolbar);

	window->priv->file_popup_menu = gtk_ui_manager_get_widget (ui, "/FilePopupMenu");
	window->priv->folder_popup_menu = gtk_ui_manager_get_widget (ui, "/FolderPopupMenu");
	window->priv->sidebar_folder_popup_menu = gtk_ui_manager_get_widget (ui, "/SidebarFolderPopupMenu");

	/* Create the statusbar. */

	window->priv->statusbar = gtk_statusbar_new ();
	window->priv->help_message_cid = gtk_statusbar_get_context_id (GTK_STATUSBAR (window->priv->statusbar), "help_message");
	window->priv->list_info_cid = gtk_statusbar_get_context_id (GTK_STATUSBAR (window->priv->statusbar), "list_info");
	window->priv->progress_cid = gtk_statusbar_get_context_id (GTK_STATUSBAR (window->priv->statusbar), "progress");

	statusbar = GTK_STATUSBAR (window->priv->statusbar);
	statusbar_box = gtk_statusbar_get_message_area (statusbar);
	gtk_box_set_homogeneous (GTK_BOX (statusbar_box), FALSE);
	gtk_box_set_spacing (GTK_BOX (statusbar_box), 4);
	gtk_box_set_child_packing (GTK_BOX (statusbar_box), gtk_statusbar_get_message_area (statusbar), TRUE, TRUE, 0, GTK_PACK_START );

	window->priv->progress_bar = gtk_progress_bar_new ();
	gtk_progress_bar_set_pulse_step (GTK_PROGRESS_BAR (window->priv->progress_bar), ACTIVITY_PULSE_STEP);
	gtk_widget_set_size_request (window->priv->progress_bar, -1, PROGRESS_BAR_HEIGHT);
	{
		GtkWidget *vbox;

		vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_box_pack_start (GTK_BOX (statusbar_box), vbox, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), window->priv->progress_bar, TRUE, TRUE, 1);
		gtk_widget_show (vbox);
	}
	gtk_widget_show (statusbar_box);

	fr_window_attach (FR_WINDOW (window), window->priv->statusbar, FR_WINDOW_AREA_STATUSBAR);
	if (g_settings_get_boolean (window->priv->settings_ui, PREF_UI_VIEW_STATUSBAR))
		gtk_widget_show (window->priv->statusbar);
	else
		gtk_widget_hide (window->priv->statusbar);

	/**/

	toolbar_size_group = gtk_size_group_new (GTK_SIZE_GROUP_VERTICAL);
	gtk_size_group_add_widget (toolbar_size_group, window->priv->location_bar);
	gtk_size_group_add_widget (toolbar_size_group, window->priv->filter_bar);

	/**/

	fr_window_update_title (window);
	fr_window_update_sensitivity (window);
	fr_window_update_current_location (window);
	fr_window_update_columns_visibility (window);

	/* Add notification callbacks. */

	g_signal_connect (window->priv->settings_ui,
			  "changed::" PREF_UI_HISTORY_LEN,
			  G_CALLBACK (pref_history_len_changed),
			  window);
	g_signal_connect (window->priv->settings_ui,
			  "changed::" PREF_UI_VIEW_STATUSBAR,
			  G_CALLBACK (pref_view_statusbar_changed),
			  window);
	g_signal_connect (window->priv->settings_ui,
			  "changed::" PREF_UI_VIEW_FOLDERS,
			  G_CALLBACK (pref_view_folders_changed),
			  window);
	g_signal_connect (window->priv->settings_listing,
			  "changed::" PREF_LISTING_SHOW_TYPE,
			  G_CALLBACK (pref_show_field_changed),
			  window);
	g_signal_connect (window->priv->settings_listing,
			  "changed::" PREF_LISTING_SHOW_SIZE,
			  G_CALLBACK (pref_show_field_changed),
			  window);
	g_signal_connect (window->priv->settings_listing,
			  "changed::" PREF_LISTING_SHOW_TIME,
			  G_CALLBACK (pref_show_field_changed),
			  window);
	g_signal_connect (window->priv->settings_listing,
			  "changed::" PREF_LISTING_SHOW_PATH,
			  G_CALLBACK (pref_show_field_changed),
			  window);
	g_signal_connect (window->priv->settings_listing,
			  "changed::" PREF_LISTING_LIST_MODE,
			  G_CALLBACK (pref_list_mode_changed),
			  window);

	if (window->priv->settings_nautilus)
		g_signal_connect (window->priv->settings_nautilus,
				  "changed::" NAUTILUS_CLICK_POLICY,
				  G_CALLBACK (pref_click_policy_changed),
				  window);

	/* Give focus to the list. */

	gtk_widget_grab_focus (window->priv->list_view);
}