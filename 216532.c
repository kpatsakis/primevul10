const char *Commissioner::StateToString(otCommissionerState aState)
{
    const char *str = "Unknown";

    switch (aState)
    {
    case OT_COMMISSIONER_STATE_DISABLED:
        str = "disabled";
        break;
    case OT_COMMISSIONER_STATE_PETITION:
        str = "petition";
        break;
    case OT_COMMISSIONER_STATE_ACTIVE:
        str = "active";
        break;
    default:
        break;
    }

    return str;
}