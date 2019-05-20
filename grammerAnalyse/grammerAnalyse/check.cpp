#include "grammerAnalyse.h"
vector<string> errIden[6];

void loseSymbol(){
    errState = correct;
    errIden[4].push_back(lastS);
    printf("error: loseSymbol %s\n", lastS.c_str());
}

void matchErr(){
    errState = correct;
    errIden[5].push_back(buffer[wordIdx].word);
    errIden[5].push_back(nowS);
    printf("error: unknow symbol '%s', did you mean '%s'?\n", buffer[wordIdx].word.c_str(), nowS.c_str());
}

/*when word correct, do:*/
void loseSymbolCheck(){
    if (errState == err){
        loseSymbol();
    }
}

bool check_pro_dec(int idx){
    for (int i = 0; i < proTable.size(); i++){ //只找当前深度，是否有重定义
        if (proTable[i].plev == deep && proTable[i].pname == buffer[idx].word) return false;
    }
    return true;
}
bool check_var_dec(int idx){
    for (int i = 0; i < varTable.size(); i++){ //只找当前深度，是否有重定义
        if (varTable[i].vlev == deep && varTable[i].vname == buffer[idx].word) return false;
    }
    for (int i = 0; i < tmpVarTable.size(); i++){
        if (tmpVarTable[i].vlev == deep && tmpVarTable[i].vname == buffer[idx].word) return false;
    }
    return true;
}
bool check_pro_exe(int idx){
    for (int i = deep; i >= 0; i--){ //从当前深度找起
        for (int i = 0; i < proTable.size(); i++){
            if (proTable[i].plev == deep && proTable[i].pname == buffer[idx].word) return true;
        }
        for (int i = 0; i < tmpProTable.size(); i++){
            if (tmpProTable[i].plev == deep && tmpProTable[i].pname == buffer[idx].word) return true;
        }
    }
    return false;
}
bool check_var_exe(int idx){
    for (int i = deep; i >= 0; i--){ //从当前深度找起
        for (int i = 0; i < varTable.size(); i++){
            if (varTable[i].vlev == deep && varTable[i].vname == buffer[idx].word) return true;
        }
        for (int i = 0; i < tmpVarTable.size(); i++){
            if (tmpVarTable[i].vlev == deep && tmpVarTable[i].vname == buffer[idx].word) return true;
        }
    }
    return false;
}
void setTable(int idx){
    ProTable pt(buffer[idx].word, deep, varTable.size(), varTable.size());
    VarTable vt(buffer[idx].word, proStack.top(), 0, deep, varTable.size()); //变量
    VarTable vt_(buffer[idx].word, proStack.top(), 1, deep, varTable.size()); //形参

    switch(state){
        case dec_pro: 
            if (check_pro_dec(idx)) tmpProTable.push_back(pt);
            else errIden[state].push_back(buffer[idx].word);
            break;
        case dec_var: 
            if (check_var_dec(idx)){//未重定义
                if (buffer[idx].word == buffer[parIdx].word){ //形参
                    tmpVarTable.push_back(vt_);
                } else tmpVarTable.push_back(vt); //变量
            } else errIden[state].push_back(buffer[idx].word);
            break;
        case exe_pro: 
            if (!check_pro_exe(idx)) errIden[state].push_back(buffer[idx].word);
            break;
        case exe_var: 
            if (!check_var_exe(idx)) errIden[state].push_back(buffer[idx].word);
            break;
        case par_var: 
            parIdx = idx;
            break;
    }
}
void reportErr(){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < errIden[i].size(); j++){
            switch(i){
                case 0:  //exe pro
                    printf("error: undeclared of function: '%s'\n", errIden[i][j].c_str());
                    break;
                case 1:  //exe var
                    printf("error: undeclared of variable: '%s'\n", errIden[i][j].c_str());
                    break;
                case 2:  //dec pro
                    printf("error: redefinition of function: '%s'\n", errIden[i][j].c_str());
                    break;
                case 3: //dec var
                    printf("error: redefinition of variable: '%s'\n", errIden[i][j].c_str());
                    break;
            }
        }
    }
    push();
    clearErrRec();
}
void clearErrRec(){
    for (int i = 0; i < 4; i++) errIden[i].clear();
    tmpVarTable.clear();
    tmpProTable.clear();
}
void push(){
    for (int i = 0; i < tmpVarTable.size(); i++) {
        varTable.push_back(tmpVarTable[i]);
        varTable.end()->vadr = varTable.size() - 1;
    }
    for (int i = 0; i < tmpProTable.size(); i++) proTable.push_back(tmpProTable[i]);
    for (int i = 0; i < tmpVarTable.size(); i++){
        for (int j = 0; j < proTable.size(); j++){
            if (tmpVarTable[i].vlev == proTable[j].plev && 
                    tmpVarTable[i].vpro == proTable[i].pname){ 
                proTable[j].ladr = i + varTable.size() - 1;
                break;
            }
        }
    }
}
