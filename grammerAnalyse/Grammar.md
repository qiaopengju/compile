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
<FUNC_DECLARE> => interger function <IDENTIFIER> (<PARAMETER>); <FUNC_BODY>
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
    integer function F(n);
        begin
            integer n;
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