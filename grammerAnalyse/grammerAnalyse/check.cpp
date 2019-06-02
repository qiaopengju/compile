#include "grammerAnalyse.h"

vector<string> errIden[6];
vector<string> tmpLoseSymbol;
vector<string> tmpMatchErr;
vector<int> loseS, matchE;

void loseSymbol(){
    errState = correct;
    //errIden[4].push_back(lastS);
    //fprintf(gErr, "\033[1;31m error:\033[0m loseSymbol %s\n", lastS.c_str());
    tmpLoseSymbol.push_back(lastS);
    /*loseS.push_back((int)tmpLoseSymbol.size() - 1);
    if (lastS == "variable") printf("lose identifier\n");
    else printf("\033[1;31m error:\033[0m loseSymbol %s\n", lastS.c_str());*/
}

void matchErr(){
    errState = correct;
    /*errIden[5].push_back(buffer[wordIdx].word);
    errIden[5].push_back(nowS);*/
    tmpMatchErr.push_back(buffer[wordIdx].word);
    tmpMatchErr.push_back(lastS);
    matchE.push_back((int)tmpMatchErr.size() - 1);
    matchE.push_back((int)tmpMatchErr.size() - 2);
    if (tmpMatchErr[tmpMatchErr.size()-1] == "read"){
        getWord();
    }
    //printf("\033[1;31m error:\033[0m unknow symbol '%s', did you mean '%s'?\n", buffer[wordIdx].word.c_str(), nowS.c_str());
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
        for (int j = 0; j < proTable.size(); j++){
            if (proTable[j].plev == i && proTable[j].pname == buffer[idx].word) return true;
        }
        for (int j = 0; j < tmpProTable.size(); j++){
            if (tmpProTable[j].plev == i && tmpProTable[j].pname == buffer[idx].word) return true;
        }
    }
    return false;
}
bool check_var_exe(int idx){
    for (int i = deep; i >= 0; i--){ //从当前深度找起
        for (int j = 0; j < varTable.size(); j++){
            if (varTable[j].vlev == i && varTable[j].vname == buffer[idx].word) return true;
        }
        for (int j = 0; j < tmpVarTable.size(); j++){
            if (tmpVarTable[j].vlev == i && tmpVarTable[j].vname == buffer[idx].word) return true;
        }
    }
    return false;
}
void setTable(int idx){
    ProTable pt(buffer[idx].word, deep, (int)varTable.size(), (int)varTable.size());
    VarTable vt(buffer[idx].word, proStack.top(), 0, deep, (int)varTable.size()); //变量
    VarTable vt_(buffer[idx].word, proStack.top(), 1, deep, (int)varTable.size()); //形参

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
    push();
    for (int i = 0; i < 6; i++){
        for (int j = 0; j < errIden[i].size(); j++){
            switch(i){
                case 0:  //exe pro
                    fprintf(gErr, "\033[1;31m error:\033[0m undeclared of function: '%s'\n", errIden[i][j].c_str());
                    break;
                case 1:  //exe var
                    fprintf(gErr, "\033[1;31m error:\033[0m undeclared of variable: '%s'\n", errIden[i][j].c_str());
                    break;
                case 2:  //dec pro
                    fprintf(gErr, "\033[1;31m error:\033[0m redefinition of function: '%s'\n", errIden[i][j].c_str());
                    break;
                case 3: //dec var
                    fprintf(gErr, "\033[1;31m error:\033[0m redefinition of variable: '%s'\n", errIden[i][j].c_str());
                    break;
                case 4: //lose symbol
                    fprintf(gErr, "\033[1;31m error:\033[0m loseSymbol %s\n", errIden[i][j].c_str());
                    break;
                case 5: //match err
                    if (2 * j >= errIden[i].size()) break;
                    fprintf(gErr, "\033[1;31m error:\033[0m unknow symbol '%s', did you mean '%s'?\n", errIden[i][2 * j].c_str(), errIden[i][2 * j + 1].c_str());
                    break;
            }
        } 
    }
    clearErrRec(0, 0);
    errIden[4].clear();
    errIden[5].clear();
}
void clearErrRec(int lIdx, int mIdx){
    for (int i = 0; i < 5; i++) errIden[i].clear();
    tmpVarTable.clear();
    tmpProTable.clear();
    //if (tmpLoseSymbol.size() != 0) tmpLoseSymbol.erase(tmpLoseSymbol.begin() + loseS[0], tmpLoseSymbol.end());
    //if (tmpMatchErr.size() != 0) tmpMatchErr.erase(tmpMatchErr.begin() + matchE[0], tmpMatchErr.end());
    //loseS.clear();
    //matchE.clear();
    if (tmpLoseSymbol.size() > lIdx)
        tmpLoseSymbol.erase(tmpLoseSymbol.begin() + lIdx, tmpLoseSymbol.end());
    if (tmpMatchErr.size() > mIdx)
        tmpMatchErr.erase(tmpMatchErr.begin() + mIdx, tmpMatchErr.end());
    //if (loseIdx.size() > 0) loseIdx.pop();
    //if (matchIdx.size() > 0) matchIdx.pop();
}
void push(){
    for (int i = 0; i < tmpVarTable.size(); i++) {
        varTable.push_back(tmpVarTable[i]);
        varTable.end()->vadr = (int)varTable.size() - 1;
    }
    for (int i = 0; i < tmpProTable.size(); i++) proTable.push_back(tmpProTable[i]);
    for (int i = 0; i < tmpVarTable.size(); i++){
        for (int j = 0; j < proTable.size(); j++){
            if (tmpVarTable[i].vlev == proTable[j].plev+1 && 
                    tmpVarTable[i].vpro == proTable[j].pname){ 
                proTable[j].ladr = i + (int)varTable.size() - 1;
                break;
            }
        }
    }
    for (int i = 0; i < tmpLoseSymbol.size(); i++) errIden[4].push_back(tmpLoseSymbol[i]);
    for (int i = 0; i < tmpMatchErr.size(); i++) errIden[5].push_back(tmpMatchErr[i]);
}
