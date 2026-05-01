# NDS Game Store 🎮

Własny sklep z grami na Nintendo DS. Składa się z serwera (FastAPI) oraz klienta (.nds) napisanego w C++.

## 🚀 Jak to działa?
1. **Serwer** skanuje folder `/roms`, generuje listę gier w JSON i wystawia pliki przez HTTP.
2. **Klient NDS** łączy się przez WiFi, pobiera listę gier, wyświetla interfejs i pozwala pobrać wybraną grę bezpośrednio na kartę SD.

## 🛠️ Instalacja Serwera
Wymagany Python 3.10+.

```bash
cd server
pip install fastapi uvicorn
# Wrzuć pliki .nds do folderu /roms
python3 main.py
```

## 🏗️ Kompilacja Klienta (GitHub Actions)
Nie musisz instalować nic u siebie!
1. Wypchnij folder `client` na nowe repozytorium GitHub.
2. Wejdź w zakładkę **Actions**.
3. Po zakończeniu budowania pobierz gotowy plik `NDSStore.nds` z sekcji **Artifacts**.

## 🔌 Konfiguracja WiFi
Aplikacja korzysta z ustawień WiFi zapisanych w konsoli (WFC).
* **Fizyczny DS:** Skonfiguruj połączenie w dowolnej oryginalnej grze Nintendo z obsługą WiFi (np. Mario Kart DS). Obsługiwane są tylko stare standardy (WEP lub sieć otwarta).
* **melonDS:** Obsługuje emulację WiFi "out of the box".

## 📝 Uwagi
* Zmień `SERVER_IP` w `source/main.cpp` na adres swojego komputera w sieci lokalnej przed kompilacją.
* Interfejs: Góra/Dół (Wybór), (A) Szczegóły/Pobierz, (B) Powrót.

---
*Projekt stworzony z pomocą Gemini CLI.*
