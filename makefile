CC = g++ -std=c++17
CFLAGS = -Wall -Wextra -Werror
LFLAGS = -lgtest
BD=build
INSTALL=install
BACKUP=backup_maze
APP=maze.app

OS = $(shell uname -s)
ifeq ($(OS), Linux)
	LFLAGS += -pthread -lsubunit
	OPEN = -xdg-open
	INSTFOLDER=$(HOME)/maze
else
	OPEN = -open
	INSTFOLDER=$(HOME)/Applications/$(APP)
endif

PROFILE = maze.pro

all: clean build

build : mk replication
	cd $(BD); qmake $(PROFILE)
	make -C $(BD)/
	cd $(BD); rm -rf *.cpp *.h *.hpp *.ui *.o *.icns *.pro *.user makefile icon.png 

mk :
	-mkdir $(BD)/

replication :
	cp $(PROFILE) $(BD)/
	cp *.cpp $(BD)/
	cp *hpp $(BD)/
	cp *.ui $(BD)/
	cp icon.png $(BD)/
ifeq ($(OS), Linux)
	cp -r files_maze_cave/ $(BD)/
else
	-mkdir $(BD)/files_maze_cave
	cp -r files_maze_cave/ $(BD)/files_maze_cave
endif

install : uninstall build
ifeq ($(OS), Linux)
	cp -r $(BD)/ $(INSTFOLDER)
else
	cp -rf $(BD)/$(APP) $(HOME)/Applications
endif
	$(RM) -rf $(BD)

uninstall :
	rm -rf $(INSTFOLDER)

tests : clean
	$(CC) $(FLAGS) tests.cpp maze.cpp cave.cpp -o test $(LFLAGS)
	./test

leaks :	tests
	leaks --atExit -- ./test

dist : clean
	rm -rf $(BACKUP)
	-mkdir $(BACKUP)
	-mkdir $(BACKUP)/misc
	-mkdir $(BACKUP)/src
	cp ../*.md $(BACKUP)/
ifeq ($(OS), Linux)
	cp -r ../misc $(BACKUP)/
	cp -r ./files_maze_cave $(BACKUP)/src/
	cp -r ./linters $(BACKUP)/src/
else
	cp -r ../misc/ $(BACKUP)/misc
	-mkdir $(BACKUP)/src/files_maze_cave
	cp -r ./files_maze_cave/ $(BACKUP)/src/files_maze_cave
	-mkdir $(BACKUP)/src/linters
	cp -r linters/ $(BACKUP)/src/linters/
endif
	cp *.cpp $(BACKUP)/src/
	cp *hpp $(BACKUP)/src/
	cp $(PROFILE) $(BACKUP)/src/
	cp *.ui $(BACKUP)/src/
	cp makefile $(BACKUP)/src/
	cp icon.png $(BACKUP)/src/
	tar -cvzf $(HOME)/Desktop/dist_maze.tgz $(BACKUP)/
	rm -rf $(BACKUP)/

cpplint :
	cp linters/CPPLINT.cfg ./
	-python3 linters/cpplint.py --extensions=cpp *.cpp
	-python3 linters/cpplint.py --extensions=hpp *.hpp
	$(RM) CPPLINT.cfg

cppcheck :
	cppcheck --std=c++17 --enable=all --check-config --suppress=missingIncludeSystem --suppress=missingInclude --suppress=unmatchedSuppression *.cpp *.hpp *.h

gcov_report : clean
	$(MAKE) LFLAGS="$(LFLAGS) --coverage" tests
	lcov -t test -o test.info -c -d . --no-external
	genhtml -o report test.info
	$(OPEN) report/index.html

dvi :
	-makeinfo --html --force man.tex
	$(OPEN) man/index.html

report_clean :
	$(RM) -rf ./*.gcda ./*.gcno ./*.info ./*.gch ./report

clean : report_clean
	$(RM) -rf test *.a *.so *.o *.cfg *.gcda *.gcno *.html *.info *.dSYM report man cave_test.txt
	$(RM) -rf $(BD)
