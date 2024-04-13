GENERAL_NAME *GENERAL_NAME_dup(const GENERAL_NAME *a)
{
    return (GENERAL_NAME *)ASN1_dup((i2d_of_void *)i2d_GENERAL_NAME,
                                    (d2i_of_void *)d2i_GENERAL_NAME,
                                    (char *)a);
}