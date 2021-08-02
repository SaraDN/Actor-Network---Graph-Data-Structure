# Nucuta Diana Cristina 314 CC

Aceasta tema prezinta un caz real de utilizare a algoritmilor de traversare, cautare, drum minim in grafuri pe un exemplu de "network" inedit
similar cu retelele formate de sistemele de friends/followers din diferite aplicatii ce pot sta la baza algoritmilor de sugestie 
(You may know X or Y).

## C1
Pentru a determina productia cu numarul maxim de actori am folosit algoritmul de DFS pentru a obtine o lista cu toate componentele conexe
unde o componenta conexa = o productie. Dupa aceea am aplicat un algoritm de determinare max pe vectorul de productii si sortat actorii din
productia maxima inaintea scrierii in fisier

## C2
Gradul de rudenie intre doi actori reprezinta defapt numarul de muchii (lungimea drumului) dintre cei doi actori. Din algoritmi de determinare
a drumului dintre doua noduri am ales o implementare a BFS folosind structura de tip coada pentru vizitarea nodurilor si un vector de distante/noduri
vizitate. La terminarea traversarii este returnata distanta inregistrata la pozitia [idDestinatie] avand distanta de la nodul start la nodul destinatie.

## C3
Pentru determinarea perechilor de punti am folosit o parcurgerea a listei de adiacenta si determinare a nodurilor cu o singura muchie pe care le salvez
in structura Punte (pereche de actorStart, actorDestinatie) ordonati alfabetic in pereche. Ultima etapa este cea de sortare a vectorului de punti conform
cerintei si scrierea in fisier.