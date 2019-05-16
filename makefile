word: wordAnalyse/wordAnalyse.cpp makefile program
	clang++ -o wordAnalyse wordAnalyse.cpp
	make runWordAnalyse
grammer: grammerAnalyse/grammerAnalyse/main.cpp grammerAnalyse.h makefile
	clang++ -o grammerAnalyse -g grammerAnalyse/grammerAnalyse/main.cpp
	make runGrammerAnalyse
runWordAnalyse:
	./wordAnalyse
	echo "\t================log analyse============"
	cat log/.wAnalyse
	echo "\t================log err================"
	cat log/.err
runGrammerAnalyse:
	./grammerAnalyse
