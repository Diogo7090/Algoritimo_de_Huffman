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
class Frequencia {
    private:
        void contadorDeFrequencia();
        FILE *arquivoLeitura;
        int tabelaDireta [256] = {0};
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
};


int main() {
    FILE *arquivo = fopen("../teste.txt", "r");
    if (arquivo == nullptr) {
        printf("Erro ao abrir\n");
    }
    Frequencia contadorFreq(arquivo);
    contadorFreq.escreveTabela();

    fclose(arquivo);
    return 0;
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