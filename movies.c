#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_ACTORI 10000
#define LEN_ACTOR 101

// Nod lista simpla pentru graf
typedef struct nod
{
	int id;
	char actor[LEN_ACTOR];
	struct nod* next;
} Nod;

typedef struct listaAdiacenta
{
	Nod* start;
} ListaAdiacenta;

// Graf reprezentat sub forma de vector de liste de adiacenta si nr de noduri din graf
typedef struct graf
{
	int nr;
	ListaAdiacenta* adjLists;
} Graf;

// Vector de actori dintr o productie (componenta conexa)
typedef struct productie
{
	int nr;
	char** actori;
} Productie;


// Pereche actori intre care exista punte
typedef struct punte
{
	char* actorStart;
	char* actorDestinatie;
} Punte;

typedef struct coada
{
	int start;
	int sfarsit;
	int nr;
	int capacitate;
	int* v;
} Coada;

// Functie pentru cautare nod din graf dupa numele actorului
Nod* getNod(Graf* graf, char actor[LEN_ACTOR])
{
	for (int i = 0; i < graf->nr; i++)
	{
		if (strcmp(graf->adjLists[i].start->actor, actor) == 0)
		{
			Nod* copie = graf->adjLists[i].start;
			return copie;
		}
	}
	return NULL;
}


// Functie de initializare a nodului in graf si alocare a unui nou id dupa nr de noduri din graf
Nod* createNode(char actor[LEN_ACTOR], Graf* graf)
{
	Nod* nou = getNod(graf, actor);

	if (nou == NULL)
	{
		nou = (Nod*)malloc(sizeof(Nod));
		memcpy(nou->actor, actor, sizeof nou->actor);
		nou->id = graf->nr;
		nou->next = NULL;
		graf->adjLists[graf->nr].start = nou;
		graf->nr++;
	}

	return nou;
}

// Initializarea vectorului (lista de adiacenta) cu max de noduri
Graf* createGraph()
{
	Graf* graf = (Graf*)malloc(sizeof(Graf));
	graf->nr = 0;
	graf->adjLists = (ListaAdiacenta*)malloc(sizeof(ListaAdiacenta) * MAX_ACTORI);

	for (int i = 0; i < MAX_ACTORI; ++i)
		graf->adjLists[i].start = NULL;

	return graf;
}

// Adauga muchie(intrare in lista de adiacenta) intre actor sursa si destinatie si invers
void addEdge(Graf* graf, char actorSrc[LEN_ACTOR], char actorDest[LEN_ACTOR])
{
	Nod* sursa = createNode(actorSrc, graf);
	Nod* destinatie = createNode(actorDest, graf);

	Nod* copieSursa = sursa;
	Nod* copieDestinatie = destinatie;

	while (copieSursa->next != NULL)
	{
		copieSursa = copieSursa->next;
	}
	while (copieDestinatie->next != NULL)
	{
		copieDestinatie = copieDestinatie->next;
	}

	Nod* nouSursa = (Nod*)malloc(sizeof(Nod));
	Nod* nouDestinatie = (Nod*)malloc(sizeof(Nod));

	nouSursa->id = sursa->id;
	memcpy(nouSursa->actor, sursa->actor, sizeof nouSursa->actor);
	nouSursa->next = NULL;

	nouDestinatie->id = destinatie->id;
	memcpy(nouDestinatie->actor, destinatie->actor, sizeof nouDestinatie->actor);
	nouDestinatie->next = NULL;

	copieSursa->next = nouDestinatie;
	copieDestinatie->next = nouSursa;
}

// Scrie in fisier actorii apartinand productiei maxime
void scrieProductieMaxima(FILE* dest, Productie productie)
{
	if (fprintf(dest, "%d\n", productie.nr))
		;

	for (int i = 0; i < productie.nr; i++)
	{
		fprintf(dest, "%s", productie.actori[i]);
	}
}

// Scrie nr de perechi si perechile de actori
void scriePunti(FILE* dest, Punte* punti, int nr)
{
	if (fprintf(dest, "%d\n", nr))
		;

	for (int i = 0; i < nr; i++)
	{
		fprintf(dest, "%s %s\n", punti[i].actorStart, punti[i].actorDestinatie);
	}
}


bool verificaActor(Nod* start, char actor[LEN_ACTOR])
{
	bool gasit = false;
	Nod* copie = start;
	while (copie != NULL)
	{
		if (strcmp(copie->actor, actor) == 0)
			copie = copie->next;
	}
	free(copie);
	return gasit;
}

void afiseazaNoduri(Graf* graf)
{
	for (int i = 0; i < graf->nr; ++i)
	{
		printf("%s", graf->adjLists[i].start->actor);
	}
}


// Algoritm de parcurgere DFS si determinare a componentelor conexe (vector de productii)
void parcurgeDFS(Graf* graf, Productie* productie, int id, bool* vizitate)
{
	// marcam nodul curent ca vizitat
	vizitate[id] = true;
	// adaugam la prodcutie
	strcpy(productie->actori[productie->nr], graf->adjLists[id].start->actor);
	// crestem nr de noduri
	productie->nr++;

	// Vizitam nodurile adiacente nevizitate si reapelam DFS pe el 
	Nod* copie = graf->adjLists[id].start;
	int k = 0;
	while (copie != NULL || k == 0)
	{
		if (vizitate[copie->id] == false)
		{
			parcurgeDFS(graf, productie, copie->id, vizitate);
		}
		copie = copie->next;
		k = 1;
	}
}

Coada* initCoada(int capacitate)
{
	Coada* coada = (Coada*)malloc(sizeof(Coada));
	coada->capacitate = capacitate;
	coada->start = coada->nr = 0;

	coada->sfarsit = capacitate - 1;
	coada->v = (int*)malloc(coada->capacitate * sizeof(int));
	return coada;
}

int isFull(Coada* coada)
{
	return (coada->nr == coada->capacitate);
}

int isEmpty(Coada* coada)
{
	return (coada->nr == 0);
}

void push(Coada* coada, int item)
{
	if (isFull(coada))
		return;
	coada->sfarsit = (coada->sfarsit + 1) % coada->capacitate;
	coada->v[coada->sfarsit] = item;
	coada->nr = coada->nr + 1;
}

int pop(Coada* coada)
{
	if (isEmpty(coada))
		return -1;
	int item = coada->v[coada->start];
	coada->start = (coada->start + 1) % coada->capacitate;
	coada->nr = coada->nr - 1;
	return item;
}

int start(Coada* coada)
{
	if (isEmpty(coada))
		return -1;
	return coada->v[coada->start];
}

int sfarsit(Coada* coada)
{
	if (isEmpty(coada))
		return -1;
	return coada->v[coada->sfarsit];
}


// Implementare de BFS pentru a determina distanta dintre 2 noduri
int determinaGradDeRudenie(Graf* graf, int id, int idDestinatie)
{
	bool* vizitate = (bool*)malloc(sizeof(bool) * graf->nr);
	int* distante = (int*)malloc(sizeof(int) * graf->nr);

	//initializare vectori
	for (int i = 0; i < graf->nr; i++) {
		vizitate[i] = false;
		distante[i] = 0;
	}


	Coada* coada = initCoada(graf->nr);

	push(coada, id);
	vizitate[id] = true;

	while (!isEmpty(coada))
	{
		int x = pop(coada);

		Nod* copie = graf->adjLists[x].start;
		while (copie != NULL)
		{
			if (vizitate[copie->id])
				copie = copie->next;
			else {
				distante[copie->id] = distante[x] + 1;
				push(coada, copie->id);
				vizitate[copie->id] = true;
			}
		}
	}
	int grad = distante[idDestinatie] == 0 ? -1 : distante[idDestinatie];

	free(vizitate);
	free(distante);

	return grad;
}

Productie getProductiaMaxima(Graf* graf)
{
	bool* vizitate = (bool*)malloc(sizeof(bool) * graf->nr);

	for (int i = 0; i < graf->nr; i++)
		vizitate[i] = false;

	Productie productiaMaxima;
	productiaMaxima.nr = 0;
	for (int i = 0; i < graf->nr; i++)
	{
		if (vizitate[i] == false)
		{
			Productie productie;
			productie.nr = 0;
			productie.actori = (char**)malloc(sizeof(char*) * MAX_ACTORI);
			for (int j = 0; j < MAX_ACTORI; j++)
			{
				productie.actori[j] = (char*)malloc(sizeof(char) * LEN_ACTOR);
			}
			parcurgeDFS(graf, &productie, i, vizitate);
			if (productiaMaxima.nr < productie.nr)
			{
				productiaMaxima = productie;
			}
			else
			{
				for (int j = 0; j < MAX_ACTORI; j++)
				{
					free(productie.actori[j]);
				}
				free(productie.actori);
			}
		}
	}

	free(vizitate);

	return productiaMaxima;
}

Productie getProductiaMaxima2(Graf* graf)
{
	Productie productie;
	productie.nr = 0;

	for (int i = 0; i < graf->nr; i++)
	{
		Nod* copie = graf->adjLists[i].start;
		int nr = 0;
		char** actori = (char**)malloc(sizeof(char*) * graf->nr);
		while (copie != NULL)
		{
			actori[nr] = (char*)malloc(sizeof(char) * LEN_ACTOR);
			memcpy(actori[nr], copie->actor, sizeof actori[nr]);
			copie = copie->next;
			nr++;
		}
		if (nr > productie.nr)
		{
			productie.nr = nr;
			productie.actori = actori;
		}
		else
			for (int j = 0; j < nr; j++)
			{
				free(actori[j]);
			}
	}

	return productie;
}

void sortareActoriProductie(Productie* productie)
{
	for (int i = 0; i < productie->nr - 1; i++)
	{
		for (int j = i + 1; j < productie->nr; j++)
		{
			if (strcmp(productie->actori[i], productie->actori[j]) > 0)
			{
				char* aux = (char*)malloc(sizeof(char) * LEN_ACTOR);
				strcpy(aux, productie->actori[i]);
				strcpy(productie->actori[i], productie->actori[j]);
				strcpy(productie->actori[j], aux);
				free(aux);
			}
		}
	}
}

void sortarePunti(Punte* punti, int nr)
{
	for (int i = 0; i < nr - 1; i++)
	{
		for (int j = i + 1; j < nr; j++)
		{
			if (strcmp(punti[i].actorStart, punti[j].actorStart) > 0)
			{
				Punte aux;
				aux.actorStart = (char*)malloc(sizeof(char) * LEN_ACTOR);
				aux.actorDestinatie = (char*)malloc(sizeof(char) * LEN_ACTOR);

				strcpy(aux.actorStart, punti[i].actorStart);
				strcpy(aux.actorDestinatie, punti[i].actorDestinatie);

				strcpy(punti[i].actorStart, punti[j].actorStart);
				strcpy(punti[i].actorDestinatie, punti[j].actorDestinatie);

				strcpy(punti[j].actorStart, aux.actorStart);
				strcpy(punti[j].actorDestinatie, aux.actorDestinatie);

				free(aux.actorStart);
				free(aux.actorDestinatie);
			}
		}
	}
}


void determinaPunti(Graf* graf, Punte* punti, int* nr) {
	*nr = 0;
	for (int i = 0; i < graf->nr; i++) {
		Nod* copie = graf->adjLists[i].start;
		int nrMuchii = 0;
		char* actor = (char*)malloc(sizeof(char) * LEN_ACTOR);

		if (copie->next != NULL && copie->next->next != NULL) {
			strcpy(actor, copie->next->next->actor);
		}
		else strcpy(actor, "");

		while (copie != NULL) {
			if (nrMuchii == 0 || strcmp(actor, copie->actor) != 0)
				nrMuchii++;
			copie = copie->next;
		}
		if (nrMuchii == 1) {
			int poz = *nr;
			punti[poz].actorStart = (char*)malloc(sizeof(char) * LEN_ACTOR);
			punti[poz].actorDestinatie = (char*)malloc(sizeof(char) * LEN_ACTOR);

			if (strcmp(graf->adjLists[i].start->actor, actor) > 0) {
				strcpy(punti[poz].actorStart, strtok(actor, "\n"));
				strcpy(punti[poz].actorDestinatie, strtok(graf->adjLists[i].start->actor, "\n"));
			}
			else {
				strcpy(punti[poz].actorStart, strtok(graf->adjLists[i].start->actor, "\n"));
				strcpy(punti[poz].actorDestinatie, strtok(actor, "\n"));
			}
			*nr = *nr + 1;
		}
	}
}

int main(int argc, char** argv)
{
	FILE* dest, * src;
	src = fopen(argv[2], "r");
	dest = fopen(argv[3], "w");

	int nrFilme;
	if (fscanf(src, "%d\n", &nrFilme))
		;

	Graf* graf = createGraph();

	int lastId = 0;

	for (int i = 0; i < nrFilme; i++)
	{
		char numeFilm[101];
		fgets(numeFilm, 101, src);

		int nrActori;
		fscanf(src, "%d\n", &nrActori);

		char** actori = (char**)malloc(sizeof(char*) * nrActori);
		for (int j = 0; j < nrActori; j++)
		{
			actori[j] = (char*)malloc(sizeof(char) * LEN_ACTOR);
			fgets(actori[j], LEN_ACTOR, src);
		}
		// adaugam muchii intre actorii filmului curent
		for (int j = 0; j < nrActori; j++)
		{
			for (int k = 0; k < nrActori; k++)
			{
				// daca actorii intre care vrem sa adaugam muchie sunt diferiti
				if (strcmp(actori[j], actori[k]) != 0)
				{
					addEdge(graf, actori[j], actori[k]);
				}
			}
		}
		for (int j = 0; j < nrActori; j++)
		{
			free(actori[j]);
		}
		// free(actori);
	}

	//afiseazaNoduri(graf);
	if (strcmp(argv[1], "-c1") == 0)
	{
		Productie productieMaxima = getProductiaMaxima(graf);
		sortareActoriProductie(&productieMaxima);
		scrieProductieMaxima(dest, productieMaxima);

		// free(graf);
	}
	else if (strcmp(argv[1], "-c2") == 0)
	{
		char* actorStart = (char*)malloc(sizeof(char) * LEN_ACTOR);
		char* actorDestinatie = (char*)malloc(sizeof(char) * LEN_ACTOR);
		fgets(actorStart, 101, src);
		fgets(actorDestinatie, 101, src);

		int idStart = getNod(graf, actorStart)->id;
		int idDestinatie = getNod(graf, actorDestinatie)->id;

		int grad = determinaGradDeRudenie(graf, idStart, idDestinatie);
		fprintf(dest, "%d\n", grad);
	}
	else if (strcmp(argv[1], "-c3") == 0)
	{
		int nr = 0;
		Punte* punti = (Punte*)malloc(sizeof(Punte) * graf->nr);

		determinaPunti(graf, punti, &nr);

		sortarePunti(punti, nr);

		scriePunti(dest, punti, nr);

	}

	fclose(src);
	fclose(dest);
}