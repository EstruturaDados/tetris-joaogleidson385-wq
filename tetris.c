/*
  Nível Mestre: Integração Total com Estratégia
  - Fila circular (peças em espera)
  - Pilha (peças prontas / reserva)
  - Reserva única (apenas 1 peça pode estar reservada)
  - Operações:
      1 - Jogar peça (dequeue da fila)  -> pode ser DESFEITA (undo)
      2 - Reservar peça (pop da pilha -> reserva)
      3 - Usar peça reservada (coloca de volta na pilha)
      4 - Trocar topo da pilha com frente da fila
      5 - Desfazer última jogada (só para ação 1)
      6 - Inverter fila com pilha (topo <-> frente)
      7 - Adicionar peça na fila (auxiliar para testes)
      8 - Empilhar peça (push) (auxiliar para testes)
      9 - Mostrar estado (fila, pilha, reserva, histórico)
      0 - Sair
  - Undo: guarda apenas a última ação do tipo "Jogar peça" (1).
  - Proteções contra underflow/overflow e integridade dos dados.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PECA   64
#define MAX_QUEUE  100
#define MAX_STACK  100

typedef struct {
    char nome[MAX_PECA];
} Peca;

/* ==== FILA CIRCULAR ==== */
typedef struct {
    Peca dados[MAX_QUEUE];
    int head; // índice do elemento da frente
    int tail; // índice da próxima posição livre após o último
    int qtd;
} Fila;

/* ==== PILHA ==== */
typedef struct {
    Peca dados[MAX_STACK];
    int top; // índice do próximo livre (top-1 é o topo real)
} Pilha;

/* ==== RESERVA ==== */
typedef struct {
    Peca p;
    bool tem;
} Reserva;

/* ==== HISTÓRICO PARA UNDO (apenas para ação "Jogar peça") ==== */
typedef struct {
    bool tem;        // se há uma ação gravada
    Peca peca;       // peça removida da fila pela jogada
} HistoricoUndo;

/* Prototipos */
void inicializar_fila(Fila *f);
bool fila_vazia(const Fila *f);
bool fila_cheia(const Fila *f);
bool enqueue(Fila *f, const Peca *p);
bool enqueue_front(Fila *f, const Peca *p); // insere na frente (usado pelo undo)
bool dequeue(Fila *f, Peca *out);

void inicializar_pilha(Pilha *s);
bool pilha_vazia(const Pilha *s);
bool pilha_cheia(const Pilha *s);
bool push(Pilha *s, const Peca *p);
bool pop(Pilha *s, Peca *out);
Peca *pilha_top(Pilha *s); // retorna ponteiro ao topo (ou NULL)

void mostrar_estado(const Fila *f, const Pilha *s, const Reserva *r, const HistoricoUndo *h);

void limpar_buffer();
void ler_string(const char *prompt, char *dest, int maxlen);

/* Operações complexas */
bool trocar_topo_pilha_frente_fila(Pilha *s, Fila *f);
bool reservar_peça(Pilha *s, Reserva *res);
bool usar_reserva(Pilha *s, Reserva *res);
bool inverter_fila_pilha(Fila *f, Pilha *s);
void gravar_undo_jogada(HistoricoUndo *h, const Peca *p);
void desfazer_ultima_jogada(Fila *f, HistoricoUndo *h);

int main(void) {
    Fila fila;
    Pilha pilha;
    Reserva reserva = { .tem = false };
    HistoricoUndo historico = { .tem = false };
    inicializar_fila(&fila);
    inicializar_pilha(&pilha);

    int opc;
    printf("=== Nível Mestre: Integração Total com Estratégia ===\n");

    while (1) {
        printf("\nMenu:\n");
        printf("1 - Jogar peça (dequeue da fila)\n");
        printf("2 - Reservar peça (pop da pilha -> reserva)\n");
        printf("3 - Usar peça reservada (volta para topo da pilha)\n");
        printf("4 - Trocar topo da pilha com frente da fila\n");
        printf("5 - Desfazer última jogada (undo da ação 1)\n");
        printf("6 - Inverter fila com pilha (topo <-> frente)\n");
        printf("7 - Adicionar peça na fila (auxiliar)\n");
        printf("8 - Empilhar peça (push) (auxiliar)\n");
        printf("9 - Mostrar estado\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        if (scanf("%d", &opc) != 1) {
            printf("Entrada inválida.\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();

        if (opc == 0) {
            printf("Saindo. Boa sorte!\n");
            break;
        } else if (opc == 1) {
            // Jogar peça: dequeue da fila e grava no histórico para undo
            Peca p;
            if (!dequeue(&fila, &p)) {
                printf("[Jogar] Falha: fila vazia.\n");
            } else {
                printf("[Jogar] Peça jogada: %s\n", p.nome);
                gravar_undo_jogada(&historico, &p);
                // A lógica do jogo pode usar essa peça (ex: pontuar). Aqui apenas registramos.
            }
        } else if (opc == 2) {
            if (reservar_peça(&pilha, &reserva)) {
                printf("[Reserva] Peça reservada com sucesso.\n");
            } else {
                printf("[Reserva] Falha ao reservar (pilha vazia ou reserva ocupada).\n");
            }
        } else if (opc == 3) {
            if (usar_reserva(&pilha, &reserva)) {
                printf("[Usar Reserva] Peça colocada de volta no topo da pilha.\n");
            } else {
                printf("[Usar Reserva] Falha: não há peça na reserva ou pilha cheia.\n");
            }
        } else if (opc == 4) {
            if (trocar_topo_pilha_frente_fila(&pilha, &fila)) {
                printf("[Troca] Troca efetuada entre topo da pilha e frente da fila.\n");
            } else {
                printf("[Troca] Falha na troca (pilha ou fila vazia).\n");
            }
        } else if (opc == 5) {
            desfazer_ultima_jogada(&fila, &historico);
        } else if (opc == 6) {
            if (inverter_fila_pilha(&fila, &pilha)) {
                printf("[Inverter] Fila e pilha invertidas com sucesso.\n");
            } else {
                printf("[Inverter] Operação realizada (atenção a limites de tamanho se excederem).\n");
            }
        } else if (opc == 7) {
            // Adicionar peça na fila (aux)
            Peca p;
            ler_string("Nome da peça para enfileirar: ", p.nome, MAX_PECA);
            if (enqueue(&fila, &p)) {
                printf("[Enqueue] Peça '%s' adicionada à fila.\n", p.nome);
            } else {
                printf("[Enqueue] Falha: fila cheia.\n");
            }
        } else if (opc == 8) {
            // Empilhar peça (aux)
            Peca p;
            ler_string("Nome da peça para empilhar: ", p.nome, MAX_PECA);
            if (push(&pilha, &p)) {
                printf("[Push] Peça '%s' empilhada.\n", p.nome);
            } else {
                printf("[Push] Falha: pilha cheia.\n");
            }
        } else if (opc == 9) {
            mostrar_estado(&fila, &pilha, &reserva, &historico);
        } else {
            printf("Opção inválida.\n");
        }
    }
    return 0;
}

/* ==================== Implementação da fila circular ==================== */

void inicializar_fila(Fila *f) {
    f->head = 0;
    f->tail = 0;
    f->qtd = 0;
}

bool fila_vazia(const Fila *f) {
    return f->qtd == 0;
}

bool fila_cheia(const Fila *f) {
    return f->qtd == MAX_QUEUE;
}

bool enqueue(Fila *f, const Peca *p) {
    if (fila_cheia(f)) return false;
    f->dados[f->tail] = *p;
    f->tail = (f->tail + 1) % MAX_QUEUE;
    f->qtd++;
    return true;
}

/* insere na frente da fila (útil para desfazer uma remoção) */
bool enqueue_front(Fila *f, const Peca *p) {
    if (fila_cheia(f)) return false;
    // mover head "para trás" e inserir
    f->head = (f->head - 1 + MAX_QUEUE) % MAX_QUEUE;
    f->dados[f->head] = *p;
    f->qtd++;
    return true;
}

bool dequeue(Fila *f, Peca *out) {
    if (fila_vazia(f)) return false;
    if (out) *out = f->dados[f->head];
    f->head = (f->head + 1) % MAX_QUEUE;
    f->qtd--;
    return true;
}

/* ==================== Implementação da pilha ==================== */

void inicializar_pilha(Pilha *s) {
    s->top = 0;
}

bool pilha_vazia(const Pilha *s) {
    return s->top == 0;
}

bool pilha_cheia(const Pilha *s) {
    return s->top == MAX_STACK;
}

bool push(Pilha *s, const Peca *p) {
    if (pilha_cheia(s)) return false;
    s->dados[s->top++] = *p;
    return true;
}

bool pop(Pilha *s, Peca *out) {
    if (pilha_vazia(s)) return false;
    s->top--;
    if (out) *out = s->dados[s->top];
    return true;
}

Peca *pilha_top(Pilha *s) {
    if (pilha_vazia(s)) return NULL;
    return &s->dados[s->top - 1];
}

/* ==================== Mostrar estado (diagnóstico) ==================== */

void mostrar_estado(const Fila *f, const Pilha *s, const Reserva *r, const HistoricoUndo *h) {
    printf("\n--- ESTADO ATUAL ---\n");
    printf("Fila (qtd=%d):\n", f->qtd);
    if (f->qtd == 0) {
        printf("  [vazia]\n");
    } else {
        int idx = f->head;
        for (int i = 0; i < f->qtd; ++i) {
            printf("  [%d] %s\n", i, f->dados[idx].nome);
            idx = (idx + 1) % MAX_QUEUE;
        }
    }

    printf("Pilha (top=%d):\n", s->top);
    if (s->top == 0) {
        printf("  [vazia]\n");
    } else {
        for (int i = s->top - 1; i >= 0; --i) {
            printf("  (%d) %s\n", i, s->dados[i].nome);
        }
    }

    printf("Reserva: %s\n", r->tem ? r->p.nome : "[vazia]");

    if (h->tem) {
        printf("Undo disponível: sim (última jogada: %s)\n", h->peca.nome);
    } else {
        printf("Undo disponível: não\n");
    }
    printf("--------------------\n");
}

/* ==================== Utilitários de I/O ==================== */

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void ler_string(const char *prompt, char *dest, int maxlen) {
    printf("%s", prompt);
    if (fgets(dest, maxlen, stdin) == NULL) {
        dest[0] = '\0';
        return;
    }
    size_t len = strlen(dest);
    if (len > 0 && dest[len - 1] == '\n') dest[len - 1] = '\0';
}

/* ==================== Operações complexas ==================== */

/* Troca topo da pilha com frente da fila */
bool trocar_topo_pilha_frente_fila(Pilha *s, Fila *f) {
    if (pilha_vazia(s) || fila_vazia(f)) return false;
    // intercala os elementos
    Peca tmp_pilha_top;
    Peca tmp_fila_front;
    // obter fila.front
    tmp_fila_front = f->dados[f->head];
    // obter pilha.top
    tmp_pilha_top = s->dados[s->top - 1];

    // trocar
    s->dados[s->top - 1] = tmp_fila_front;
    f->dados[f->head] = tmp_pilha_top;
    return true;
}

/* Reserva: remove do topo da pilha e coloca na reserva (se livre) */
bool reservar_peça(Pilha *s, Reserva *res) {
    if (res->tem) return false;
    Peca tmp;
    if (!pop(s, &tmp)) return false;
    res->p = tmp;
    res->tem = true;
    return true;
}

/* Usa a reserva: coloca a peça de volta no topo da pilha */
bool usar_reserva(Pilha *s, Reserva *res) {
    if (!res->tem) return false;
    if (pilha_cheia(s)) return false;
    if (!push(s, &res->p)) return false;
    res->tem = false;
    return true;
}

/* Inverter fila com pilha:
   Depois: fila (frente -> trás) = elementos da pilha do topo para a base (top->0)
           pilha (bottom->top) = elementos da fila do final para a frente (rear-1 -> head)
   A operação preserva "topo vira frente" e "frente vira topo".
*/
bool inverter_fila_pilha(Fila *f, Pilha *s) {
    // coletar elementos atuais
    int qtd_fila = f->qtd;
    int qtd_pilha = s->top;

    // checar se tamanhos cabem (essa implementação sobrescreve sem overflow se ultrapassar limites)
    if (qtd_pilha > MAX_QUEUE || qtd_fila > MAX_STACK) {
        // operação pode exceder capacidades se não couberem; retornamos false para indicar alerta
        // mas ainda tentamos executar parcialmente (neste código, rejeitamos por segurança)
        printf("[Inverter] Falha: tamanhos excederiam limites da fila/pilha.\n");
        return false;
    }

    // Arrays temporários
    Peca tmp_from_pilha[MAX_STACK];
    Peca tmp_from_fila[MAX_QUEUE];

    // copiar pilha (topo->base)
    for (int i = 0; i < qtd_pilha; ++i) {
        tmp_from_pilha[i] = s->dados[qtd_pilha - 1 - i]; // 0 = topo, 1 = topo-1, ...
    }
    // copiar fila (frente->trás)
    int idx = f->head;
    for (int i = 0; i < qtd_fila; ++i) {
        tmp_from_fila[i] = f->dados[idx];
        idx = (idx + 1) % MAX_QUEUE;
    }

    // construir nova fila: elementos da pilha do topo->base (já em tmp_from_pilha 0..qtd_pilha-1)
    inicializar_fila(f);
    for (int i = 0; i < qtd_pilha; ++i) {
        enqueue(f, &tmp_from_pilha[i]);
    }

    // construir nova pilha: queremos top == old_queue.front
    // Se empilharmos elementos do final->frente (rear-1 .. head), o último push será old front -> top
    inicializar_pilha(s);
    // Para obter rear index, compute (head + qtd_fila - 1) % MAX_QUEUE
    for (int i = qtd_fila - 1; i >= 0; --i) {
        push(s, &tmp_from_fila[i]);
    }

    return true;
}

/* Gravacao simples do undo para última jogada */
void gravar_undo_jogada(HistoricoUndo *h, const Peca *p) {
    h->tem = true;
    h->peca = *p;
}

/* Desfaz ultima jogada: insere a peça removida de volta na frente da fila */
void desfazer_ultima_jogada(Fila *f, HistoricoUndo *h) {
    if (!h->tem) {
        printf("[Undo] Não há jogada para desfazer.\n");
        return;
    }
    if (fila_cheia(f)) {
        printf("[Undo] Falha: fila cheia, não é possível restituir a peça.\n");
        return;
    }
    if (!enqueue_front(f, &h->peca)) {
        printf("[Undo] Falha ao restaurar peça.\n");
        return;
    }
    printf("[Undo] A jogada foi desfeita. Peça '%s' colocada de volta na frente da fila.\n", h->peca.nome);
    h->tem = false;
}
