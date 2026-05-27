#!/usr/bin/env python3
"""
setup_data.py — Prepara a pasta data/ para o executável C++
  1. Converte Mobilidade_Urbana_Lisboa.xlsx → data/mobilidade_urbana.csv
  2. Copia / renomeia os GeoJSON para os nomes esperados pelo programa

Utilização:
    python3 setup_data.py <pasta_com_ficheiros_originais>
"""

import sys, shutil, os

try:
    import openpyxl
except ImportError:
    print("Instalar: pip install openpyxl"); sys.exit(1)

SRC = sys.argv[1] if len(sys.argv) > 1 else "."
DST = os.path.join(os.path.dirname(__file__), "data")
os.makedirs(DST, exist_ok=True)

# ── 1. XLSX → CSV ─────────────────────────────────────────────────────────────
xlsx_path = os.path.join(SRC, "Mobilidade_Urbana_Lisboa.xlsx")
if os.path.exists(xlsx_path):
    wb  = openpyxl.load_workbook(xlsx_path, read_only=True)
    ws  = wb["Mobilidade_Urbana"]
    rows = list(ws.iter_rows(values_only=True))
    out = os.path.join(DST, "mobilidade_urbana.csv")
    import csv
    with open(out, "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        for row in rows:
            w.writerow([str(v) if v is not None else "" for v in row])
    print(f"✓ mobilidade_urbana.csv  ({len(rows)-1} linhas de dados)")
else:
    print(f"[AVISO] Não encontrado: {xlsx_path}")

# ── 2. Copiar GeoJSON ─────────────────────────────────────────────────────────
files = {
    "estacionamento_velocipedes.geojson": [
        "Estacionamento_Vel_Ciclovias_245247086299410223.geojson",
    ],
    "rede_ciclavel.geojson": [
        "Ciclovias_-9143642382126759207.geojson",
    ],
}
for dst_name, candidates in files.items():
    for cand in candidates:
        src = os.path.join(SRC, cand)
        if os.path.exists(src):
            shutil.copy2(src, os.path.join(DST, dst_name))
            print(f"✓ {dst_name}  ← {cand}")
            break
    else:
        print(f"[AVISO] Não encontrado para: {dst_name}")

print("\nPasta data/ pronta. Agora compila e executa:")
print("  mkdir build && cd build")
print("  cmake .. && cmake --build .")
print("  ./mobilidade ../data ../output")
