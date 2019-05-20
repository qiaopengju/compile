#include "grammerAnalyse.h"

using namespace std;

/*when word not correct, do:*/
#define ERR_HANDLE(s)\
    if (errState == correct) errState = err;\
    else if (errState == err && buffer[nextWordIdx].word == s){\
        matchErr();\
        getWord();\
    }
/*when handle a word*/
#define CHECK_WORD(s)\
    nowS = s;\
    if (buffer[wordIdx].word != s) {\
        ERR_HANDLE(s);\
    } else {\
        loseSymbolCheck();\
        getWord();\
    }
/*check backtracking*/
#define RETURN_BACKTRACKING()\
    if (errState == err){\
        wordIdx = bufferIdx;\
        nextWordIdx = wordIdx+1;\
        errState = correct;\
        clearErrRec();\
        return false;\
    }
#define CHECK_BACKTRACKING()\
    if (errState == err){\
        wordIdx = bufferIdx;\
        nextWordIdx = wordIdx+1;\
        errState = correct;\
        clearErrRec();\
        return;\
    }
#define PUSH_TO_VECTOR()\
    sTmpW = cTmpW;\
    tmpWord.word = sTmpW;\
    tmpWord.type = tmpT;\
    buffer.push_back(tmpWord);
#define DEBUG()\
    printf("Debug:\tstate:%d index:%d%16s%16s\n", errState, wordIdx, buffer[wordIdx].word.c_str(), buffer[nextWordIdx].word.c_str())

ErrType errState(correct);
State state;
FILE *fWords, *gVarTable, *gProcessTable, *gErr;
unsigned wordIdx(0), nextWordIdx(1);
vector<Word> buffer;
vector<VarTable> varTable, tmpVarTable;
vector<ProTable> proTable, tmpProTable;
stack<string> proStack;
int deep(-1), parIdx(-1);
string lastS, nowS;

void getWord(){
    char cTmpW[17], cEndW[17];
    string sTmpW, sEndW;
    struct Word tmpWord;
    int tmpT;

    if (buffer.size() != 0) lastS = buffer[wordIdx].word;
    if (nextWordIdx+1 == buffer.size()){ //在buffer顶部,读取新的词
        fscanf(fWords, "%16s %2d%16s 24\n", cTmpW, &tmpT, cEndW);
        PUSH_TO_VECTOR();
        wordIdx++;
        nextWordIdx++;
    } else if (nextWordIdx+1 < buffer.size()){ //后移指针
        wordIdx++;
        nextWordIdx++;
    } else{ //buffer为空,读取两个新的词
        fscanf(fWords, "%16s %2d%16s 24\n", cTmpW, &tmpT, cEndW);
        PUSH_TO_VECTOR();
        fscanf(fWords, "%16s %2d%16s 24\n", cTmpW, &tmpT, cEndW);
        PUSH_TO_VECTOR();
    }
}
void tableToFile(){
    for (int i = 0; i < proTable.size(); i++){
        fprintf(gProcessTable, "pname:%-16s\tplev:%d\tfadr:%d\t ladr:%d\n",
                proTable[i].pname.c_str(), proTable[i].plev, proTable[i].fadr, proTable[i].ladr);
    }
    for (int i = 0; i < varTable.size(); i++){
        fprintf(gVarTable, "pname:%-16s\tvproc:%-16s\tvkind:%d\tvlev:%d\tvadr:%d\ttypes:ints\n", 
                varTable[i].vname.c_str(), varTable[i].vpro.c_str(), varTable[i].vkind,
                varTable[i].vlev, varTable[i].vadr);
    }
}

int main(){
    fWords = fopen("wAnalyse", "r");
    gVarTable= fopen(".var", "w");
    gProcessTable = fopen(".pro", "w");
    gErr = fopen(".err", "w");

    getWord();
    program();
    tableToFile();

    return 0;
}

/*===========================*/
void program(){
    subProgram();
}
void subProgram(){
    CHECK_WORD("begin");
    proStack.push("main");
    deep++;

    /*记录相关信息,生成表*/
    declareList();

    CHECK_WORD(";");

    /*检查标识符定义情况*/
    executeList();
    CHECK_WORD("end");
    deep--;
}
void declareList(){
    declare();
    declareList_();
}
void declareList_(){
    unsigned bufferIdx(wordIdx);

    CHECK_WORD(";");
    if (!declare()) errState = err;
    CHECK_BACKTRACKING();
    declareList_();
}
bool declare(){
    if (!varDeclare()) 
        if (!funcDeclare()) return false;
    return true;
}
bool varDeclare(){
    unsigned bufferIdx = wordIdx;

    CHECK_WORD("integer");
    if (buffer[wordIdx].type != 4){
        state = dec_var;
        var();
    }

    RETURN_BACKTRACKING();
    reportErr();
    return true;
}
void var(){
    identifier();
}
void identifier(){
    if (buffer[wordIdx].type == 10) {
        loseSymbolCheck();
        setTable(wordIdx);
        getWord();
    } else{
        if (errState == correct) errState = err;
        else if (errState == err && buffer[nextWordIdx].type == 10){
            matchErr();
            setTable(nextWordIdx);
            getWord();
        }
    }
}
bool funcDeclare(){
    state = dec_pro;
    unsigned bufferIdx = wordIdx;

    CHECK_WORD("integer");
    CHECK_WORD("function");
    RETURN_BACKTRACKING();
    identifier();
    CHECK_WORD("(");
    parameter();
    CHECK_WORD(")");
    CHECK_WORD(";");
    funcBody();
    RETURN_BACKTRACKING();
    reportErr();
    return true;
}
void parameter(){
    state = par_var;
    var();
}
void funcBody(){
    CHECK_WORD("begin");
    for (int i = tmpProTable.size() - 1; i >= 0; i--){
        if (tmpProTable[i].plev == deep){
            proStack.push(tmpProTable[i].pname);
            break;
        }
    }
    deep++;
    declareList();
    CHECK_WORD(";");
    executeList();
    CHECK_WORD("end");
    deep--;
}
void executeList(){
    execute();
    executeList_();
}
void executeList_(){
    unsigned bufferIdx(wordIdx);
    CHECK_WORD(";");
    CHECK_BACKTRACKING();
    if (!execute()) errState = err;
    CHECK_BACKTRACKING();

    reportErr();
    executeList_();
}
bool execute(){
    if (!read()) 
        if (!write())
            if (!assignment())
                if (!condition())
                    return false;
    return true;
}
bool read(){
    unsigned bufferIdx(wordIdx);

    CHECK_WORD("read");
    CHECK_WORD("(");
    RETURN_BACKTRACKING();
    state = exe_var;
    var();
    CHECK_WORD(")");
    RETURN_BACKTRACKING();
    reportErr();
    return true;
}
bool write(){
    unsigned bufferIdx(wordIdx);

    CHECK_WORD("write");
    CHECK_WORD("(");
    RETURN_BACKTRACKING();
    state = exe_var;
    var();
    CHECK_WORD(")");
    RETURN_BACKTRACKING();
    reportErr();
    return true;
}
bool assignment(){
    unsigned bufferIdx(wordIdx);

    state = exe_var;
    var();
    CHECK_WORD(":=");
    RETURN_BACKTRACKING();
    reportErr();
    arithmenticExp();
    RETURN_BACKTRACKING();
    reportErr();
    return true;
}
void arithmenticExp(){
    item();
    arithmenticExp_();
}
void arithmenticExp_(){
    unsigned bufferIdx(wordIdx);

    CHECK_WORD("-");
    CHECK_BACKTRACKING();
    if (!item()) errState = err;
    CHECK_BACKTRACKING();

    reportErr();
    arithmenticExp_();
}
bool item(){
    if (!factor()) return false;
    item_();

    return true;
}
bool item_(){
    unsigned bufferIdx(wordIdx);

    CHECK_WORD("*");
    RETURN_BACKTRACKING();
    if (!factor()) errState = err;
    RETURN_BACKTRACKING();
    reportErr();
    return true;
}
bool factor(){
    unsigned bufferIdx(wordIdx);
    unsigned backTrackNum(0);

    state = exe_pro;
    if (!funcCall()){
        backTrackNum++;
        errState = correct;
        state = exe_var;
        var();
        if (errState == err){
            backTrackNum++;
            errState = correct;
            if (!constNum()){
                backTrackNum++;
            }
        }
    }
    if (backTrackNum == 3) {
        errState = err;
        RETURN_BACKTRACKING();
    }
    errState = correct;
    reportErr();
    return true;
}
bool constNum(){
    if (buffer[wordIdx].type == 11) {
        loseSymbolCheck();
        getWord();
    } else{
        if (errState == correct) {
            errState = err;
        }
        else if (errState == err && buffer[nextWordIdx].type == 11){
            matchErr();
            getWord();
            return true;
        }
        return false;
    }
    return true;
}
bool funcCall(){
    unsigned bufferIdx(wordIdx);
    state = exe_pro;

    identifier();
    CHECK_WORD("(");
    RETURN_BACKTRACKING();

    arithmenticExp();
    CHECK_WORD(")");
    RETURN_BACKTRACKING();
    reportErr();
    return true;
}
bool condition(){
    unsigned bufferIdx(wordIdx);
    unsigned backTrackNum(0);

    CHECK_WORD("if");
    RETURN_BACKTRACKING();
    conditionExp();
    CHECK_WORD("then");
    execute();
    CHECK_WORD("else");
    execute();
    RETURN_BACKTRACKING();
    reportErr();
    return true;
}
void conditionExp(){
    arithmenticExp();
    conditionOpe();
    arithmenticExp();
}
bool conditionOpe(){
    if (buffer[wordIdx].type >= 13 && buffer[wordIdx].type <= 17) {
        loseSymbolCheck();
        getWord();
    } else{
        if (errState == correct) {
            errState = err;
        }
        else if (errState == err && buffer[nextWordIdx].type >= 13 && 
                buffer[wordIdx].type <= 17){
            matchErr();
            getWord();
            return true;
        }
        return false;
    }
    return true;
}
