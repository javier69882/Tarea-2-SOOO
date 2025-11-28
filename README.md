	Tarea-2-SOOO
	Parte 1: Los arhivos de la parte 1 son barrera.c, barrera.h y main.c.
	Para compilarlo gcc -Wall main.c barrera.c -o main -pthread.
	Luego ./main, o ./main a b , donde a numero de hebras y b es numero de etapas
	Parte 2: Los archivos de la parte dos son sim.c, marcos.h, trace1.txt, trace2.txt.
	Para compilarlo gcc -g -Wall -o sim sim.c.
	Luego ./sim Nmarcos tamañoMarco [--verbose] traze.txt  , donde verbose es opcional y trace es trace1.txt o trace2.txt.
