# Fontes

Esta pasta contem a fonte usada pela interface SDL_ttf.

- `Roboto-Regular.ttf`: fonte Roboto obtida do Google Fonts.
- `OFL.txt`: licenca SIL Open Font License da fonte.

O programa carrega a fonte por caminho relativo para nao depender de fontes instaladas no sistema operacional.

A fonte e localizada a partir da pasta do executavel via SDL_GetBasePath, independentemente da pasta atual do terminal.
