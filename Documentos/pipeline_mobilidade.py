"""
Estudo sobre Dados de Mobilidade de Lisboa
Pipeline completo — Tarefas 1 a 4 (com Rede Ciclável)
"""

import json, csv, os, statistics
from datetime import datetime
from collections import defaultdict

INPUT_GEOJSON   = "/mnt/user-data/uploads/Estacionamento_Vel_Ciclovias_245247086299410223.geojson"
INPUT_XLSX      = "/mnt/user-data/uploads/Mobilidade_Urbana_Lisboa.xlsx"
INPUT_CICLAVEL  = "/mnt/user-data/uploads/Ciclovias_-9143642382126759207.geojson"
OUT_DIR         = "/home/claude/output"
os.makedirs(OUT_DIR, exist_ok=True)

def sep(title): print(f"\n{'='*62}\n{title}\n{'='*62}")
def norm_str(v): return str(v).strip() if v else None
def safe_int(v):
    try: return int(v)
    except: return None
def safe_float(v):
    try: return float(v)
    except: return None
def parse_date(val):
    if not val: return None
    val = str(val).strip()
    for fmt in ("%m/%d/%Y %I:%M:%S %p","%a, %d %b %Y %H:%M:%S %Z","%Y-%m-%d","%d/%m/%Y"):
        try: return datetime.strptime(val, fmt).strftime("%Y-%m-%d")
        except: pass
    return val

# ═══════════════════════════════════════════════════════════════════════════════
# TAREFA 1
# ═══════════════════════════════════════════════════════════════════════════════
sep("TAREFA 1 — Exploração e leitura inicial")

CAMPOS_ESTAC = ["OBJECTID","COD_SIG_ESTACIONAMENTO","MORADA","FREGUESIA",
                "LOCALIZACAO","TIPO_ESTACIONAMENTO","MODELO","NUM_SUPORTES",
                "CAPACIDADE","COBERTO","DOMINIALIDADE","ESTACIONAMENTO_ESTADO","DATA_INSTALACAO"]
CAMPOS_MOB   = CAMPOS_ESTAC + ["x","y"]
CAMPOS_CIC   = ["OBJECTID","COD_CICLOVIA","DESIGNACAO","NOME_PROJETO","HIERARQUIA",
                "TIPOLOGIA","NIVEL_SEGREGACAO","TIPO_INTERVENCAO","SITUACAO","ANO",
                "ENTIDADE_RESP","FREGUESIA","COMPRIMENTO","COMP_KM"]

# -- GeoJSON Estacionamento --
with open(INPUT_GEOJSON, encoding="utf-8") as f: geo_raw = json.load(f)
estac_raw = []
for feat in geo_raw["features"]:
    p = feat["properties"]; lon,lat = feat["geometry"]["coordinates"]
    rec = {k: p.get(k) for k in CAMPOS_ESTAC}
    rec["lon"]=lon; rec["lat"]=lat
    estac_raw.append(rec)
print(f"GeoJSON estacionamento: {len(estac_raw)} registos")
for r in estac_raw[:5]: print(f"  [{r['OBJECTID']}] {r['FREGUESIA']} | {r['TIPO_ESTACIONAMENTO']} | cap={r['CAPACIDADE']}")

# -- XLSX Mobilidade --
import openpyxl
wb = openpyxl.load_workbook(INPUT_XLSX, read_only=True)
rows_iter = wb["Mobilidade_Urbana"].iter_rows(values_only=True)
hdr = list(next(rows_iter))
mob_raw = [{k: dict(zip(hdr,row)).get(k) for k in CAMPOS_MOB} for row in rows_iter]
print(f"XLSX mobilidade: {len(mob_raw)} registos")
for r in mob_raw[:5]: print(f"  [{r['OBJECTID']}] {r['FREGUESIA']} | {r['TIPO_ESTACIONAMENTO']} | cap={r['CAPACIDADE']}")

# -- GeoJSON Rede Ciclável --
with open(INPUT_CICLAVEL, encoding="utf-8") as f: cic_raw_geo = json.load(f)
cic_raw = []
skipped = 0
for feat in cic_raw_geo["features"]:
    if feat.get("geometry") is None:
        skipped += 1; continue
    p = feat["properties"]
    rec = {k: p.get(k) for k in CAMPOS_CIC}
    rec["geometry_coords"] = feat["geometry"]["coordinates"]
    cic_raw.append(rec)
if skipped: print(f"  (ignorados {skipped} segmentos com geometria nula)")
print(f"GeoJSON rede ciclável: {len(cic_raw)} segmentos")
for r in cic_raw[:3]: print(f"  [{r['OBJECTID']}] {r['FREGUESIA']} | {r['TIPOLOGIA']} | {r['COMP_KM']:.2f} km")

# -- Cleaned CSVs --
def write_csv(path, records, fields):
    with open(path,"w",newline="",encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=fields, extrasaction="ignore")
        w.writeheader(); w.writerows(records)

write_csv(f"{OUT_DIR}/cleaned_estacionamento.csv", estac_raw, CAMPOS_ESTAC+["lon","lat"])
write_csv(f"{OUT_DIR}/cleaned_mobilidade.csv",     mob_raw,   CAMPOS_MOB)
write_csv(f"{OUT_DIR}/cleaned_rede_ciclavel.csv",  cic_raw,   CAMPOS_CIC)
print(f"\n✓ cleaned_estacionamento.csv | cleaned_mobilidade.csv | cleaned_rede_ciclavel.csv")

# ═══════════════════════════════════════════════════════════════════════════════
# TAREFA 2
# ═══════════════════════════════════════════════════════════════════════════════
sep("TAREFA 2 — Limpeza, normalização e estatísticas")

from pyproj import Transformer
tr = Transformer.from_crs("EPSG:3857","EPSG:4326",always_xy=True)

# Normalizar estacionamento
norm_estac = []
for r in estac_raw:
    norm_estac.append({
        "objectid": safe_int(r["OBJECTID"]),
        "cod_sig":  safe_int(r["COD_SIG_ESTACIONAMENTO"]),
        "morada":   norm_str(r["MORADA"]),
        "freguesia":norm_str(r["FREGUESIA"]),
        "localizacao":       norm_str(r["LOCALIZACAO"]),
        "tipo_estacionamento":norm_str(r["TIPO_ESTACIONAMENTO"]),
        "modelo":   norm_str(r["MODELO"]) or "Desconhecido",
        "num_suportes": safe_int(r["NUM_SUPORTES"]),
        "capacidade":   safe_int(r["CAPACIDADE"]),
        "coberto": 1 if str(r.get("COBERTO","")).strip().lower()=="sim" else 0,
        "dominialidade": norm_str(r["DOMINIALIDADE"]),
        "estado":  norm_str(r["ESTACIONAMENTO_ESTADO"]),
        "data_instalacao": parse_date(r["DATA_INSTALACAO"]),
        "lon": safe_float(r["lon"]),
        "lat": safe_float(r["lat"]),
    })

# Normalizar mobilidade (reprojectar x/y)
norm_mob = []
for r in mob_raw:
    x,y = safe_float(r.get("x")), safe_float(r.get("y"))
    lon,lat = (round(v,7) for v in tr.transform(x,y)) if x and y else (None,None)
    norm_mob.append({
        "objectid": safe_int(r["OBJECTID"]),
        "cod_sig":  safe_int(r["COD_SIG_ESTACIONAMENTO"]),
        "morada":   norm_str(r["MORADA"]),
        "freguesia":norm_str(r["FREGUESIA"]),
        "localizacao":        norm_str(r["LOCALIZACAO"]),
        "tipo_estacionamento":norm_str(r["TIPO_ESTACIONAMENTO"]),
        "modelo":   norm_str(r["MODELO"]) or "Desconhecido",
        "num_suportes": safe_int(r["NUM_SUPORTES"]),
        "capacidade":   safe_int(r["CAPACIDADE"]),
        "coberto": 1 if str(r.get("COBERTO","")).strip().lower()=="sim" else 0,
        "dominialidade": norm_str(r["DOMINIALIDADE"]),
        "estado":  norm_str(r["ESTACIONAMENTO_ESTADO"]),
        "data_instalacao": parse_date(r["DATA_INSTALACAO"]),
        "lon": lon, "lat": lat,
    })

# Normalizar rede ciclável
norm_cic = []
for r in cic_raw:
    norm_cic.append({
        "objectid":         safe_int(r["OBJECTID"]),
        "cod_ciclovia":     norm_str(r["COD_CICLOVIA"]),
        "designacao":       norm_str(r["DESIGNACAO"]),
        "nome_projeto":     norm_str(r["NOME_PROJETO"]),
        "hierarquia":       norm_str(r["HIERARQUIA"]),
        "tipologia":        norm_str(r["TIPOLOGIA"]),
        "nivel_segregacao": norm_str(r["NIVEL_SEGREGACAO"]),
        "tipo_intervencao": norm_str(r["TIPO_INTERVENCAO"]),
        "situacao":         norm_str(r["SITUACAO"]),
        "ano":              norm_str(r["ANO"]),
        "entidade_resp":    norm_str(r["ENTIDADE_RESP"]),
        "freguesia":        norm_str(r["FREGUESIA"]),
        "comprimento_m":    safe_float(r["COMPRIMENTO"]),
        "comp_km":          round(safe_float(r["COMP_KM"]),5) if r["COMP_KM"] else None,
        "geometry_coords":  r["geometry_coords"],
    })

write_csv(f"{OUT_DIR}/normalized_estacionamento.csv", norm_estac, list(norm_estac[0].keys()))
write_csv(f"{OUT_DIR}/normalized_mobilidade.csv",     norm_mob,   list(norm_mob[0].keys()))
cic_fields = [k for k in norm_cic[0].keys() if k!="geometry_coords"]
write_csv(f"{OUT_DIR}/normalized_rede_ciclavel.csv",  norm_cic,   cic_fields)
print(f"✓ normalized_estacionamento.csv | normalized_mobilidade.csv | normalized_rede_ciclavel.csv")

# Estatísticas gerais
caps     = [r["capacidade"] for r in norm_estac if r["capacidade"]]
suportes = [r["num_suportes"] for r in norm_estac if r["num_suportes"]]
km_freg  = defaultdict(float)
seg_freg = defaultdict(int)
for r in norm_cic:
    if r["freguesia"] and r["comp_km"]:
        km_freg[r["freguesia"]] += r["comp_km"]
        seg_freg[r["freguesia"]] += 1

hist_bins = [(1,2),(3,4),(5,10),(11,20),(21,50),(51,1000)]
hist = {f"{lo}-{hi}": sum(1 for c in caps if lo<=c<=hi) for lo,hi in hist_bins}

with open(f"{OUT_DIR}/estatisticas.csv","w",newline="",encoding="utf-8") as f:
    w = csv.writer(f)
    w.writerow(["metrica","valor"])
    for row in [
        ("total_estacionamentos",len(norm_estac)),
        ("capacidade_soma",sum(caps)),
        ("capacidade_media",round(statistics.mean(caps),2)),
        ("capacidade_mediana",statistics.median(caps)),
        ("capacidade_min",min(caps)),("capacidade_max",max(caps)),
        ("suportes_soma",sum(suportes)),
        ("suportes_media",round(statistics.mean(suportes),2)),
        ("total_segmentos_ciclavel",len(norm_cic)),
        ("km_ciclavel_total",round(sum(km_freg.values()),2)),
        ("",""),("contagem_por_freguesia",""),
    ]: w.writerow(row)
    freq_count = defaultdict(int)
    for r in norm_estac:
        if r["freguesia"]: freq_count[r["freguesia"]] += 1
    for freg,cnt in sorted(freq_count.items()): w.writerow([freg,cnt])
    w.writerow(["",""]); w.writerow(["histograma_capacidade",""])
    for label,cnt in hist.items(): w.writerow([label,cnt])
print(f"✓ estatisticas.csv")

# ═══════════════════════════════════════════════════════════════════════════════
# TAREFA 3
# ═══════════════════════════════════════════════════════════════════════════════
sep("TAREFA 3 — Estatísticas e análises auxiliares")

freg_data = defaultdict(lambda:{"n":0,"cap_total":0,"sup_total":0,"caps":[]})
for r in norm_estac:
    freg = r.get("freguesia") or "Desconhecida"
    d = freg_data[freg]
    d["n"] += 1
    d["cap_total"] += r["capacidade"] or 0
    d["sup_total"]  += r["num_suportes"] or 0
    if r["capacidade"]: d["caps"].append(r["capacidade"])

total = len(norm_estac)

# estatisticas_locais.csv — inclui km_rede_ciclavel e densidade
fields_loc = ["freguesia","n_estacionamentos","capacidade_total","num_suportes_total",
              "capacidade_media","km_rede_ciclavel","n_segmentos_ciclavel",
              "estac_por_km_ciclavel","capacidade_por_km_ciclavel"]
with open(f"{OUT_DIR}/estatisticas_locais.csv","w",newline="",encoding="utf-8") as f:
    w = csv.DictWriter(f, fieldnames=fields_loc)
    w.writeheader()
    for freg, d in sorted(freg_data.items()):
        km  = round(km_freg.get(freg,0),3)
        cap = d["cap_total"]
        w.writerow({
            "freguesia":                freg,
            "n_estacionamentos":        d["n"],
            "capacidade_total":         cap,
            "num_suportes_total":       d["sup_total"],
            "capacidade_media":         round(statistics.mean(d["caps"]),2) if d["caps"] else 0,
            "km_rede_ciclavel":         km,
            "n_segmentos_ciclavel":     seg_freg.get(freg,0),
            "estac_por_km_ciclavel":    round(d["n"]/km,2) if km>0 else None,
            "capacidade_por_km_ciclavel": round(cap/km,2) if km>0 else None,
        })
print(f"✓ estatisticas_locais.csv")

sorted_freg = sorted(freg_data.items(), key=lambda x: x[1]["cap_total"], reverse=True)

# top_10_freguesias_por_capacidade.csv
with open(f"{OUT_DIR}/top_10_freguesias_por_capacidade.csv","w",newline="",encoding="utf-8") as f:
    w = csv.writer(f)
    w.writerow(["rank","freguesia","capacidade_total","n_estacionamentos","km_ciclavel"])
    for i,(freg,d) in enumerate(sorted_freg[:10],1):
        w.writerow([i,freg,d["cap_total"],d["n"],round(km_freg.get(freg,0),2)])
print(f"✓ top_10_freguesias_por_capacidade.csv")

# distribuicao_modelos.csv
mod_count = defaultdict(int)
for r in norm_estac: mod_count[r["modelo"]] += 1
with open(f"{OUT_DIR}/distribuicao_modelos.csv","w",newline="",encoding="utf-8") as f:
    w=csv.writer(f); w.writerow(["modelo","count","pct"])
    for mod,cnt in sorted(mod_count.items(),key=lambda x:-x[1]):
        w.writerow([mod,cnt,round(cnt/total*100,1)])
print(f"✓ distribuicao_modelos.csv")

# distribuicao_coberto.csv
cob = defaultdict(int)
for r in norm_estac: cob["Coberto" if r["coberto"] else "Descoberto"] += 1
with open(f"{OUT_DIR}/distribuicao_coberto.csv","w",newline="",encoding="utf-8") as f:
    w=csv.writer(f); w.writerow(["tipo","count","pct"])
    for k,cnt in cob.items(): w.writerow([k,cnt,round(cnt/total*100,1)])
print(f"✓ distribuicao_coberto.csv")

# distribuicao_tipologia_ciclavel.csv
tip_km = defaultdict(float)
tip_n  = defaultdict(int)
for r in norm_cic:
    if r["tipologia"] and r["comp_km"]:
        tip_km[r["tipologia"]] += r["comp_km"]
        tip_n[r["tipologia"]]  += 1
tot_km = sum(tip_km.values())
with open(f"{OUT_DIR}/distribuicao_tipologia_ciclavel.csv","w",newline="",encoding="utf-8") as f:
    w=csv.writer(f); w.writerow(["tipologia","n_segmentos","km_total","pct_km"])
    for tip,km in sorted(tip_km.items(),key=lambda x:-x[1]):
        w.writerow([tip,tip_n[tip],round(km,3),round(km/tot_km*100,1)])
print(f"✓ distribuicao_tipologia_ciclavel.csv")

# histograma_capacidade.csv
with open(f"{OUT_DIR}/histograma_capacidade.csv","w",newline="",encoding="utf-8") as f:
    w=csv.writer(f); w.writerow(["intervalo_capacidade","count"])
    for label,cnt in hist.items(): w.writerow([label,cnt])
print(f"✓ histograma_capacidade.csv")

# GeoJSON estacionamento normalizado
geo_estac = {"type":"FeatureCollection","features":[]}
for r in norm_estac:
    if r["lon"] is None: continue
    geo_estac["features"].append({
        "type":"Feature",
        "geometry":{"type":"Point","coordinates":[r["lon"],r["lat"]]},
        "properties":{k:v for k,v in r.items() if k not in ("lon","lat")}
    })
with open(f"{OUT_DIR}/normalized_estacionamento.geojson","w",encoding="utf-8") as f:
    json.dump(geo_estac,f,ensure_ascii=False,indent=2)
print(f"✓ normalized_estacionamento.geojson ({len(geo_estac['features'])} pontos)")

# GeoJSON rede ciclável normalizada
geo_cic = {"type":"FeatureCollection","features":[]}
for r in norm_cic:
    props = {k:v for k,v in r.items() if k!="geometry_coords"}
    geo_cic["features"].append({
        "type":"Feature",
        "geometry":{"type":"LineString","coordinates":r["geometry_coords"]},
        "properties":props
    })
with open(f"{OUT_DIR}/normalized_rede_ciclavel.geojson","w",encoding="utf-8") as f:
    json.dump(geo_cic,f,ensure_ascii=False,indent=2)
print(f"✓ normalized_rede_ciclavel.geojson ({len(geo_cic['features'])} segmentos)")

# ═══════════════════════════════════════════════════════════════════════════════
# TAREFA 4
# ═══════════════════════════════════════════════════════════════════════════════
sep("TAREFA 4 — Agregação por freguesia")

agg_fields = ["freguesia","n_estacionamentos","capacidade_total","num_suportes_total",
              "capacidade_media","km_rede_ciclavel","n_segmentos_ciclavel",
              "estac_por_km_ciclavel","capacidade_por_km_ciclavel"]

agg_rows = []
for freg,d in sorted(freg_data.items()):
    km  = round(km_freg.get(freg,0),3)
    cap = d["cap_total"]
    agg_rows.append({
        "freguesia":                  freg,
        "n_estacionamentos":          d["n"],
        "capacidade_total":           cap,
        "num_suportes_total":         d["sup_total"],
        "capacidade_media":           round(statistics.mean(d["caps"]),2) if d["caps"] else 0,
        "km_rede_ciclavel":           km,
        "n_segmentos_ciclavel":       seg_freg.get(freg,0),
        "estac_por_km_ciclavel":      round(d["n"]/km,2) if km>0 else None,
        "capacidade_por_km_ciclavel": round(cap/km,2) if km>0 else None,
    })

write_csv(f"{OUT_DIR}/agregacao_freguesia.csv", agg_rows, agg_fields)
print(f"✓ agregacao_freguesia.csv ({len(agg_rows)} freguesias)")

# ranking por capacidade_total DESC
with open(f"{OUT_DIR}/ranking_freguesias.csv","w",newline="",encoding="utf-8") as f:
    w=csv.writer(f)
    w.writerow(["rank","freguesia","capacidade_total","n_estacionamentos",
                "capacidade_media","km_rede_ciclavel","capacidade_por_km_ciclavel"])
    for i,(freg,d) in enumerate(sorted_freg,1):
        km  = round(km_freg.get(freg,0),2)
        cap = d["cap_total"]
        w.writerow([i,freg,cap,d["n"],
                    round(statistics.mean(d["caps"]),2) if d["caps"] else 0,
                    km, round(cap/km,2) if km>0 else None])
print(f"✓ ranking_freguesias.csv")

# README
with open(f"{OUT_DIR}/README.md","w",encoding="utf-8") as f:
    f.write("# Mobilidade Urbana Lisboa — Pipeline de Dados\n\n")
    f.write("## Ficheiros de Entrada\n| Ficheiro | Tipo | Registos |\n|---|---|---|\n")
    f.write(f"| Estacionamento GeoJSON | Pontos WGS84 | {len(estac_raw)} |\n")
    f.write(f"| Mobilidade XLSX | Tabela | {len(mob_raw)} |\n")
    f.write(f"| Ciclovias GeoJSON | LineString WGS84 | {len(cic_raw)} |\n\n")
    f.write("## Campos Removidos — Estacionamento\n")
    f.write("| Campo | Motivo |\n|---|---|\n")
    for row in [
        ("COD_SIG","Redundante com COD_SIG_ESTACIONAMENTO"),
        ("MORADA_DETALHE","201 nulos — informação secundária"),
        ("BALIZADORES","820 nulos (37%) — irrelevante para análise de capacidade"),
        ("OBS","1 775 nulos (80%) — texto livre sem estrutura"),
        ("EQUIPAMENTO_SERVIDO","1 430 nulos (65%) — campo muito incompleto"),
        ("FONTE_LEVANTAMENTO","Metadado interno sem valor analítico"),
        ("GlobalID","Redundante com OBJECTID"),
    ]: f.write(f"| {row[0]} | {row[1]} |\n")
    f.write("\n## Campos Removidos — Rede Ciclável\n")
    f.write("| Campo | Motivo |\n|---|---|\n")
    for row in [
        ("COD_SIG / COD_VIA","Identificadores de sistema internos"),
        ("EIXO","Maioritariamente nulo"),
        ("IDTIPO","Código interno sem valor analítico"),
        ("ZONAMENTO","Redundante face ao campo FREGUESIA"),
        ("GlobalID","Redundante com OBJECTID"),
    ]: f.write(f"| {row[0]} | {row[1]} |\n")
    f.write("\n## Decisões de Normalização\n")
    f.write("- Datas → ISO 8601 (YYYY-MM-DD)\n- COBERTO → 0/1\n")
    f.write("- MODELO nulo → 'Desconhecido'\n")
    f.write("- Coordenadas XLSX (EPSG:3857) → WGS84 via pyproj\n")
    f.write("- GeoJSON já em WGS84\n")
    f.write("- `estac_por_km_ciclavel` e `capacidade_por_km_ciclavel` calculadas por freguesia\n\n")
    f.write("## Ficheiros Gerados\n| Ficheiro | Tarefa | Descrição |\n|---|---|---|\n")
    for row in [
        ("cleaned_estacionamento.csv","T1","Campos selecionados do GeoJSON estacionamento"),
        ("cleaned_mobilidade.csv","T1","Campos selecionados do XLSX"),
        ("cleaned_rede_ciclavel.csv","T1","Campos selecionados do GeoJSON ciclovias"),
        ("normalized_estacionamento.csv","T2","Estacionamento normalizado"),
        ("normalized_mobilidade.csv","T2","Mobilidade normalizada (reprojectada WGS84)"),
        ("normalized_rede_ciclavel.csv","T2","Rede ciclável normalizada"),
        ("estatisticas.csv","T2","Estatísticas descritivas gerais"),
        ("estatisticas_locais.csv","T3","Métricas por freguesia + km ciclável + densidade"),
        ("top_10_freguesias_por_capacidade.csv","T3","Top 10 por capacidade total"),
        ("distribuicao_modelos.csv","T3","Distribuição de modelos de suporte"),
        ("distribuicao_coberto.csv","T3","Coberto vs Descoberto"),
        ("distribuicao_tipologia_ciclavel.csv","T3","km por tipologia de ciclovia"),
        ("histograma_capacidade.csv","T3","Histograma de capacidade"),
        ("normalized_estacionamento.geojson","T3","GeoJSON pontos normalizado"),
        ("normalized_rede_ciclavel.geojson","T3","GeoJSON linhas ciclável normalizado"),
        ("agregacao_freguesia.csv","T4","Agregação completa por freguesia"),
        ("ranking_freguesias.csv","T4","Ranking por capacidade total"),
    ]: f.write(f"| {row[0]} | {row[1]} | {row[2]} |\n")

print(f"✓ README.md")
sep("Pipeline concluído — /home/claude/output/")
