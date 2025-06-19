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
#include<iostream>
#include <cstdio>
#include <vector>
#include <string>

using namespace std;

class No;
class Frequencia;
class Heap;
class tabelaCodigo;

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
    friend class tabelaCodigo;
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
        ~Heap();
        No *juntaNos();
        void insere(No *noCaractere);
        No *extraiMinimo();
        Heap(vector <No*> listaTabela);
        void imprimeHeap();
        void imprimePreOrdem(No *noz);
        No *raizArvore = nullptr;
        void limpaArvore(No *no);
};

typedef struct {
    string codigoChar = "vazio";
}codigoCaractere;

class tabelaCodigo {
    private:
        vector <char> vetorArmazenaCodigoCompacto;
        
        //void constroiTabelaDeCodigos(No *no);
    public:
        No *raizArvore;
        vector <codigoCaractere> vetorTabelaDeCodigo{256};
        tabelaCodigo(No *raiz): raizArvore(raiz){}
        void constroiTabelaDeCodigos(No *no);
        void imprimeTabelaDeCodigo();
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
    //contadorFreq.escreveTabela();
    vector <No*> vetor = contadorFreq.criarVetorFinal();
    Heap heap(vetor);
    heap.imprimeHeap();
    heap.juntaNos();
    //heap.imprimePreOrdem(heap.raizArvore);
    tabelaCodigo arvoreHuffman(heap.raizArvore);
    arvoreHuffman.constroiTabelaDeCodigos(arvoreHuffman.raizArvore);
    arvoreHuffman.imprimeTabelaDeCodigo();
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

Heap::~Heap() {
    limpaArvore(raizArvore);
}

void Heap::limpaArvore(No *no) {
    if (no == nullptr) {
        return;
    }
    limpaArvore(no->esq);
    limpaArvore(no->dir);
    delete no;
}
//2i+1 = fe -> fe-1/2
int Heap::pai(int posicao) {
    //printf("|PAI retornando: %d|\n", (posicao - 1)/2);
    return (posicao - 1)/2;
}
int Heap::filhoEsquerdo(int posicao) {
    //printf("|FE retornando: %d|\n", (2 * posicao + 1));
    return (2 * posicao + 1);
}
int Heap::filhoDireito(int posicao) {
    //printf("|FD retornando: %d|\n", (2 * posicao + 2));
    return (2 * posicao + 2);
}
void Heap::sobe(int posicao) {
    //printf("-----------%d###########\n", (lista[posicao])->frequenciaChar);
    while (lista[pai(posicao)]->frequenciaChar > (lista[posicao])->frequenciaChar) {
        //printf("Chegou aqui");
        troca(posicao, pai(posicao));
        //printf("sobe posicao %d \n", posicao);
        posicao = pai(posicao);
    }
    //imprimeHeap();
}
void Heap::desce(int posicao) {
    int menor, esq, dir, tam;
    menor = posicao;
    esq = filhoEsquerdo(posicao);
    dir = filhoDireito(posicao);
    tam = (int)lista.size();
    //printf("-----------%d###########\n", (lista[posicao])->frequenciaChar);
    if (esq < tam && lista[posicao]->frequenciaChar > lista[esq]->frequenciaChar) {
        //printf("|%d e esq %d|\n", posicao, esq);
        menor = esq;
    }
    else {
        menor = posicao;
    }
    if (dir < tam && lista[menor]->frequenciaChar > lista[dir]->frequenciaChar) {
        //printf("|%d e esq %d|\n", dir, menor);
        menor = dir;
    }

    if (menor != posicao) {
        //printf("Vai trocar\n");
        troca(posicao, menor);
        desce(menor);
    }
}

void Heap::troca(int i, int j) {
    //printf("i:%p e j:%p\n", lista[i], lista[j]);
    No *aux = lista[i];
    lista[i] = lista[j];
    lista[j] = aux;
    //printf("i:%p e j:%p\n", lista[i], lista[j]);
}

void Heap::insere(No *noCaractere){
    lista.push_back(noCaractere);
    int posicao = (int) (lista.size()) - 1;
    sobe(posicao);
    printf("subiu\n");
}

No* Heap::juntaNos() {
    No *no1 = nullptr;
    No *no2 = nullptr;
    int frequencia;

    while ((lista.size()) > 1) {
        //printf("|Tamanho heap %zu|\n",(lista.size()) );
        //imprimeHeap();
        no1 = extraiMinimo();
        no2 = extraiMinimo();

        frequencia = (no1->frequenciaChar) + (no2->frequenciaChar);

        No *novo = new No(0, frequencia);
        novo->esq = no1;
        novo->dir = no2;
            
        insere(novo);
    }
    raizArvore = lista[0];
    return lista[0];
}

No* Heap::extraiMinimo() {
    No *armazena = lista[0];
    troca(0, (lista.size()) - 1);

    lista.pop_back();
    desce(0);

    return armazena;
}

Heap::Heap(vector <No*> listaTabela) {
    
    lista = listaTabela;
    int meio = (int)(lista.size())/2;
    for (int i = (meio); i >= 0; i--) {
        desce(i);
    }
    // printf("Imprime HEAP DENTRO\n");
    // imprimeHeap();
    // printf("####################\n");
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
    int tam =(int) lista.size();

    for (int i = 0; i < tam; i++) {
        printf("Caractere -> %c, frequencia->%d\n\n", (lista[i])->caractereChave, 
        (lista[i])->frequenciaChar);
    }
}

void Heap::imprimePreOrdem(No *noz) {
    //printf("\n----%d", noz->frequenciaChar);
    // printf("Caractere -> %c, frequencia->%d\n\n", noz->caractereChave, 
    // noz->frequenciaChar);
    if (noz == nullptr) {
        return;
    }
    imprimePreOrdem(noz->esq);
    printf("\nCaractere -> %c, frequencia->%d\n\n", noz->caractereChave, 
    noz->frequenciaChar);
    imprimePreOrdem(noz->dir);
}

/*
***********************************
**********ARVORE HUFFMAN***********
***********************************
*/
/*
01
*/
void tabelaCodigo::constroiTabelaDeCodigos(No *no) {
    if (no->esq == nullptr && no->dir == nullptr) {
        string temporaria (vetorArmazenaCodigoCompacto.begin(), vetorArmazenaCodigoCompacto.end());
        vetorTabelaDeCodigo[no->caractereChave].codigoChar = temporaria;
        vetorArmazenaCodigoCompacto.pop_back();

        return;
    }
    vetorArmazenaCodigoCompacto.push_back('0');
    constroiTabelaDeCodigos(no->esq);
    vetorArmazenaCodigoCompacto.push_back('1');
    constroiTabelaDeCodigos(no->dir);
    vetorArmazenaCodigoCompacto.pop_back();
}

void tabelaCodigo::imprimeTabelaDeCodigo() {
    for(int i = 0; i < 256; i++) {
        if (vetorTabelaDeCodigo[i].codigoChar != "vazio"){
            printf("caractere ->%c | ", i);
            std::cout << vetorTabelaDeCodigo[i].codigoChar <<'\n';
        }
    }
}