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
#include <cstdint>

using namespace std;

class No;
class Frequencia;
class Heap;
class tabelaCodigo;
class arquivoCompactado;
class BufferBits;
class BufferBitsLeitura;
class BufferBitsEscrita;


/*DEBUGA*/

void escreve_bit(uint8_t bit) {
    //printf("COMO ISSO ESTA ALTERANDO\n");
    printf("bit: %d (", bit);
    printf(")\n");
}

////////////////////////

void opcaoCompactar(FILE *arquivoPtr);
void escrever_binario(uint8_t numero);

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

class tabelaCodigo {
    private:
        vector <char> vetorArmazenaCodigoCompacto;
        
        //void constroiTabelaDeCodigos(No *no);
    public:
        No *raizArvore;
        vector <string> vetorTabelaDeCodigo{256, "vazio"};
        tabelaCodigo(No *raiz): raizArvore(raiz){}
        void constroiTabelaDeCodigos(No *no);
        void imprimeTabelaDeCodigo();
};

class arquivoCompactado {
    private:
        FILE *arquivoParaLeitura;
        vector <string> vetorTabelaDeCodigo {256, "vazio"};
        No *raizArvore;
        FILE *arquivoParaEscrita;
        uint16_t tamanhoAlfabeto;
        vector <unsigned char> letrasDoAlfabeto;
        vector <bool> bitPercusoPreOrdem;
        vector <int> bitsUltimoByte;

        BufferBits *buffer;
        BufferBitsLeitura *leitura;
        BufferBitsEscrita *escrita;
    public:
        arquivoCompactado(FILE *arquivoLeitura, vector <string> vetorTabCodigo, No* raiz);
        ~arquivoCompactado();
        void geraCabecalho();
        void geraBitPercursoPreOrdem(No *no);
        void codifica();
        void contaFolhas(No *no);
        void traduzParaHuffman();
        void escreve8Bits(uint8_t byteInteiro);
        void editaQuantidadeUltimoBit(uint8_t ultimoBit);
        void opcaoCompactaTudo();
};


class BufferBits {
protected:
  FILE *arquivo;  
  uint8_t byte;   
  uint8_t n;     

public:
  BufferBits(FILE *arquivo);  
  uint8_t ocupados();         
  uint8_t livres();           
};

// Buffer de leitura de bits
class BufferBitsLeitura : public BufferBits {
public:
  BufferBitsLeitura(FILE *arquivo); 
  uint8_t le_bit();                 
};

// Buffer de escrita de bits
class BufferBitsEscrita : public BufferBits {
public:
  BufferBitsEscrita(FILE *arquivo);  
  void escreve_bit(uint8_t bit);    
  void descarrega();                
};


int DEBUG_BITS = 1;
int VALORES[8]={128,64,32,16,8,4,2,1};
/*
#####################################
#####################################
*/
int main() {
    FILE *arquivo = fopen("teste.txt", "r");
    if (arquivo == nullptr) {
        printf("Erro ao abrir arquivo\n");
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
    //heap.imprimeHeap();
    heap.juntaNos();
    //heap.imprimePreOrdem(heap.raizArvore);
    tabelaCodigo arvoreHuffman(heap.raizArvore);
    arvoreHuffman.constroiTabelaDeCodigos(arvoreHuffman.raizArvore);
    arvoreHuffman.imprimeTabelaDeCodigo();
    arquivoCompactado compactando(arquivoPtr, arvoreHuffman.vetorTabelaDeCodigo, arvoreHuffman.raizArvore);
    compactando.opcaoCompactaTudo();
}

/*
***********************************
********** FREQUENCIA *************
***********************************
*/

/*
    Le cada caractere do arquivo e soma a frequencia, o vetor de
    256 posicoes usa caractere em ASCII como indice
*/
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

/*
    A partir tabela de frequencia gera um vetor
    com somente os caracteres que apareceram no arquivo
*/
vector <No*> Frequencia::criarVetorFinal() {
    for (int i = 0; i < 256; i++) {
        if (tabelaDireta[i] != 0) {
            No *novo = new No(i, tabelaDireta[i]);
            vetorFinal.push_back(novo);
        }
    }

    return vetorFinal;
}

/*
***********************************
************** HEAP ***************
***********************************
*/

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
    //printf("subiu\n");
}

/*
    A partir da min-heap, constroi a arvore de huffman
    para posteriormente gerar os codigos de Huffman para
    cada caractere
*/
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
}



void Heap::imprimeHeap() {
    int tam =(int) lista.size();

    for (int i = 0; i < tam; i++) {
        printf("Caractere -> %c, frequencia->%d\n\n", (lista[i])->caractereChave, 
        (lista[i])->frequenciaChar);
    }
}

void Heap::imprimePreOrdem(No *noz) {
    ////printf("\n----%d", noz->frequenciaChar);
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
    Pela percurso pre-ordem gera os codigos de Huffman de cada
    caractere, sendo 0 para subarvore esquerda e 1 para direita,
    quando encontra caractere(folha) armazena no vetor de string o código
*/
void tabelaCodigo::constroiTabelaDeCodigos(No *no) {
    if (no->esq == nullptr && no->dir == nullptr) {
        string temporaria (vetorArmazenaCodigoCompacto.begin(), vetorArmazenaCodigoCompacto.end());
        vetorTabelaDeCodigo[no->caractereChave] = temporaria;
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
        if (vetorTabelaDeCodigo[i] != "vazio"){
            printf("caractere ->%c | ", i);
            std::cout << vetorTabelaDeCodigo[i] <<'\n';
        }
    }
}

/*
************************************
******** COMPACTA ARQUIVO **********
************************************
*/

arquivoCompactado::arquivoCompactado(FILE *arquivoLeitura, vector <string> vetorTabCodigo, No* raiz):
arquivoParaLeitura(arquivoLeitura), vetorTabelaDeCodigo(vetorTabCodigo), raizArvore(raiz), tamanhoAlfabeto(0){
    //buffer = new BufferBits(arquivoLeitura);
    //leitura = new BufferBitsLeitura(arquivoLeitura);
    arquivoParaEscrita = fopen("saida.huff", "wb+");
    if (arquivoParaEscrita == nullptr) {
        printf("Erro para abrir arquivo de saida\n");
    }
    printf("Criou arquivo\n");
    escrita = new BufferBitsEscrita(arquivoParaEscrita);
    contaFolhas(raizArvore);
}

arquivoCompactado::~arquivoCompactado() {
    delete escrita;
    fclose(arquivoParaEscrita);
}

void arquivoCompactado::opcaoCompactaTudo() {
    geraBitPercursoPreOrdem(raizArvore);
    geraCabecalho();
    traduzParaHuffman();
}

void arquivoCompactado::escreve8Bits(uint8_t byteInteiro) {
    printf("Valor entrado\n");
    escrever_binario(byteInteiro);
    printf("&&&&&&&&&&\n");
    uint8_t bitWrite;
    for (int i = 128; i > 0 ; i >>= 1) {
        // 0100 0001
        //
        if (byteInteiro & i) {
            bitWrite = 1;
            
        } else {
            bitWrite = 0;
        }
        //printf("\nValor saindo de write %d\n", bitWrite);
        //escreve_bit(bitWrite);
        escrita->escreve_bit(bitWrite);
        //printf("\nValor de n no primeiro%d\n\n&&&&", (escrita->ocupados()));
    }
    printf("\n\n\n");
}
// 1000 0000 0000 0100
// 
void arquivoCompactado::geraCabecalho() {
    uint8_t segundaParte = (uint8_t)(tamanhoAlfabeto);
    uint8_t primeiraParte = (uint8_t)(tamanhoAlfabeto>>8);
    escreve8Bits(segundaParte);
    escreve8Bits(primeiraParte);
    // escrever_binario(segundaParte);
    // escrever_binario(primeiraParte);
    //Deixa espaço para quantidade de bits que sobra no ultimo
    escreve8Bits(0);
    //printf("Chegou aqui\n");
    int tam = tamanhoAlfabeto;
    //printf("Tamanho alfabeto %d\n", tam);

    for (int i = 0; i < tam; i++) {
        //printf("#|%c|#\n",(char) letrasDoAlfabeto[i]);
        escreve8Bits((uint8_t) letrasDoAlfabeto[i]);
    }

    tam = letrasDoAlfabeto.size();
    for (int i = 0; i < tam; i++) {
        escrita->escreve_bit((uint8_t)(letrasDoAlfabeto[i]));
    }
}

void arquivoCompactado::editaQuantidadeUltimoBit(uint8_t ultimoBit) {
    unsigned char ultimo = (char) ultimoBit;
    fseek(arquivoParaEscrita, 2, SEEK_SET);
    fwrite(&ultimo, 1, 1,arquivoParaEscrita);
}

void arquivoCompactado::traduzParaHuffman() {
    char armazena;
    int tam = (int) bitPercusoPreOrdem.size();

    for (int i = 0; i < tam; i++) {
        uint8_t bit = (int)bitPercusoPreOrdem[i];
        escrita->escreve_bit(bit);
        //printf("Pre-ordem %d---\n", bit);
    }
    fseek(arquivoParaLeitura, 0, SEEK_SET);
    fread(&armazena, 1, 1, arquivoParaLeitura);
    // cout<<"funciona"<<endl;
    printf("ENTRANDO NO TEXTO EM SI %c\n", armazena);

    while (!feof(arquivoParaLeitura)) {
        string bitParaEscrever = vetorTabelaDeCodigo[armazena];
        std::cout << "VOLOR " <<bitParaEscrever << endl;
        int tam = (vetorTabelaDeCodigo[armazena]).size();
        // printf("----\n");
        // for (int i = 0; i < tam; i++){
        //     printf("%c ", bitParaEscrever[i]);
        // }
        // cout<<"\nteste\n"<<endl;
        for (int i = 0; i < tam; i++){
            escrita->escreve_bit(bitParaEscrever[i]);
            //printf("%c ", bitParaEscrever[i]);
        }
        fread(&armazena, 1, 1, arquivoParaLeitura);
    }

    int n = escrita->ocupados();
    // printf("O VALOR DE N %d\n", n);
    if (n != 0) {
        editaQuantidadeUltimoBit((uint8_t)(8 - n));
        fseek(arquivoParaEscrita, 0, SEEK_END);
        escrita->descarrega();
    }
}
void arquivoCompactado::geraBitPercursoPreOrdem(No *no) {
    if (no == nullptr) {
        bitPercusoPreOrdem.pop_back();
        return;
    }

    bitPercusoPreOrdem.push_back(0);
    geraBitPercursoPreOrdem(no->esq);
    bitPercusoPreOrdem.push_back(1);
    geraBitPercursoPreOrdem(no->dir);
}
void arquivoCompactado::codifica() {

}
void arquivoCompactado::contaFolhas(No *no) {
    if (no->esq == nullptr && no->dir == nullptr) {
        tamanhoAlfabeto = tamanhoAlfabeto + 1;
        //printf("Tamnaho alfabeto %d\n", tamanhoAlfabeto);
        letrasDoAlfabeto.push_back(no->caractereChave);
        return;
    }
    contaFolhas(no->esq);
    contaFolhas(no->dir);
}

/*
******************************
*********** BITS *************
******************************
*/


void escrever_binario(uint8_t numero)
{
  for (int i = 128; i > 0; i >>= 1)
    printf("%c", numero & i ? '1' : '0');
}

BufferBits::BufferBits(FILE *arquivo) :
  arquivo(arquivo),
  byte(0),
  n(0)
{ }

uint8_t BufferBits::ocupados()
{
  return n;
}

uint8_t BufferBits::livres()
{
  return 8 - n;
}

BufferBitsLeitura::BufferBitsLeitura(FILE *f) :
  BufferBits(f)
{ printf("Incializou");}

uint8_t BufferBitsLeitura::le_bit()
{

  void* aux=&byte;
  if (n == 0){
    int test=fread(aux, 1, 1, arquivo);
    if(test!=1){
    return 2;
    }
    n = 8;
}




  if (DEBUG_BITS) printf("n: %d, byte: %d (", n, byte);
  if (DEBUG_BITS) escrever_binario(byte);

  
  uint8_t bit;
  bit = (VALORES[n-1] & byte) ? 1 : 0;
  n-=1;

  if (DEBUG_BITS) printf(") --> %d(", byte);
  if (DEBUG_BITS) escrever_binario(byte);
  if (DEBUG_BITS) printf("), bit: %d\n", bit);

  return bit;
}

BufferBitsEscrita::BufferBitsEscrita(FILE *f) :
  BufferBits(f)
{ }

void BufferBitsEscrita::escreve_bit(uint8_t bit)
{
  if (DEBUG_BITS) printf("bit: %d, n: %d, byte: %d (", bit, n, byte);
  if (DEBUG_BITS) escrever_binario(byte);

    //printf("\n9999 Valor de n: %d 999999\n", n);
  byte = byte | (VALORES[n]*bit);
  n+=1;
  //printf("\nValor de n: %d\n", n);

  if (DEBUG_BITS) printf(") --> %d(", byte);
  if (DEBUG_BITS) escrever_binario(byte);
  if (DEBUG_BITS) printf(")\n");

    //printf("\n###############%d#################\n", n);
  if (n == 8) {
    //printf("ANTES ERA\n");
    descarrega();
    //printf("Sera MESMO\n");
    //printf("DESCARREGA");
  }
    
}

void BufferBitsEscrita::descarrega()
{
    void *aux=&byte;
    fwrite(aux,1,1,arquivo);
    printf("DESCARREGANDO\n");
    escrever_binario(byte);
    byte=0;
    n = 0;
}

