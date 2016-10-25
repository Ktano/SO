i-banco: commandlinereader.o contas.o i-banco.o
	gcc -pthread -o i-banco i-banco.o commandlinereader.o contas.o 
commandlinereader.o: commandlinereader.c
	gcc -g -Wall -pedantic -c commandlinereader.c
contas.o: contas.c contas.h
	gcc -g -Wall -pedantic -c contas.c
i-banco.o: i-banco.c
	gcc -g -Wall -pedantic -c i-banco.c
clean:
	rm -f *.o i-banco
zip:
	zip G91_56564_63108.zip *.c *.h Makefile
