gsd_xrandr_manager_class_init (GsdXrandrManagerClass *klass)
{
        GObjectClass *object_class = G_OBJECT_CLASS (klass);

        object_class->get_property = gsd_xrandr_manager_get_property;
        object_class->set_property = gsd_xrandr_manager_set_property;
        object_class->constructor = gsd_xrandr_manager_constructor;
        object_class->dispose = gsd_xrandr_manager_dispose;
        object_class->finalize = gsd_xrandr_manager_finalize;

        dbus_g_object_type_install_info (GSD_TYPE_XRANDR_MANAGER, &dbus_glib_gsd_xrandr_manager_object_info);

        g_type_class_add_private (klass, sizeof (GsdXrandrManagerPrivate));
}