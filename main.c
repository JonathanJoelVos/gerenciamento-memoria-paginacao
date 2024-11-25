#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAMANHO_MEM_FISICA 1024
#define TAMANHO_PAGINA 64
#define TAMANHO_MAXIMO_PROCESSO 256
#define TOTAL_QUADROS (TAMANHO_MEM_FISICA / TAMANHO_PAGINA)

typedef struct {
    int ocupado;           // 0 para livre, 1 para ocupado
    unsigned char dados[TAMANHO_PAGINA];
} Quadro;

typedef struct {
    int pid;                
    int tamanho;            
    int num_paginas;        
    int *tabela_paginas;    
} Processo;

int quadros_livres[TOTAL_QUADROS]; // 0 para livre, 1 para ocupado

Quadro memoria_fisica[TOTAL_QUADROS];
Processo *processos[100];   // Supondo um máximo de 100 processos
int num_processos = 0;

void inicializar_memoria() {
    for (int i = 0; i < TOTAL_QUADROS; i++) {
        memoria_fisica[i].ocupado = 0;
        memset(memoria_fisica[i].dados, 0, TAMANHO_PAGINA);
        quadros_livres[i] = 0;
    }
}

void criar_processo(int pid, int tamanho) {
    if (tamanho > TAMANHO_MAXIMO_PROCESSO) {
        printf("Erro: Tamanho do processo excede o tamanho máximo permitido.\n");
        return;
    }

    int num_paginas = (tamanho + TAMANHO_PAGINA - 1) / TAMANHO_PAGINA;
    int quadros_necessarios = num_paginas;

    // Verificar se há quadros livres suficientes
    int quadros_disponiveis = 0;
    for (int i = 0; i < TOTAL_QUADROS; i++) {
        if (quadros_livres[i] == 0) {
            quadros_disponiveis++;
        }
    }

    if (quadros_disponiveis < quadros_necessarios) {
        printf("Erro: Memória insuficiente para alocar o processo.\n");
        return;
    }

    // Criar o processo
    Processo *proc = (Processo *)malloc(sizeof(Processo));
    proc->pid = pid;
    proc->tamanho = tamanho;
    proc->num_paginas = num_paginas;
    proc->tabela_paginas = (int *)malloc(sizeof(int) * num_paginas);

    // Alocar quadros para as páginas do processo
    int pagina_atual = 0;
    for (int i = 0; i < TOTAL_QUADROS && pagina_atual < num_paginas; i++) {
        if (quadros_livres[i] == 0) {
            quadros_livres[i] = 1;
            memoria_fisica[i].ocupado = 1;

            // Inicializar os dados da página com valores aleatórios
            for (int j = 0; j < TAMANHO_PAGINA; j++) {
                memoria_fisica[i].dados[j] = rand() % 256;
            }

            proc->tabela_paginas[pagina_atual] = i;
            pagina_atual++;
        }
    }

    // Adicionar o processo à lista de processos
    processos[num_processos++] = proc;

    printf("Processo %d criado com sucesso.\n", pid);
}


void visualizar_memoria() {
    int quadros_ocupados = 0;
    for (int i = 0; i < TOTAL_QUADROS; i++) {
        if (memoria_fisica[i].ocupado) {
            quadros_ocupados++;
        }
    }

    float percentual_livre = ((TOTAL_QUADROS - quadros_ocupados) / (float)TOTAL_QUADROS) * 100;
    printf("Memória Livre: %.2f%%\n", percentual_livre);

    for (int i = 0; i < TOTAL_QUADROS; i++) {
        printf("Quadro %d: %s\n", i, memoria_fisica[i].ocupado ? "Ocupado" : "Livre");
        if (memoria_fisica[i].ocupado) {
            printf("Dados: ");
            for (int j = 0; j < TAMANHO_PAGINA; j++) {
                printf("%02X ", memoria_fisica[i].dados[j]);
            }
            printf("\n");
        }
    }
}


void visualizar_tabela_paginas(int pid) {
    Processo *proc = NULL;
    for (int i = 0; i < num_processos; i++) {
        if (processos[i]->pid == pid) {
            proc = processos[i];
            break;
        }
    }

    if (proc == NULL) {
        printf("Erro: Processo não encontrado.\n");
        return;
    }

    printf("Processo %d - Tamanho: %d bytes\n", proc->pid, proc->tamanho);
    printf("Tabela de Páginas:\n");
    for (int i = 0; i < proc->num_paginas; i++) {
        printf("Página %d -> Quadro %d\n", i, proc->tabela_paginas[i]);
    }
}

void menu() {
    int opcao;
    do {
        printf("\n=== Simulador de Gerenciamento de Memória ===\n");
        printf("1. Visualizar Memória\n");
        printf("2. Criar Processo\n");
        printf("3. Visualizar Tabela de Páginas\n");
        printf("4. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        if (opcao == 1) {
            visualizar_memoria();
        } else if (opcao == 2) {
            int pid, tamanho;
            printf("Informe o PID do processo: ");
            scanf("%d", &pid);
            printf("Informe o tamanho do processo em bytes: ");
            scanf("%d", &tamanho);
            criar_processo(pid, tamanho);
        } else if (opcao == 3) {
            int pid;
            printf("Informe o PID do processo: ");
            scanf("%d", &pid);
            visualizar_tabela_paginas(pid);
        } else if (opcao == 4) {
            printf("Encerrando o simulador.\n");
        } else {
            printf("Opção inválida. Tente novamente.\n");
        }
    } while (opcao != 4);
}


int main(){
    srand(time(NULL));  // Inicializa o gerador de números aleatórios
    inicializar_memoria();
    menu();
    return 0;
}