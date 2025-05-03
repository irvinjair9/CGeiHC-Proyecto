#include <windows.h>
#include "musica.h"
#pragma comment(lib, "winmm.lib")

void reproducirMusica() {
    PlaySound(TEXT("sounds/Fondo_Feria.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
}
