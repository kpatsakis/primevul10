otError Commissioner::GetNextJoinerInfo(uint16_t &aIterator, otJoinerInfo &aJoiner) const
{
    otError error = OT_ERROR_NONE;
    size_t  index;

    for (index = aIterator; index < OT_ARRAY_LENGTH(mJoiners); index++)
    {
        if (!mJoiners[index].mValid)
        {
            continue;
        }

        memset(&aJoiner, 0, sizeof(aJoiner));

        aJoiner.mAny   = mJoiners[index].mAny;
        aJoiner.mEui64 = mJoiners[index].mEui64;
        strlcpy(aJoiner.mPsk, mJoiners[index].mPsk, sizeof(aJoiner.mPsk));
        aJoiner.mExpirationTime = mJoiners[index].mExpirationTime - TimerMilli::GetNow();
        aIterator               = static_cast<uint16_t>(index) + 1;
        ExitNow();
    }

    error = OT_ERROR_NOT_FOUND;

exit:
    return error;
}