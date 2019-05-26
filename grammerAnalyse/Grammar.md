```pascal
 begin
    integer k;
    integer function F(n);
        begin
            integer n;
            if n<=0 then F:=1
            else F:=n*F(n-1)
        end;
    read(m);
    k:=F(m);
    write(k)
end
```

```c++
<PROGRAM> => <SUB_PROGRAM>
<SUB_PROGRAM> => begin <DECLARE_LIST>; <EXECUTE_LIST> end
<DECLARE_LIST> => <DECLARE><DECLARE_LIST_>
<DECLARE_LIST_> => ;<DECLARE><DECLARE_LIST_> | null
<DECLARE> => <VAR_DECLARE> | <FUNC_DECLARE>
<VAR_DECLARE> => integer <VAR>
<VAR => <IDENTIFIER>
<IDENTIFIER> => <CHARACTER> | <IDENTIFIER_>
<IDENTIFIER_> => <CHARACTER><IDENTIFIER_> | <NUM><IDENTIFIER_> | null
<CHARACTER> => a│b│c│d│e│f│g│h│i│j│k│l│m│n│o │p│q │r│s│t│u│v│w│x│y│z
<NUM> => 0│1│2│3│4│5│6│7│8│9
*<FUNC_DECLARE> => interger function <IDENTIFIER> (<PARAMETER>); <FUNC_BODY>
<PARAMETER> => <VAR>
<FUNC_BODY> => begin <DECLARE_LIST>; <EXECUTE_LIST> end
<EXECUTE_LIST> => <EXECUTE><EXECUTE_LIST_>
<EXECUTE_LIST_> => ;<EXECUTE><EXECUTE_LIST_> | null
<EXECUTE> => <READ> | <WRITE> | <ASSIGNMENT> | <CONDITION>
<READ> => read(<VAR>)
<WRITE> = > write(<VAR>)
<ASSIGNMENT> => <VAR> := <ARITHMETIC_EXP>
<ARITHMETIC_EXP> => <ITEM><ARITHMETIC_EXP_>
<ARITHMETIC_EXP_> => -<ITEM><ARITHMETIC_EXP_> | null
<ITEM> => <FACTOR><ITEM_>
<ITEM_> => *<FACTOR> | null
<FACTOR> => <VAR> | <CONST_NUM> | <FUNC_CALL>
<CONST_NUM> => <UNSIGNED_INT>
<FUNC_CALL> => <IDENTIFIER> (<ARITHMETIC_EXP>)
<UNSIGNED_INT> => <NUM><UNSIGNED_INT_>
<UNSIGNED_INT_> => <NUM><UNSIGNED_INT_> | null
<CONDITION> => if <CONDITION_EXP> then <EXECUTE> else <EXECUTE>
<CONDITION_EXP> => <ARITHMETIC_EXP><CONDITION_OPE><ARITHMETIC_EXP>
<CONDITION_OPE> => <│<=│>│>=│=│<>
```



```pascal
	begin
    integer k;
    integer m; //add
    integer func F(n);
        begin
            integer n;
            integer F; //add
            if n<=0 then F:=1
            else F:=n*F(n-1)
        end
    read(m);
    k:=F(m);
    write(k)
end
```

* 缺少函数调用文法
* 需回溯判错

---

变量名表

* 变量名vname: char(16)

* 所属过程vproc:char(16)
* //变量类型vtype: types

* 分类vkind: 0..1(0—变量、1—形参)

* 变量层次vlev: int

* 变量在变量表中的位置vadr: int(相对第一个变量而言)

* types=(ints)



---

过程名表

* 过程名pname: char(16)
* //过程类型ptype: types
* 过程层次plev: int
* 第一个变量在变量表中的位置fadr: int
* 最后一个变量在变量表中的位置ladr: int



```dart
<P> => <SUB_P> //<程序>→<分程序>
/*<分程序>→begin <说明语句表>；<执行语句表> end*/
<SUB_P> => begin <DECLARE_LIST>; <EXECUTE_LIST>; end
/*<说明语句表>→<说明语句>│<说明语句表> ；<说明语句>*/
<DECLARE_LIST> => <DECLARE> | <DECLARE_LIST> ; <DECLARE>
<DECLARE> => <VAR_DECLARE> | <FUNC_DECLARE> /*<说明语句>→<变量说明>│<函数说明>*/
<VAR_DECLARE> => integer <VAR> 							/*<变量说明>→integer <变量>*/
<VAR => <IDENTIFIER> 												/*<变量>→<标识符>*/
<IDENTIFIER> => <CHARACTER> | <IDENTIFIER><CHARACTER> | <IDENTIFIER><NUM>
<CHARACTER> => a│b│c│d│e│f│g│h│i│j│k│l│m│n│o │p│q │r│s│t│u│v│w│x│y│z
<NUM> => 0│1│2│3│4│5│6│7│8│9
/*<函数说明>→integer function <标识符>（<参数>）；<函数体>*/
<FUNC_DECLARE> => interger function <IDENTIFIER> (<PARAMETER>); <FUNC_BODY>
<PARAMETER> => <VAR> //<参数>→<变量>
/*<函数体>→begin <说明语句表>；<执行语句表> end*/
<FUNC_BODY> => begin <DECLARE_LIST>; <EXECUTE_LIST> end
/*<执行语句表>→<执行语句>│<执行语句表>；<执行语句>*/
<EXECUTE_LIST> => <EXECUTE> | <EXECUTE_LIST>; <EXECUTE>
/*<执行语句>→<读语句>│<写语句>│<赋值语句>│<条件语句>*/
<EXECUTE> => <READ> | <WRITE> | <ASSIGNMENT> | <CONDITION>
<READ> => read(<VAR>)											/*<读语句>→read(<变量>)*/
<WRITE> = > write(<VAR>)									/*<写语句>→write(<变量>)*/
<ASSIGNMENT> => <VAR> := <ARITHMETIC_EXP>/*<赋值语句>→<变量>:=<算术表达式>*/
/*<算术表达式>→<算术表达式>-<项>│<项>*/
<ARITHMETIC_EXP> => <ARITHMETIC_EXP> - <ITEM> | <ITEM>
/*<项>→<项>*<因子>│<因子>*/
<ITEM> => <ITEM> * <FACTOR> | <FACTOR>
/*<因子>→<变量>│<常数>│<函数调用>*/
<FACTOR> => <VAR> | <CONST> | <FUNC_CALL>
<CONST> => <UNSIGNED_INT>
<UNSIGNED_INT> => <NUM> | <UNSIGNED_INT><INT>
/*<条件语句>→if<条件表达式>then<执行语句>else <执行语句>*/
<CONDITION> => if <CONDITION_EXP> then <EXECUTE> else <EXECUTE>
/*<条件表达式>→<算术表达式><关系运算符><算术表达式>*/
<CONDITION_EXP> => <ARITHMETIC_EXP><CONDITION_OPE><ARITHMETIC_EXP>
/*<关系运算符> →<│<=│>│>=│=│<>*/
<CONDITION_OPE> => <│<=│>│>=│=│<>
/*函数调用*/
<FUNC_CALL> => <IDENTIFIER> (<ARITHMETIC_EXP>)
```

