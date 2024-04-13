format_alg(int port, struct ds *s)
{
    switch(port) {
    case IPPORT_FTP:
        ds_put_format(s, "%salg=%sftp,", colors.param, colors.end);
        break;
    case IPPORT_TFTP:
        ds_put_format(s, "%salg=%stftp,", colors.param, colors.end);
        break;
    case 0:
        /* Don't print. */
        break;
    default:
        ds_put_format(s, "%salg=%s%d,", colors.param, colors.end, port);
        break;
    }
}