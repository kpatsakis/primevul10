sync_result_err(Slapi_PBlock *pb, int err, char *msg)
{
    int rc = 0;

    slapi_send_ldap_result(pb, err, NULL, msg, 0, NULL);

    return (rc);
}