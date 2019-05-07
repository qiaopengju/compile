wordAnalyse: wordAnalyse.cpp makefile program
	clang++ -o wordAnalyse -g wordAnalyse.cpp
	clear
	make run
run:
	./wordAnalyse
	echo "\t================log analyse============"
	cat log/.wAnalyse
	echo "\t================log err================"
	cat log/.err
