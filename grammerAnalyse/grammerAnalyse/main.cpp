#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
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
        return false;\
    }
#define CHECK_BACKTRACKING()\
    if (errState == err){\
        wordIdx = bufferIdx;\
        nextWordIdx = wordIdx+1;\
        errState = correct;\
        return;\
    }
#define PUSH_TO_VECTOR()\
    sTmpW = cTmpW;\
    tmpWord.word = sTmpW;\
    tmpWord.type = tmpT;\
    buffer.push_back(tmpWord);
#define DEBUG()\
    printf("Debug:\tstate:%d index:%d%16s%16s\n", errState, wordIdx, buffer[wordIdx].word.c_str(), buffer[nextWordIdx].word.c_str())

struct Word{
    string word;
    int type;
};
enum ErrType{correct, err, unDefine, duplicateDefine} errState(correct);
vector<Word> buffer;
FILE *fWords, *gVarTable, *gProcessTable;
unsigned wordIdx(0), nextWordIdx(1);

void getWord(){
    char cTmpW[17], cEndW[17];
    string sTmpW, sEndW;
    struct Word tmpWord;
    int tmpT;

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

void loseSymbol(){
    errState = correct;
    printf("loseSymbol\n");
}

void matchErr(){
    errState = correct;
    printf("matchErr\n");
}


/*when word correct, do:*/
void loseSymbolCheck(){
    if (errState == err){
        loseSymbol();
    }
}

int main(){
    fWords = fopen("wAnalyse", "r");
    gVarTable= fopen(".var", "w");
    gProcessTable = fopen(".pro", "w");

    getWord();
    program();

    return 0;
}

void program(){
    subProgram();
}
void subProgram(){
    CHECK_WORD("begin");
    DEBUG();

    declareList();
    DEBUG();

    CHECK_WORD(";");

    executeList();
    CHECK_WORD("end");
    DEBUG();
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
    //unsigned backTrackNum(0);
    //if (!varDeclare()) backTrackNum++;
    //if (!funcDeclare()) backTrackNum++;
    //if (backTrackNum == 2) return false;
    if (!varDeclare()) 
        if (!funcDeclare()) return false;
    return true;
}
bool varDeclare(){
    unsigned bufferIdx = wordIdx;

    CHECK_WORD("integer");
    if (buffer[wordIdx].type != 4){
        var();
    }

    RETURN_BACKTRACKING();
    return true;
}
void var(){
    identifier();
}
void identifier(){
    if (buffer[wordIdx].type == 10) {
        loseSymbolCheck();
        getWord();
    } else{
        if (errState == correct) errState = err;
        else if (errState == err && buffer[nextWordIdx].type == 10){
            matchErr();
            getWord();
        }
    }
}
bool funcDeclare(){
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
    return true;
}
void parameter(){
    var();
}
void funcBody(){
    CHECK_WORD("begin");
    declareList();
    CHECK_WORD(";");
    executeList();
    CHECK_WORD("end");
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

    executeList_();
}
bool execute(){
    //unsigned backTrackNum(0);
    //if (!read()) backTrackNum++;
    //if (!write()) backTrackNum++;
    //if (!assignment()) backTrackNum++;
    //if (!condition()) backTrackNum++;
    //if (backTrackNum == 4) return false;
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
    var();
    CHECK_WORD(")");
    RETURN_BACKTRACKING();
    return true;
}
bool write(){
    unsigned bufferIdx(wordIdx);

    CHECK_WORD("write");
    CHECK_WORD("(");
    RETURN_BACKTRACKING();
    var();
    CHECK_WORD(")");
    RETURN_BACKTRACKING();
    return true;
}
bool assignment(){
    unsigned bufferIdx(wordIdx);
    var();
    CHECK_WORD(":=");
    RETURN_BACKTRACKING();
    arithmenticExp();
    RETURN_BACKTRACKING();
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
    return true;
}
bool factor(){
    unsigned bufferIdx(wordIdx);
    unsigned backTrackNum(0);

    if (!funcCall()){
        backTrackNum++;
        errState = correct;
        var();
        if (errState == err){
            backTrackNum++;
            errState = correct;
            if (!constNum()){
                backTrackNum++;
            }
        }
    }
    /*var();
    if (errState == err) {
        backTrackNum++; 
        errState = correct;
        if (!constNum()){
            backTrackNum++;
            errState = correct;
            if (!funcCall()) backTrackNum++;
        }
    }*/
    if (backTrackNum == 3) {
        errState = err;
        //return false;
        RETURN_BACKTRACKING();
    }
    errState = correct;
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

    identifier();
    CHECK_WORD("(");
    RETURN_BACKTRACKING();
    arithmenticExp();
    CHECK_WORD(")");
    RETURN_BACKTRACKING();
    return true;
}
/*void unsignedInt(){
}
void unsignedInt_(){
}*/
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
