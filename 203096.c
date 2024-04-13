empathy_theme_adium_class_init (EmpathyThemeAdiumClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass* widget_class = GTK_WIDGET_CLASS (klass);

	object_class->finalize = theme_adium_finalize;
	object_class->dispose = theme_adium_dispose;
	object_class->constructed = theme_adium_constructed;
	object_class->get_property = theme_adium_get_property;
	object_class->set_property = theme_adium_set_property;

	widget_class->button_press_event = theme_adium_button_press_event;

	g_object_class_install_property (object_class,
					 PROP_ADIUM_DATA,
					 g_param_spec_boxed ("adium-data",
							     "The theme data",
							     "Data for the adium theme",
							      EMPATHY_TYPE_ADIUM_DATA,
							      G_PARAM_CONSTRUCT_ONLY |
							      G_PARAM_READWRITE |
							      G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (object_class,
					 PROP_VARIANT,
					 g_param_spec_string ("variant",
							      "The theme variant",
							      "Variant name for the theme",
							      NULL,
							      G_PARAM_CONSTRUCT |
							      G_PARAM_READWRITE |
							      G_PARAM_STATIC_STRINGS));

	g_type_class_add_private (object_class, sizeof (EmpathyThemeAdiumPriv));
}