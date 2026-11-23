#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITENS 20

typedef struct {
    int id;
    char nome[50];
    char descricao[100];
    float preco;
    int estoque;
} Produto;

typedef struct {
    int id;
    char cliente[50];
    int quantidade;
    int produtos[MAX_ITENS];
} Venda;

/* FUNCOES DE LEITURA */

int lerInteiro(char mensagem[]) {
    int valor;

    printf("%s", mensagem);
    scanf("%d", &valor);
    getchar();

    return valor;
}

void lerTexto(char mensagem[], char texto[], int tamanho) {
    printf("%s", mensagem);
    fgets(texto, tamanho, stdin);
    texto[strcspn(texto, "\n")] = '\0';
}

/* GERAR IDS */

int gerarIdProduto() {
    FILE *arquivo;
    Produto p;
    int maior = 0;

    arquivo = fopen("produtos.dat", "rb");

    if (arquivo != NULL) {
        while (fread(&p, sizeof(Produto), 1, arquivo)) {
            if (p.id > maior) {
                maior = p.id;
            }
        }

        fclose(arquivo);
    }

    return maior + 1;
}

int gerarIdVenda() {
    FILE *arquivo;
    Venda v;
    int maior = 0;

    arquivo = fopen("vendas.dat", "rb");

    if (arquivo != NULL) {
        while (fread(&v, sizeof(Venda), 1, arquivo)) {
            if (v.id > maior) {
                maior = v.id;
            }
        }

        fclose(arquivo);
    }

    return maior + 1;
}

/* BUSCAR PRODUTO */

int buscarProduto(int id, Produto *p) {
    FILE *arquivo;

    arquivo = fopen("produtos.dat", "rb");

    if (arquivo == NULL) {
        return 0;
    }

    while (fread(p, sizeof(Produto), 1, arquivo)) {
        if (p->id == id) {
            fclose(arquivo);
            return 1;
        }
    }

    fclose(arquivo);
    return 0;
}

/* CADASTRAR PRODUTO */

void cadastrarProduto() {
    FILE *arquivo;
    Produto p;

    p.id = gerarIdProduto();

    printf("\n=== CADASTRAR PRODUTO ===\n");

    lerTexto("Nome: ", p.nome, 50);
    lerTexto("Descricao: ", p.descricao, 100);

    printf("Preco: ");
    scanf("%f", &p.preco);

    printf("Estoque: ");
    scanf("%d", &p.estoque);
    getchar();

    if (p.preco < 0 || p.estoque < 0) {
        printf("Valores invalidos!\n");
        return;
    }

    arquivo = fopen("produtos.dat", "ab");

    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo!\n");
        return;
    }

    fwrite(&p, sizeof(Produto), 1, arquivo);

    fclose(arquivo);

    printf("Produto cadastrado! ID: %d\n", p.id);
}

/* LISTAR PRODUTOS */

void listarProdutos() {
    FILE *arquivo;
    Produto p;

    arquivo = fopen("produtos.dat", "rb");

    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo!\n");
        return;
    }

    printf("\n=== PRODUTOS CADASTRADOS ===\n");

    while (fread(&p, sizeof(Produto), 1, arquivo)) {
        printf("\nID: %d\n", p.id);
        printf("Nome: %s\n", p.nome);
        printf("Descricao: %s\n", p.descricao);
        printf("Preco: R$ %.2f\n", p.preco);
        printf("Estoque: %d\n", p.estoque);
    }

    fclose(arquivo);
}

/* CONSULTAR PRODUTO */

void consultarProduto() {
    Produto p;
    int id;

    id = lerInteiro("Digite o ID do produto: ");

    if (buscarProduto(id, &p)) {
        printf("\nID: %d\n", p.id);
        printf("Nome: %s\n", p.nome);
        printf("Descricao: %s\n", p.descricao);
        printf("Preco: R$ %.2f\n", p.preco);
        printf("Estoque: %d\n", p.estoque);
    } else {
        printf("Produto nao encontrado!\n");
    }
}

/* ATUALIZAR PRODUTO */

void atualizarProduto() {
    FILE *arquivo;
    Produto p;
    int id;

    id = lerInteiro("ID do produto: ");

    arquivo = fopen("produtos.dat", "rb+");

    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo!\n");
        return;
    }

    while (fread(&p, sizeof(Produto), 1, arquivo)) {
        if (p.id == id) {
            lerTexto("Novo nome: ", p.nome, 50);
            lerTexto("Nova descricao: ", p.descricao, 100);

            printf("Novo preco: ");
            scanf("%f", &p.preco);

            printf("Novo estoque: ");
            scanf("%d", &p.estoque);
            getchar();

            if (p.preco < 0 || p.estoque < 0) {
                printf("Valores invalidos!\n");
                fclose(arquivo);
                return;
            }

            fseek(arquivo, -(long)sizeof(Produto), SEEK_CUR);
            fwrite(&p, sizeof(Produto), 1, arquivo);

            printf("Produto atualizado!\n");

            fclose(arquivo);
            return;
        }
    }

    printf("Produto nao encontrado!\n");
    fclose(arquivo);
}

/* EXCLUIR PRODUTO */

void excluirProduto() {
    FILE *arquivo, *temp;
    Produto p;
    int id, encontrou = 0;

    id = lerInteiro("ID do produto que deseja excluir: ");

    arquivo = fopen("produtos.dat", "rb");
    temp = fopen("temporario.dat", "wb");

    if (arquivo == NULL || temp == NULL) {
        printf("Erro ao abrir arquivos!\n");

        if (arquivo != NULL) fclose(arquivo);
        if (temp != NULL) fclose(temp);

        return;
    }

    while (fread(&p, sizeof(Produto), 1, arquivo)) {
        if (p.id == id) {
            encontrou = 1;
        } else {
            fwrite(&p, sizeof(Produto), 1, temp);
        }
    }

    fclose(arquivo);
    fclose(temp);

    if (!encontrou) {
        remove("temporario.dat");
        printf("Produto nao encontrado!\n");
        return;
    }

    remove("produtos.dat");
    rename("temporario.dat", "produtos.dat");

    printf("Produto excluido!\n");
}

/* CADASTRAR VENDA */

void cadastrarVenda() {
    FILE *arquivo;
    Venda v;
    Produto p;
    int i;

    v.id = gerarIdVenda();

    printf("\n=== CADASTRAR VENDA ===\n");

    lerTexto("Nome do cliente: ", v.cliente, 50);

    v.quantidade = lerInteiro("Quantidade de produtos: ");

    if (v.quantidade < 1 || v.quantidade > MAX_ITENS) {
        printf("Quantidade invalida!\n");
        return;
    }

    for (i = 0; i < v.quantidade; i++) {
        char mensagem[50];

        sprintf(mensagem, "ID do produto %d: ", i + 1);

        v.produtos[i] = lerInteiro(mensagem);

        if (!buscarProduto(v.produtos[i], &p)) {
            printf("Produto nao encontrado!\n");
            return;
        }

        if (p.estoque <= 0) {
            printf("Produto sem estoque!\n");
            return;
        }
    }

    arquivo = fopen("vendas.dat", "ab");

    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo!\n");
        return;
    }

    fwrite(&v, sizeof(Venda), 1, arquivo);

    fclose(arquivo);

    /* Diminuir o estoque dos produtos vendidos */

    for (i = 0; i < v.quantidade; i++) {
        FILE *produtos;

        produtos = fopen("produtos.dat", "rb+");

        if (produtos == NULL) {
            continue;
        }

        while (fread(&p, sizeof(Produto), 1, produtos)) {
            if (p.id == v.produtos[i]) {
                p.estoque--;

                fseek(produtos, -(long)sizeof(Produto), SEEK_CUR);
                fwrite(&p, sizeof(Produto), 1, produtos);

                break;
            }
        }

        fclose(produtos);
    }

    printf("Venda cadastrada! ID: %d\n", v.id);
}

/* LISTAR VENDAS */

void listarVendas() {
    FILE *arquivo;
    Venda v;

    arquivo = fopen("vendas.dat", "rb");

    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo!\n");
        return;
    }

    printf("\n=== VENDAS CADASTRADAS ===\n");

    while (fread(&v, sizeof(Venda), 1, arquivo)) {
        int i;

        printf("\nID da venda: %d\n", v.id);
        printf("Cliente: %s\n", v.cliente);
        printf("Produtos:\n");

        for (i = 0; i < v.quantidade; i++) {
            printf("ID do produto: %d\n", v.produtos[i]);
        }
    }

    fclose(arquivo);
}

/* CONSULTAR VENDA */

void consultarVenda() {
    FILE *arquivo;
    Venda v;
    int id, encontrou = 0, i;

    id = lerInteiro("ID da venda: ");

    arquivo = fopen("vendas.dat", "rb");

    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo!\n");
        return;
    }

    while (fread(&v, sizeof(Venda), 1, arquivo)) {
        if (v.id == id) {
            encontrou = 1;

            printf("\nVenda: %d\n", v.id);
            printf("Cliente: %s\n", v.cliente);

            for (i = 0; i < v.quantidade; i++) {
                printf("Produto ID: %d\n", v.produtos[i]);
            }

            break;
        }
    }

    if (!encontrou) {
        printf("Venda nao encontrada!\n");
    }

    fclose(arquivo);
}

/* ATUALIZAR VENDA */

void atualizarVenda() {
    FILE *arquivo;
    Venda v;
    int id;

    id = lerInteiro("ID da venda: ");

    arquivo = fopen("vendas.dat", "rb+");

    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo!\n");
        return;
    }

    while (fread(&v, sizeof(Venda), 1, arquivo)) {
        if (v.id == id) {
            lerTexto("Novo nome do cliente: ", v.cliente, 50);

            fseek(arquivo, -(long)sizeof(Venda), SEEK_CUR);
            fwrite(&v, sizeof(Venda), 1, arquivo);

            printf("Venda atualizada!\n");

            fclose(arquivo);
            return;
        }
    }

    printf("Venda nao encontrada!\n");

    fclose(arquivo);
}

/* EXCLUIR VENDA */

void excluirVenda() {
    FILE *arquivo, *temp;
    Venda v;
    int id, encontrou = 0;

    id = lerInteiro("ID da venda que deseja excluir: ");

    arquivo = fopen("vendas.dat", "rb");
    temp = fopen("temporario.dat", "wb");

    if (arquivo == NULL || temp == NULL) {
        printf("Erro ao abrir arquivos!\n");

        if (arquivo != NULL) fclose(arquivo);
        if (temp != NULL) fclose(temp);

        return;
    }

    while (fread(&v, sizeof(Venda), 1, arquivo)) {
        if (v.id == id) {
            encontrou = 1;
        } else {
            fwrite(&v, sizeof(Venda), 1, temp);
        }
    }

    fclose(arquivo);
    fclose(temp);

    if (!encontrou) {
        remove("temporario.dat");
        printf("Venda nao encontrada!\n");
        return;
    }

    remove("vendas.dat");
    rename("temporario.dat", "vendas.dat");

    printf("Venda excluida!\n");
}

/* RELATORIO DE VENDAS COM NOMES DOS PRODUTOS */

void relatorioVendas() {
    FILE *arquivo;
    Venda v;
    Produto p;
    int i;

    arquivo = fopen("vendas.dat", "rb");

    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo!\n");
        return;
    }

    printf("\n=== RELATORIO DE VENDAS ===\n");

    while (fread(&v, sizeof(Venda), 1, arquivo)) {
        printf("\nVenda: %d\n", v.id);
        printf("Cliente: %s\n", v.cliente);

        for (i = 0; i < v.quantidade; i++) {
            if (buscarProduto(v.produtos[i], &p)) {
                printf("Produto: %s\n", p.nome);
            } else {
                printf("Produto nao encontrado!\n");
            }
        }
    }

    fclose(arquivo);
}

/* MENU PRINCIPAL */

int main() {
    FILE *arquivo;
    int opcao;

    arquivo = fopen("produtos.dat", "ab");
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo de produtos!\n");
        return 1;
    }
    fclose(arquivo);

    arquivo = fopen("vendas.dat", "ab");
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo de vendas!\n");
        return 1;
    }
    fclose(arquivo);

    do {
        printf("\n========== SISTEMA DE VENDAS ==========\n");
        printf("1 - Cadastrar produto\n");
        printf("2 - Listar produtos\n");
        printf("3 - Consultar produto\n");
        printf("4 - Atualizar produto\n");
        printf("5 - Excluir produto\n");
        printf("6 - Cadastrar venda\n");
        printf("7 - Listar vendas\n");
        printf("8 - Consultar venda\n");
        printf("9 - Atualizar venda\n");
        printf("10 - Excluir venda\n");
        printf("11 - Relatorio de vendas\n");
        printf("0 - Sair\n");

        opcao = lerInteiro("Escolha: ");

        switch (opcao) {
            case 1:
                cadastrarProduto();
                break;

            case 2:
                listarProdutos();
                break;

            case 3:
                consultarProduto();
                break;

            case 4:
                atualizarProduto();
                break;

            case 5:
                excluirProduto();
                break;

            case 6:
                cadastrarVenda();
                break;

            case 7:
                listarVendas();
                break;

            case 8:
                consultarVenda();
                break;

            case 9:
                atualizarVenda();
                break;

            case 10:
                excluirVenda();
                break;

            case 11:
                relatorioVendas();
                break;

            case 0:
                printf("Encerrando o sistema...\n");
                break;

            default:
                printf("Opcao invalida!\n");
        }

    } while (opcao != 0);

    return 0;
}