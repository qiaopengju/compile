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
    lastS = nowS;\
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
        clearErrRec(loseIdx.top(), matchIdx.top());\
        return false;\
    }
#define CHECK_BACKTRACKING()\
    if (errState == err){\
        wordIdx = bufferIdx;\
        nextWordIdx = wordIdx+1;\
        errState = correct;\
        clearErrRec(loseIdx.top(), matchIdx.top());\
        return;\
    }
#define PUSH_TO_VECTOR()\
    sTmpW = cTmpW;\
    tmpWord.word = sTmpW;\
    tmpWord.type = tmpT;\
    buffer.push_back(tmpWord);
#define MY_DEBUG()\
    printf("Debug:\tstate:%d index:%d%16s%16s\n", errState, wordIdx, buffer[wordIdx].word.c_str(), buffer[nextWordIdx].word.c_str())
#define INIT_IDX()\
    loseIdx.push(tmpLoseSymbol.size());\
    matchIdx.push(tmpMatchErr.size())

ErrType errState(correct);
State state;
FILE *fWords, *gVarTable, *gProcessTable, *gErr;
unsigned wordIdx(0), nextWordIdx(1);
vector<Word> buffer;
vector<VarTable> varTable, tmpVarTable;
vector<ProTable> proTable, tmpProTable;
stack<string> proStack;
stack<int> loseIdx, matchIdx;
int deep(-1), parIdx(-1), lines(0);
string lastS, nowS("");

void getWord(){
    char cTmpW[17], cEndW[17];
    string sTmpW;
    struct Word tmpWord;
    int tmpT;

    //if (buffer.size() != 0) lastS = buffer[wordIdx].word;
    if (nextWordIdx+1 == buffer.size()){ //在buffer顶部,读取新的词
        //fscanf(fWords, "%16s %2d\n", cTmpW, &tmpT);
        do {
            fscanf(fWords, "%16s %2d\n", cTmpW, &tmpT);
            if (tmpT == 24) lines++;
        } while (tmpT == 24);
        PUSH_TO_VECTOR();
        wordIdx++;
        nextWordIdx++;
    } else if (nextWordIdx+1 < buffer.size()){ //后移指针
        wordIdx++;
        nextWordIdx++;
    } else{ //buffer为空,读取两个新的词
        do {
            fscanf(fWords, "%16s %2d\n", cTmpW, &tmpT);
            if (tmpT == 24) lines++;
        } while (tmpT == 24);
        PUSH_TO_VECTOR();
        do {
            fscanf(fWords, "%16s %2d\n", cTmpW, &tmpT);
            if (tmpT == 24) lines++;
        } while (tmpT == 24);
        //fscanf(fWords, "%16s %2d\n", cTmpW, &tmpT);
        PUSH_TO_VECTOR();
    }
}
void printNormalErr(){
    for (int i = 4; i <=5; i++){
        for (int j = 0; j < errIden[i].size(); j++){
            if (i == 4){ //lose symbol
                fprintf(gErr, "error: loseSymbol %s\n", errIden[i][j].c_str());
            }else{
                if (2 * j >= errIden[i].size()) break;
                fprintf(gErr, "error: unknow symbol '%s', did you mean '%s'?\n", errIden[i][2 * j].c_str(), errIden[i][2 * j + 1].c_str());
            }
        }
    }
    errIden[4].clear();
    errIden[5].clear();
}
void tableToFile(){
    for (int i = 0; i < proTable.size(); i++){
        fprintf(gProcessTable, "pname:%-16s\tplev:%d\tfadr:%d\t ladr:%d\n",
                proTable[i].pname.c_str(), proTable[i].plev, proTable[i].fadr, proTable[i].ladr);
    }
    for (int i = 0; i < varTable.size(); i++){
        fprintf(gVarTable, "vname:%-16s\tvproc:%-16s\tvkind:%d\tvlev:%d\tvadr:%d\ttypes:ints\n", 
                varTable[i].vname.c_str(), varTable[i].vpro.c_str(), varTable[i].vkind,
                varTable[i].vlev, varTable[i].vadr);
    }
}

int main(){
    fWords = fopen("wAnalyse", "r");
    gVarTable= fopen(".var", "w");
    gProcessTable = fopen(".pro", "w");
    gErr = fopen(".grammerErr", "w");

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

    if (buffer[nextWordIdx].word == "integer"){
        declareList_();
    }
}
void declareList_(){
    CHECK_WORD(";");
    declare();

    if (buffer[nextWordIdx].word == "integer"){
        declareList_();
    }
}
void declare(){
    if (buffer[nextWordIdx].word == "function"){
        funcDeclare();
    } else{
        varDeclare();
    }
}
void varDeclare(){
    CHECK_WORD("integer");
    state = dec_var;
    var();

    reportErr();
}
void var(){
    identifier();
}
void identifier(){
    lastS = nowS;
    nowS = "variable";
    if (buffer[wordIdx].type == 10) {
        loseSymbolCheck();
        setTable(wordIdx);
        getWord();
    } else{
        if (errState == correct) {
            errState = err;
        } else if (errState == err && buffer[nextWordIdx].type == 10){
            matchErr();
            setTable(nextWordIdx);
            getWord();
            getWord();
        }
    }
}
void funcDeclare(){
    state = dec_pro;

    CHECK_WORD("integer");
    CHECK_WORD("function");
    identifier();

    CHECK_WORD("(");
    parameter();
    CHECK_WORD(")");
    CHECK_WORD(";");
    funcBody();
    reportErr();
}
void parameter(){
    state = par_var;
    var();
}
void funcBody(){
    CHECK_WORD("begin");
    for (int i = (int)tmpProTable.size() - 1; i >= 0; i--){
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

    if (buffer[wordIdx].word == ";" || buffer[nextWordIdx].word == "read" ||
            buffer[nextWordIdx].word == "write" || buffer[nextWordIdx].type == 10 || 
            buffer[nextWordIdx].word == "if"){
        executeList_();
    }
}
void executeList_(){
    CHECK_WORD(";");
    execute();

    reportErr();
    if (buffer[wordIdx].word == ";" || buffer[nextWordIdx].word == "read" ||
            buffer[nextWordIdx].word == "write" || buffer[nextWordIdx].type == 10 || 
            buffer[nextWordIdx].word == "if"){
        executeList_();
    }
}
void execute(){
    if (buffer[wordIdx].word == "read"){
        read();
    } else if (buffer[wordIdx].word == "write"){
        write();
    } else if (buffer[wordIdx].type == 10){
        assignment();
    } else if (buffer[wordIdx].word == "if"){
        condition();
    } else{//如果执行语句中出现声明，则跳转到其fllow集';'中，并打出错误
        //err
        string tS = "";
        while(buffer[wordIdx].word != ";"){
            tS = tS + buffer[wordIdx].word;
            getWord();
        }
        fprintf(gErr, "error: wrong paragraphs: '%s'\n", tS.c_str());
        errState = correct;
    }
}
void read(){
    CHECK_WORD("read");
    CHECK_WORD("(");
    state = exe_var;
    var();
    CHECK_WORD(")");
    reportErr();
}
void write(){
    CHECK_WORD("write");
    CHECK_WORD("(");
    state = exe_var;
    var();
    CHECK_WORD(")");
    reportErr();
}
void assignment(){
    state = exe_var;
    var();
    CHECK_WORD(":=");
    arithmenticExp();
    reportErr();
}
void arithmenticExp(){
    item();
    if (buffer[wordIdx].word == "-" && (buffer[nextWordIdx].type == 10 ||
            buffer[nextWordIdx].type == 11)){
        arithmenticExp_();
    }
}
void arithmenticExp_(){
    CHECK_WORD("-");
    item();

    reportErr();
    if (buffer[wordIdx].word == "-" || buffer[nextWordIdx].type == 10 ||
            buffer[nextWordIdx].type == 11){
        arithmenticExp_();
    }
}
void item(){
    factor();
    if (buffer[wordIdx].word == "*"){
        item_();
    }
}
void item_(){
    CHECK_WORD("*");
    factor();

    reportErr();
}
void factor(){
    getWord(); getWord(); 
    wordIdx -= 2; nextWordIdx -= 2;
    if (buffer[wordIdx].type == 10){
        if (buffer[nextWordIdx].word == "("){
            state = exe_pro;
            funcCall();
        } else{
            state = exe_var;
            var();
        }
    } else if (buffer[wordIdx].type == 11){
        constNum();
    }
    reportErr();
}
void constNum(){
    if (buffer[wordIdx].type == 11) {
        loseSymbolCheck();
        getWord();
    } else{
        if (errState == correct) {
            errState = err;
        } else if (errState == err && buffer[nextWordIdx].type == 11){
            matchErr();
            getWord();
        }
    }
}
void funcCall(){
    identifier();
    CHECK_WORD("(");

    arithmenticExp();
    CHECK_WORD(")");
    reportErr();
}
void condition(){
    CHECK_WORD("if");
    conditionExp();
    CHECK_WORD("then");
    execute();
    CHECK_WORD("else");
    execute();
    reportErr();
}
void conditionExp(){
    arithmenticExp();
    conditionOpe();
    arithmenticExp();
}
void conditionOpe(){
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
        }
    }
}
