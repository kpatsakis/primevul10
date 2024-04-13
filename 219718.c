url_entry_sort(const void *A, const void *B)
{
    const url_entry *u1 = (const url_entry *)A;
    const url_entry *u2 = (const url_entry *)B;

    if (u2->rtt == u1->rtt)
        return 0;
    else if (0 == u1->rtt)
        return 1;
    else if (0 == u2->rtt)
        return -1;
    else
        return u1->rtt - u2->rtt;
}