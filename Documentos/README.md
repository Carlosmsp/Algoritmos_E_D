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

## Ficheiros Gerados
| Ficheiro | Tarefa | Descrição |
|---|---|---|
| cleaned_estacionamento.csv | T1 | Campos selecionados do GeoJSON estacionamento |
| cleaned_mobilidade.csv | T1 | Campos selecionados do XLSX |
| cleaned_rede_ciclavel.csv | T1 | Campos selecionados do GeoJSON ciclovias |
| normalized_estacionamento.csv | T2 | Estacionamento normalizado |
| normalized_mobilidade.csv | T2 | Mobilidade normalizada (reprojectada WGS84) |
| normalized_rede_ciclavel.csv | T2 | Rede ciclável normalizada |
| estatisticas.csv | T2 | Estatísticas descritivas gerais |
| estatisticas_locais.csv | T3 | Métricas por freguesia + km ciclável + densidade |
| top_10_freguesias_por_capacidade.csv | T3 | Top 10 por capacidade total |
| distribuicao_modelos.csv | T3 | Distribuição de modelos de suporte |
| distribuicao_coberto.csv | T3 | Coberto vs Descoberto |
| distribuicao_tipologia_ciclavel.csv | T3 | km por tipologia de ciclovia |
| histograma_capacidade.csv | T3 | Histograma de capacidade |
| normalized_estacionamento.geojson | T3 | GeoJSON pontos normalizado |
| normalized_rede_ciclavel.geojson | T3 | GeoJSON linhas ciclável normalizado |
| agregacao_freguesia.csv | T4 | Agregação completa por freguesia |
| ranking_freguesias.csv | T4 | Ranking por capacidade total |
