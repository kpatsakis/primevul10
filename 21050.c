fr_window_class_init (FrWindowClass *klass)
{
	GObjectClass   *gobject_class;
	GtkWidgetClass *widget_class;

	fr_window_parent_class = g_type_class_peek_parent (klass);

	fr_window_signals[ARCHIVE_LOADED] =
		g_signal_new ("archive-loaded",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (FrWindowClass, archive_loaded),
			      NULL, NULL,
			      fr_marshal_VOID__BOOLEAN,
			      G_TYPE_NONE, 1,
			      G_TYPE_BOOLEAN);
	fr_window_signals[PROGRESS] =
		g_signal_new ("progress",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (FrWindowClass, progress),
			      NULL, NULL,
			      fr_marshal_VOID__DOUBLE_STRING,
			      G_TYPE_NONE, 2,
			      G_TYPE_DOUBLE,
			      G_TYPE_STRING);
	fr_window_signals[READY] =
		g_signal_new ("ready",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (FrWindowClass, ready),
			      NULL, NULL,
			      fr_marshal_VOID__POINTER,
			      G_TYPE_NONE, 1,
			      G_TYPE_POINTER);

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = fr_window_finalize;

	widget_class = GTK_WIDGET_CLASS (klass);
	widget_class->realize = fr_window_realize;
	widget_class->unrealize = fr_window_unrealize;
	widget_class->unmap = fr_window_unmap;
}