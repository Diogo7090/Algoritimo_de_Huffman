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
class DescompactaArquivo;

void opcaoCompactar(FILE *arquivoPtr, FILE *arquivoPtr1);
void opcaoDescompactar(FILE *arquivoCompactado, FILE *arquivoPtr2);
void escrever_binario(uint8_t numero);
void limpaArvore(No *no);

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
    friend class DescompactaArquivo;
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
        arquivoCompactado(FILE *arquivoLeitura, FILE *arquivoEscrita, vector <string> vetorTabCodigo, No* raiz);
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
    uint8_t fimDoArquivo;
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

class DescompactaArquivo {
    private: 
        No *raizArvoreD;
        FILE *fileCompacto;
        FILE *fileSaida;
        int tamanhoAlfabetoD;
        int charEncontradosNaPreOrdem;
        int sobraUltimoByte;
        vector <unsigned char> vetorCharNoArquivoCompacto;
        BufferBitsLeitura *leitor;

    public:
        DescompactaArquivo(FILE *arquivo,FILE *arquivo2);
        ~DescompactaArquivo();
        No* reconstroiArvorePreOrdem(No *no);
        void opcaoDescompacta();
        void leCabecalho();
};      


int DEBUG_BITS = 1;
int VALORES[8]={128,64,32,16,8,4,2,1};
/*
#####################################
#####################################
*/
int main(int argc, char *argv[]) {
    string opcao = argv[1];
    if (opcao == "c") {
        FILE *arquivo1 = fopen(argv[2], "r");
        FILE *arquivo2 = fopen(argv[3], "wb+");
        if (arquivo1 == nullptr) {
            printf("Erro ao abrir arquivo\n");

        } else {
            opcaoCompactar(arquivo1, arquivo2);
            fclose(arquivo1);
            fclose(arquivo2);
        }
    }
    else if (opcao == "d") {
        FILE *arquivo3 = fopen(argv[2], "rb");
        FILE *arquivo4 = fopen(argv[3], "w");
        if (arquivo3 == nullptr) {
            printf("Erro ao abrir arquivo\n");

        } else {
            opcaoDescompactar(arquivo3, arquivo4);
            printf("Descompactar\n");
            fclose(arquivo3);
            fclose(arquivo4);
        }
    }
    printf("Concluido\n");
    return 0;
}

void opcaoCompactar(FILE *arquivoPtr, FILE *arquivoPtr1) {
    Frequencia contadorFreq(arquivoPtr);
    //contadorFreq.escreveTabela();
    vector <No*> vetor = contadorFreq.criarVetorFinal();
    int tam = (int) vetor.size();
    if (tam == 0) {
        return;
    }
    Heap heap(vetor);
    //heap.imprimeHeap();
    heap.juntaNos();
    //heap.imprimePreOrdem(heap.raizArvore);
    tabelaCodigo arvoreHuffman(heap.raizArvore);
    arvoreHuffman.constroiTabelaDeCodigos(arvoreHuffman.raizArvore);
    //arvoreHuffman.imprimeTabelaDeCodigo();
    arquivoCompactado compactando(arquivoPtr,arquivoPtr1, arvoreHuffman.vetorTabelaDeCodigo, arvoreHuffman.raizArvore);
    compactando.opcaoCompactaTudo();
}

void opcaoDescompactar(FILE *arquivoCompactado, FILE *arquivoPtr2) {
    DescompactaArquivo descompacta(arquivoCompactado, arquivoPtr2);
    printf("-----\n");
    descompacta.opcaoDescompacta();
    printf("-----\n");
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

void limpaArvore(No *no) {
    if (no == nullptr) {
        return;
    }
    limpaArvore(no->esq);
    limpaArvore(no->dir);
    delete no;
    no = nullptr;
}

int Heap::pai(int posicao) {
    return (posicao - 1)/2;
}
int Heap::filhoEsquerdo(int posicao) {
    return (2 * posicao + 1);
}
int Heap::filhoDireito(int posicao) {
    return (2 * posicao + 2);
}
void Heap::sobe(int posicao) {

    while (lista[pai(posicao)]->frequenciaChar > (lista[posicao])->frequenciaChar) {
        troca(posicao, pai(posicao));

        posicao = pai(posicao);
    }
}
void Heap::desce(int posicao) {
    int menor, esq, dir, tam;
    menor = posicao;
    esq = filhoEsquerdo(posicao);
    dir = filhoDireito(posicao);
    tam = (int)lista.size();
    if (esq < tam && lista[posicao]->frequenciaChar > lista[esq]->frequenciaChar) {
        menor = esq;
    }
    else {
        menor = posicao;
    }
    if (dir < tam && lista[menor]->frequenciaChar > lista[dir]->frequenciaChar) {
        menor = dir;
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
    int posicao = (int) (lista.size()) - 1;
    sobe(posicao);
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
    if (lista.size() != 0) {
        No *armazena = lista[0];
        troca(0, (lista.size()) - 1);
        lista.pop_back();
        desce(0);
    
        return armazena;
    }
    return nullptr;
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

    if (noz == nullptr) {
        return;
    }
    printf("\nCaractere -> %c, frequencia->%d\n\n", noz->caractereChave, 
    noz->frequenciaChar);
    imprimePreOrdem(noz->esq);
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
        if (vetorArmazenaCodigoCompacto.size() != 0) {
            vetorArmazenaCodigoCompacto.pop_back();
        }
        return;
    }
    vetorArmazenaCodigoCompacto.push_back('0');
    constroiTabelaDeCodigos(no->esq);
    vetorArmazenaCodigoCompacto.push_back('1');
    constroiTabelaDeCodigos(no->dir);
     if (vetorArmazenaCodigoCompacto.size() != 0) {
        vetorArmazenaCodigoCompacto.pop_back();
     }
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

arquivoCompactado::arquivoCompactado(FILE *arquivoLeitura, FILE *arquivoEscrita, vector <string> vetorTabCodigo, No* raiz):
arquivoParaLeitura(arquivoLeitura), vetorTabelaDeCodigo(vetorTabCodigo), raizArvore(raiz), arquivoParaEscrita(arquivoEscrita), 
tamanhoAlfabeto(0){
    //buffer = new BufferBits(arquivoLeitura);
    //leitura = new BufferBitsLeitura(arquivoLeitura);

    if (arquivoParaEscrita == nullptr) {
        printf("Erro para abrir arquivo de saida\n");
    }
    escrita = new BufferBitsEscrita(arquivoParaEscrita);
    contaFolhas(raizArvore);
}

arquivoCompactado::~arquivoCompactado() {
    delete escrita;
    escrita = nullptr;
}

void arquivoCompactado::opcaoCompactaTudo() {
    geraBitPercursoPreOrdem(raizArvore);
    geraCabecalho();
    traduzParaHuffman();
}

void arquivoCompactado::escreve8Bits(uint8_t byteInteiro) {
    // escrever_binario(byteInteiro);
    // printf("\n");
    uint8_t bitWrite;
    for (int i = 128; i > 0 ; i >>= 1) {
        // 0100 0001
        //
        if (byteInteiro & i) {
            bitWrite = 1;
            
        } else {
            bitWrite = 0;
        }

        escrita->escreve_bit(bitWrite);
    }
}
// 1000 0000 0000 0100
// 
void arquivoCompactado::geraCabecalho() {
    uint8_t segundaParte = (uint8_t)(tamanhoAlfabeto);
    uint8_t primeiraParte = (uint8_t)(tamanhoAlfabeto>>8);
    escreve8Bits(segundaParte);
    escreve8Bits(primeiraParte);
    //Deixa espaço para quantidade de bits que sobra no ultimo
    escreve8Bits(0);
    int tam = letrasDoAlfabeto.size();

    for (int i = 0; i < tam; i++) {
        //printf("#|%c|#\n",(char) letrasDoAlfabeto[i]);
        escreve8Bits((uint8_t) letrasDoAlfabeto[i]);
    }

    tam = (int) bitPercusoPreOrdem.size();

    for (int i = 0; i < tam; i++) {
        uint8_t bit = (int)bitPercusoPreOrdem[i];
        escrita->escreve_bit(bit);
    }
}

void arquivoCompactado::editaQuantidadeUltimoBit(uint8_t ultimoBit) {
    unsigned char ultimo = (char) ultimoBit;
    fseek(arquivoParaEscrita, 2, SEEK_SET);
    fwrite(&ultimo, 1, 1,arquivoParaEscrita);
}

void arquivoCompactado::traduzParaHuffman() {
    char armazena;

    fseek(arquivoParaLeitura, 0, SEEK_SET);
    fread(&armazena, 1, 1, arquivoParaLeitura);

    while (!feof(arquivoParaLeitura)) {
        string bitParaEscrever = vetorTabelaDeCodigo[armazena];
        int tam = (vetorTabelaDeCodigo[armazena]).size();
        for (int i = 0; i < tam; i++){
            uint8_t valorParaEscrita;
            if (bitParaEscrever[i] == '1') {
                valorParaEscrita = (uint8_t)1;
            } else {
                valorParaEscrita = (uint8_t)0;
            }
            escrita->escreve_bit(valorParaEscrita);
        }
        fread(&armazena, 1, 1, arquivoParaLeitura);
    }

    int n = escrita->ocupados();
    if (n != 0) {
        editaQuantidadeUltimoBit((uint8_t)(8 - n));
        fseek(arquivoParaEscrita, 0, SEEK_END);
        escrita->descarrega();
    }
}
void arquivoCompactado::geraBitPercursoPreOrdem(No *no) {
    if (no->esq == nullptr && no->dir == nullptr) {
        bitPercusoPreOrdem.push_back(1);
        return;
    } else {
        bitPercusoPreOrdem.push_back(0);
    }

    //bitPercusoPreOrdem.push_back(0);
    geraBitPercursoPreOrdem(no->esq);
    //bitPercusoPreOrdem.push_back(0);
    geraBitPercursoPreOrdem(no->dir);
}
void arquivoCompactado::codifica() {

}
void arquivoCompactado::contaFolhas(No *no) {
    if (no->esq == nullptr && no->dir == nullptr) {
        tamanhoAlfabeto = tamanhoAlfabeto + 1;
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
  n(0),
  fimDoArquivo(0)
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
{}

uint8_t BufferBitsLeitura::le_bit()
{
    void* aux=&byte;
    if (n == 0){
        int test=fread(aux, 1, 1, arquivo);
        fimDoArquivo = (uint8_t) test;
        //printf("%d----\n", test);
        if(test!=1){
          return 2;
        }
        n = 8;
    }
    //   if (DEBUG_BITS) printf("n: %d, byte: %d (", n, byte);
    //   if (DEBUG_BITS) escrever_binario(byte);
    uint8_t bit;
    bit = (VALORES[8-n] & byte) ? 1 : 0;
    n-=1;
    //   if (DEBUG_BITS) printf(") --> %d(", byte);
    //   if (DEBUG_BITS) escrever_binario(byte);
    //   if (DEBUG_BITS) printf("), bit: %d\n", bit);
    return bit;
}

BufferBitsEscrita::BufferBitsEscrita(FILE *f) :
  BufferBits(f)
{ }

void BufferBitsEscrita::escreve_bit(uint8_t bit)
{
//   if (DEBUG_BITS) printf("bit: %d, n: %d, byte: %d (", bit, n, byte);
//   if (DEBUG_BITS) escrever_binario(byte);

  byte = byte | (VALORES[n]*bit);
  n+=1;

//   if (DEBUG_BITS) printf(") --> %d(", byte);
//   if (DEBUG_BITS) escrever_binario(byte);
//   if (DEBUG_BITS) printf(")\n");

  if (n == 8) {
    descarrega();
  }
}

void BufferBitsEscrita::descarrega()
{
    void *aux=&byte;
    fwrite(aux,1,1,arquivo);
    byte = 0;
    n = 0;
}

/*
******************************
******* DESCOMPACTA **********
******************************
*/
 DescompactaArquivo::DescompactaArquivo(FILE *arquivo, FILE *arquivo2): fileCompacto(arquivo),
 fileSaida(arquivo2), tamanhoAlfabetoD(0), charEncontradosNaPreOrdem(0), sobraUltimoByte(0) {
    if (fileSaida == nullptr) {
        printf("Erro na abriu arquivo");
    }
    leitor = new BufferBitsLeitura(arquivo);
 }
DescompactaArquivo::~DescompactaArquivo() {
    limpaArvore(raizArvoreD);
    delete leitor;
    leitor = nullptr;
}
void DescompactaArquivo::leCabecalho() {
    fseek(fileCompacto, 0, SEEK_SET);
    unsigned char buff[2];
    unsigned char temp;
    fread(buff, sizeof(unsigned char), 2, fileCompacto);
    //printf("%d e %d", buff[0], buff[1]);
    uint16_t maisSignificativo = (uint16_t) buff[1] << 8;
    uint16_t menosSignificativo = (uint16_t) buff[0];
    tamanhoAlfabetoD = (int) (maisSignificativo | menosSignificativo);
    fread(&temp, sizeof(unsigned char), 1, fileCompacto);
    sobraUltimoByte = (int) temp;
    vetorCharNoArquivoCompacto.resize(tamanhoAlfabetoD);
    fread(&vetorCharNoArquivoCompacto[0], sizeof(unsigned char), tamanhoAlfabetoD, fileCompacto);
}   

No* DescompactaArquivo::reconstroiArvorePreOrdem(No *no) {
    if (charEncontradosNaPreOrdem < tamanhoAlfabetoD) {
        uint8_t bit = leitor->le_bit();
        if (bit == 1) {
            unsigned char caractere = vetorCharNoArquivoCompacto[charEncontradosNaPreOrdem++];
            //printf("%d\n", charEncontradosNaPreOrdem);
            No *novo = new No(caractere, 0);
            novo->pai = no;
            return novo; 
        }
        No *novo = new No(0, 0);
        novo->esq = reconstroiArvorePreOrdem(novo);
        novo->dir = reconstroiArvorePreOrdem(novo);
        novo->pai = no;
    
        return novo;
    }
    return nullptr;
}

void DescompactaArquivo::opcaoDescompacta() {
    leCabecalho();
    raizArvoreD = reconstroiArvorePreOrdem(nullptr);
    No *atual = raizArvoreD;
    uint8_t bit;
    while (true) {
        if (!feof(fileCompacto)) {
            bit = leitor->le_bit();
            if (bit == 2) {
                break;
            }
            if (bit == 0) {
                atual = atual->esq;
            } else {
                atual = atual->dir;
            }
    
            if (atual->esq == nullptr && atual->dir == nullptr) {
                void *aux = &(atual->caractereChave);
                fwrite(aux, 1, 1, fileSaida);
                atual = raizArvoreD;
            }
        } 
    }
}

