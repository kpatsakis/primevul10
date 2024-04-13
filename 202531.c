print_output (GnomeOutputInfo *info)
{
        g_print ("  Output: %s attached to %s\n", info->display_name, info->name);
        g_print ("     status: %s\n", info->on ? "on" : "off");
        g_print ("     width: %d\n", info->width);
        g_print ("     height: %d\n", info->height);
        g_print ("     rate: %d\n", info->rate);
        g_print ("     position: %d %d\n", info->x, info->y);
}