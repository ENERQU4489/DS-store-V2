#include <nds.h>
#include <dswifi9.h>
#include <fat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <string>

// Konfiguracja
char server_ip[16] = "10.42.0.1"; // Domyslne IP hotspotu Linuxa (NetworkManager)
#define SERVER_PORT 8000

enum AppState { STATE_INIT, STATE_LIST, STATE_DETAILS, STATE_DOWNLOADING };
AppState currentState = STATE_INIT;

struct Game {
    int id;
    std::string title;
    std::string description;
    std::string iconUrl;
    std::string romUrl;
    std::string size;
};

std::vector<Game> games;
int selectedGameIdx = 0;

// Prosty parser JSON dla NDS (reczne wyciaganie pol)
std::string getJsonValue(const std::string& json, const std::string& key) {
    size_t keyPos = json.find("\"" + key + "\"");
    if (keyPos == std::string::npos) return "";
    size_t valPos = json.find(":", keyPos) + 1;
    while(json[valPos] == ' ' || json[valPos] == '\"') valPos++;
    size_t endPos = json.find_first_of("\",}", valPos);
    return json.substr(valPos, endPos - valPos);
}

void fetchGamesList();
void downloadFile(const char* url, const char* filename);

void drawList() {
    consoleClear();
    iprintf("\x1b[32;1m      NDS STORE - LISTA      \x1b[39m\n");
    iprintf("============================\n");
    if (games.empty()) {
        iprintf("\n   Brak gier na serwerze...");
    } else {
        for (int i = 0; i < (int)games.size(); i++) {
            if (i == selectedGameIdx) iprintf("> \x1b[47;30m %-24s \x1b[0m\n", games[i].title.c_str());
            else iprintf("  [ ] %-24s \n", games[i].title.c_str());
        }
    }
    iprintf("\x1b[23;0H(START) IP  (A) Detale  (B) Odswiez");
}

void drawDetails() {
    consoleClear();
    if (selectedGameIdx >= (int)games.size()) return;
    Game& g = games[selectedGameIdx];
    iprintf("\x1b[32;1m      SZCZEGOLY GRY      \x1b[39m\n");
    iprintf("============================\n");
    iprintf("Tytul: %s\n", g.title.c_str());
    iprintf("Rozmiar: %s\n", g.size.c_str());
    iprintf("\nOpis: %s\n", g.description.c_str());
    iprintf("\n\n\n\n\n\n\n\n\x1b[23;0H(A) ZAINSTALUJ        (B) ANULUJ");
}

int main(void) {
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);
    
    consoleDemoInit(); // Gora
    PrintConsole subConsole;
    consoleInit(&subConsole, 1, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
    consoleSelect(&subConsole); // Dol

    if (!fatInitDefault()) iprintf("SD Error!\n");

    iprintf("Laczenie z WiFi...\n");
    if (Wifi_InitDefault(WFC_CONNECT)) {
        currentState = STATE_LIST;
        fetchGamesList();
    } else {
        iprintf("WiFi Fail! Sprawdz WFC.");
    }

    while(1) {
        swiWaitForVBlank();
        scanKeys();
        int keys = keysDown();

        if (currentState == STATE_LIST) {
            drawList();
            if (keys & KEY_UP && selectedGameIdx > 0) selectedGameIdx--;
            if (keys & KEY_DOWN && selectedGameIdx < (int)games.size()-1) selectedGameIdx++;
            if (keys & KEY_A && !games.empty()) currentState = STATE_DETAILS;
            if (keys & KEY_B) fetchGamesList();
            if (keys & KEY_START) {
                // Tu mozna by dodac klawiature do zmiany IP
            }
        } 
        else if (currentState == STATE_DETAILS) {
            drawDetails();
            if (keys & KEY_B) currentState = STATE_LIST;
            if (keys & KEY_A) {
                downloadFile(games[selectedGameIdx].romUrl.c_str(), (games[selectedGameIdx].title + ".nds").c_str());
                currentState = STATE_LIST;
            }
        }

        if (keys & KEY_SELECT) break;
    }
    return 0;
}

void fetchGamesList() {
    int sock;
    struct sockaddr_in server;
    char buffer[4096];
    
    games.clear();
    sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) return;

    const char* req = "GET /api/games HTTP/1.1\r\nHost: ds\r\nConnection: close\r\n\r\n";
    send(sock, req, strlen(req), 0);

    std::string response;
    int bytes;
    while((bytes = recv(sock, buffer, sizeof(buffer)-1, 0)) > 0) {
        buffer[bytes] = 0;
        response += buffer;
    }
    close(sock);

    // Szukanie poczatku JSONa
    size_t start = response.find("[");
    if (start == std::string::npos) return;
    std::string json = response.substr(start);

    // Bardzo proste dzielenie na obiekty {}
    size_t pos = 0;
    while((pos = json.find("{", pos)) != std::string::npos) {
        size_t end = json.find("}", pos);
        std::string obj = json.substr(pos, end - pos + 1);
        Game g;
        g.title = getJsonValue(obj, "title");
        g.description = getJsonValue(obj, "description");
        g.romUrl = getJsonValue(obj, "url");
        g.size = getJsonValue(obj, "size");
        games.push_back(g);
        pos = end;
    }
}

void downloadFile(const char* url, const char* filename) {
    int sock;
    struct sockaddr_in server;
    char buffer[1024];
    char req[256];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) return;

    sprintf(req, "GET %s HTTP/1.1\r\nHost: ds\r\nConnection: close\r\n\r\n", url);
    send(sock, req, strlen(req), 0);

    FILE* f = fopen(filename, "wb");
    bool body = false;
    int bytes;
    while((bytes = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        if (!body) {
            char* p = strstr(buffer, "\r\n\r\n");
            if (p) {
                body = true;
                fwrite(p+4, 1, bytes - (p+4-buffer), f);
            }
        } else {
            fwrite(buffer, 1, bytes, f);
        }
    }
    fclose(f);
    close(sock);
}
