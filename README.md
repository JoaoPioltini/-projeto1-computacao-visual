# Projeto 1 - Processamento de Imagens

Projeto da disciplina de Computação Visual para carregar uma imagem, converter para escala de cinza, analisar o histograma, equalizar a imagem e salvar o resultado.

## Integrantes

- Alexandre Eiji Tomimura Carvalho: 10371680
- João Pedro Pioltini de Oliveira: 10425643
- Matheus Veiga Bacetic Joaquim: 10425638

## Contribuições

As contribuições foram divididas igualmente entre os integrantes do grupo.

- Alexandre Eiji Tomimura Carvalho: implementação e revisão da conversão para escala de cinza, apoio na validação do carregamento da imagem e revisão da documentação.
- João Pedro Pioltini de Oliveira: organização da base do projeto, ajustes de compilação, integração com SDL3/SDL_image/SDL_ttf e revisão dos testes de execução.
- Matheus Veiga Bacetic Joaquim: revisão e melhoria da interface SDL, com ajustes nos controles da janela secundária, centralização dos textos dos botões e renderização da imagem com preservação da proporção ao alternar entre 1024x768 e resolução original.

## Funcionalidades

- Carregamento de imagem por linha de comando.
- Conversão para escala de cinza usando luminância.
- Histograma com 256 intensidades.
- Média, desvio padrão e classificações de brilho/contraste.
- Janela secundária com SDL_ttf, histograma e botões.
- Botão `Equalizar` / `Ver original`.
- Botão `Resolução original` / `1024x768`.
- Preservação da proporção da imagem durante o redimensionamento.
- Centralização automática dos textos nos botões da interface.
- Tecla `S` para salvar `output_image.png`.

## Bibliotecas

- SDL3
- SDL3_image
- SDL3_ttf

Fonte usada pela interface:

- `assets/fonts/Roboto-Regular.ttf`

## Ambiente Usado no Desenvolvimento

- Sistema operacional: macOS Darwin 25.6.0 arm64.
- Compilador testado: Apple clang 21.0.0.
- SDL3: 3.4.16.
- SDL3_image: 3.4.6.
- SDL3_ttf: 3.2.2.

## Ambientes de Correção Previstos

O projeto foi escrito em linguagem C e o `makefile` usa `gcc` como compilador padrão.

- Windows 10/11 com GCC 15.1.0.
- WSL Ubuntu 26.04 com GCC 15.2.0.

No macOS e no WSL, o projeto usa `pkg-config` para obter os caminhos de include e linkagem.
No Windows com MinGW, o `makefile` usa a variável `SDL_DIR` para encontrar as bibliotecas da SDL.

## Dependências

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

## Compilação

Verificar dependências:

```sh
make check-deps
```

Compilar:

```sh
make
```

Caso precise informar explicitamente a versão do GCC:

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

## Execução

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
- `Ver original`: restaura a cópia original em escala de cinza sem recarregar do disco.
- `Resolução original`: mostra a janela principal no tamanho original da imagem.
- `1024x768`: volta a janela principal ao tamanho padrão.
- `S`: salva a imagem ativa em `output_image.png`.

## Estrutura

- `src/main.c`: código-fonte principal.
- `assets/images/`: imagens de teste.
- `assets/fonts/`: fonte usada pela interface SDL_ttf.
- `makefile`: regras de compilação, execução e limpeza.

## Observações

Os thresholds de classificação foram definidos no código como constantes para facilitar a justificativa e o ajuste no relatório:

- brilho escuro: média menor que 85.
- brilho claro: média maior que 170.
- contraste baixo: desvio padrão menor que 40.
- contraste alto: desvio padrão maior que 80.
