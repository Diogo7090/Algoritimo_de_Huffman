/*
*
* Carlos Henrique Nascimento Lopes
* Diogo Bispo Da Silva
* Guilherme Paes Harmon
*
* Trabalho de Estruturas de Dados
* Diego Padilha Rupert
*
*/

#include <cstdio>
#include <vector>

using namespace std;

class No;
class Frequencia;
class Heap;

void opcaoCompactar(FILE *arquivoPtr);

class Frequencia {
    private:
        void contadorDeFrequencia();
        FILE *arquivoLeitura;
        int tabelaDireta [256] = {0};
        vector <No*> vetorFinal;
    public:
        Frequencia(FILE *arquivoPtr): arquivoLeitura(arquivoPtr) {
            contadorDeFrequencia();
        }
        ~Frequencia(){
            for (int i = 0; i < 256; i++) {
                tabelaDireta[i] = 0;
            }
        }
        void escreveTabela ();
        vector <No*> criarVetorFinal();
};

class No {
    friend class Heap;
    friend class Frequencia;
    public:
        No *esq;
        No *dir;
        No *pai;
        unsigned char caractereChave;
        int frequenciaChar;

        No(): esq(nullptr), dir(nullptr), pai(nullptr) {}
        No(unsigned char chave, int frequencia): esq(nullptr), dir(nullptr), pai(nullptr),
        caractereChave(chave), frequenciaChar(frequencia) {}


        ~No() {
            esq = nullptr; 
            dir = nullptr;
            pai = nullptr;
            caractereChave = 0;
            frequenciaChar = 0;
        }
};

class Heap {
    private:
        vector <No*> lista;

        int pai(int posicao);
        int filhoEsquerdo(int posicao);
        int filhoDireito(int posicao);
        void sobe(int posicao);
        void desce(int posicao);
        void troca(int i, int j);

    public:
        Heap();
        ~Heap() {
            lista[0] = nullptr;
        }
        No *juntaNos();
        void insere(No *noCaractere);
        No *extraiMinimo();
        Heap(vector <No*> listaTabela);
        void imprimeHeap();
};

int main() {
    FILE *arquivo = fopen("../teste.txt", "r");
    if (arquivo == nullptr) {
        printf("Erro ao abrir\n");
    }
    opcaoCompactar(arquivo);


    fclose(arquivo);
    return 0;
}


void opcaoCompactar(FILE *arquivoPtr) {
    Frequencia contadorFreq(arquivoPtr);
    contadorFreq.escreveTabela();
    vector <No*> vetor = contadorFreq.criarVetorFinal();
    Heap heap(vetor);
    heap.imprimeHeap();
    heap.juntaNos();
    heap.imprimeHeap();
}

void Frequencia::contadorDeFrequencia() {
    unsigned char armazena;
    
    fread(&armazena, 1, 1, arquivoLeitura);
    while (!feof(arquivoLeitura)) {
        tabelaDireta[armazena]+=1;
        //printf("%d ", armazena);
        fread(&armazena, 1, 1, arquivoLeitura);
    }
}

void Frequencia::escreveTabela() {
    for (int i = 0; i < 256; i++) {
        printf("[%d] -> %d\n", i, tabelaDireta[i]);
    }
}
//2i+1 = fe -> fe-1/2
int Heap::pai(int posicao) {
    return (posicao - 1)/2;
}
int Heap::filhoEsquerdo(int posicao) {
    return (2 * posicao + 2);
}
int Heap::filhoDireito(int posicao) {
    return (2 * posicao + 1);
}
void Heap::sobe(int posicao) {
    while (lista[pai(posicao)]->frequenciaChar > (lista[posicao])->frequenciaChar) {
        printf("Chegou aqui");
        troca(posicao, pai(posicao));

        posicao = pai(posicao);
        sobe(posicao);
    }
}
void Heap::desce(int posicao) {
    int menor, esq, dir, tam;
    menor = posicao;
    esq = filhoDireito(posicao);
    dir = filhoEsquerdo(posicao);
    tam = lista.size();

    if (dir < tam && lista[posicao] > lista[dir]) {
        menor = filhoDireito(posicao);
    }
    if (esq < tam && lista[posicao] > lista[esq]) {
        menor = filhoDireito(posicao);
    }

    if (menor != posicao) {
        troca(posicao, menor);
        desce(menor);
    }
}

void Heap::troca(int i, int j) {
    No *aux = lista[i];
    lista[i] = lista[j];
    lista[j] = aux;
}

void Heap::insere(No *noCaractere){
    lista.push_back(noCaractere);
    int posicao = (lista.size()) - 1;
    sobe(posicao);
}

No* Heap::juntaNos() {
    No *no1 = nullptr;
    No *no2 = nullptr;
    int frequencia;

    while ((lista.size()) != 1) {
        no1 = extraiMinimo();
        no2 = extraiMinimo();

        frequencia = (no1->frequenciaChar) + (no2->frequenciaChar);

        No *novo = new No(0, frequencia);
        novo->dir = no1;
        novo->esq = no2;
            
        insere(novo);
    }
    return lista[0];
}

No* Heap::extraiMinimo() {
    No *armazena = lista[0];
    troca(0, (lista.size()) - 1);

    lista.pop_back();

    return armazena;
}

Heap::Heap(vector <No*> listaTabela) {
    lista = listaTabela;
}

vector <No*> Frequencia::criarVetorFinal() {
    for (int i = 0; i < 256; i++) {
        if (tabelaDireta[i] != 0) {
            No *novo = new No(i, tabelaDireta[i]);
            vetorFinal.push_back(novo);
        }
    }

    return vetorFinal;
}

void Heap::imprimeHeap() {
    int tam = lista.size();

    for (int i = 0; i < tam; i++) {
        printf("Caractere -> %c, frequencia->%d\n\n", (lista[i])->caractereChave, 
        (lista[i])->frequenciaChar);
    }
}