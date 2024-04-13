otError Commissioner::SendCommissionerSet(void)
{
    otError                error;
    otCommissioningDataset dataset;
    SteeringDataTlv        steeringData;
    Mac::ExtAddress        joinerId;

    VerifyOrExit(mState == OT_COMMISSIONER_STATE_ACTIVE, error = OT_ERROR_INVALID_STATE);

    memset(&dataset, 0, sizeof(dataset));

    // session id
    dataset.mSessionId      = mSessionId;
    dataset.mIsSessionIdSet = true;

    // compute bloom filter
    steeringData.Init();
    steeringData.Clear();

    for (Joiner *joiner = &mJoiners[0]; joiner < OT_ARRAY_END(mJoiners); joiner++)
    {
        if (!joiner->mValid)
        {
            continue;
        }

        if (joiner->mAny)
        {
            steeringData.SetLength(1);
            steeringData.Set();
            break;
        }

        ComputeJoinerId(joiner->mEui64, joinerId);
        steeringData.ComputeBloomFilter(joinerId);
    }

    // set bloom filter
    dataset.mSteeringData.mLength = steeringData.GetSteeringDataLength();
    memcpy(dataset.mSteeringData.m8, steeringData.GetValue(), dataset.mSteeringData.mLength);
    dataset.mIsSteeringDataSet = true;

    SuccessOrExit(error = SendMgmtCommissionerSetRequest(dataset, NULL, 0));

exit:
    return error;
}