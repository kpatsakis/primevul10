const char *mysqlGetbindParamName(MysqlPrep* pStmt, int n) {
    if (n>=1 && n<=pStmt->paramCnt)
        return pStmt->paramNames[n-1];
    return NULL;
}