/*
 * Mini Projeto 03 - Backend de Jogo: Biblioteca Magica
 * Structs, Ponteiros, Alocacao Dinamica e Manipulacao de Arquivos
 *
 * Uso: ./programa nomeDoArquivo.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#endif

#define MAX_LIVROS 100
#define NOME_INVENTARIO "GRIMORIO ANCESTRAL DE ELDRANOR"

/* ---------- Cores (codigos ANSI) ---------- */
#define COR_RESET   "\x1b[0m"
#define COR_TITULO  "\x1b[1;36m"     /* ciano negrito */
#define COR_OPCAO   "\x1b[33m"       /* amarelo */
#define COR_SAIR    "\x1b[1;31m"     /* vermelho negrito */
#define COR_SUCESSO "\x1b[32m"       /* verde */
#define COR_ERRO    "\x1b[31m"       /* vermelho */
#define COR_LARANJA "\x1b[38;5;208m" /* laranja */
#define COR_ROXO    "\x1b[35m"       /* roxo/magenta */

/* ---------- Structs ---------- */

struct Data {
    int dia;
    int mes;
    int ano;
};

struct Autor {
    char nome[100];
    struct Data data_nascimento;
};

/* Atributos extras do livro (alem do minimo pedido no enunciado) */
struct StatusLivro {
    int vida;
    int estamina;
    int forca;
    int destreza;
    int encantamento;
    int magia;
};

struct LivroMagico {
    int id;
    char titulo[100];
    struct Autor autor;
    struct Data data_escrita;
    struct StatusLivro status;
};

/* ---------- Prototipos ---------- */

void inicializarVetor(struct LivroMagico **biblioteca);
void cadastrarLivro(struct LivroMagico **biblioteca);
void deletarLivro(struct LivroMagico **biblioteca, int id);
void mostrarLivro(struct LivroMagico **biblioteca, int id);
void editarLivro(struct LivroMagico **biblioteca, int id);
void listarTitulos(struct LivroMagico **biblioteca);
void salvarBiblioteca(struct LivroMagico **biblioteca, const char *nomeArquivo);
void carregarBiblioteca(struct LivroMagico **biblioteca, const char *nomeArquivo);
void criptografar(char *str);
void descriptografar(char *str);

/* Funcoes auxiliares (nao pedidas no enunciado, mas facilitam o codigo) */
void limparBufferEntrada(void);
void lerLinha(char *destino, int tamanho);
void liberarBiblioteca(struct LivroMagico **biblioteca);
void lerData(struct Data *data);
void habilitarCoresTerminal(void);
int contarOcupados(struct LivroMagico **biblioteca);
int lerStatusIndividual(const char *nome, const char *descricao);
void lerStatus(struct StatusLivro *status);
float calcularMedia(struct StatusLivro *status);
char nivelPoderLetra(float media);
const char *corNivelPoder(char nivel);
void imprimirMenu(struct LivroMagico **biblioteca);
int ehBissexto(int ano);
int diasNoMes(int mes, int ano);
int dataAnterior(struct Data *d1, struct Data *d2);
void lerDataEscritaValida(struct Data *dataEscrita, struct Data *dataNascimento);

/* ---------- Main ---------- */

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Uso correto: %s <nomeDoArquivo>\n", argv[0]);
        return 1;
    }

    struct LivroMagico *biblioteca[MAX_LIVROS];
    int opcao, id;

    habilitarCoresTerminal();

    inicializarVetor(biblioteca);
    carregarBiblioteca(biblioteca, argv[1]);

    do {
        imprimirMenu(biblioteca);

        if (scanf("%d", &opcao) != 1) {
            printf(COR_ERRO "Entrada invalida!" COR_RESET "\n");
            limparBufferEntrada();
            opcao = -1;
            continue;
        }
        limparBufferEntrada();

        switch (opcao) {
            case 1:
                cadastrarLivro(biblioteca);
                break;
            case 2:
                printf("Digite o id do livro a ser deletado: ");
                scanf("%d", &id);
                limparBufferEntrada();
                deletarLivro(biblioteca, id);
                break;
            case 3:
                printf("Digite o id do livro a ser mostrado: ");
                scanf("%d", &id);
                limparBufferEntrada();
                mostrarLivro(biblioteca, id);
                break;
            case 4:
                printf("Digite o id do livro a ser editado: ");
                scanf("%d", &id);
                limparBufferEntrada();
                editarLivro(biblioteca, id);
                break;
            case 5:
                listarTitulos(biblioteca);
                break;
            case 6:
                salvarBiblioteca(biblioteca, argv[1]);
                liberarBiblioteca(biblioteca);
                printf(COR_SAIR "Ate a proxima, aventureiro!" COR_RESET "\n");
                break;
            default:
                printf(COR_ERRO "Opcao invalida!" COR_RESET "\n");
        }

    } while (opcao != 6);

    return 0;
}

/* ---------- Implementacao das funcoes ---------- */

void inicializarVetor(struct LivroMagico **biblioteca) {
    int i;
    for (i = 0; i < MAX_LIVROS; i++) {
        biblioteca[i] = NULL;
    }
}

int contarOcupados(struct LivroMagico **biblioteca) {
    int i, total = 0;
    for (i = 0; i < MAX_LIVROS; i++) {
        if (biblioteca[i] != NULL) {
            total++;
        }
    }
    return total;
}

void imprimirMenu(struct LivroMagico **biblioteca) {
    int ocupados = contarOcupados(biblioteca);

    printf("\n" COR_TITULO "============================================================" COR_RESET "\n");
    printf(COR_TITULO "     %s" COR_RESET "\n", NOME_INVENTARIO);
    printf(COR_TITULO "============================================================" COR_RESET "\n");
    printf("  Slots ocupados: " COR_OPCAO "%d/%d" COR_RESET "\n", ocupados, MAX_LIVROS);
    printf(COR_TITULO "------------------------------------------------------------" COR_RESET "\n");
    printf("   " COR_OPCAO "[1]" COR_RESET " Cadastrar livro\n");
    printf("   " COR_OPCAO "[2]" COR_RESET " Deletar livro\n");
    printf("   " COR_OPCAO "[3]" COR_RESET " Mostrar livro\n");
    printf("   " COR_OPCAO "[4]" COR_RESET " Editar livro\n");
    printf("   " COR_OPCAO "[5]" COR_RESET " Listar titulos\n");
    printf("   " COR_SAIR  "[6] Sair" COR_RESET "\n");
    printf(COR_TITULO "============================================================" COR_RESET "\n");
    printf("Escolha uma opcao: ");
}

int lerStatusIndividual(const char *nome, const char *descricao) {
    int valor;

    printf("\n" COR_OPCAO ">> %s" COR_RESET " - %s\n", nome, descricao);
    do {
        printf("   Valor (1 a 99): ");
        if (scanf("%d", &valor) != 1) {
            valor = -1;
        }
        limparBufferEntrada();

        if (valor < 1 || valor > 99) {
            printf(COR_ERRO "   Valor invalido! Deve ser um numero entre 1 e 99." COR_RESET "\n");
        }
    } while (valor < 1 || valor > 99);

    return valor;
}

void lerStatus(struct StatusLivro *status) {
    printf("\n" COR_TITULO "-- Atributos magicos do livro (cada um de 1 a 99) --" COR_RESET "\n");
    status->vida = lerStatusIndividual("Vida", "Quantos pontos de HP o personagem tera.");
    status->estamina = lerStatusIndividual("Estamina", "Capacidade de usar ataques sem precisar descansar.");
    status->forca = lerStatusIndividual("Forca", "Poder fisico bruto do personagem.");
    status->destreza = lerStatusIndividual("Destreza", "Agilidade e precisao nos movimentos.");
    status->encantamento = lerStatusIndividual("Encantamento", "Magia de reforco que fortalece o personagem.");
    status->magia = lerStatusIndividual("Magia", "Fonte de poder magico em geral.");
}

float calcularMedia(struct StatusLivro *status) {
    int soma = status->vida + status->estamina + status->forca +
               status->destreza + status->encantamento + status->magia;
    return soma / 6.0f;
}

char nivelPoderLetra(float media) {
    if (media < 20) return 'D';
    if (media < 40) return 'C';
    if (media < 60) return 'B';
    if (media < 80) return 'A';
    return 'S';
}

const char *corNivelPoder(char nivel) {
    switch (nivel) {
        case 'D': return COR_SUCESSO;
        case 'C': return COR_OPCAO;
        case 'B': return COR_LARANJA;
        case 'A': return COR_ERRO;
        case 'S': return COR_ROXO;
        default:  return COR_RESET;
    }
}

void cadastrarLivro(struct LivroMagico **biblioteca) {
    int posLivre = -1;
    int i;
    int posEscolhida;

    printf("\n" COR_TITULO "==================== CADASTRO DE NOVO LIVRO ====================" COR_RESET "\n");

    printf("\n" COR_OPCAO ">> Posicao no inventario" COR_RESET " (1 a %d, ou 0 = automatico): ", MAX_LIVROS);
    scanf("%d", &posEscolhida);
    limparBufferEntrada();

    if (posEscolhida == 0) {
        /* Posicao automatica: procura a primeira posicao livre */
        for (i = 0; i < MAX_LIVROS; i++) {
            if (biblioteca[i] == NULL) {
                posLivre = i;
                break;
            }
        }

        if (posLivre == -1) {
            printf(COR_ERRO "A biblioteca esta cheia! Nao e possivel cadastrar mais livros." COR_RESET "\n");
            return;
        }
    } else if (posEscolhida >= 1 && posEscolhida <= MAX_LIVROS) {
        /* Posicao escolhida manualmente (convertida para indice de 0 a MAX_LIVROS-1) */
        posLivre = posEscolhida - 1;

        if (biblioteca[posLivre] != NULL) {
            printf(COR_ERRO "Essa posicao ja esta ocupada! Escolha outra posicao ou digite 0 para automatico." COR_RESET "\n");
            return;
        }
    } else {
        printf(COR_ERRO "Posicao invalida! Digite um numero entre 1 e %d, ou 0." COR_RESET "\n", MAX_LIVROS);
        return;
    }

    struct LivroMagico *novoLivro = (struct LivroMagico *) malloc(sizeof(struct LivroMagico));
    if (novoLivro == NULL) {
        printf(COR_ERRO "Erro ao alocar memoria!" COR_RESET "\n");
        return;
    }

    printf("\n" COR_OPCAO ">> ID do livro" COR_RESET ": ");
    scanf("%d", &novoLivro->id);
    limparBufferEntrada();

    /* Verifica se ja existe outro livro cadastrado com esse mesmo id */
    for (i = 0; i < MAX_LIVROS; i++) {
        if (biblioteca[i] != NULL && biblioteca[i]->id == novoLivro->id) {
            printf(COR_ERRO "Ja existe um livro cadastrado com o id %d! Cadastro cancelado." COR_RESET "\n", novoLivro->id);
            free(novoLivro);
            return;
        }
    }

    printf("\n" COR_OPCAO ">> Titulo do livro" COR_RESET ": ");
    lerLinha(novoLivro->titulo, sizeof(novoLivro->titulo));

    printf("\n" COR_OPCAO ">> Nome do autor" COR_RESET ": ");
    lerLinha(novoLivro->autor.nome, sizeof(novoLivro->autor.nome));

    printf("\n" COR_TITULO "-- Data de nascimento do autor --" COR_RESET "\n");
    lerData(&novoLivro->autor.data_nascimento);

    lerDataEscritaValida(&novoLivro->data_escrita, &novoLivro->autor.data_nascimento);

    lerStatus(&novoLivro->status);

    biblioteca[posLivre] = novoLivro;

    printf("\n" COR_TITULO "==================================================================" COR_RESET "\n");
    printf(COR_SUCESSO "Livro cadastrado com sucesso na posicao %d do inventario!" COR_RESET "\n", posLivre + 1);
}

void deletarLivro(struct LivroMagico **biblioteca, int id) {
    int i;

    for (i = 0; i < MAX_LIVROS; i++) {
        if (biblioteca[i] != NULL && biblioteca[i]->id == id) {
            free(biblioteca[i]);
            biblioteca[i] = NULL;
            printf(COR_SUCESSO "Livro de id %d deletado com sucesso!" COR_RESET "\n", id);
            return;
        }
    }

    printf(COR_ERRO "Livro de id %d nao encontrado!" COR_RESET "\n", id);
}

void mostrarLivro(struct LivroMagico **biblioteca, int id) {
    int i;

    for (i = 0; i < MAX_LIVROS; i++) {
        if (biblioteca[i] != NULL && biblioteca[i]->id == id) {
            float media = calcularMedia(&biblioteca[i]->status);
            char nivel = nivelPoderLetra(media);

            printf("\n----- Livro encontrado -----\n");
            printf("Posicao: %d\n", i + 1);
            printf("ID: %d\n", biblioteca[i]->id);
            printf("Titulo: %s\n", biblioteca[i]->titulo);
            printf("Autor: %s\n", biblioteca[i]->autor.nome);
            printf("Nascimento do autor: %02d/%02d/%d\n",
                   biblioteca[i]->autor.data_nascimento.dia,
                   biblioteca[i]->autor.data_nascimento.mes,
                   biblioteca[i]->autor.data_nascimento.ano);
            printf("Data de escrita: %02d/%02d/%d\n",
                   biblioteca[i]->data_escrita.dia,
                   biblioteca[i]->data_escrita.mes,
                   biblioteca[i]->data_escrita.ano);

            printf("\n--- Atributos ---\n");
            printf("Vida: %d\n", biblioteca[i]->status.vida);
            printf("Estamina: %d\n", biblioteca[i]->status.estamina);
            printf("Forca: %d\n", biblioteca[i]->status.forca);
            printf("Destreza: %d\n", biblioteca[i]->status.destreza);
            printf("Encantamento: %d\n", biblioteca[i]->status.encantamento);
            printf("Magia: %d\n", biblioteca[i]->status.magia);
            printf("Nivel de Poder: %s%c" COR_RESET " (media dos atributos: %.1f)\n",
                   corNivelPoder(nivel), nivel, media);
            return;
        }
    }

    printf(COR_ERRO "Livro de id %d nao encontrado!" COR_RESET "\n", id);
}

void editarLivro(struct LivroMagico **biblioteca, int id) {
    int i;

    for (i = 0; i < MAX_LIVROS; i++) {
        if (biblioteca[i] != NULL && biblioteca[i]->id == id) {

            printf("\n" COR_TITULO "==================== EDITANDO LIVRO (id %d) ====================" COR_RESET "\n", id);

            printf("\n" COR_OPCAO ">> Novo titulo" COR_RESET ": ");
            lerLinha(biblioteca[i]->titulo, sizeof(biblioteca[i]->titulo));

            printf("\n" COR_OPCAO ">> Novo nome do autor" COR_RESET ": ");
            lerLinha(biblioteca[i]->autor.nome, sizeof(biblioteca[i]->autor.nome));

            printf("\n" COR_TITULO "-- Nova data de nascimento do autor --" COR_RESET "\n");
            lerData(&biblioteca[i]->autor.data_nascimento);

            lerDataEscritaValida(&biblioteca[i]->data_escrita, &biblioteca[i]->autor.data_nascimento);

            lerStatus(&biblioteca[i]->status);

            printf("\n" COR_TITULO "==================================================================" COR_RESET "\n");
            printf(COR_SUCESSO "Livro atualizado com sucesso!" COR_RESET "\n");
            return;
        }
    }

    printf(COR_ERRO "Livro de id %d nao encontrado!" COR_RESET "\n", id);
}

void listarTitulos(struct LivroMagico **biblioteca) {
    int i;
    int encontrouAlgum = 0;

    printf("\n----- Titulos da Biblioteca -----\n");
    for (i = 0; i < MAX_LIVROS; i++) {
        if (biblioteca[i] != NULL) {
            float media = calcularMedia(&biblioteca[i]->status);
            char nivel = nivelPoderLetra(media);

            printf("Posicao: %d - ID: %d - Titulo: %s - Nivel de Poder: %s%c" COR_RESET "\n",
                   i + 1, biblioteca[i]->id, biblioteca[i]->titulo,
                   corNivelPoder(nivel), nivel);
            encontrouAlgum = 1;
        }
    }

    if (!encontrouAlgum) {
        printf("Nenhum livro cadastrado ainda.\n");
    }
}

void salvarBiblioteca(struct LivroMagico **biblioteca, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "wb");
    if (arquivo == NULL) {
        printf(COR_ERRO "Erro ao abrir o arquivo para salvar!" COR_RESET "\n");
        return;
    }

    int i;
    int quantidade = 0;

    /* Conta quantos livros existem para gravar no cabecalho do arquivo */
    for (i = 0; i < MAX_LIVROS; i++) {
        if (biblioteca[i] != NULL) {
            quantidade++;
        }
    }

    fwrite(&quantidade, sizeof(int), 1, arquivo);

    for (i = 0; i < MAX_LIVROS; i++) {
        if (biblioteca[i] != NULL) {
            /* Criptografa o titulo somente para gravar, depois restaura em memoria */
            criptografar(biblioteca[i]->titulo);
            fwrite(biblioteca[i], sizeof(struct LivroMagico), 1, arquivo);
            descriptografar(biblioteca[i]->titulo);
        }
    }

    fclose(arquivo);
    printf(COR_SUCESSO "Biblioteca salva no arquivo '%s' com sucesso!" COR_RESET "\n", nomeArquivo);
}

void carregarBiblioteca(struct LivroMagico **biblioteca, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL) {
        printf(COR_OPCAO "Nenhum save encontrado neste computador. Iniciando uma nova aventura!" COR_RESET "\n");
        return;
    }

    int quantidade, i, posLivre;

    if (fread(&quantidade, sizeof(int), 1, arquivo) != 1) {
        printf(COR_OPCAO "Arquivo de save vazio ou corrompido. Iniciando uma nova aventura!" COR_RESET "\n");
        fclose(arquivo);
        return;
    }

    printf(COR_SUCESSO "Save encontrado! Carregando %d livro(s) salvos anteriormente..." COR_RESET "\n", quantidade);

    for (i = 0; i < quantidade; i++) {
        struct LivroMagico *livro = (struct LivroMagico *) malloc(sizeof(struct LivroMagico));
        if (livro == NULL) {
            printf(COR_ERRO "Erro ao alocar memoria durante o carregamento!" COR_RESET "\n");
            break;
        }

        if (fread(livro, sizeof(struct LivroMagico), 1, arquivo) != 1) {
            free(livro);
            break;
        }

        descriptografar(livro->titulo);

        /* Procura uma posicao livre no vetor para colocar o livro carregado */
        posLivre = -1;
        for (int j = 0; j < MAX_LIVROS; j++) {
            if (biblioteca[j] == NULL) {
                posLivre = j;
                break;
            }
        }

        if (posLivre == -1) {
            printf(COR_ERRO "Vetor cheio durante o carregamento, alguns livros nao foram carregados!" COR_RESET "\n");
            free(livro);
            break;
        }

        biblioteca[posLivre] = livro;
    }

    fclose(arquivo);
}

void criptografar(char *str) {
    int i;
    int tamanho = strlen(str);

    for (i = 0; i < tamanho; i++) {
        str[i] = (char)(255 - (unsigned char)str[i]);
    }
}

void descriptografar(char *str) {
    /* A operacao de complemento de 255 e a sua propria inversa */
    criptografar(str);
}

/* ---------- Funcoes auxiliares ---------- */

void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void lerLinha(char *destino, int tamanho) {
    if (fgets(destino, tamanho, stdin) != NULL) {
        /* Remove o '\n' que o fgets deixa no final, se existir */
        size_t len = strlen(destino);
        if (len > 0 && destino[len - 1] == '\n') {
            destino[len - 1] = '\0';
        }
    }
}

int ehBissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

int diasNoMes(int mes, int ano) {
    int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (mes == 2 && ehBissexto(ano)) {
        return 29;
    }
    if (mes >= 1 && mes <= 12) {
        return diasPorMes[mes - 1];
    }
    return 31; /* nao deveria acontecer, ja que o mes e validado antes */
}

int dataAnterior(struct Data *d1, struct Data *d2) {
    /* Retorna 1 se d1 for uma data anterior a d2 */
    if (d1->ano != d2->ano) {
        return d1->ano < d2->ano;
    }
    if (d1->mes != d2->mes) {
        return d1->mes < d2->mes;
    }
    return d1->dia < d2->dia;
}

void lerData(struct Data *data) {
    int valida;

    do {
        valida = 1;

        printf(COR_OPCAO "  Dia" COR_RESET ": ");
        scanf("%d", &data->dia);
        printf(COR_OPCAO "  Mes" COR_RESET ": ");
        scanf("%d", &data->mes);
        printf(COR_OPCAO "  Ano" COR_RESET ": ");
        scanf("%d", &data->ano);
        limparBufferEntrada();

        if (data->mes < 1 || data->mes > 12) {
            printf(COR_ERRO "  Mes invalido! Deve ser um numero entre 1 e 12." COR_RESET "\n");
            valida = 0;
        } else if (data->dia < 1 || data->dia > diasNoMes(data->mes, data->ano)) {
            printf(COR_ERRO "  Dia invalido para o mes/ano informado!" COR_RESET "\n");
            valida = 0;
        }

        if (!valida) {
            printf(COR_ERRO "  Digite a data novamente." COR_RESET "\n");
        }
    } while (!valida);
}

void lerDataEscritaValida(struct Data *dataEscrita, struct Data *dataNascimento) {
    int valida;

    do {
        valida = 1;

        printf("\n" COR_TITULO "-- Data de escrita do livro --" COR_RESET "\n");
        lerData(dataEscrita);

        if (dataAnterior(dataEscrita, dataNascimento)) {
            printf(COR_ERRO "A data de escrita nao pode ser anterior ao nascimento do autor! Tente novamente." COR_RESET "\n");
            valida = 0;
        }
    } while (!valida);
}

void liberarBiblioteca(struct LivroMagico **biblioteca) {
    int i;
    for (i = 0; i < MAX_LIVROS; i++) {
        if (biblioteca[i] != NULL) {
            free(biblioteca[i]);
            biblioteca[i] = NULL;
        }
    }
}

void habilitarCoresTerminal(void) {
    /* No Windows, o terminal precisa ser configurado para interpretar
       os codigos de cor ANSI corretamente (nao e necessario no Linux/Mac) */
#ifdef _WIN32
    HANDLE saida = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD modo = 0;
    GetConsoleMode(saida, &modo);
    SetConsoleMode(saida, modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}
