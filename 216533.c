otError Commissioner::SetProvisioningUrl(const char *aProvisioningUrl)
{
    otError error = OT_ERROR_NONE;
    uint8_t len;

    if (aProvisioningUrl == NULL)
    {
        mProvisioningUrl[0] = '\0';
        ExitNow();
    }

    len = static_cast<uint8_t>(strnlen(aProvisioningUrl, sizeof(mProvisioningUrl)));

    VerifyOrExit(len < sizeof(mProvisioningUrl), error = OT_ERROR_INVALID_ARGS);

    memcpy(mProvisioningUrl, aProvisioningUrl, len);
    mProvisioningUrl[len] = '\0';

exit:
    return error;
}