#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "gfx.h"

//construcao
struct s_no{
    int32_t chave:30;
    int32_t reservado:2;
    struct s_no *esq;
    struct s_no *dir;
};

struct s_arq_no{
    int32_t chave:30;
    uint32_t esq:1;
    uint32_t dir:1;
};

struct s_no *nvno(int val){
    struct s_no *temp = (struct s_no*)malloc(sizeof(struct s_no));
    temp->chave = val;
    temp->reservado = 0;
    temp->esq = temp->dir = NULL;
    return temp;
}

//busca
void busc(struct s_no* raiz, int chave){
    if (raiz->chave == chave){
        printf("VALOR '%d' ENCONTRADO\n\n", chave);
        return;
    }
    if (raiz->chave < chave){
        if (raiz->dir != NULL){
            busc(raiz->dir, chave);
            return;
        }else{
            printf("VALOR '%d' NAO ENCONTRADO\n\n", chave);
            return;
        }
    }
    if(raiz->esq != NULL){
        busc(raiz->esq, chave);
        return;
    }else{
        printf("VALOR '%d' NAO ENCONTRADO\n\n", chave);
        return;
    }
    
}
//printa a arvore em preordem
void most(struct s_no *raiz, unsigned short xi, unsigned short xf, unsigned short y){
    char str[64];
    unsigned short xm;
    if (raiz == NULL){
        return; 
    }
    gfx_set_color(255,255,255);
    xm=(xi+xf)/2;
    //printf("%i ", raiz->chave); pra printar em preordem da forma usual
    snprintf(str,64,"%i",raiz->chave);
    gfx_text(xm, y, str);
    
    if(raiz->esq!=NULL){
        gfx_set_color(0,0,255);
        gfx_line(xm, y, (xi+xm)/2, y+70);
    }
    most(raiz->esq, xi, xm, y+70);
    
    if(raiz->dir!=NULL){
        gfx_set_color(0,0,255);
        gfx_line(xm, y, (xf+xm)/2, y+70);
    }
    most(raiz->dir, xm, xf, y+70); 
}
//insercao
struct s_no* ins(struct s_no* no, int chave){
    if (no == NULL){
        return nvno(chave);
    }
    if (chave < no->chave){
        no->esq  = ins(no->esq, chave);
    }else if (chave > no->chave){
        no->dir = ins(no->dir, chave);
    }
    return no;
}
//funcao auxiliar pra remocao
struct s_no * nomin(struct s_no* meno){
    struct s_no* atu = meno;
    while (atu && atu->esq != NULL){
        atu = atu->esq;
    }
    return atu;
}
//remocao
struct s_no* rem(struct s_no* raiz, int chave){
    if (raiz == NULL){
        return raiz;
    }
    if (chave < raiz->chave){
        raiz->esq = rem(raiz->esq, chave);
    }else if (chave > raiz->chave){
        raiz->dir = rem(raiz->dir, chave);
    }else{
        if (raiz->esq == NULL){
            struct s_no *temp = raiz->dir;
            free(raiz);
            return temp;
        }else if (raiz->dir == NULL){
            struct s_no *temp = raiz->esq;
            free(raiz);
            return temp;
        }
        struct s_no* temp = nomin(raiz->dir);
        raiz->chave = temp->chave;
        raiz->dir = rem(raiz->dir, temp->chave);
    }
    return raiz;
}
//desalocacao
void desaloc(struct s_no* raiz){ 
    if (raiz == NULL){
        return; 
    }
    desaloc(raiz->esq);
    desaloc(raiz->dir);
    free(raiz);
}
//escrita da arvore em arquivo binario usando a estrutura especificada
void warq(struct s_no* raiz, struct s_arq_no *arqr, FILE* fp_bin){
    if(raiz==NULL){
        return;
    }
    arqr = (struct s_arq_no *)malloc(sizeof(struct s_arq_no));
    arqr->chave=raiz->chave;
    if(raiz->esq==NULL){
        arqr->esq=0;
    }else{
        arqr->esq=1;
    }
    if(raiz->dir==NULL){
        arqr->dir=0;
    }else{
        arqr->dir=1;
    }
    fwrite(arqr, sizeof(struct s_arq_no), 1, fp_bin);
    warq(raiz->esq, arqr, fp_bin);
    warq(raiz->dir, arqr, fp_bin);
}
//leitura da arvore em arquivo binario usando a estrutura especificada
struct s_no* rarq(struct s_no* no, FILE* fl_bin){
    struct s_arq_no arq;
    fread(&arq, sizeof(arq), 1, fl_bin);
    if(!feof(fl_bin)){
        no = (struct s_no *)malloc(sizeof(struct s_no));
        no->chave = arq.chave;
        no->reservado = 0;
        if(arq.esq==1){
            no->esq = rarq(no->esq, fl_bin);
        }else{
            no->esq = NULL;
        }
        if(arq.dir==1){
            no->dir = rarq(no->dir, fl_bin);
        }else{
            no->dir = NULL;
        }
    }
    return no;
}

int main(){
    int menu, bus, nv;
    unsigned long lsize;
    char nome[20];
    struct s_no *raiz = NULL;
    struct s_arq_no *arqr = NULL;
    FILE* fp_bin;
    FILE* fl_bin;

    printf("ARVORE BINARIA DE BUSCA\n\n");
    do{
        printf("ESCOLHA A OPERACAO\n");
        printf("0- IMPRESSAO NO GFX\n");
        printf("1- BUSCA\n");
        printf("2- INSERCAO\n");
        printf("3- REMOCAO\n");
        printf("4- ESCRITA EM ARQUIVO BINARIO\n");
        printf("5- LEITURA DE ARQUIVO BINARIO (ARVORE ATUAL SERA APAGADA)\n");
        printf("6- FINALIZAR\n");

        scanf("%d", &menu);
        switch(menu){
            case 0:;
                if (raiz == NULL){
                    printf("\nARVORE VAZIA\n\n");
                }else{
                    printf("\n");
                    gfx_init(800, 600, "ARVORE BINARIA DE BUSCA");
                    gfx_set_font_size(30);
                    most(raiz, 0, 800, 70);
                    sleep(15);
                    gfx_quit();                    
                }
                break;
            case 1:;
                if(raiz != NULL){
                    printf("DIGITE O VALOR A SER BUSCADO\n");
                    scanf("%d", &bus);
                    busc(raiz, bus);
                }else{
                    printf("\nARVORE VAZIA\n\n");
                }
                break;
            case 2:;
                printf("DIGITE O VALOR A SER INSERIDO\n");
                scanf("%d", &nv);
                if (raiz != NULL){
                    ins(raiz, nv);
                }else{
                    raiz = ins(raiz, nv);
                }
                printf("VALOR '%d' INSERIDO OU JA PRESENTE\n\n", nv);
                break;
            case 3:;
                if (raiz == NULL){
                    printf("ARVORE VAZIA\n\n");
                }else{
                    printf("DIGITE O VALOR A SER REMOVIDO\n");
                    scanf("%d", &nv);
                    raiz = rem(raiz, nv);
                    printf("VALOR '%d' REMOVIDO OU NAO ENCONTRADO\n\n", nv);
                }
                break;
            case 4:;
                if (raiz == NULL){
                    printf("ARVORE VAZIA\n\n");
                }else{
                    printf("\nDIGITE O NOME QUE O ARQUIVO BINARIO TERA:\n");
                    scanf("%s", nome);
                    fp_bin = fopen(nome, "wb");
                    warq(raiz, arqr, fp_bin);
                    printf("\nARVORE ESCRITA EM ARQUIVO\n\n");
                    fclose(fp_bin);
                }
                
                break;
            case 5:;
                desaloc(raiz);
                printf("\nDIGITE O NOME DO ARQUIVO (COM A EXTENSAO):\n");
                scanf("%s", nome);
                fl_bin = fopen(nome, "rb");
                if (fl_bin==NULL){
                    printf("ARQUIVO NAO ENCONTRADO\n");
                }else{
                    raiz = (struct s_no*)malloc(sizeof(struct s_no));
                    fseek (fl_bin , 0 , SEEK_END);
                    lsize = ftell (fl_bin);
                    rewind (fl_bin);
                    arqr = (struct s_arq_no*)malloc(sizeof(struct s_arq_no)*lsize);
                    if (arqr == NULL){
                        printf("ERRO DE MEMORIA\n");
                    }else{
                        raiz = rarq(raiz, fl_bin);
                        printf("ARQUIVO LIDO\n\n");
                    }
                }
                fclose(fl_bin);
                break;
            case 6:;
                desaloc(raiz);
                printf("\nARVORE DESALOCADA\n\n"); //eu testei pra ver se realmente foi desalocada, isso e so um aviso de confirmacao
                break;
        }
    }while(menu!=6);
    
    return 0;
}
