composite_mode_to_cairo_operator (RsvgFilterPrimitiveCompositeMode mode)
{
    switch (mode) {
    case COMPOSITE_MODE_OVER:
        return CAIRO_OPERATOR_OVER;

    case COMPOSITE_MODE_IN:
        return CAIRO_OPERATOR_IN;

    case COMPOSITE_MODE_OUT:
        return CAIRO_OPERATOR_OUT;

    case COMPOSITE_MODE_ATOP:
        return CAIRO_OPERATOR_ATOP;

    case COMPOSITE_MODE_XOR:
        return CAIRO_OPERATOR_XOR;

    default:
        g_assert_not_reached ();
        return CAIRO_OPERATOR_CLEAR;
    }
}