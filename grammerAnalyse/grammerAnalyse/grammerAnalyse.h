#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <stack>

using namespace std;

struct Word{
    string word;
    int type;
};
struct VarTable{
    string vname, vpro, type;
    int vkind; //0:变量 1:形参
    int vadr; //变量位置
    int vlev; //变量层次
    VarTable(string vname, string vpro, int vkind, int vlev, int vadr){
        this->vname =vname;
        this->vpro =vpro;
        this->type = "integer";
        this->vkind =vkind;
        this->vlev = vlev;
        this->vadr =vadr;
    }
};
struct ProTable{
    string pname;
    int plev; //过程层次
    int fadr; //第一个变量在表中的位置
    int ladr; //最后一个变量在表中的位置
    ProTable(string pname, int plev, int fadr, int ladr){
        this->pname = pname;
        this->plev = plev;
        this->fadr = fadr;
        this->ladr = ladr;
    }
};
extern enum ErrType{correct, err, unDefine, duplicateDefine} errState;
extern enum State{exe_pro, exe_var, dec_pro, dec_var, par_var} state;
extern FILE *fWords, *gVarTable, *gProcessTable;
extern unsigned wordIdx, nextWordIdx;
extern vector<Word> buffer;
extern vector<VarTable> varTable, tmpVarTable;
extern vector<ProTable> proTable, tmpProTable;
extern vector<string> tmpLoseSymbol;
extern vector<string> tmpMatchErr;
extern vector<string> errIden[6];
extern stack<string> proStack;
extern stack<int> loseIdx, matchIdx;
extern int deep;
extern int parIdx;
extern string lastS, nowS;
extern FILE *gErr;

void program();
void subProgram();
void declareList();
void declareList_();
bool declare();
bool varDeclare();
void var();
void identifier();
void identifier_();
void character();
void num();
bool funcDeclare();
void parameter();
void funcBody();
void executeList();
void executeList_();
bool execute();
bool read();
bool write();
bool assignment();
void arithmenticExp();
void arithmenticExp_();
bool item();
bool item_();
bool factor();
bool constNum();
bool funcCall();
void unsignedInt();
void unsignedInt_();
bool condition();
void conditionExp();
bool conditionOpe();
void loseSymbol();
void matchErr();
void loseSymbolCheck();
void tableToFile();
bool check_pro_dec(int);
bool check_var_dec(int);
bool check_pro_exe(int);
bool check_var_exe(int);
void setTable(int);
void reportErr();
void clearErrRec(int, int);
void push();
void getWord();
