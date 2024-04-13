static void mdbEvalRowInfo( MyDbEvalContext *eval, int *pnCol, char ***papColName, Jsi_OptionId **papColType) {
    if (!papColName) {
        //TODO: Array ???
    }
    *papColName = eval->prep->colNames;
    *papColType = eval->prep->colTypes;
    *pnCol = eval->prep->numCol;
}