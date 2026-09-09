# Projeto 1 - Processamento de Imagens

Projeto da disciplina de Computacao Visual para carregar uma imagem, converter para escala de cinza, analisar o histograma, equalizar a imagem e salvar o resultado.

## Integrantes

- Alexandre Eiji Tomimura Carvalho: 10371680
- Joao Pedro Pioltini de Oliveira: 10425643
- Matheus Veiga Bacetic Joaquim: 10425638

## Contribuicoes

As contribuicoes foram divididas igualmente entre os integrantes do grupo.

- Alexandre Eiji Tomimura Carvalho: implementacao e revisao da conversao para escala de cinza, apoio na validacao do carregamento da imagem e revisao da documentacao.
- Joao Pedro Pioltini de Oliveira: organizacao da base do projeto, ajustes de compilacao, integracao com SDL3/SDL_image/SDL_ttf e revisao dos testes de execucao.
- Matheus Veiga Bacetic Joaquim: implementacao e revisao do histograma, equalizacao, calculos de media/desvio padrao e apoio na interface de controle.

## Funcionalidades

- Carregamento de imagem por linha de comando.
- Conversao para escala de cinza usando luminancia.
- Histograma com 256 intensidades.
- Media, desvio padrao e classificacoes de brilho/contraste.
- Janela secundaria com SDL_ttf, histograma e botoes.
- Botao `Equalizar` / `Ver original`.
- Botao `Resolucao original` / `1024x768`.
- Tecla `S` para salvar `output_image.png`.

## Bibliotecas

- SDL3
- SDL3_image
- SDL3_ttf

Fonte usada pela interface:

- `assets/fonts/Roboto-Regular.ttf`

## Ambiente Usado No Desenvolvimento

- Sistema operacional: macOS Darwin 25.6.0 arm64
- Compilador testado: Apple clang 21.0.0
- SDL3: 3.4.16
- SDL3_image: 3.4.6
- SDL3_ttf: 3.2.2

## Ambientes de Correcao Previstos

O projeto foi escrito em linguagem C e o `makefile` usa `gcc` como compilador padrao.

- Windows 10/11 com GCC 15.1.0.
- WSL Ubuntu 26.04 com GCC 15.2.0.

No macOS e no WSL, o projeto usa `pkg-config` para obter os caminhos de include e linkagem.
No Windows com MinGW, o `makefile` usa a variavel `SDL_DIR` para encontrar as bibliotecas da SDL.

## Dependencias

No macOS com Homebrew:

```sh
brew install pkg-config sdl3 sdl3_image sdl3_ttf
```

No Linux/WSL, instale pacotes equivalentes de desenvolvimento para SDL3, SDL3_image, SDL3_ttf, `pkg-config` e `gcc`.

No Windows com MinGW, deixe as bibliotecas da SDL no caminho esperado pelo `makefile`:

```text
d:/dev/compvis/libs/SDL3
```

Se estiverem em outro local, informe o caminho ao compilar:

```sh
mingw32-make SDL_DIR=c:/caminho/para/SDL3
```

## Compilacao

Verificar dependencias:

```sh
make check-deps
```

Compilar:

```sh
make
```

Caso precise informar explicitamente a versao do GCC:

```sh
make CC=gcc-15
```

No Windows com MinGW:

```sh
mingw32-make
```

Ou, informando explicitamente o compilador:

```sh
mingw32-make CC=gcc
```

Limpar arquivos gerados:

```sh
make clean
```

## Execucao

Rodar com a imagem de exemplo:

```sh
./programa assets/images/kodim23.png
```

No Windows:

```sh
programa.exe assets/images/kodim23.png
```

Ou:

```sh
make run
```

Rodar com outra imagem:

```sh
./programa caminho/da/imagem.png
```

## Controles

- `Equalizar`: equaliza o histograma da imagem atual em escala de cinza.
- `Ver original`: restaura a copia original em escala de cinza sem recarregar do disco.
- `Resolucao original`: mostra a janela principal no tamanho original da imagem.
- `1024x768`: volta a janela principal ao tamanho padrao.
- `S`: salva a imagem ativa em `output_image.png`.

## Estrutura

- `src/main.c`: codigo-fonte principal.
- `assets/images/`: imagens de teste.
- `assets/fonts/`: fonte usada pela interface SDL_ttf.
- `makefile`: regras de compilacao, execucao e limpeza.

## Observacoes

Os thresholds de classificacao foram definidos no codigo como constantes para facilitar justificativa e ajuste no relatorio:

- brilho escuro: media menor que 85
- brilho claro: media maior que 170
- contraste baixo: desvio padrao menor que 40
- contraste alto: desvio padrao maior que 80
