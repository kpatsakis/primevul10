otError Commissioner::RemoveJoiner(const Mac::ExtAddress *aEui64, uint32_t aDelay)
{
    otError error = OT_ERROR_NOT_FOUND;

    VerifyOrExit(mState == OT_COMMISSIONER_STATE_ACTIVE, error = OT_ERROR_INVALID_STATE);

    for (Joiner *joiner = &mJoiners[0]; joiner < OT_ARRAY_END(mJoiners); joiner++)
    {
        if (!joiner->mValid)
        {
            continue;
        }

        if (aEui64 != NULL)
        {
            if (joiner->mEui64 != *aEui64)
            {
                continue;
            }
        }
        else if (!joiner->mAny)
        {
            continue;
        }

        if (aDelay > 0)
        {
            TimeMilli now = TimerMilli::GetNow();

            if ((joiner->mExpirationTime > now) && (joiner->mExpirationTime - now > Time::SecToMsec(aDelay)))
            {
                joiner->mExpirationTime = now + Time::SecToMsec(aDelay);
                UpdateJoinerExpirationTimer();
            }
        }
        else
        {
            Mac::ExtAddress joinerId;

            joiner->mValid = false;
            UpdateJoinerExpirationTimer();
            SendCommissionerSet();

            otLogInfoMeshCoP("Removed Joiner (%s)", (aEui64 != NULL) ? aEui64->ToString().AsCString() : "*");

            ComputeJoinerId(joiner->mEui64, joinerId);
            SignalJoinerEvent(OT_COMMISSIONER_JOINER_REMOVED, joinerId);
        }

        ExitNow(error = OT_ERROR_NONE);
    }

exit:
    return error;
}