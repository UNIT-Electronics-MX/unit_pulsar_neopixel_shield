#include <Adafruit_NeoPixel.h>

#define PIN        8
#define NUM_LEDS   257  // Total: 1 LED indicador (0) + 256 LEDs de matriz (1-256)
#define MATRIX_WIDTH  8    // 8 columnas
#define MATRIX_HEIGHT 32   // 32 filas
#define DELAYVAL   100

// Control de brillo general (0-255)
// 255 = máximo brillo, 128 = 50% brillo, 64 = 25% brillo
#define BRILLO_GENERAL 128  // Cambia este valor para ajustar la luminosidad general

Adafruit_NeoPixel strip(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// Función para escalar colores según el brillo general
uint32_t escalarColor(uint32_t color) {
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;
  
  // Escalar cada componente según BRILLO_GENERAL
  r = (r * BRILLO_GENERAL) / 255;
  g = (g * BRILLO_GENERAL) / 255;
  b = (b * BRILLO_GENERAL) / 255;
  
  return strip.Color(r, g, b);
}

// Función para convertir coordenadas x,y a índice LED
// NOTA: Ignora el LED 0 y comienza desde el LED 1 (n+1)
// Patrón ZIGZAG: filas pares van izq->der, filas impares van der->izq
int getPixelIndex(int x, int y) {
  if (x < 0 || x >= MATRIX_WIDTH || y < 0 || y >= MATRIX_HEIGHT) {
    return -1; // Fuera de los límites
  }
  
  // Patrón zigzag (serpentina):
  // Fila 0: 0->7 (izq a der)
  // Fila 1: 7->0 (der a izq)
  // Fila 2: 0->7 (izq a der)
  // etc...
  
  int index;
  if (y % 2 == 0) {
    // Fila par: izquierda a derecha
    index = y * MATRIX_WIDTH + x;
  } else {
    // Fila impar: derecha a izquierda (invertir X)
    index = y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x);
  }
  
  // Añadir +1 para saltar el LED 0 (indicador)
  return index + 1;
}

// Definición de caracteres 5x8 ASCII completo (bit 1 = pixel encendido)
// Índices del 32 al 126 (caracteres imprimibles ASCII)
const uint8_t font5x8[][5] = {
  // 32: Espacio
  {0x00, 0x00, 0x00, 0x00, 0x00},
  // 33: !
  {0x00, 0x00, 0x5F, 0x00, 0x00},
  // 34: "
  {0x00, 0x07, 0x00, 0x07, 0x00},
  // 35: #
  {0x14, 0x7F, 0x14, 0x7F, 0x14},
  // 36: $
  {0x24, 0x2A, 0x7F, 0x2A, 0x12},
  // 37: %
  {0x23, 0x13, 0x08, 0x64, 0x62},
  // 38: &
  {0x36, 0x49, 0x55, 0x22, 0x50},
  // 39: '
  {0x00, 0x05, 0x03, 0x00, 0x00},
  // 40: (
  {0x00, 0x1C, 0x22, 0x41, 0x00},
  // 41: )
  {0x00, 0x41, 0x22, 0x1C, 0x00},
  // 42: *
  {0x14, 0x08, 0x3E, 0x08, 0x14},
  // 43: +
  {0x08, 0x08, 0x3E, 0x08, 0x08},
  // 44: ,
  {0x00, 0x50, 0x30, 0x00, 0x00},
  // 45: -
  {0x08, 0x08, 0x08, 0x08, 0x08},
  // 46: .
  {0x00, 0x60, 0x60, 0x00, 0x00},
  // 47: /
  {0x20, 0x10, 0x08, 0x04, 0x02},
  // 48: 0
  {0x3E, 0x51, 0x49, 0x45, 0x3E},
  // 49: 1
  {0x00, 0x42, 0x7F, 0x40, 0x00},
  // 50: 2
  {0x42, 0x61, 0x51, 0x49, 0x46},
  // 51: 3
  {0x21, 0x41, 0x45, 0x4B, 0x31},
  // 52: 4
  {0x18, 0x14, 0x12, 0x7F, 0x10},
  // 53: 5
  {0x27, 0x45, 0x45, 0x45, 0x39},
  // 54: 6
  {0x3C, 0x4A, 0x49, 0x49, 0x30},
  // 55: 7
  {0x01, 0x71, 0x09, 0x05, 0x03},
  // 56: 8
  {0x36, 0x49, 0x49, 0x49, 0x36},
  // 57: 9
  {0x06, 0x49, 0x49, 0x29, 0x1E},
  // 58: :
  {0x00, 0x36, 0x36, 0x00, 0x00},
  // 59: ;
  {0x00, 0x56, 0x36, 0x00, 0x00},
  // 60: <
  {0x08, 0x14, 0x22, 0x41, 0x00},
  // 61: =
  {0x14, 0x14, 0x14, 0x14, 0x14},
  // 62: >
  {0x00, 0x41, 0x22, 0x14, 0x08},
  // 63: ?
  {0x02, 0x01, 0x51, 0x09, 0x06},
  // 64: @
  {0x32, 0x49, 0x79, 0x41, 0x3E},
  // 65: A
  {0x7E, 0x11, 0x11, 0x11, 0x7E},
  // 66: B
  {0x7F, 0x49, 0x49, 0x49, 0x36},
  // 67: C
  {0x3E, 0x41, 0x41, 0x41, 0x22},
  // 68: D
  {0x7F, 0x41, 0x41, 0x22, 0x1C},
  // 69: E
  {0x7F, 0x49, 0x49, 0x49, 0x41},
  // 70: F
  {0x7F, 0x09, 0x09, 0x09, 0x01},
  // 71: G
  {0x3E, 0x41, 0x49, 0x49, 0x7A},
  // 72: H
  {0x7F, 0x08, 0x08, 0x08, 0x7F},
  // 73: I
  {0x00, 0x41, 0x7F, 0x41, 0x00},
  // 74: J
  {0x20, 0x40, 0x41, 0x3F, 0x01},
  // 75: K
  {0x7F, 0x08, 0x14, 0x22, 0x41},
  // 76: L
  {0x7F, 0x40, 0x40, 0x40, 0x40},
  // 77: M
  {0x7F, 0x02, 0x0C, 0x02, 0x7F},
  // 78: N
  {0x7F, 0x04, 0x08, 0x10, 0x7F},
  // 79: O
  {0x3E, 0x41, 0x41, 0x41, 0x3E},
  // 80: P
  {0x7F, 0x09, 0x09, 0x09, 0x06},
  // 81: Q
  {0x3E, 0x41, 0x51, 0x21, 0x5E},
  // 82: R
  {0x7F, 0x09, 0x19, 0x29, 0x46},
  // 83: S
  {0x46, 0x49, 0x49, 0x49, 0x31},
  // 84: T
  {0x01, 0x01, 0x7F, 0x01, 0x01},
  // 85: U
  {0x3F, 0x40, 0x40, 0x40, 0x3F},
  // 86: V
  {0x1F, 0x20, 0x40, 0x20, 0x1F},
  // 87: W
  {0x3F, 0x40, 0x38, 0x40, 0x3F},
  // 88: X
  {0x63, 0x14, 0x08, 0x14, 0x63},
  // 89: Y
  {0x07, 0x08, 0x70, 0x08, 0x07},
  // 90: Z
  {0x61, 0x51, 0x49, 0x45, 0x43}
};

// Mapeo de caracteres ASCII a índices en el array font5x8
int getCharIndex(char c) {
  // Mapeo directo ASCII: caracteres del 32 al 90 (espacio a Z)
  if (c >= 32 && c <= 90) {
    return c - 32; // Índice directo basado en código ASCII
  }
  // Convertir minúsculas a mayúsculas
  else if (c >= 97 && c <= 122) { // a-z
    return (c - 97) + (65 - 32); // Mapear a A-Z
  }
  else {
    return 0; // Espacio por defecto para caracteres no soportados
  }
}

// ============== SPRITES DE PERSONAJES ==============
// Sprites 8x8 píxeles (1 bit por píxel, 1 = encendido, 0 = apagado)

// Pikachu mejorado (8x8 píxeles)
const uint8_t spritePikachu[8] = {
  0b00111100,  // Fila 0: Orejas
  0b01111110,  // Fila 1: Parte superior cabeza
  0b11111111,  // Fila 2: Cabeza con orejas
  0b11011011,  // Fila 3: Ojos
  0b11111111,  // Fila 4: Mejillas rojas
  0b01111110,  // Fila 5: Cuerpo
  0b00111100,  // Fila 6: Parte inferior
  0b01000010   // Fila 7: Pies
};

// Pokébola (8x8 píxeles)
const uint8_t spritePokebola[8] = {
  0b00111100,  // Fila 0
  0b01111110,  // Fila 1: Parte superior roja
  0b11111111,  // Fila 2
  0b11111111,  // Fila 3: Línea negra centro
  0b11111111,  // Fila 4: Botón blanco centro
  0b11111111,  // Fila 5: Parte inferior blanca
  0b01111110,  // Fila 6
  0b00111100   // Fila 7
};

// Mario mejorado (8x8 píxeles)
const uint8_t spriteMario[8] = {
  0b00111100,  // Fila 0
  0b01111110,  // Fila 1
  0b00111100,  // Fila 2
  0b01011010,  // Fila 3
  0b00111100,  // Fila 4
  0b01111110,  // Fila 5
  0b01011010,  // Fila 6
  0b01100110   // Fila 7
};

// Mario saltando mejorado (8x8 píxeles)
const uint8_t spriteMarioJump[8] = {
  0b00111100,  // Fila 0
  0b01111110,  // Fila 1
  0b11111111,  // Fila 2
  0b00111100,  // Fila 3
  0b01000010,  // Fila 4
  0b11000011,  // Fila 5
  0b01000010,  // Fila 6
  0b01000010   // Fila 7
};

// Pac-Man (8x8 píxeles)
const uint8_t spritePacman[8] = {
  0b00111100,  // Fila 0
  0b01111110,  // Fila 1
  0b11111110,  // Fila 2: Boca abierta
  0b11111100,  // Fila 3
  0b11111100,  // Fila 4
  0b11111110,  // Fila 5
  0b01111110,  // Fila 6
  0b00111100   // Fila 7
};

// Hongo de Mario (8x8 píxeles)
const uint8_t spriteHongo[8] = {
  0b00000000,  // Fila 0
  0b00111100,  // Fila 1: Parte superior
  0b01111110,  // Fila 2: Sombrero rojo
  0b11111111,  // Fila 3: Puntos blancos
  0b11111111,  // Fila 4
  0b01111110,  // Fila 5: Tallo blanco
  0b01111110,  // Fila 6
  0b00111100   // Fila 7
};

// Corazón (8x8 píxeles)
const uint8_t spriteCorazon[8] = {
  0b00000000,  // Fila 0
  0b01100110,  // Fila 1
  0b11111111,  // Fila 2
  0b11111111,  // Fila 3
  0b11111111,  // Fila 4
  0b01111110,  // Fila 5
  0b00111100,  // Fila 6
  0b00011000   // Fila 7
};

// Estrella (8x8 píxeles)
const uint8_t spriteEstrella[8] = {
  0b00011000,  // Fila 0: Punta superior
  0b00011000,  // Fila 1
  0b01111110,  // Fila 2: Brazos
  0b00111100,  // Fila 3: Centro
  0b01111110,  // Fila 4
  0b11111111,  // Fila 5: Parte ancha
  0b01111110,  // Fila 6
  0b00100100   // Fila 7: Puntas inferiores
};

// Fantasma Pac-Man (8x8 píxeles)
const uint8_t spriteFantasma[8] = {
  0b00111100,  // Fila 0: Parte superior
  0b01111110,  // Fila 1
  0b11111111,  // Fila 2: Ojos
  0b11111111,  // Fila 3
  0b11111111,  // Fila 4: Cuerpo
  0b11111111,  // Fila 5
  0b11111111,  // Fila 6
  0b10110101   // Fila 7: Parte inferior ondulada
};

// Nota musical (8x8 píxeles)
const uint8_t spriteNota[8] = {
  0b00000011,  // Fila 0: Parte superior
  0b00000011,  // Fila 1: Barra
  0b00000011,  // Fila 2
  0b00000011,  // Fila 3
  0b00000011,  // Fila 4
  0b01100011,  // Fila 5: Cabeza de nota
  0b11110011,  // Fila 6
  0b01100000   // Fila 7
};

// Función para dibujar un sprite rotado 90° (horizontal)
// Los sprites se mueven a lo largo de las 32 filas (eje vertical de la matriz)
void drawSprite(const uint8_t sprite[], int startX, int startY, uint32_t color) {
  uint32_t scaledColor = escalarColor(color);
  
  // Rotación 90° en sentido horario del sprite
  for (int row = 0; row < 8; row++) {
    uint8_t rowData = sprite[row];
    
    for (int col = 0; col < 8; col++) {
      if (rowData & (1 << (7 - col))) {  // Bit encendido
        // Rotar el sprite 90° horario: (row, col) -> (col, 7-row)
        int x = startX + col;          // col se convierte en X
        int y = startY + (7 - row);    // 7-row se convierte en Y (rotado)
        
        if (x >= 0 && x < MATRIX_WIDTH && y >= 0 && y < MATRIX_HEIGHT) {
          int pixelIndex = getPixelIndex(x, y);
          if (pixelIndex >= 0) {
            strip.setPixelColor(pixelIndex, scaledColor);
          }
        }
      }
    }
  }
}

// Función para dibujar Pokébola con colores específicos
void drawPokebola(int startX, int startY) {
  for (int row = 0; row < 8; row++) {
    uint8_t rowData = spritePokebola[row];
    
    for (int col = 0; col < 8; col++) {
      if (rowData & (1 << (7 - col))) {
        // Rotar 90° horario
        int x = startX + col;
        int y = startY + (7 - row);
        
        if (x >= 0 && x < MATRIX_WIDTH && y >= 0 && y < MATRIX_HEIGHT) {
          int pixelIndex = getPixelIndex(x, y);
          if (pixelIndex >= 0) {
            // Parte superior roja (columnas 0-2 después de rotar)
            if (col < 3) {
              strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 0, 0)));
            }
            // Línea negra centro (columna 3)
            else if (col == 3) {
              strip.setPixelColor(pixelIndex, escalarColor(strip.Color(50, 50, 50)));
            }
            // Botón blanco centro (columna 4)
            else if (col == 4 && (7-row) >= 3 && (7-row) <= 4) {
              strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 255, 255)));
            }
            // Parte inferior blanca (columnas 5-7)
            else {
              strip.setPixelColor(pixelIndex, escalarColor(strip.Color(200, 200, 200)));
            }
          }
        }
      }
    }
  }
}

// Función para dibujar hongo de Mario con colores
void drawHongo(int startX, int startY) {
  for (int row = 0; row < 8; row++) {
    uint8_t rowData = spriteHongo[row];
    
    for (int col = 0; col < 8; col++) {
      if (rowData & (1 << (7 - col))) {
        // Rotar 90° horario
        int x = startX + col;
        int y = startY + (7 - row);
        
        if (x >= 0 && x < MATRIX_WIDTH && y >= 0 && y < MATRIX_HEIGHT) {
          int pixelIndex = getPixelIndex(x, y);
          if (pixelIndex >= 0) {
            // Sombrero rojo (columnas 1-4 después de rotar)
            if (col >= 1 && col <= 4) {
              // Puntos blancos en el sombrero
              if ((x == startX + 2 && y == startY + 5) || (x == startX + 5 && y == startY + 4)) {
                strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 255, 255)));
              } else {
                strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 0, 0)));
              }
            }
            // Tallo blanco (columnas 5-7)
            else {
              strip.setPixelColor(pixelIndex, escalarColor(strip.Color(240, 230, 200)));
            }
          }
        }
      }
    }
  }
}

// Función para dibujar Mario sin rotación (vertical)
void drawMario(const uint8_t sprite[], int startX, int startY, uint32_t color) {
  uint32_t scaledColor = escalarColor(color);
  
  for (int row = 0; row < 8; row++) {
    uint8_t rowData = sprite[row];
    
    for (int col = 0; col < 8; col++) {
      if (rowData & (1 << (7 - col))) {
        // Igual que Pokébola: rotar 90° horario
        int x = startX + col;
        int y = startY + (7 - row);
        
        if (x >= 0 && x < MATRIX_WIDTH && y >= 0 && y < MATRIX_HEIGHT) {
          int pixelIndex = getPixelIndex(x, y);
          if (pixelIndex >= 0) {
            strip.setPixelColor(pixelIndex, scaledColor);
          }
        }
      }
    }
  }
}

// Función para dibujar Corazón rotado 90° adicional con color personalizado
void drawCorazon(int startX, int startY, uint32_t color) {
  uint32_t scaledColor = escalarColor(color);
  
  for (int row = 0; row < 8; row++) {
    uint8_t rowData = spriteCorazon[row];
    
    for (int col = 0; col < 8; col++) {
      if (rowData & (1 << (7 - col))) {
        // Rotación ajustada para orientación correcta (no de cabeza)
        int x = startX + row;
        int y = startY + col;
        
        if (x >= 0 && x < MATRIX_WIDTH && y >= 0 && y < MATRIX_HEIGHT) {
          int pixelIndex = getPixelIndex(x, y);
          if (pixelIndex >= 0) {
            strip.setPixelColor(pixelIndex, scaledColor);
          }
        }
      }
    }
  }
}

// ============== ANIMACIONES DE SPRITES ==============

// Mostrar sprite estático centrado
void mostrarSpriteEstatico(const uint8_t sprite[], uint32_t color, int duracion) {
  strip.clear();
  drawSprite(sprite, 0, 12, color); // Centrado verticalmente
  strip.show();
  delay(duracion);
}

// Galería de sprites estáticos
void galeriaSprites() {
  // Pikachu
  mostrarSpriteEstatico(spritePikachu, strip.Color(255, 255, 0), 2000);
  delay(200);
  
  // Pokébola con colores
  strip.clear();
  drawPokebola(0, 12);
  strip.show();
  delay(2000);
  delay(200);
  
  // Mario
  mostrarSpriteEstatico(spriteMario, strip.Color(255, 0, 0), 2000);
  delay(200);
  
  // Hongo con colores
  strip.clear();
  drawHongo(0, 12);
  strip.show();
  delay(2000);
  delay(200);
  
  // Pac-Man
  mostrarSpriteEstatico(spritePacman, strip.Color(255, 255, 0), 2000);
  delay(200);
  
  // Corazón
  mostrarSpriteEstatico(spriteCorazon, strip.Color(255, 20, 147), 2000);
  delay(200);
  
  // Estrella
  mostrarSpriteEstatico(spriteEstrella, strip.Color(255, 215, 0), 2000);
  delay(200);
}

// Animación de aparición gradual (fade in)
void apareceSprite(const uint8_t sprite[], uint32_t color) {
  for (int brillo = 0; brillo <= 10; brillo++) {
    strip.clear();
    
    // Ajustar temporalmente el brillo
    int brilloOriginal = BRILLO_GENERAL;
    
    for (int row = 0; row < 8; row++) {
      uint8_t rowData = sprite[row];
      
      for (int col = 0; col < 8; col++) {
        if (rowData & (1 << (7 - col))) {
          int x = 0 + row;
          int y = 12 + col;
          
          if (x >= 0 && x < MATRIX_WIDTH && y >= 0 && y < MATRIX_HEIGHT) {
            int pixelIndex = getPixelIndex(x, y);
            if (pixelIndex >= 0) {
              uint8_t r = ((color >> 16) & 0xFF) * brillo / 10;
              uint8_t g = ((color >> 8) & 0xFF) * brillo / 10;
              uint8_t b = (color & 0xFF) * brillo / 10;
              strip.setPixelColor(pixelIndex, escalarColor(strip.Color(r, g, b)));
            }
          }
        }
      }
    }
    
    strip.show();
    delay(100);
  }
  delay(1500);
}

// Secuencia de apariciones
void secuenciaApariciones() {
  apareceSprite(spritePikachu, strip.Color(255, 255, 0));
  strip.clear();
  strip.show();
  delay(300);
  
  apareceSprite(spriteMario, strip.Color(255, 0, 0));
  strip.clear();
  strip.show();
  delay(300);
  
  apareceSprite(spriteCorazon, strip.Color(255, 20, 147));
  strip.clear();
  strip.show();
  delay(300);
}

// Pikachu parpadeando (centrado horizontalmente)
void pikachuParpadeo(int cycles) {
  for (int i = 0; i < cycles; i++) {
    strip.clear();
    drawSprite(spritePikachu, 0, 12, strip.Color(255, 255, 0)); // Amarillo, centrado
    strip.show();
    delay(800);
    
    strip.clear();
    strip.show();
    delay(200);
  }
}

// Mario caminando horizontalmente (a lo largo de las 32 filas)
void marioCamina(int duration) {
  for (int pos = -8; pos < MATRIX_HEIGHT + 8; pos++) {
    strip.clear();
    
    // Alternar entre sprite normal y saltando para simular caminar
    int xPos = 0; // Posición X fija (columna 0)
    if ((pos / 2) % 2 == 0) {
      drawMario(spriteMario, xPos, pos, strip.Color(255, 0, 0)); // Rojo
    } else {
      drawMario(spriteMarioJump, xPos, pos, strip.Color(255, 0, 0)); // Rojo
    }
    
    strip.show();
    delay(duration);
  }
}

// Animación de corazones moviéndose horizontalmente
void corazonesSubiendo(int numCorazones) {
  for (int i = 0; i < numCorazones; i++) {
    int offsetX = random(0, MATRIX_WIDTH - 8); // Posición X aleatoria
    
    for (int y = -8; y < MATRIX_HEIGHT + 8; y += 2) {
      strip.clear();
      drawCorazon(offsetX, y, strip.Color(255, 20, 147)); // Rosa/Magenta
      strip.show();
      delay(80);
    }
    delay(100);
  }
}

// Múltiples corazones moviéndose simultáneamente con diferentes colores
void corazonesSimultaneos(int numCorazones, int duracion) {
  int offsetsX[numCorazones];
  int posicionesY[numCorazones];
  uint32_t colores[numCorazones];
  
  // Colores variados para los corazones
  uint32_t paletaColores[] = {
    strip.Color(255, 20, 147),  // Rosa/Magenta
    strip.Color(255, 0, 100),   // Rosa oscuro
    strip.Color(255, 100, 150), // Rosa claro
    strip.Color(255, 50, 200),  // Rosa-púrpura
    strip.Color(255, 0, 255),   // Magenta puro
    strip.Color(200, 0, 150)    // Magenta oscuro
  };
  
  // Inicializar posiciones aleatorias más separadas
  for (int i = 0; i < numCorazones; i++) {
    offsetsX[i] = random(0, MATRIX_WIDTH - 8);
    posicionesY[i] = -8 - (i * 15); // Más separados (15 píxeles entre cada uno)
    colores[i] = paletaColores[i % 6]; // Asignar color de la paleta
  }
  
  // Animar todos simultáneamente
  for (int frame = 0; frame < duracion; frame++) {
    strip.clear();
    
    for (int i = 0; i < numCorazones; i++) {
      drawCorazon(offsetsX[i], posicionesY[i], colores[i]);
      posicionesY[i] += 2;
      
      // Reiniciar cuando sale por abajo
      if (posicionesY[i] > MATRIX_HEIGHT + 8) {
        posicionesY[i] = -8;
        offsetsX[i] = random(0, MATRIX_WIDTH - 8);
        colores[i] = paletaColores[random(0, 6)]; // Nuevo color aleatorio
      }
    }
    
    strip.show();
    delay(80);
  }
}

// Efecto de lluvia con colores arcoíris
void lluviaArcoiris(int duracion) {
  for (int frame = 0; frame < duracion; frame++) {
    // Agregar nuevas gotas aleatorias en la parte superior
    for (int i = 0; i < 3; i++) {
      int x = random(0, MATRIX_WIDTH);
      int hue = random(0, 65536);
      uint32_t color = escalarColor(strip.gamma32(strip.ColorHSV(hue)));
      int pixelIndex = getPixelIndex(x, 0);
      if (pixelIndex >= 0) {
        strip.setPixelColor(pixelIndex, color);
      }
    }
    
    // Mover todas las gotas hacia abajo
    for (int y = MATRIX_HEIGHT - 1; y > 0; y--) {
      for (int x = 0; x < MATRIX_WIDTH; x++) {
        int currentIndex = getPixelIndex(x, y);
        int aboveIndex = getPixelIndex(x, y - 1);
        if (currentIndex >= 0 && aboveIndex >= 0) {
          uint32_t colorAbove = strip.getPixelColor(aboveIndex);
          strip.setPixelColor(currentIndex, colorAbove);
        }
      }
    }
    
    strip.show();
    delay(80);
  }
}

// Efecto de estrellas parpadeando
void estrellasAleatorias(int duracion) {
  for (int frame = 0; frame < duracion; frame++) {
    strip.clear();
    
    // Dibujar estrellas aleatorias
    int numEstrellas = random(8, 15);
    for (int i = 0; i < numEstrellas; i++) {
      int x = random(0, MATRIX_WIDTH);
      int y = random(0, MATRIX_HEIGHT);
      int hue = random(0, 65536);
      uint32_t color = escalarColor(strip.gamma32(strip.ColorHSV(hue)));
      int pixelIndex = getPixelIndex(x, y);
      if (pixelIndex >= 0) {
        strip.setPixelColor(pixelIndex, color);
      }
    }
    
    strip.show();
    delay(100);
  }
}

// Onda de color que recorre la matriz
void ondaColor(int ciclos) {
  for (int ciclo = 0; ciclo < ciclos; ciclo++) {
    for (int offset = 0; offset < MATRIX_HEIGHT; offset++) {
      strip.clear();
      
      for (int y = 0; y < MATRIX_HEIGHT; y++) {
        for (int x = 0; x < MATRIX_WIDTH; x++) {
          int distancia = abs(y - offset);
          if (distancia < 4) {
            int brillo = 255 - (distancia * 64);
            int hue = (offset * 2000 + ciclo * 5000) % 65536;
            uint32_t color = escalarColor(strip.gamma32(strip.ColorHSV(hue, 255, brillo)));
            int pixelIndex = getPixelIndex(x, y);
            if (pixelIndex >= 0) {
              strip.setPixelColor(pixelIndex, color);
            }
          }
        }
      }
      
      strip.show();
      delay(50);
    }
  }
}

// Estrellas parpadeando en posiciones aleatorias
void estrellasParpadeo(int duration) {
  for (int i = 0; i < duration; i++) {
    strip.clear();
    
    // Dibujar varias estrellas en posiciones aleatorias
    int numEstrellas = random(2, 4);
    for (int e = 0; e < numEstrellas; e++) {
      int xPos = random(-4, MATRIX_WIDTH - 4);
      int yPos = random(0, MATRIX_HEIGHT - 8);
      drawSprite(spriteEstrella, xPos, yPos, strip.Color(255, 255, 0)); // Amarillo
    }
    
    strip.show();
    delay(300);
    
    strip.clear();
    strip.show();
    delay(200);
  }
}

// Fantasma flotando horizontalmente
void fantasmaFlotando(int cycles) {
  for (int cycle = 0; cycle < cycles; cycle++) {
    // Movimiento de izquierda a derecha
    int yPos = 12; // Centrado verticalmente
    
    for (int x = -8; x < MATRIX_WIDTH + 8; x++) {
      strip.clear();
      
      // Cambiar colores: azul, rosa, naranja, cyan
      uint32_t color;
      switch(cycle % 4) {
        case 0: color = strip.Color(0, 255, 255); break;   // Cyan
        case 1: color = strip.Color(255, 0, 255); break;   // Magenta
        case 2: color = strip.Color(255, 165, 0); break;   // Naranja
        case 3: color = strip.Color(255, 255, 255); break; // Blanco
      }
      
      drawSprite(spriteFantasma, x, yPos, color);
      strip.show();
      delay(60);
    }
  }
}

// Notas musicales moviéndose horizontalmente
void notasMusicales(int numNotas) {
  for (int i = 0; i < numNotas; i++) {
    int yPos = random(4, MATRIX_HEIGHT - 12); // Posición vertical aleatoria
    
    for (int x = -8; x < MATRIX_WIDTH + 8; x++) {
      strip.clear();
      
      // Colores variados para las notas
      uint32_t color;
      switch(i % 3) {
        case 0: color = strip.Color(255, 0, 255); break;   // Magenta
        case 1: color = strip.Color(0, 255, 255); break;   // Cyan
        case 2: color = strip.Color(255, 255, 0); break;   // Amarillo
      }
      
      drawSprite(spriteNota, x, yPos, color);
      strip.show();
      delay(50);
    }
    delay(50);
  }
}

// Sprite parade - desfile de personajes (horizontal)
void spriteParade() {
  const uint8_t* sprites[] = {spritePikachu, spriteMario, spriteCorazon, spriteEstrella, spriteFantasma, spriteNota};
  uint32_t colors[] = {
    strip.Color(255, 255, 0),   // Amarillo (Pikachu)
    strip.Color(255, 0, 0),     // Rojo (Mario)
    strip.Color(255, 20, 147),  // Rosa (Corazón)
    strip.Color(255, 215, 0),   // Dorado (Estrella)
    strip.Color(0, 255, 255),   // Cyan (Fantasma)
    strip.Color(255, 0, 255)    // Magenta (Nota)
  };
  
  for (int s = 0; s < 6; s++) {
    int yPos = 8 + (s % 3) * 8; // Variar posición vertical
    
    for (int x = -8; x < MATRIX_WIDTH + 8; x++) {
      strip.clear();
      drawSprite(sprites[s], x, yPos, colors[s]);
      strip.show();
      delay(50);
    }
    delay(200);
  }
}

void displayChar(char c, int offsetX, int offsetY, uint32_t color) {
  int charIndex = getCharIndex(c);
  
  // Escalar el color según el brillo general
  uint32_t scaledColor = escalarColor(color);
  
  // Orientación horizontal con columnas invertidas para evitar efecto espejo
  for (int col = 0; col < 5; col++) {
    uint8_t columnData = font5x8[charIndex][4 - col]; // Invertir columnas
    
    for (int row = 0; row < 8; row++) {
      if (columnData & (1 << row)) {
        // Mapeo directo para orientación horizontal correcta
        int newX = offsetX + row;        // row se convierte en X (horizontal)
        int newY = offsetY + col;        // col se convierte en Y (vertical)
        
        int pixelIndex = getPixelIndex(newX, newY);
        if (pixelIndex >= 1 && pixelIndex <= 256) {  // Verificar rango de matriz (LEDs 1-256)
          strip.setPixelColor(pixelIndex, scaledColor);
        }
      }
    }
  }
}

void displayText(String text, int offsetX, int offsetY, uint32_t color) {
  strip.clear();
  
  for (int i = 0; i < text.length(); i++) {
    // Cada carácter se coloca verticalmente uno debajo del otro
    int charY = offsetY + (i * 6); // 6 píxeles de separación vertical entre caracteres
    if (offsetX >= -8 && offsetX < MATRIX_WIDTH && charY >= -5 && charY + 4 < MATRIX_HEIGHT + 5) {
      displayChar(text[i], offsetX, charY, color);
    }
  }
  
  strip.show();
}

void scrollText(String text, uint32_t color, int delayTime) {
  // Barrido en el eje vertical (de arriba hacia abajo)
  // El texto entra desde arriba (posición negativa) y sale por abajo
  int textLength = text.length() * 6; // Longitud total del texto en píxeles
  
  for (int offset = -textLength; offset < MATRIX_HEIGHT + 5; offset++) {
    displayText(text, 0, offset, color); // Posición X fija, Y variable
    delay(delayTime);
  }
}

void rainbowText(String text, int delayTime) {
  static int hue = 0;
  
  // Barrido vertical de arriba hacia abajo con colores arcoíris
  int textLength = text.length() * 6; // Longitud total del texto en píxeles
  
  for (int offset = -textLength; offset < MATRIX_HEIGHT + 5; offset++) {
    strip.clear();
    
    for (int i = 0; i < text.length(); i++) {
      // Cada carácter tiene un color diferente del arcoíris con escalado
      uint32_t color = escalarColor(strip.gamma32(strip.ColorHSV((hue + i * 65536 / text.length()) % 65536)));
      int charY = offset + (i * 6); // Caracteres espaciados verticalmente
      if (charY >= -5 && charY < MATRIX_HEIGHT + 5) {
        displayChar(text[i], 0, charY, color);
      }
    }
    
    strip.show();
    delay(delayTime);
    hue += 256; // Cambia el color base
  }
}

// Función para invertir cadenas de texto automáticamente
String reverseString(String text) {
  String reversed = "";
  for (int i = text.length() - 1; i >= 0; i--) {
    reversed += text[i];
  }
  return reversed;
}

// Función auxiliar para generar colores del arcoíris
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return escalarColor(strip.Color(255 - WheelPos * 3, 0, WheelPos * 3)); // Con escalado
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return escalarColor(strip.Color(0, WheelPos * 3, 255 - WheelPos * 3)); // Con escalado
  }
  WheelPos -= 170;
  return escalarColor(strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0)); // Con escalado
}

// Efecto arcoíris completo
void rainbowCycle(int wait) {
  for (int j = 0; j < 256; j++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, Wheel((i * 256 / NUM_LEDS + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Efecto bandera mexicana horizontal (franjas horizontales correctas)
void banderaMexicana(int duration) {
  for (int cycle = 0; cycle < duration; cycle++) {
    strip.clear();
    
    // Dividir la matriz en 3 secciones horizontales para la bandera real
    // Rojo arriba, Blanco centro, Verde abajo
    // Para 32 filas: Rojo (0-10), Blanco (11-21), Verde (22-31)
    for (int y = 0; y < MATRIX_HEIGHT; y++) {
      for (int x = 0; x < MATRIX_WIDTH; x++) {
        int pixelIndex = getPixelIndex(x, y);
        if (pixelIndex >= 0) {
          // Simular el águila: área central apagada en el centro de la bandera
          bool isEagleArea = (x >= 3 && x <= 4 && y >= 14 && y <= 17);
          
          if (isEagleArea) {
            // Área del águila apagada
            strip.setPixelColor(pixelIndex, strip.Color(0, 0, 0));
          } else if (y < 11) {
            // Rojo (parte superior) - filas 0-10
            strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 0, 0)));
          } else if (y < 22) {
            // Blanco (centro) - filas 11-21
            strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 255, 255)));
          } else {
            // Verde (parte inferior) - filas 22-31
            strip.setPixelColor(pixelIndex, escalarColor(strip.Color(0, 255, 0)));
          }
        }
      }
    }
    
    strip.show();
    delay(50);
  }
}

// Efecto bandera ondeando (más realista)
void banderaOndeando(int cycles) {
  for (int cycle = 0; cycle < cycles; cycle++) {
    for (int wave = 0; wave < 16; wave++) {
      strip.clear();
      
      for (int y = 0; y < MATRIX_HEIGHT; y++) {
        for (int x = 0; x < MATRIX_WIDTH; x++) {
          int pixelIndex = getPixelIndex(x, y);
          if (pixelIndex >= 0) {
            // Crear efecto de ondeo usando función seno simulada
            int waveOffset = (x + wave) % 4; // Onda que se mueve de izquierda a derecha
            int adjustedY = y;
            
            // Ajustar posición Y basada en la onda para simular movimiento
            if (waveOffset == 1) adjustedY = (y > 0) ? y - 1 : y;
            else if (waveOffset == 3) adjustedY = (y < MATRIX_HEIGHT - 1) ? y + 1 : y;
            
            // Simular el águila: área central apagada durante el ondeo
            bool isEagleArea = (x >= 3 && x <= 4 && adjustedY >= 14 && adjustedY <= 17);
            
            if (isEagleArea) {
              // Área del águila apagada
              strip.setPixelColor(pixelIndex, strip.Color(0, 0, 0));
            } else if (adjustedY < 11) {
              // Rojo (parte superior) - filas 0-10
              strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 0, 0)));
            } else if (adjustedY < 22) {
              // Blanco (centro) - filas 11-21
              strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 255, 255)));
            } else {
              // Verde (parte inferior) - filas 22-31
              strip.setPixelColor(pixelIndex, escalarColor(strip.Color(0, 255, 0)));
            }
          }
        }
      }
      
      strip.show();
      delay(80);
    }
  }
}

// Efecto bandera ondulante suave (alternativo)
void banderaOndulante(int cycles) {
  for (int cycle = 0; cycle < cycles; cycle++) {
    for (int offset = 0; offset < 12; offset++) {
      strip.clear();
      
      for (int y = 0; y < MATRIX_HEIGHT; y++) {
        for (int x = 0; x < MATRIX_WIDTH; x++) {
          int pixelIndex = getPixelIndex(x, y);
          if (pixelIndex >= 0) {
            // Crear efecto ondulante diagonal
            int waveY = y + (x + offset) / 3;
            int colorSection = (waveY / 11) % 3; // Ajustado para 32 filas (32/3 ≈ 11)
            
            // Simular el águila en el centro
            bool isEagleArea = (x >= 3 && x <= 4 && y >= 14 && y <= 17);
            
            if (isEagleArea) {
              // Área del águila apagada
              strip.setPixelColor(pixelIndex, strip.Color(0, 0, 0));
            } else if (colorSection == 0) {
              strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 0, 0))); // Rojo
            } else if (colorSection == 1) {
              strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 255, 255))); // Blanco
            } else {
              strip.setPixelColor(pixelIndex, escalarColor(strip.Color(0, 255, 0))); // Verde
            }
          }
        }
      }
      
      strip.show();
      delay(100);
    }
  }
}

// Fuego artificial individual que puede lanzarse desde cualquier lado
void fuegoIndividual(uint32_t color) {
  // Elegir lado aleatorio para el lanzamiento
  int lado = random(0, 4); // 0=abajo, 1=arriba, 2=izquierda, 3=derecha
  int launchX, launchY, targetX, targetY;
  int dx, dy; // Dirección del movimiento
  
  // Configurar punto de inicio y destino según el lado
  switch(lado) {
    case 0: // Desde abajo hacia centro
      launchX = random(1, MATRIX_WIDTH - 1);
      launchY = MATRIX_HEIGHT - 1;
      targetX = launchX;
      targetY = random(2, 6);
      dx = 0;
      dy = -1;
      break;
    case 1: // Desde arriba hacia centro
      launchX = random(1, MATRIX_WIDTH - 1);
      launchY = 0;
      targetX = launchX;
      targetY = random(8, 14);
      dx = 0;
      dy = 1;
      break;
    case 2: // Desde izquierda hacia centro
      launchX = 0;
      launchY = random(2, MATRIX_HEIGHT - 2);
      targetX = random(3, 6);
      targetY = launchY;
      dx = 1;
      dy = 0;
      break;
    case 3: // Desde derecha hacia centro
      launchX = MATRIX_WIDTH - 1;
      launchY = random(2, MATRIX_HEIGHT - 2);
      targetX = random(2, 5);
      targetY = launchY;
      dx = -1;
      dy = 0;
      break;
  }
  
  int currentX = launchX;
  int currentY = launchY;
  
  // FASE 1: Trayectoria hacia el punto de explosión
  while (currentX != targetX || currentY != targetY) {
    strip.clear();
    
    // Dibujar la trayectoria (estela)
    for (int trail = 0; trail < 3; trail++) {
      int trailX = currentX - (dx * trail);
      int trailY = currentY - (dy * trail);
      
      if (trailX >= 0 && trailX < MATRIX_WIDTH && trailY >= 0 && trailY < MATRIX_HEIGHT) {
        int pixelIndex = getPixelIndex(trailX, trailY);
        if (pixelIndex >= 0) {
          int intensity = 255 - (trail * 80);
          if (trail == 0) {
            strip.setPixelColor(pixelIndex, escalarColor(color)); // Punto principal brillante con escalado
          } else {
            // Estela con menor intensidad con escalado
            strip.setPixelColor(pixelIndex, escalarColor(strip.Color(intensity/4, intensity/4, 0)));
          }
        }
      }
    }
    
    strip.show();
    delay(120);
    
    // Mover hacia el objetivo
    if (currentX != targetX) currentX += dx;
    if (currentY != targetY) currentY += dy;
  }
  
  // FASE 2: Explosión en el punto objetivo
  int explodeX = targetX;
  int explodeY = targetY;
  
  // Explosión en círculos concéntricos
  for (int radius = 0; radius <= 4; radius++) {
    strip.clear();
    
    // Dibujar la explosión
    for (int dx = -radius; dx <= radius; dx++) {
      for (int dy = -radius; dy <= radius; dy++) {
        // Crear patrón circular aproximado
        if (abs(dx) + abs(dy) <= radius) {
          int explodePixelX = explodeX + dx;
          int explodePixelY = explodeY + dy;
          
          if (explodePixelX >= 0 && explodePixelX < MATRIX_WIDTH && 
              explodePixelY >= 0 && explodePixelY < MATRIX_HEIGHT) {
            int pixelIndex = getPixelIndex(explodePixelX, explodePixelY);
            if (pixelIndex >= 0) {
              // Colores de explosión variados con escalado
              if (radius == 0) {
                strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 255, 255))); // Centro blanco con escalado
              } else if (radius == 1) {
                strip.setPixelColor(pixelIndex, escalarColor(color)); // Color principal con escalado
              } else if (radius == 2) {
                strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 100, 0))); // Naranja con escalado
              } else if (radius == 3) {
                strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 0, 0))); // Rojo con escalado
              } else {
                strip.setPixelColor(pixelIndex, escalarColor(strip.Color(100, 0, 0))); // Rojo oscuro con escalado
              }
            }
          }
        }
      }
    }
    
    strip.show();
    delay(150);
  }
  
  // FASE 3: Partículas cayendo desde el punto de explosión
  for (int fall = 0; fall < 6; fall++) {
    strip.clear();
    
    // Simular partículas cayendo desde el punto de explosión
    for (int particle = 0; particle < 4; particle++) {
      int particleX = explodeX + random(-2, 3);
      int particleY = explodeY + fall + random(0, 2);
      
      if (particleX >= 0 && particleX < MATRIX_WIDTH && 
          particleY >= 0 && particleY < MATRIX_HEIGHT) {
        int pixelIndex = getPixelIndex(particleX, particleY);
        if (pixelIndex >= 0) {
          // Partículas que se desvanecen con escalado
          int intensity = 255 - (fall * 40);
          if (intensity > 0) {
            strip.setPixelColor(pixelIndex, escalarColor(strip.Color(intensity/3, intensity/6, 0)));
          }
        }
      }
    }
    
    strip.show();
    delay(100);
  }
}

// Fuegos artificiales múltiples simultáneos con lanzamiento desde múltiples lados
void fuegosSimultaneos(int numFuegos) {
  // Arrays para manejar múltiples fuegos
  int launchX[numFuegos], launchY[numFuegos];
  int currentX[numFuegos], currentY[numFuegos];
  int targetX[numFuegos], targetY[numFuegos];
  int dx[numFuegos], dy[numFuegos];
  uint32_t colors[numFuegos];
  bool exploded[numFuegos];
  int explodeRadius[numFuegos];
  int fallFrame[numFuegos];
  bool finished[numFuegos];
  
  // Inicializar cada fuego con lado aleatorio
  for (int i = 0; i < numFuegos; i++) {
    int lado = random(0, 4); // 0=abajo, 1=arriba, 2=izquierda, 3=derecha
    
    // Configurar punto de inicio y destino según el lado
    switch(lado) {
      case 0: // Desde abajo hacia centro
        launchX[i] = random(1, MATRIX_WIDTH - 1);
        launchY[i] = MATRIX_HEIGHT - 1;
        targetX[i] = launchX[i];
        targetY[i] = random(2, 6);
        dx[i] = 0;
        dy[i] = -1;
        break;
      case 1: // Desde arriba hacia centro
        launchX[i] = random(1, MATRIX_WIDTH - 1);
        launchY[i] = 0;
        targetX[i] = launchX[i];
        targetY[i] = random(8, 14);
        dx[i] = 0;
        dy[i] = 1;
        break;
      case 2: // Desde izquierda hacia centro
        launchX[i] = 0;
        launchY[i] = random(2, MATRIX_HEIGHT - 2);
        targetX[i] = random(3, 6);
        targetY[i] = launchY[i];
        dx[i] = 1;
        dy[i] = 0;
        break;
      case 3: // Desde derecha hacia centro
        launchX[i] = MATRIX_WIDTH - 1;
        launchY[i] = random(2, MATRIX_HEIGHT - 2);
        targetX[i] = random(2, 5);
        targetY[i] = launchY[i];
        dx[i] = -1;
        dy[i] = 0;
        break;
    }
    
    currentX[i] = launchX[i];
    currentY[i] = launchY[i];
    exploded[i] = false;
    explodeRadius[i] = 0;
    fallFrame[i] = 0;
    finished[i] = false;
    
    // Colores aleatorios festivos
    int colorChoice = random(0, 5);
    switch(colorChoice) {
      case 0: colors[i] = strip.Color(255, 0, 0); break;    // Rojo
      case 1: colors[i] = strip.Color(0, 255, 0); break;    // Verde
      case 2: colors[i] = strip.Color(255, 200, 0); break;  // Dorado
      case 3: colors[i] = strip.Color(255, 255, 255); break; // Blanco
      case 4: colors[i] = strip.Color(255, 100, 0); break;  // Naranja
    }
  }
  
  // Animar todos los fuegos simultáneamente
  bool allFinished = false;
  while (!allFinished) {
    strip.clear();
    allFinished = true;
    
    for (int i = 0; i < numFuegos; i++) {
      if (!finished[i]) {
        allFinished = false;
        
        if (!exploded[i]) {
          // FASE 1: Trayectoria hacia el objetivo
          if (currentX[i] != targetX[i] || currentY[i] != targetY[i]) {
            // Dibujar la trayectoria (estela)
            for (int trail = 0; trail < 3; trail++) {
              int trailX = currentX[i] - (dx[i] * trail);
              int trailY = currentY[i] - (dy[i] * trail);
              
              if (trailX >= 0 && trailX < MATRIX_WIDTH && 
                  trailY >= 0 && trailY < MATRIX_HEIGHT) {
                int pixelIndex = getPixelIndex(trailX, trailY);
                if (pixelIndex >= 0) {
                  int intensity = 255 - (trail * 80);
                  if (trail == 0) {
                    strip.setPixelColor(pixelIndex, escalarColor(colors[i])); // Punto principal con escalado
                  } else {
                    // Estela amarillenta con escalado
                    strip.setPixelColor(pixelIndex, escalarColor(strip.Color(intensity/4, intensity/4, 0)));
                  }
                }
              }
            }
            
            // Mover hacia el objetivo
            if (currentX[i] != targetX[i]) currentX[i] += dx[i];
            if (currentY[i] != targetY[i]) currentY[i] += dy[i];
          } else {
            exploded[i] = true; // Comenzar explosión
          }
        } else {
          // FASE 2: Explosión
          if (explodeRadius[i] <= 4) {
            int explodeX = targetX[i];
            int explodeY = targetY[i];
            
            // Dibujar la explosión
            for (int dx_exp = -explodeRadius[i]; dx_exp <= explodeRadius[i]; dx_exp++) {
              for (int dy_exp = -explodeRadius[i]; dy_exp <= explodeRadius[i]; dy_exp++) {
                if (abs(dx_exp) + abs(dy_exp) <= explodeRadius[i]) {
                  int explodePixelX = explodeX + dx_exp;
                  int explodePixelY = explodeY + dy_exp;
                  
                  if (explodePixelX >= 0 && explodePixelX < MATRIX_WIDTH && 
                      explodePixelY >= 0 && explodePixelY < MATRIX_HEIGHT) {
                    int pixelIndex = getPixelIndex(explodePixelX, explodePixelY);
                    if (pixelIndex >= 0) {
                      // Colores de explosión variados con escalado
                      if (explodeRadius[i] == 0) {
                        strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 255, 255))); // Centro blanco con escalado
                      } else if (explodeRadius[i] == 1) {
                        strip.setPixelColor(pixelIndex, escalarColor(colors[i])); // Color principal con escalado
                      } else if (explodeRadius[i] == 2) {
                        strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 100, 0))); // Naranja con escalado
                      } else if (explodeRadius[i] == 3) {
                        strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 0, 0))); // Rojo con escalado
                      } else {
                        strip.setPixelColor(pixelIndex, escalarColor(strip.Color(100, 0, 0))); // Rojo oscuro con escalado
                      }
                    }
                  }
                }
              }
            }
            explodeRadius[i]++;
          } else {
            // FASE 3: Partículas cayendo
            if (fallFrame[i] < 6) {
              int explodeX = targetX[i];
              int explodeY = targetY[i];
              
              // Simular partículas cayendo
              for (int particle = 0; particle < 4; particle++) {
                int particleX = explodeX + random(-2, 3);
                int particleY = explodeY + fallFrame[i] + random(0, 2);
                
                if (particleX >= 0 && particleX < MATRIX_WIDTH && 
                    particleY >= 0 && particleY < MATRIX_HEIGHT) {
                  int pixelIndex = getPixelIndex(particleX, particleY);
                  if (pixelIndex >= 0) {
                    int intensity = 255 - (fallFrame[i] * 40);
                    if (intensity > 0) {
                      strip.setPixelColor(pixelIndex, escalarColor(strip.Color(intensity/3, intensity/6, 0))); // Partículas con escalado
                    }
                  }
                }
              }
              fallFrame[i]++;
            } else {
              finished[i] = true; // Este fuego ha terminado
            }
          }
        }
      }
    }
    
    strip.show();
    delay(120);
  }
}

// Fuegos artificiales múltiples (espectáculo)
void espectaculoFuegos() {
  // Espectáculo con 3 fuegos simultáneos
  fuegosSimultaneos(3);
  delay(100);
  
  // Segundo round con 2 fuegos
  fuegosSimultaneos(2);
  delay(100);
  
  // Gran final con 4 fuegos
  fuegosSimultaneos(4);
}

// Función de prueba: Encender los 4 LEDs clave
void testCuatroPuntos() {
  strip.clear();
  
  // LED 1 - Primera posición de fila 0 - ROJO
  strip.setPixelColor(1, strip.Color(255, 0, 0));
  
  // LED 8 - Última posición de fila 0 - VERDE
  strip.setPixelColor(8, strip.Color(0, 255, 0));
  
  // LED 9 - Primera posición de fila 1 (zigzag derecha) - AZUL
  strip.setPixelColor(9, strip.Color(0, 0, 255));
  
  // LED 15 - Penúltima posición de fila 1 (zigzag) - AMARILLO
  strip.setPixelColor(15, strip.Color(255, 255, 0));
  
  // LED 16 - Última posición de fila 1 (zigzag izquierda) - BLANCO
  strip.setPixelColor(16, strip.Color(255, 255, 255));
  
  strip.show();
  
  Serial.println("=== TEST 5 PUNTOS CLAVE ===");
  Serial.println("LED 1 - Fila 0, inicio: ROJO");
  Serial.println("LED 8 - Fila 0, final: VERDE");
  Serial.println("LED 9 - Fila 1, inicio (der): AZUL");
  Serial.println("LED 15 - Fila 1, penultimo: AMARILLO");
  Serial.println("LED 16 - Fila 1, final (izq): BLANCO");
  Serial.println("");
  Serial.println("Patron esperado (ZIGZAG):");
  Serial.println("Fila 0: ROJO -> -> -> -> -> -> -> VERDE (izq a der)");
  Serial.println("Fila 1: BLANCO-AMARILLO <- <- <- <- <- AZUL (der a izq)");
  Serial.println("");
  Serial.println("Verifica que colores ves y en que posiciones");
}

// Prueba de colores RGB principales al arranque
void testColoresRGB() {
  Serial.println("=== TEST COLORES RGB ===");
  
  // ROJO
  Serial.println("ROJO");
  for (int i = 1; i <= 256; i++) {
    strip.setPixelColor(i, escalarColor(strip.Color(255, 0, 0)));
  }
  strip.show();
  delay(1000);
  
  // VERDE
  Serial.println("VERDE");
  for (int i = 1; i <= 256; i++) {
    strip.setPixelColor(i, escalarColor(strip.Color(0, 255, 0)));
  }
  strip.show();
  delay(1000);
  
  // AZUL
  Serial.println("AZUL");
  for (int i = 1; i <= 256; i++) {
    strip.setPixelColor(i, escalarColor(strip.Color(0, 0, 255)));
  }
  strip.show();
  delay(1000);
  
  // Apagar todo
  strip.clear();
  strip.show();
  delay(300);
  
  Serial.println("Test RGB completado");
  Serial.println("");
}

void setup() {
  strip.begin();
  strip.setBrightness(64);
  strip.clear();
  
  // Opcional: Encender LED 0 como indicador de que el sistema está funcionando
  strip.setPixelColor(0, strip.Color(0, 50, 0)); // Verde tenue como indicador
  
  strip.show();
  
  Serial.begin(115200);
  Serial.println("=== FIESTAS PATRIAS - UNIT ELECTRONICS ===");
  Serial.println("Matriz NeoPixel 8x32 iniciada (ZIGZAG)");
  Serial.println("LED 0: Indicador de sistema (no pertenece a la matriz)");
  Serial.println("Matriz: LEDs 1-256 en patron serpentina");
  Serial.println("8 columnas x 32 filas = 256 LEDs");
  Serial.println("¡VIVA MEXICO!");
  Serial.println("");
  
  // Prueba de colores RGB al arranque
  testColoresRGB();
}

void loop() {
  // === ANIMACIONES SELECCIONADAS ===
  
  // 1. Texto "UNIT" desplazándose con color blanco
  scrollText(reverseString("UNIT Electronics"), strip.Color(255, 255, 255), 100);
  delay(500);
  
  // 2. Tres Pokébolas moviéndose simultáneamente
  int pokebolaX[3];
  int pokebolaY[3];
  
  // Inicializar posiciones
  for (int i = 0; i < 3; i++) {
    pokebolaX[i] = random(0, MATRIX_WIDTH - 8);
    pokebolaY[i] = -8 - (i * 10); // Escalonadas
  }
  
  // Animar las 3 pokébolas al mismo tiempo
  bool todasTerminaron = false;
  while (!todasTerminaron) {
    strip.clear();
    todasTerminaron = true;
    
    for (int i = 0; i < 3; i++) {
      if (pokebolaY[i] < MATRIX_HEIGHT + 8) {
        drawPokebola(pokebolaX[i], pokebolaY[i]);
        pokebolaY[i] += 2;
        todasTerminaron = false;
      }
    }
    
    strip.show();
    delay(60);
  }
  
  delay(300);
  
  // 3. Corazones moviéndose simultáneamente (6 corazones durante 60 frames)
  corazonesSimultaneos(6, 60);
  delay(300);
  
  // 4. Lluvia arcoíris
  lluviaArcoiris(50);
  delay(300);
  
  // 5. Estrellas parpadeando
  estrellasAleatorias(30);
  delay(300);
  
  // 6. Onda de color
  ondaColor(2);
  delay(300);
  
  // 7. Texto arcoíris con corrección de orientación
  rainbowText(reverseString("MEXICO"), 80);
  delay(500);
}
