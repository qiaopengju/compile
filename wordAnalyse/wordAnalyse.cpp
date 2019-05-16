#include <cstdio>
#include <cstdlib>
#include <string>

#define PRINT(f, name, type) \
    fprintf(f, "%16s %2d%16s %d\n", name, type, "EOLN", 24)

#define IDENTIFIER() \
    if (!s.compare("begin")) state = 1;\
    else if (!s.compare("end")) state = 2;\
    else if (!s.compare("integer")) state = 3;\
    else if (!s.compare("if")) state = 4;\
    else if (!s.compare("then"))state = 5;\
    else if (!s.compare("else")) state = 6;\
    else if (!s.compare("function")) state = 7;\
    else if (!s.compare("read")) state = 8;\
    else if (!s.compare("write"))state = 9;\
    else

FILE *fProgram, *fwAnalyse, *fwErr;
enum errorTpye{illegal, colonErr};
int state(0), lineNum(1);
std::string s("");

void errPrint(errorTpye);
void wordAnalyse(char);
void accomplishOneW();

int main(){
    fProgram = fopen("../program", "r");
    fwAnalyse = fopen("../log/.wAnalyse", "w");
    fwErr = fopen("../log/.err", "w");

    char c = getc(fProgram);
    while(c != EOF){
        //c = getc(fProgram);
        if (c == ' ' || c == '\n'){
            if (state == 10 || state == 11) accomplishOneW();
            else if (state == 20) errPrint(colonErr);
            if (c == '\n') ++lineNum;
        }else wordAnalyse(c);
        c = getc(fProgram);
    }
    PRINT(fwAnalyse, "EOF", 25); //EOF
    fclose(fProgram);
    fclose(fwAnalyse);
    fclose(fwErr);

    return 0;
}

void wordAnalyse(char c){
    if (state == 0){
        s += c;
        if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122)){ //character
            state = 10;
        }else if (c >= 48 && c <= 57){ //number
            state = 11;
        }else{
            switch (c){
                case '=': state = 12; accomplishOneW(); break;
                case '<': state = 15; break;
                case '>': state = 17; break;
                case '-': state = 18; accomplishOneW(); break;
                case '*': state = 19; accomplishOneW(); break;
                case ':': state = 20; break; //:=
                case '(': state = 21; accomplishOneW(); break;
                case ')': state = 22; accomplishOneW(); break;
                case ';': state = 23; accomplishOneW(); break;
                default : errPrint(illegal); break;
            }
        }
    }else{
        switch (state){
            case 10: //identififier
                if ((c < 65 || c > 90) && (c < 97 || c > 122)){ //not a character or a number
                    accomplishOneW();
                    wordAnalyse(c);
                }else s += c;
                break; 
            case 11: //number
                if (c < 48 || c > 57){ //error
                    accomplishOneW();
                    wordAnalyse(c);
                }else s += c;
                break;
            case 15: //<
                if (c == '='){ //<=
                    s += c;
                    state = 14;
                    accomplishOneW();
                }else if (c == '>'){ //<>
                    s += c;
                    state = 13;
                    accomplishOneW();
                }else{ //<
                    accomplishOneW();
                    wordAnalyse(c);
                }
                break; 
            case 17: //>
                if (c == '='){ //>=
                    s += c;
                    state = 16;
                    accomplishOneW();
                }else{ //>
                    accomplishOneW();
                    wordAnalyse(c);
                }
                break; 
            case 20: //:
                if (c == '='){
                    s += c;
                    state = 20;
                    accomplishOneW();
                }else{
                    errPrint(colonErr);
                    wordAnalyse(c);
                }
                break; 
        }
    }
}

void errPrint(errorTpye type){
    fprintf(fwErr, "***LINE:%d  %s: '%s'\n", lineNum, type == illegal ? "illegal word" : "colonErr", s.c_str());
    state = 0;
    s = "";
}

void accomplishOneW(){
    if (state == 10){
        IDENTIFIER();
    }
    PRINT(fwAnalyse, (char*)s.c_str(), state);
    s = "";
    state = 0;
}
