# COOL Compiler — TP02 Compiladores

Repositório do TP02 de Compiladores para implementação do analisador léxico da linguagem COOL.

## Ambiente

```text
Windows 11
└── WSL2
    └── Ubuntu
        └── Docker Desktop
            └── Ubuntu 16.04
                ├── g++
                ├── make
                ├── csh
                ├── sharutils / uudecode
                ├── flex
                ├── bison
                └── /var/tmp/cool
```

## Primeira configuração

1. Instale WSL2, Ubuntu, Docker Desktop e Git.
2. Habilite a integração WSL do Docker Desktop.
3. Clone o repositório preferencialmente em `~/projects/`.
4. Baixe `x86_64.u` do Canvas e coloque em `assets/x86_64.u`.
5. Construa o ambiente:

```bash
docker compose build
```

6. Instale a infraestrutura COOL:

```bash
docker compose run --rm cool ./scripts/setup-cool.sh
```

7. Valide:

```bash
docker compose run --rm cool ./scripts/verify-env.sh
```

## Gerando o PA2

```bash
docker compose run --rm cool
```

Dentro do container:

```bash
mkdir -p /workspace/PA2
cd /workspace/PA2
make -f /var/tmp/cool/assignments/PA2/Makefile
make lexer
exit
```

## Testes

Na raiz do projeto, passe o arquivo que deseja testar, sem precisar entrar no container:

```bash
docker compose run --rm cool ./scripts/test.sh tests/codes/teste-lexico.cl
```

Os caminhos relativos são resolvidos a partir do diretório de execução no container
(`/workspace` por padrão, que corresponde à raiz do projeto). Para caminhos com
espaços, use aspas. Sem argumento, o script executa `PA2/test.cl`:

```bash
docker compose run --rm cool ./scripts/test.sh
```

## Git

Use branches de feature e Pull Requests. Não trabalhe diretamente em `main`.

Branches iniciais sugeridas:

```text
feature/core-scanner
feature/strings
feature/comments-tests
```

Consulte `docs/CONTRIBUTING.md` e `docs/ENVIRONMENT.md`.
