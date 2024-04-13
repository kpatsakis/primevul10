int ssh_scp_read_string(ssh_scp scp, char *buffer, size_t len)
{
    size_t read = 0;
    int err = SSH_OK;

    if (scp == NULL) {
        return SSH_ERROR;
    }

    while (read < len - 1) {
        err = ssh_channel_read(scp->channel, &buffer[read], 1, 0);
        if (err == SSH_ERROR) {
            break;
        }

        if (err == 0) {
            ssh_set_error(scp->session, SSH_FATAL,
                          "End of file while reading string");
            err = SSH_ERROR;
            break;
        }

        read++;
        if (buffer[read - 1] == '\n') {
            break;
        }
    }

    buffer[read] = 0;
    return err;
}