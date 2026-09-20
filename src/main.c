// Copyright (c) 2026 Andre Kishimoto - https://kishimoto.com.br/
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------
// Projeto 1 - Processamento de imagens
// Baseado no exemplo fornecido pelo Prof. Andre Kishimoto.
// Integrantes:
// - Alexandre Eiji Tomimura Carvalho
// - Joao Pedro Pioltini de Oliveira
// - Matheus Veiga Bacetic Joaquim
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

//------------------------------------------------------------------------------
// Custom types, structs, constants, etc.
//------------------------------------------------------------------------------
static const char *WINDOW_TITLE = "Projeto 1 - Imagem";
static const char *SECONDARY_WINDOW_TITLE = "Histograma";
static const char *FONT_FILENAME = "assets/fonts/Roboto-Regular.ttf";
static const char *OUTPUT_FILENAME = "output_image.png";

enum constants
{
  DEFAULT_WINDOW_WIDTH = 1024,
  DEFAULT_WINDOW_HEIGHT = 768,
  SECONDARY_WINDOW_WIDTH = 500,
  SECONDARY_WINDOW_HEIGHT = 400,
  HISTOGRAM_SIZE = 256,
  FONT_SIZE = 16,
};

// Limiares definidos pelo grupo para classificacao de luminosidade e contraste.
static const double BRIGHTNESS_DARK_THRESHOLD = 85.0;
static const double BRIGHTNESS_LIGHT_THRESHOLD = 170.0;
static const double CONTRAST_LOW_THRESHOLD = 40.0;
static const double CONTRAST_HIGH_THRESHOLD = 80.0;

typedef struct MyWindow MyWindow;
struct MyWindow
{
  SDL_Window *window;
  SDL_Renderer *renderer;
};

typedef struct MyImage MyImage;
struct MyImage
{
  SDL_Surface *surface;
  SDL_Texture *texture;
  SDL_FRect rect;
};

typedef struct ImageStats ImageStats;
struct ImageStats
{
  Uint32 histogram[HISTOGRAM_SIZE];
  Uint64 total_pixels;
  double mean;
  double stddev;
};

typedef struct Button Button;
struct Button
{
  SDL_FRect rect;
  const char *text;
  bool hovered;
  bool pressed;
};

//------------------------------------------------------------------------------
// Globals (argh!)
//------------------------------------------------------------------------------
static MyWindow g_window = { .window = NULL, .renderer = NULL };
static MyWindow g_secondaryWindow = {
  .window = NULL,
  .renderer = NULL
};

static MyImage g_image = {
  .surface = NULL,
  .texture = NULL,
  .rect = { .x = 0.0f, .y = 0.0f, .w = 0.0f, .h = 0.0f }
};

static SDL_Surface *g_originalGraySurface = NULL;
static TTF_Font *g_font = NULL;
static ImageStats g_stats = { 0 };

static Button g_equalizeButton = {
  .rect = { .x = 40.0f, .y = 292.0f, .w = 190.0f, .h = 42.0f },
  .text = "Equalizar",
  .hovered = false,
  .pressed = false
};

static Button g_resolutionButton = {
  .rect = { .x = 260.0f, .y = 292.0f, .w = 200.0f, .h = 42.0f },
  .text = "Resolucao original",
  .hovered = false,
  .pressed = false
};

static bool g_isEqualized = false;
static bool g_showOriginalResolution = false;

//------------------------------------------------------------------------------
// Function declaration
//------------------------------------------------------------------------------
static bool MyWindow_initialize(MyWindow *window, const char *title, int width, int height, SDL_WindowFlags window_flags);
static void MyWindow_destroy(MyWindow *window);
static void MyImage_destroy(MyImage *image);
static bool MyImage_update_texture_with_surface(MyImage* image, SDL_Renderer *renderer, SDL_Surface *surface);
static bool MyImage_restore_texture(MyImage* image, SDL_Renderer *renderer);
static bool MyImage_is_grayscale(MyImage *image);
static bool MyImage_convert_to_grayscale(MyImage *image);
static bool MyImage_copy_surface_to_active(MyImage *image, SDL_Renderer *renderer, SDL_Surface *source);
static bool MyImage_calculate_histogram(MyImage *image, Uint32 histogram[HISTOGRAM_SIZE], Uint64 *total_pixels);
static bool MyImage_equalize(MyImage *image, SDL_Renderer *renderer);

/**
 * Carrega a imagem indicada no parâmetro `filename` e a converte para o formato
 * RGBA32, eliminando dependência do formato original da imagem. A imagem
 * carregada é armazenada em output_image.
 * Caso ocorra algum erro no processo, a função retorna false.
 */
static bool load_rgba32(const char *filename, SDL_Renderer *renderer, MyImage *output_image);
static bool copy_original_grayscale(MyImage *image);
static void calculate_histogram_analysis(ImageStats *stats);
static const char *classify_brightness(double mean);
static const char *classify_contrast(double stddev);
static bool update_image_stats(void);
static bool restore_original_grayscale(void);
static bool save_current_image(void);

static SDL_AppResult initialize(void);
static void shutdown(void);
static void update_main_window_size_and_position(void);
static void render_text(SDL_Renderer *renderer, const char *text, float x, float y, SDL_Color color);
static void render_button(SDL_Renderer *renderer, Button *button);
static void render_histogram(SDL_Renderer *renderer, const SDL_FRect *rect, const Uint32 histogram[HISTOGRAM_SIZE]);
static void render_main_window(void);
static void render_secondary_window(void);
static void render(void);
static bool Button_contains(Button *button, float x, float y);
static bool Button_handle_event(Button *button, const SDL_Event *event);
static void toggle_equalization(void);
static void toggle_resolution(void);
static void loop(void);

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool MyWindow_initialize(MyWindow *window, const char *title, int width, int height, SDL_WindowFlags window_flags)
{
  SDL_Log("\tMyWindow_initialize(%s, %d, %d)", title, width, height);

  if (!window)
  {
    SDL_Log("\t\t*** Erro: Janela/renderizador inválidos (window == NULL).");
    return false;
  }

  return SDL_CreateWindowAndRenderer(title, width, height, window_flags, &window->window, &window->renderer);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void MyWindow_destroy(MyWindow *window)
{
  SDL_Log(">>> MyWindow_destroy()");

  if (!window)
  {
    SDL_Log("\t*** Erro: Janela/renderizador inválidos (window == NULL).");
    SDL_Log("<<< MyWindow_destroy()");
    return;
  }

  SDL_Log("\tDestruindo MyWindow->renderer...");
  SDL_DestroyRenderer(window->renderer);
  window->renderer = NULL;

  SDL_Log("\tDestruindo MyWindow->window...");
  SDL_DestroyWindow(window->window);
  window->window = NULL;

  SDL_Log("<<< MyWindow_destroy()");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void MyImage_destroy(MyImage *image)
{
  SDL_Log(">>> MyImage_destroy()");

  if (!image)
  {
    SDL_Log("\t*** Erro: Imagem inválida (image == NULL).");
    SDL_Log("<<< MyImage_destroy()");
    return;
  }

  if (image->texture)
  {
    SDL_Log("\tDestruindo MyImage->texture...");
    SDL_DestroyTexture(image->texture);
    image->texture = NULL;
  }

  if (image->surface)
  {
    SDL_Log("\tDestruindo MyImage->surface...");
    SDL_DestroySurface(image->surface);
    image->surface = NULL;
  }

  SDL_Log("\tRedefinindo MyImage->rect...");
  image->rect.x = image->rect.y = image->rect.w = image->rect.h = 0.0f;

  SDL_Log("<<< MyImage_destroy()");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool MyImage_update_texture_with_surface(MyImage* image, SDL_Renderer *renderer, SDL_Surface *surface)
{
  SDL_Log(">>> MyImage_update_texture_with_surface()");

  if (!image)
  {
    SDL_Log("\t*** Erro: Imagem inválida (image == NULL).");
    SDL_Log("<<< MyImage_update_texture_with_surface()");
    return false;
  }

  if (!renderer)
  {
    SDL_Log("\t*** Erro: Renderer inválido (renderer == NULL).");
    SDL_Log("<<< MyImage_update_texture_with_surface()");
    return false;
  }

  if (!surface)
  {
    SDL_Log("\t*** Erro: Superfície inválida (surface == NULL).");
    SDL_Log("<<< MyImage_update_texture_with_surface()");
    return false;
  }

  SDL_DestroyTexture(image->texture);

  image->texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!image->texture)
  {
    SDL_Log("\t*** Erro ao criar textura: %s", SDL_GetError());
    SDL_Log("<<< MyImage_update_texture_with_surface()");
    return false;
  }

  SDL_Log("\tObtendo dimensões da textura...");
  SDL_GetTextureSize(image->texture, &image->rect.w, &image->rect.h);

  SDL_Log("<<< MyImage_update_texture_with_surface()");
  return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool MyImage_restore_texture(MyImage* image, SDL_Renderer *renderer)
{
  SDL_Log(">>> MyImage_restore_texture()");
  
  if (!MyImage_update_texture_with_surface(image, renderer, image->surface))
  {
    SDL_Log("\t*** Erro ao restaurar a textura da imagem.");
    return false;
  }

  SDL_Log("<<< MyImage_restore_texture()");
  return true;  
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool load_rgba32(const char *filename, SDL_Renderer *renderer, MyImage *output_image)
{
  SDL_Log(">>> load_rgba32(\"%s\")", filename);

  if (!filename)
  {
    SDL_Log("\t*** Erro: Nome do arquivo inválido (filename == NULL).");
    SDL_Log("<<< load_rgba32(\"%s\")", filename);
    return false;
  }

  if (!renderer)
  {
    SDL_Log("\t*** Erro: Renderer inválido (renderer == NULL).");
    SDL_Log("<<< load_rgba32(\"%s\")", filename);
    return false;
  }

  if (!output_image)
  {
    SDL_Log("\t*** Erro: Imagem de saída inválida (output_image == NULL).");
    SDL_Log("<<< load_rgba32(\"%s\")", filename);
    return false;
  }

  MyImage_destroy(output_image);

  SDL_Log("\tCarregando imagem \"%s\" em uma superfície...", filename);
  SDL_Surface *surface = IMG_Load(filename);
  if (!surface)
  {
    SDL_Log("\t*** Erro ao carregar a imagem: %s", SDL_GetError());
    SDL_Log("<<< load_rgba32(\"%s\")", filename);
    return false;
  }

  SDL_Log("\tConvertendo superfície para formato RGBA32...");
  output_image->surface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
  SDL_DestroySurface(surface);
  if (!output_image->surface)
  {
    SDL_Log("\t*** Erro ao converter superfície para formato RGBA32: %s", SDL_GetError());
    SDL_Log("<<< load_rgba32(\"%s\")", filename);
    return false;
  }

  SDL_Log("\tCriando textura a partir da superfície...");
  if (!MyImage_update_texture_with_surface(output_image, renderer, output_image->surface))
  {
    SDL_Log("\t*** Erro ao criar textura.");
    SDL_Log("<<< load_rgba32(\"%s\")", filename);
    return false;
  }

  SDL_Log("<<< load_rgba32(\"%s\")", filename);
  return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool MyImage_is_grayscale(MyImage *image)
{
  if (!image || !image->surface)
  {
    SDL_Log("*** Erro: Imagem inválida.");
    return false;
  }

  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(image->surface->format);
  Uint32 *pixels = (Uint32 *)image->surface->pixels;

  Uint8 r = 0;
  Uint8 g = 0;
  Uint8 b = 0;

  SDL_LockSurface(image->surface);

  for (int row = 0; row < image->surface->h; ++row)
  {
    for (int col = 0; col < image->surface->w; ++col)
    {
      int index = row * image->surface->w + col;

      SDL_GetRGB(pixels[index], format, NULL, &r, &g, &b);
      if (r != g || g != b)
      {
        SDL_UnlockSurface(image->surface);
        return false;
      }
    }
  }

  SDL_UnlockSurface(image->surface);
  return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool MyImage_convert_to_grayscale(MyImage *image)
{
  if (!image || !image->surface)
  {
    SDL_Log("*** Erro: Imagem inválida.");
    return false;
  }

  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(image->surface->format);
  Uint32 *pixels = (Uint32 *)image->surface->pixels;

  Uint8 r = 0;
  Uint8 g = 0;
  Uint8 b = 0;

  SDL_LockSurface(image->surface);

  for (int row = 0; row < image->surface->h; ++row)
  {
    for (int col = 0; col < image->surface->w; ++col)
    {
      int index = row * image->surface->w + col;

      SDL_GetRGB(pixels[index], format, NULL, &r, &g, &b);

      Uint8 y = (Uint8)(0.2125f * r + 0.7154f * g + 0.0721f * b);
      pixels[index] = SDL_MapRGB(format, NULL, y, y, y);
    }
  }

  SDL_UnlockSurface(image->surface);
  return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool copy_original_grayscale(MyImage *image)
{
  if (!image || !image->surface)
  {
    SDL_Log("*** Erro: Imagem invalida para copia.");
    return false;
  }

  SDL_DestroySurface(g_originalGraySurface);
  g_originalGraySurface = SDL_DuplicateSurface(image->surface);
  if (!g_originalGraySurface)
  {
    SDL_Log("*** Erro ao copiar superficie original em cinza: %s", SDL_GetError());
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool MyImage_copy_surface_to_active(MyImage *image, SDL_Renderer *renderer, SDL_Surface *source)
{
  if (!image || !renderer || !source)
  {
    SDL_Log("*** Erro: Parametros invalidos para copiar superficie ativa.");
    return false;
  }

  SDL_Surface *copy = SDL_DuplicateSurface(source);
  if (!copy)
  {
    SDL_Log("*** Erro ao duplicar superficie: %s", SDL_GetError());
    return false;
  }

  SDL_DestroySurface(image->surface);
  image->surface = copy;

  return MyImage_update_texture_with_surface(image, renderer, image->surface);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool MyImage_calculate_histogram(
    MyImage *image,
    Uint32 histogram[HISTOGRAM_SIZE],
    Uint64 *total_pixels)
{
  if (!image || !image->surface || !histogram || !total_pixels)
  {
    SDL_Log("*** Erro: Parametros invalidos para calculo do histograma.");
    return false;
  }

  memset(histogram, 0, sizeof(Uint32) * HISTOGRAM_SIZE);
  *total_pixels = 0;

  const SDL_PixelFormatDetails *format =
      SDL_GetPixelFormatDetails(image->surface->format);

  if (!format)
  {
    SDL_Log("*** Erro ao obter formato de pixels: %s", SDL_GetError());
    return false;
  }

  if (!SDL_LockSurface(image->surface))
  {
    SDL_Log("*** Erro ao bloquear superficie: %s", SDL_GetError());
    return false;
  }

  for (int row = 0; row < image->surface->h; ++row)
  {
    Uint32 *row_pixels =
        (Uint32 *)((Uint8 *)image->surface->pixels +
                   row * image->surface->pitch);

    for (int col = 0; col < image->surface->w; ++col)
    {
      Uint8 r = 0;
      Uint8 g = 0;
      Uint8 b = 0;

      SDL_GetRGB(
          row_pixels[col],
          format,
          NULL,
          &r,
          &g,
          &b);

      // A imagem ja esta em escala de cinza, portanto R = G = B.
      // R representa diretamente a intensidade do pixel.
      histogram[r]++;
    }
  }

  SDL_UnlockSurface(image->surface);

  *total_pixels =
      (Uint64)image->surface->w *
      (Uint64)image->surface->h;

  return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void calculate_histogram_analysis(ImageStats *stats)
{
  if (!stats)
    return;

  stats->mean = 0.0;
  stats->stddev = 0.0;

  if (stats->total_pixels == 0)
    return;

  double sum = 0.0;

  for (int i = 0; i < HISTOGRAM_SIZE; ++i)
  {
    sum += (double)i * (double)stats->histogram[i];
  }

  stats->mean = sum / (double)stats->total_pixels;

  double variance_sum = 0.0;

  for (int i = 0; i < HISTOGRAM_SIZE; ++i)
  {
    double difference = (double)i - stats->mean;

    variance_sum +=
        (double)stats->histogram[i] *
        difference *
        difference;
  }

  stats->stddev =
      sqrt(variance_sum / (double)stats->total_pixels);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
const char *classify_brightness(double mean)
{
  if (mean < BRIGHTNESS_DARK_THRESHOLD)
    return "escura";

  if (mean > BRIGHTNESS_LIGHT_THRESHOLD)
    return "clara";

  return "media";
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
const char *classify_contrast(double stddev)
{
  if (stddev < CONTRAST_LOW_THRESHOLD)
    return "baixo";

  if (stddev > CONTRAST_HIGH_THRESHOLD)
    return "alto";

  return "medio";
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool update_image_stats(void)
{
  memset(&g_stats, 0, sizeof(g_stats));

  if (!MyImage_calculate_histogram(
          &g_image,
          g_stats.histogram,
          &g_stats.total_pixels))
  {
    return false;
  }

  calculate_histogram_analysis(&g_stats);

  return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool MyImage_equalize(MyImage *image, SDL_Renderer *renderer)
{
  if (!image || !image->surface || !renderer)
  {
    SDL_Log("*** Erro: Parametros invalidos para equalizacao.");
    return false;
  }

  Uint32 histogram[HISTOGRAM_SIZE] = { 0 };
  Uint64 total_pixels = 0;

  if (!MyImage_calculate_histogram(image, histogram, &total_pixels) ||
      total_pixels == 0)
  {
    return false;
  }

  Uint64 cdf[HISTOGRAM_SIZE] = { 0 };
  cdf[0] = histogram[0];

  for (int i = 1; i < HISTOGRAM_SIZE; ++i)
  {
    cdf[i] = cdf[i - 1] + histogram[i];
  }

  // Primeiro valor acumulado nao-zero.
  Uint64 cdf_min = 0;

  for (int i = 0; i < HISTOGRAM_SIZE; ++i)
  {
    if (histogram[i] > 0)
    {
      cdf_min = cdf[i];
      break;
    }
  }

  if (cdf_min == total_pixels)
  {
    SDL_Log("Equalizacao ignorada: imagem possui uma unica intensidade.");
    return true;
  }

  Uint8 transform[HISTOGRAM_SIZE] = { 0 };

  double denominator =
      (double)(total_pixels - cdf_min);

  for (int i = 0; i < HISTOGRAM_SIZE; ++i)
  {
    double value =
        (((double)cdf[i] - (double)cdf_min) / denominator) * 255.0;

    if (value < 0.0)
      value = 0.0;

    if (value > 255.0)
      value = 255.0;

    transform[i] = (Uint8)(value + 0.5);
  }

  const SDL_PixelFormatDetails *format =
      SDL_GetPixelFormatDetails(image->surface->format);

  if (!format)
  {
    SDL_Log("*** Erro ao obter formato de pixels: %s", SDL_GetError());
    return false;
  }

  if (!SDL_LockSurface(image->surface))
  {
    SDL_Log("*** Erro ao bloquear superficie: %s", SDL_GetError());
    return false;
  }

  for (int row = 0; row < image->surface->h; ++row)
  {
    Uint32 *row_pixels =
        (Uint32 *)((Uint8 *)image->surface->pixels +
                   row * image->surface->pitch);

    for (int col = 0; col < image->surface->w; ++col)
    {
      Uint8 r = 0;
      Uint8 g = 0;
      Uint8 b = 0;

      SDL_GetRGB(
          row_pixels[col],
          format,
          NULL,
          &r,
          &g,
          &b);

      Uint8 y = transform[r];

      row_pixels[col] =
          SDL_MapRGB(format, NULL, y, y, y);
    }
  }

  SDL_UnlockSurface(image->surface);

  return MyImage_update_texture_with_surface(
      image,
      renderer,
      image->surface);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool save_current_image(void)
{
  if (!g_image.surface)
  {
    SDL_Log("*** Erro: Nao ha imagem ativa para salvar.");
    return false;
  }

  FILE *file = fopen(OUTPUT_FILENAME, "rb");
  bool existed = file != NULL;
  if (file)
    fclose(file);

  if (!IMG_SavePNG(g_image.surface, OUTPUT_FILENAME))
  {
    SDL_Log("*** Erro ao salvar PNG: %s", SDL_GetError());
    return false;
  }

  SDL_Log("%s: %s", existed ? "Arquivo sobrescrito" : "Arquivo criado", OUTPUT_FILENAME);
  return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
SDL_AppResult initialize(void)
{
  SDL_Log(">>> initialize()");

  SDL_Log("\tIniciando SDL...");
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    SDL_Log("\t*** Erro ao iniciar a SDL: %s", SDL_GetError());
    SDL_Log("<<< initialize()");
    return SDL_APP_FAILURE;
  }

  SDL_Log("\tIniciando SDL_ttf...");
  if (!TTF_Init())
  {
    SDL_Log("\t*** Erro ao iniciar SDL_ttf: %s", SDL_GetError());
    SDL_Log("<<< initialize()");
    return SDL_APP_FAILURE;
  }

  SDL_Log("\tCarregando fonte...");
  g_font = TTF_OpenFont(FONT_FILENAME, FONT_SIZE);
  if (!g_font)
  {
    SDL_Log("\t*** Erro ao carregar fonte \"%s\": %s", FONT_FILENAME, SDL_GetError());
    SDL_Log("<<< initialize()");
    return SDL_APP_FAILURE;
  }

  SDL_Log("\tCriando janela e renderizador...");
  if (!MyWindow_initialize(&g_window, WINDOW_TITLE, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0))
  {
    SDL_Log("\t*** Erro ao criar a janela e/ou renderizador: %s", SDL_GetError());
    SDL_Log("<<< initialize()");
    return SDL_APP_FAILURE;
  }

  SDL_Log("\tCriando janela secundaria...");
  if (!MyWindow_initialize(&g_secondaryWindow, SECONDARY_WINDOW_TITLE, SECONDARY_WINDOW_WIDTH, SECONDARY_WINDOW_HEIGHT, 0))
  {
    SDL_Log("\t*** Erro ao criar janela secundaria: %s", SDL_GetError());
    SDL_Log("<<< initialize()");
    return SDL_APP_FAILURE;
  }

  if (!SDL_SetWindowParent(g_secondaryWindow.window, g_window.window))
  {
    SDL_Log("\t*** Erro ao definir janela pai: %s", SDL_GetError());
    SDL_Log("<<< initialize()");
    return SDL_APP_FAILURE;
  }

  if (!SDL_SetWindowPosition(g_secondaryWindow.window, 0, 0))
  {
    SDL_Log("\t*** Erro ao posicionar janela secundaria: %s", SDL_GetError());
    SDL_Log("<<< initialize()");
    return SDL_APP_FAILURE;
  }

  SDL_Log("<<< initialize()");
  return SDL_APP_CONTINUE;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void shutdown(void)
{
  SDL_Log(">>> shutdown()");

  SDL_Log("Destruindo copia original em cinza...");
  SDL_DestroySurface(g_originalGraySurface);
  g_originalGraySurface = NULL;

  MyImage_destroy(&g_image);

  SDL_Log("Destruindo fonte...");
  TTF_CloseFont(g_font);
  g_font = NULL;

  MyWindow_destroy(&g_secondaryWindow);
  MyWindow_destroy(&g_window);

  SDL_Log("\tEncerrando SDL_ttf...");
  TTF_Quit();

  SDL_Log("\tEncerrando SDL...");
  SDL_Quit();

  SDL_Log("<<< shutdown()");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void update_main_window_size_and_position(void)
{
  if (!g_window.window || !g_image.surface)
    return;

  int target_width = DEFAULT_WINDOW_WIDTH;
  int target_height = DEFAULT_WINDOW_HEIGHT;

  if (g_showOriginalResolution)
  {
    target_width = g_image.surface->w;
    target_height = g_image.surface->h;
  }

  SDL_SetWindowSize(g_window.window, target_width, target_height);

  SDL_Rect display_bounds = { 0, 0, 0, 0 };
  SDL_DisplayID display = SDL_GetPrimaryDisplay();
  if (display && SDL_GetDisplayUsableBounds(display, &display_bounds))
  {
    if (target_width <= display_bounds.w && target_height <= display_bounds.h)
    {
      SDL_SetWindowPosition(g_window.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
    else
    {
      SDL_SetWindowPosition(g_window.window, display_bounds.x, display_bounds.y);
    }
  }
  else
  {
    SDL_Log("*** Erro ao obter resolucao do monitor: %s", SDL_GetError());
  }

  float scale_x = (float)target_width / (float)g_image.surface->w;
  float scale_y = (float)target_height / (float)g_image.surface->h;
  float scale = scale_x < scale_y ? scale_x : scale_y;

  float image_width = (float)g_image.surface->w * scale;
  float image_height = (float)g_image.surface->h * scale;

  g_image.rect.x = ((float)target_width - image_width) / 2.0f;
  g_image.rect.y = ((float)target_height - image_height) / 2.0f;
  g_image.rect.w = image_width;
  g_image.rect.h = image_height;

  g_resolutionButton.text = g_showOriginalResolution ? "1024x768" : "Resolucao original";
  SDL_SyncWindow(g_window.window);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void render_text(SDL_Renderer *renderer, const char *text, float x, float y, SDL_Color color)
{
  if (!renderer || !g_font || !text)
    return;

  SDL_Surface *surface = TTF_RenderText_Blended(g_font, text, strlen(text), color);
  if (!surface)
  {
    SDL_Log("*** Erro ao renderizar texto: %s", SDL_GetError());
    return;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture)
  {
    SDL_Log("*** Erro ao criar textura de texto: %s", SDL_GetError());
    SDL_DestroySurface(surface);
    return;
  }

  SDL_FRect dst = {
    .x = x,
    .y = y,
    .w = (float)surface->w,
    .h = (float)surface->h
  };

  SDL_RenderTexture(renderer, texture, NULL, &dst);
  SDL_DestroyTexture(texture);
  SDL_DestroySurface(surface);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void render_button(SDL_Renderer *renderer, Button *button)
{
  if (!renderer || !button)
    return;

  if (button->pressed)
    SDL_SetRenderDrawColor(renderer, 20, 74, 150, 255);
  else if (button->hovered)
    SDL_SetRenderDrawColor(renderer, 75, 145, 235, 255);
  else
    SDL_SetRenderDrawColor(renderer, 33, 113, 205, 255);

  SDL_RenderFillRect(renderer, &button->rect);

  SDL_SetRenderDrawColor(renderer, 16, 52, 105, 255);
  SDL_RenderRect(renderer, &button->rect);

  SDL_Color white = { 255, 255, 255, 255 };

  int text_width = 0;
  int text_height = 0;

  if (TTF_GetStringSize(g_font, button->text, strlen(button->text), &text_width, &text_height))
  {
    float text_x = button->rect.x + (button->rect.w - (float)text_width) / 2.0f;
    float text_y = button->rect.y + (button->rect.h - (float)text_height) / 2.0f;

    render_text(renderer, button->text, text_x, text_y, white);
  }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void render_histogram(SDL_Renderer *renderer, const SDL_FRect *rect, const Uint32 histogram[HISTOGRAM_SIZE])
{
  if (!renderer || !rect || !histogram)
    return;

  Uint32 max_value = 0;
  for (int i = 0; i < HISTOGRAM_SIZE; ++i)
  {
    if (histogram[i] > max_value)
      max_value = histogram[i];
  }

  SDL_SetRenderDrawColor(renderer, 245, 247, 250, 255);
  SDL_RenderFillRect(renderer, rect);
  SDL_SetRenderDrawColor(renderer, 90, 96, 110, 255);
  SDL_RenderRect(renderer, rect);

  if (max_value == 0)
    return;

  SDL_SetRenderDrawColor(renderer, 42, 82, 150, 255);
  float bar_width = rect->w / (float)HISTOGRAM_SIZE;

  for (int i = 0; i < HISTOGRAM_SIZE; ++i)
  {
    float normalized = (float)histogram[i] / (float)max_value;
    float bar_height = normalized * rect->h;
    SDL_FRect bar = {
      .x = rect->x + (float)i * bar_width,
      .y = rect->y + rect->h - bar_height,
      .w = bar_width < 1.0f ? 1.0f : bar_width,
      .h = bar_height
    };
    SDL_RenderFillRect(renderer, &bar);
  }

  SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
  SDL_RenderLine(renderer, rect->x, rect->y + rect->h, rect->x + rect->w, rect->y + rect->h);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void render_main_window(void)
{
  if (!g_window.renderer)
    return;

  SDL_SetRenderDrawColor(g_window.renderer, 128, 128, 128, 255);
  SDL_RenderClear(g_window.renderer);

  if (g_image.texture)
    SDL_RenderTexture(g_window.renderer, g_image.texture, NULL, &g_image.rect);

  SDL_RenderPresent(g_window.renderer);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void render_secondary_window(void)
{
  if (!g_secondaryWindow.renderer)
    return;

  SDL_SetRenderDrawColor(g_secondaryWindow.renderer, 232, 235, 240, 255);
  SDL_RenderClear(g_secondaryWindow.renderer);

  SDL_Color text_color = { 28, 32, 38, 255 };
  SDL_Color muted_color = { 70, 76, 88, 255 };

  render_text(g_secondaryWindow.renderer, "Histograma da imagem", 24.0f, 18.0f, text_color);

  SDL_FRect histogram_rect = { .x = 24.0f, .y = 50.0f, .w = 452.0f, .h = 150.0f };
  render_histogram(g_secondaryWindow.renderer, &histogram_rect, g_stats.histogram);
  render_text(g_secondaryWindow.renderer, "0", 24.0f, 205.0f, muted_color);
  render_text(g_secondaryWindow.renderer, "255", 446.0f, 205.0f, muted_color);

  char line[128];
  snprintf(line, sizeof(line), "Media: %.2f (%s)", g_stats.mean, classify_brightness(g_stats.mean));
  render_text(g_secondaryWindow.renderer, line, 24.0f, 232.0f, text_color);

  snprintf(line, sizeof(line), "Desvio padrao: %.2f (%s)", g_stats.stddev, classify_contrast(g_stats.stddev));
  render_text(g_secondaryWindow.renderer, line, 24.0f, 256.0f, text_color);

  render_button(g_secondaryWindow.renderer, &g_equalizeButton);
  render_button(g_secondaryWindow.renderer, &g_resolutionButton);

  render_text(g_secondaryWindow.renderer, "Tecla S: salvar imagem ativa", 24.0f, 354.0f, muted_color);

  SDL_RenderPresent(g_secondaryWindow.renderer);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void render(void)
{
  render_main_window();
  render_secondary_window();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool Button_contains(Button *button, float x, float y)
{
  if (!button)
    return false;

  return x >= button->rect.x
    && x <= button->rect.x + button->rect.w
    && y >= button->rect.y
    && y <= button->rect.y + button->rect.h;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool Button_handle_event(Button *button, const SDL_Event *event)
{
  if (!button || !event)
    return false;

  switch (event->type)
  {
  case SDL_EVENT_MOUSE_MOTION:
    button->hovered = Button_contains(button, event->motion.x, event->motion.y);
    return false;

  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    if (event->button.button == SDL_BUTTON_LEFT && Button_contains(button, event->button.x, event->button.y))
      button->pressed = true;
    return false;

  case SDL_EVENT_MOUSE_BUTTON_UP:
    if (event->button.button == SDL_BUTTON_LEFT)
    {
      bool clicked = button->pressed && Button_contains(button, event->button.x, event->button.y);
      button->pressed = false;
      return clicked;
    }
    return false;

  default:
    return false;
  }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void toggle_equalization(void)
{
  if (g_isEqualized)
  {
    if (!restore_original_grayscale())
      SDL_Log("*** Erro ao restaurar imagem original.");
  }
  else
  {
    if (MyImage_equalize(&g_image, g_window.renderer))
    {
      g_isEqualized = true;
      g_equalizeButton.text = "Ver original";
      update_image_stats();
    }
  }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void toggle_resolution(void)
{
  g_showOriginalResolution = !g_showOriginalResolution;
  update_main_window_size_and_position();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void loop(void)
{
  SDL_Log(">>> loop()");

  SDL_WindowID main_window_id = SDL_GetWindowID(g_window.window);
  SDL_WindowID secondary_window_id = SDL_GetWindowID(g_secondaryWindow.window);

  render();

  SDL_Event event;
  bool isRunning = true;
  while (isRunning)
  {
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
      case SDL_EVENT_QUIT:
        isRunning = false;
        break;

      case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        if (event.window.windowID == main_window_id || event.window.windowID == secondary_window_id)
          isRunning = false;
        break;

      case SDL_EVENT_KEY_DOWN:
        if (!event.key.repeat && event.key.key == SDLK_S)
          save_current_image();
        break;

      case SDL_EVENT_MOUSE_MOTION:
        if (event.motion.windowID == secondary_window_id)
        {
          Button_handle_event(&g_equalizeButton, &event);
          Button_handle_event(&g_resolutionButton, &event);
        }
        break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN:
      case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.windowID == secondary_window_id)
        {
          if (Button_handle_event(&g_equalizeButton, &event))
            toggle_equalization();

          if (Button_handle_event(&g_resolutionButton, &event))
            toggle_resolution();
        }
        break;
      }
    }

    render();
    SDL_Delay(16);
  }

  SDL_Log("<<< loop()");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    SDL_Log("Uso: %s caminho_da_imagem", argv[0]);
    return SDL_APP_FAILURE;
  }

  atexit(shutdown);

  if (initialize() == SDL_APP_FAILURE)
    return SDL_APP_FAILURE;

  if (!load_rgba32(argv[1], g_window.renderer, &g_image))
    return SDL_APP_FAILURE;

  if (MyImage_is_grayscale(&g_image))
  {
    SDL_Log("A imagem ja esta em escala de cinza.");
  }
  else
  {
    SDL_Log("A imagem e colorida.");
    if (!MyImage_convert_to_grayscale(&g_image))
      return SDL_APP_FAILURE;

    if (!MyImage_restore_texture(&g_image, g_window.renderer))
      return SDL_APP_FAILURE;

    SDL_Log("Imagem convertida para escala de cinza.");
  }

  if (!copy_original_grayscale(&g_image))
    return SDL_APP_FAILURE;

  if (!update_image_stats())
    return SDL_APP_FAILURE;

  update_main_window_size_and_position();
  loop();

  return 0;
}
