static bool vnc_check_pageflip(DisplaySurface *s1,
                               DisplaySurface *s2)
{
    return (s1 != NULL &&
            s2 != NULL &&
            surface_width(s1) == surface_width(s2) &&
            surface_height(s1) == surface_height(s2) &&
            surface_format(s1) == surface_format(s2));

}