# Ambiente de Desenvolvimento

## Objetivo

Garantir que todos executem o trabalho em ambiente equivalente.

## Arquitetura

```text
Windows 11
└── WSL2
    └── Ubuntu
        └── ~/projects/cool-compiler
            ├── Git / editor / agente
            └── Docker Compose
                └── Ubuntu 16.04
                    ├── /workspace
                    └── /var/tmp/cool
```

## Volumes

- `/workspace`: raiz do repositório.
- `/var/tmp/cool`: volume Docker persistente e local a cada integrante.

## Regra

Código, documentação e scripts devem ficar em `/workspace` e ser versionados.
A infraestrutura oficial da disciplina permanece em `/var/tmp/cool`.

`assets/x86_64.u` é local e não deve ser versionado.
