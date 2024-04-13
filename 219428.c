sync_nsuniqueid2uuid(const char *nsuniqueid)
{
    char *uuid;
    char u[17];

    u[0] = slapi_str_to_u8(nsuniqueid);
    u[1] = slapi_str_to_u8(nsuniqueid + 2);
    u[2] = slapi_str_to_u8(nsuniqueid + 4);
    u[3] = slapi_str_to_u8(nsuniqueid + 6);

    u[4] = slapi_str_to_u8(nsuniqueid + 9);
    u[5] = slapi_str_to_u8(nsuniqueid + 11);
    u[6] = slapi_str_to_u8(nsuniqueid + 13);
    u[7] = slapi_str_to_u8(nsuniqueid + 15);

    u[8] = slapi_str_to_u8(nsuniqueid + 18);
    u[9] = slapi_str_to_u8(nsuniqueid + 20);
    u[10] = slapi_str_to_u8(nsuniqueid + 22);
    u[11] = slapi_str_to_u8(nsuniqueid + 24);

    u[12] = slapi_str_to_u8(nsuniqueid + 27);
    u[13] = slapi_str_to_u8(nsuniqueid + 29);
    u[14] = slapi_str_to_u8(nsuniqueid + 31);
    u[15] = slapi_str_to_u8(nsuniqueid + 33);

    u[16] = '\0';

    uuid = slapi_ch_malloc(sizeof(u));
    memcpy(uuid, u, sizeof(u));

    return (uuid);
}