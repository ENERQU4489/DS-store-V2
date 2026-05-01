from fastapi import FastAPI, HTTPException
from fastapi.staticfiles import StaticFiles
import json
import os

app = FastAPI(title="NDS Store API")

# Katalogi
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
ROM_DIR = os.path.join(BASE_DIR, "roms")
ICON_DIR = os.path.join(BASE_DIR, "icons")

# Tworzenie katalogow jesli nie istnieja
os.makedirs(ROM_DIR, exist_ok=True)
os.makedirs(ICON_DIR, exist_ok=True)

app.mount("/icons", StaticFiles(directory=ICON_DIR), name="icons")
app.mount("/roms", StaticFiles(directory=ROM_DIR), name="roms")

@app.get("/api/games")
async def get_games():
    games = []
    # Skanowanie folderu ROMS
    for idx, filename in enumerate(os.listdir(ROM_DIR)):
        if filename.endswith(".nds"):
            game_id = idx + 1
            title = filename.replace(".nds", "").replace("_", " ").title()
            
            # Sprawdzanie czy istnieje ikona o tej samej nazwie
            icon_name = filename.replace(".nds", ".bmp")
            icon_url = f"/icons/{icon_name}" if os.path.exists(os.path.join(ICON_DIR, icon_name)) else "/icons/default.bmp"
            
            games.append({
                "id": game_id,
                "title": title,
                "description": f"Gra DS: {title}",
                "icon": icon_url,
                "url": f"/roms/{filename}",
                "size": f"{os.path.getsize(os.path.join(ROM_DIR, filename)) // (1024*1024)}MB"
            })
    return games

@app.get("/")
async def root():
    return {"status": "NDS Store Online", "msg": "Wrzuc pliki .nds do folderu /roms"}

if __name__ == "__main__":
    import uvicorn
    # Odpalenie serwera na porcie 8000
    uvicorn.run(app, host="0.0.0.0", port=8000)
