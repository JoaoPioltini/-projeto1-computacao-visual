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

## Compilação e execução

`make` compila o projeto, mas não instala as dependências. Execute na raiz do
repositório. O código usa C17.

### Windows (GCC/MinGW de 64 bits)

Instale GCC, GNU Make e os pacotes de desenvolvimento **MinGW x86_64** de SDL3,
SDL3_image e SDL3_ttf. Pacotes para Visual C++ não servem para esse build.

O makefile procura as bibliotecas nesta ordem:

1. Pasta informada por `SDL_DIR`, contendo as três bibliotecas.
2. Pacotes locais em `.local/SDL3-*/x86_64-w64-mingw32`,
   `.local/SDL3_image-*/x86_64-w64-mingw32` e
   `.local/SDL3_ttf-*/x86_64-w64-mingw32`.
3. Pasta da disciplina: `d:/dev/compvis/libs/SDL3`.
4. Bibliotecas registradas no `pkg-config`, como em um ambiente MSYS2 configurado.

Para montar uma pasta única, reúna o conteúdo de `include`, `lib` e `bin` dos
três pacotes MinGW de 64 bits, preservando as subpastas dos cabeçalhos:

```text
SDL3/
  include/SDL3/SDL.h
  include/SDL3_image/SDL_image.h
  include/SDL3_ttf/SDL_ttf.h
  lib/                         # bibliotecas de importação dos três pacotes
  bin/                         # DLLs dos três pacotes e suas dependências
```

Com GCC e Make no PATH e as bibliotecas na pasta da disciplina:

```powershell
mingw32-make
.\programa.exe assets/images/kodim23.png
```

Se GNU Make estiver instalado como `make`, use `make` no lugar de `mingw32-make`.
Para outra pasta de bibliotecas, prefira um caminho sem espaços:

```powershell
mingw32-make SDL_DIR=C:/libs/SDL3
```

As DLLs dos SDKs detectados são copiadas para a raiz do projeto. Quando o build
usa `pkg-config`, as DLLs devem estar no PATH do ambiente.

Na configuração portátil preparada neste computador:

```powershell
.\.local\w64devkit\bin\mingw32-make.exe
.\programa.exe assets/images/kodim23.png
```

O GCC em `.local/w64devkit/bin` é detectado automaticamente. A pasta `.local`
não é versionada e não acompanha um clone do repositório.

### Linux/WSL e macOS

Instale GCC (ou Clang), GNU Make, `pkg-config` e os pacotes de desenvolvimento
SDL3, SDL3_image e SDL3_ttf. As bibliotecas devem disponibilizar os módulos
`sdl3`, `sdl3-image` e `sdl3-ttf` para o `pkg-config`.

No macOS com Homebrew:

```sh
brew install pkg-config sdl3 sdl3_image sdl3_ttf
```

No Linux/WSL, os nomes dos pacotes dependem da distribuição. O WSL precisa de
suporte a janelas gráficas, como WSLg, para executar a interface.

```sh
make check-deps
make
./programa assets/images/kodim23.png
```

Para selecionar o compilador: `make CC=gcc-15` ou `make CC=clang`.

### Comandos adicionais

```sh
make run
make run IMAGE=caminho/da/imagem.png
make clean
```

`make clean` remove apenas o executável, preservando imagens salvas e dependências.
Mantenha `assets` ao lado do executável: a fonte é localizada a partir dessa pasta,
independentemente da pasta atual do terminal. A imagem de entrada e a saída
`output_image.png` usam caminhos relativos à pasta atual do terminal.

### Validação local no Windows

Build validado com GCC 16.2.0 e SDL3 3.4.16, SDL3_image 3.4.6 e SDL3_ttf 3.2.2.
As versões GCC 15.1.0/15.2.0 previstas para correção e a execução no WSL ainda
não foram testadas neste computador.

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
