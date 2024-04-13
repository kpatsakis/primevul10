    OpCode GetNop() const 
    { 
        AnalysisAssert(isDeferred || pnodeStmt != nullptr);
        return isDeferred ? op : pnodeStmt->nop; 
    }