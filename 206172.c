int ssh_scp_accept_request(ssh_scp scp)
{
    char buffer[] = {0x00};
    int rc;
    if (scp == NULL) {
        return SSH_ERROR;
    }

    if (scp->state != SSH_SCP_READ_REQUESTED) {
        ssh_set_error(scp->session, SSH_FATAL,
                      "ssh_scp_deny_request called under invalid state");
        return SSH_ERROR;
    }

    rc = ssh_channel_write(scp->channel, buffer, 1);
    if (rc == SSH_ERROR) {
        return SSH_ERROR;
    }

    if (scp->request_type == SSH_SCP_REQUEST_NEWFILE) {
        scp->state = SSH_SCP_READ_READING;
    } else {
        scp->state = SSH_SCP_READ_INITED;
    }

    return SSH_OK;
}