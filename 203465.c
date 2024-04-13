static ssize_t tnt_control_send(struct gps_device_t *session,
				char *msg, size_t len UNUSED)
/* send a control string in TNT native formal */
{
    ssize_t status;

    tnt_add_checksum(msg);
    status = write(session->gpsdata.gps_fd, msg, strlen(msg));
    (void)tcdrain(session->gpsdata.gps_fd);
    return status;
}