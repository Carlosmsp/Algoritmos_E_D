# Mobilidade Urbana Lisboa — Pipeline de Dados

## Ficheiros de Entrada
| Ficheiro | Tipo | Registos |
|---|---|---|
| Estacionamento GeoJSON | Pontos WGS84 | 2209 |
| Mobilidade XLSX | Tabela | 2214 |
| Ciclovias GeoJSON | LineString WGS84 | 1025 |

## Campos Removidos — Estacionamento
| Campo | Motivo |
|---|---|
| COD_SIG | Redundante com COD_SIG_ESTACIONAMENTO |
| MORADA_DETALHE | 201 nulos — informação secundária |
| BALIZADORES | 820 nulos (37%) — irrelevante para análise de capacidade |
| OBS | 1 775 nulos (80%) — texto livre sem estrutura |
| EQUIPAMENTO_SERVIDO | 1 430 nulos (65%) — campo muito incompleto |
| FONTE_LEVANTAMENTO | Metadado interno sem valor analítico |
| GlobalID | Redundante com OBJECTID |

## Campos Removidos — Rede Ciclável
| Campo | Motivo |
|---|---|
| COD_SIG / COD_VIA | Identificadores de sistema internos |
| EIXO | Maioritariamente nulo |
| IDTIPO | Código interno sem valor analítico |
| ZONAMENTO | Redundante face ao campo FREGUESIA |
| GlobalID | Redundante com OBJECTID |

## Decisões de Normalização
- Datas → ISO 8601 (YYYY-MM-DD)
- COBERTO → 0/1
- MODELO nulo → 'Desconhecido'
- Coordenadas XLSX (EPSG:3857) → WGS84 via pyproj
- GeoJSON já em WGS84
- `estac_por_km_ciclavel` e `capacidade_por_km_ciclavel` calculadas por freguesia

