# Projeto 1 - Processamento de Imagens

Projeto da disciplina de Computacao Visual.

## Estrutura

- `src/main.c`: codigo-fonte principal do projeto.
- `assets/images/`: imagens usadas para testes locais.
- `assets/fonts/`: fonte usada pelos textos da interface.
- `makefile`: comandos de compilacao e limpeza.

## Dependencias

- Compilador C com suporte a C23, como `gcc`.
- SDL3.
- SDL3_image.
- SDL3_ttf.
- pkg-config/pkgconf.

## Compilacao

No macOS com Homebrew:

```sh
make CC=gcc-15
```

No Linux/WSL, se `gcc` ja estiver configurado:

```sh
make
```

Para verificar se as bibliotecas SDL estao acessiveis:

```sh
make check-deps
```

Para executar:

```sh
make run
```

## Observacao

O codigo atual ainda e uma base inicial adaptada do exemplo `05-filter_image` do repositorio da disciplina. Ele ainda precisa ser alterado para receber o caminho da imagem por linha de comando e implementar as funcionalidades obrigatorias do Projeto 1.
