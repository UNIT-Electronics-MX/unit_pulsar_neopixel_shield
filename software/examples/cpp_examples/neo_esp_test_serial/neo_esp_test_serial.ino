#include <Adafruit_NeoPixel.h>

#define PIN        8
#define NUM_LEDS   257  // Total: 1 LED indicador (0) + 256 LEDs de matriz (1-256)
#define MATRIX_WIDTH  8    // 8 columnas
#define MATRIX_HEIGHT 32   // 32 filas

Adafruit_NeoPixel strip(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  
  strip.begin();
  strip.setBrightness(64);
  strip.clear();
  
  // LED 0 como indicador
  
  strip.setPixelColor(0, strip.Color(0, 50, 0));
  strip.show();
  
  Serial.println("\n\n=== PRUEBA INTERACTIVA LED MATRIZ 8x32 ===");
  Serial.println("LED 0: Indicador (verde tenue)");
  Serial.println("Matriz: LEDs 1-256 (8 columnas x 32 filas)\n");
  
  printMenu();
}

void printMenu() {
  Serial.println("\n--- MENU DE COMANDOS ---");
  Serial.println("L<num>        - Encender LED individual (ej: L1, L64, L256)");
  Serial.println("F<num>        - Encender fila completa (ej: F0, F1, F31)");
  Serial.println("C             - Limpiar todos los LEDs");
  Serial.println("T             - Test 4 esquinas (LEDs 1,8,9,16)");
  Serial.println("A             - Encender TODOS los LEDs en ROJO");
  Serial.println("S             - Secuencia 1-256 uno por uno");
  Serial.println("R<inicio>-<fin> - Rango de LEDs (ej: R1-8)");
  Serial.println("V             - Verificar LEDs 9-16 uno por uno");
  Serial.println("M             - Mostrar este menu");
  Serial.println("\nColores: ROJO por defecto");
  Serial.println("----------------------------------------\n");
}

void encenderLED(int ledNum) {
  if (ledNum < 0 || ledNum > 256) {
    Serial.println("ERROR: LED debe estar entre 0 y 256");
    return;
  }
  
  strip.clear();
  // Mantener LED 0 encendido
  strip.setPixelColor(0, strip.Color(0, 50, 0));
  
  Serial.print("DEBUG: Encendiendo LED fisico ");
  Serial.println(ledNum);
  
  // Encender el LED solicitado en ROJO
  strip.setPixelColor(ledNum, strip.Color(255, 0, 0));
  strip.show();
  
  Serial.print("LED ");
  Serial.print(ledNum);
  Serial.println(" encendido (ROJO)");
}

void encenderFila(int fila) {
  if (fila < 0 || fila >= MATRIX_HEIGHT) {
    Serial.print("ERROR: Fila debe estar entre 0 y ");
    Serial.println(MATRIX_HEIGHT - 1);
    return;
  }
  
  strip.clear();
  // Mantener LED 0 encendido
  strip.setPixelColor(0, strip.Color(0, 50, 0));
  
  // Calcular LEDs de la fila (sin offset porque son LEDs físicos directos)
  int startLED = (fila * MATRIX_WIDTH) + 1; // +1 porque LED 0 es indicador
  
  Serial.print("Fila ");
  Serial.print(fila);
  Serial.print(" - LEDs fisicos: ");
  Serial.print(startLED);
  Serial.print(" a ");
  Serial.println(startLED + MATRIX_WIDTH - 1);
  
  // Encender los 8 LEDs de la fila con colores alternados
  for (int i = 0; i < MATRIX_WIDTH; i++) {
    int ledNum = startLED + i;
    // Alternar colores para identificar cada LED
    if (i == 0) {
      strip.setPixelColor(ledNum, strip.Color(255, 0, 0)); // ROJO - primero
      Serial.print("  LED ");
      Serial.print(ledNum);
      Serial.println(": ROJO (primero)");
    } else if (i == 7) {  // Último LED (columna 7)
      strip.setPixelColor(ledNum, strip.Color(0, 255, 0)); // VERDE - último
      Serial.print("  LED ");
      Serial.print(ledNum);
      Serial.println(": VERDE (ultimo)");
    } else {
      strip.setPixelColor(ledNum, strip.Color(0, 0, 255)); // AZUL - intermedios
    }
  }
  
  strip.show();
}

void limpiarTodo() {
  strip.clear();
  strip.setPixelColor(0, strip.Color(0, 50, 0));
  strip.show();
  Serial.println("Todos los LEDs apagados (excepto indicador)");
}

void encenderTodos() {
  strip.clear();
  strip.setPixelColor(0, strip.Color(0, 50, 0)); // Mantener indicador
  
  // Encender todos los LEDs de la matriz (1-256) en ROJO
  for (int i = 1; i <= 256; i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));
  }
  
  strip.show();
  Serial.println("\n=== TODOS LOS LEDs ENCENDIDOS EN ROJO ===");
  Serial.println("LEDs 1-256 encendidos");
  Serial.println("Verifica que todos los LEDs fisicos esten encendidos");
}

void testEsquinas() {
  strip.clear();
  strip.setPixelColor(0, strip.Color(0, 50, 0));
  
  // Fila 0 - primera y última columna
  strip.setPixelColor(1, strip.Color(255, 0, 0));    // LED 1 - ROJO (esquina superior izquierda)
  strip.setPixelColor(8, strip.Color(0, 255, 0));    // LED 8 - VERDE (esquina superior derecha)
  
  // Fila 1 - primera y última columna  
  strip.setPixelColor(9, strip.Color(0, 0, 255));    // LED 9 - AZUL (fila 1, inicio)
  strip.setPixelColor(16, strip.Color(255, 255, 0)); // LED 16 - AMARILLO (fila 1, final)
  
  strip.show();
  
  Serial.println("\n=== TEST 4 PUNTOS CLAVE ===");
  Serial.println("LED 1:  ROJO     (Fila 0, columna 0 - inicio)");
  Serial.println("LED 8:  VERDE    (Fila 0, columna 7 - final)");
  Serial.println("LED 9:  AZUL     (Fila 1, columna 0 - inicio)");
  Serial.println("LED 16: AMARILLO (Fila 1, columna 7 - final)");
  Serial.println("\nPatron zigzag esperado:");
  Serial.println("Fila 0: ROJO -> -> -> VERDE (izq a der, LEDs 1-8)");
  Serial.println("Fila 1: AMARILLO <- <- <- AZUL (der a izq, LEDs 9-16)");
}

void secuencia() {
  Serial.println("\nIniciando secuencia 1-256...");
  Serial.println("Presiona cualquier tecla para detener\n");
  
  for (int i = 1; i <= 256; i++) {
    if (Serial.available() > 0) {
      Serial.read(); // Leer y descartar
      Serial.println("\nSecuencia detenida");
      break;
    }
    
    strip.clear();
    strip.setPixelColor(0, strip.Color(0, 50, 0));
    strip.setPixelColor(i, strip.Color(255, 0, 0));
    strip.show();
    
    Serial.print("LED ");
    Serial.println(i);
    delay(200);
  }
  
  Serial.println("Secuencia completada");
}

void encenderRango(int inicio, int fin) {
  if (inicio < 1 || fin > 256 || inicio > fin) {
    Serial.println("ERROR: Rango invalido (debe ser 1-256)");
    return;
  }
  
  strip.clear();
  strip.setPixelColor(0, strip.Color(0, 50, 0));
  
  for (int i = inicio; i <= fin; i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));
  }
  
  strip.show();
  
  Serial.print("LEDs ");
  Serial.print(inicio);
  Serial.print(" a ");
  Serial.print(fin);
  Serial.println(" encendidos (ROJO)");
}

void verificarProblema() {
  Serial.println("\n=== VERIFICACION LEDs 9-16 (Fila 1) ===");
  Serial.println("Encendiendo cada LED individualmente...");
  Serial.println("Observa DONDE se enciende fisicamente cada LED:\n");
  
  for (int i = 9; i <= 16; i++) {
    strip.clear();
    strip.setPixelColor(0, strip.Color(0, 50, 0));
    strip.setPixelColor(i, strip.Color(255, 0, 0));
    strip.show();
    
    Serial.print(">>> LED FISICO ");
    Serial.print(i);
    Serial.println(" ENCENDIDO <<<");
    Serial.println("Anota la posicion fisica en tu matriz (fila/columna)");
    Serial.println("Presiona Enter para continuar...\n");
    
    // Esperar input del usuario
    while (Serial.available() == 0) {
      delay(10);
    }
    // Limpiar buffer
    while (Serial.available() > 0) {
      Serial.read();
    }
  }
  
  Serial.println("\n=== Verificacion completada ===");
  Serial.println("Ahora compara las posiciones fisicas con lo esperado:");
  Serial.println("Fila 1 deberia ser: LEDs 9-16 (en zigzag: 16,15,14,13,12,11,10,9)");
  limpiarTodo();
}

void procesarComando(String comando) {
  comando.trim();
  comando.toUpperCase();
  
  Serial.print("DEBUG: Comando recibido: [");
  Serial.print(comando);
  Serial.print("], Longitud: ");
  Serial.println(comando.length());
  
  if (comando.length() == 0) {
    Serial.println("DEBUG: Comando vacio");
    return;
  }
  
  char cmd = comando.charAt(0);
  
  Serial.print("DEBUG: Primer caracter: [");
  Serial.print(cmd);
  Serial.println("]");
  
  switch(cmd) {
    case 'L': {
      // Encender LED individual: L64
      int ledNum = comando.substring(1).toInt();
      Serial.print("DEBUG: Numero parseado del substring: ");
      Serial.println(ledNum);
      encenderLED(ledNum);
      break;
    }
    
    case 'F': {
      // Encender fila: F0, F1, etc
      int fila = comando.substring(1).toInt();
      encenderFila(fila);
      break;
    }
    
    case 'C': {
      // Limpiar todo
      limpiarTodo();
      break;
    }
    
    case 'T': {
      // Test 4 esquinas
      testEsquinas();
      break;
    }
    
    case 'A': {
      // Encender TODOS los LEDs
      encenderTodos();
      break;
    }
    
    case 'S': {
      // Secuencia completa
      secuencia();
      break;
    }
    
    case 'R': {
      // Rango: R1-16
      int guionPos = comando.indexOf('-');
      if (guionPos > 0) {
        int inicio = comando.substring(1, guionPos).toInt();
        int fin = comando.substring(guionPos + 1).toInt();
        encenderRango(inicio, fin);
      } else {
        Serial.println("ERROR: Formato incorrecto. Usar R<inicio>-<fin>");
      }
      break;
    }
    
    case 'V': {
      // Verificar problema LEDs 9-17
      verificarProblema();
      break;
    }
    
    case 'M': {
      // Mostrar menu
      printMenu();
      break;
    }
    
    default:
      Serial.println("Comando no reconocido. Escribe M para ver el menu");
      break;
  }
}

void loop() {
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    procesarComando(comando);
  }
}
