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

void displayChar(char c, int offsetX, int offsetY, uint32_t color) {
  int charIndex = getCharIndex(c);
  
  // Escalar el color según el brillo general
  uint32_t scaledColor = escalarColor(color);
  
  // Orientación horizontal corregida para evitar inversión
  for (int col = 0; col < 5; col++) {
    uint8_t columnData = font5x8[charIndex][4 - col]; // Invertir el orden de las columnas
    
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
    // Verde arriba, Blanco centro, Rojo abajo
    // Para 32 filas: Verde (0-10), Blanco (11-21), Rojo (22-31)
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
            // Verde (parte superior) - filas 0-10
            strip.setPixelColor(pixelIndex, escalarColor(strip.Color(0, 255, 0)));
          } else if (y < 22) {
            // Blanco (centro) - filas 11-21
            strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 255, 255)));
          } else {
            // Rojo (parte inferior) - filas 22-31
            strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 0, 0)));
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
              // Verde (parte superior) - filas 0-10
              strip.setPixelColor(pixelIndex, escalarColor(strip.Color(0, 255, 0)));
            } else if (adjustedY < 22) {
              // Blanco (centro) - filas 11-21
              strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 255, 255)));
            } else {
              // Rojo (parte inferior) - filas 22-31
              strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 0, 0)));
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
              strip.setPixelColor(pixelIndex, escalarColor(strip.Color(0, 255, 0))); // Verde
            } else if (colorSection == 1) {
              strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 255, 255))); // Blanco
            } else {
              strip.setPixelColor(pixelIndex, escalarColor(strip.Color(255, 0, 0))); // Rojo
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
}

void loop() {
  // === TEST DE 4 PUNTOS CLAVE ===
  // Descomentar para probar los 4 LEDs clave
  testCuatroPuntos();
  delay(5000); // Mantener encendido 5 segundos
  
  /* DESCOMENTA ESTA SECCIÓN DESPUÉS DE VERIFICAR LOS 4 PUNTOS
  
  // === FIESTAS PATRIAS CON BANDERAS MEXICANAS Y FUEGOS ARTIFICIALES ===

  
  // 2. Efecto bandera mexicana horizontal estática
  banderaMexicana(40); // 2 segundos de bandera
  delay(300);
  
  // 3. Scroll "UNIT ELECTRONICS" en blanco
  scrollText(reverseString("   UNIT ELECTRONICS"), strip.Color(255, 255, 255), 45);
  fuegoIndividual(strip.Color(255, 255, 255)); // Fuego blanco
  delay(250);
  

  // 5. Scroll fecha en verde (ACTUALIZADA A 2025)
  scrollText(reverseString("   16 SEPTIEMBRE 2025"), strip.Color(0, 255, 0), 50);
  fuegoIndividual(strip.Color(0, 255, 0)); // Fuego verde
  delay(250);
  


  // 8. Scroll "VIVA MEXICO!" en verde
  scrollText(reverseString("   VIVA MEXICO!"), strip.Color(0, 255, 0), 45);
  fuegoIndividual(strip.Color(0, 255, 0)); // Fuego verde
  delay(250);
  


  // 13. ¡GRAN FINAL CON FUEGOS ARTIFICIALES!
  espectaculoFuegos();
  delay(500);
  
  */
}
