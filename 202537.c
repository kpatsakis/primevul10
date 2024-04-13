is_laptop (GnomeOutputInfo *output)
{
        const char *output_name = output->name;

        if (output->connected && output_name &&
            (strstr ("lvds", output_name)	||
             strstr ("LVDS", output_name)	||
             strstr ("Lvds", output_name)))
        {
                return TRUE;
        }

        return FALSE;
}