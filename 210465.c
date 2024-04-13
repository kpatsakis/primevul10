  StatisticsCB NewStatisticsCB() {
    return base::Bind(&MockStatisticsCB::OnStatistics,
                      base::Unretained(&statistics_cb_));
   }
