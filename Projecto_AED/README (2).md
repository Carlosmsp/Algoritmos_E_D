# Mobilidade Urbana Lisboa — C++

Implementação em C++17 do pipeline de análise de dados de mobilidade urbana de Lisboa,
desenvolvida no âmbito da disciplina de **Algoritmos e Estruturas de Dados** (IADE).

---

## Estrutura do Projeto

```
mobilidade_lisboa/
├── CMakeLists.txt
├── setup_data.py          ← converte XLSX → CSV e prepara data/
├── include/
│   └── nlohmann/
│       └── json.hpp       ← biblioteca JSON (header-only, MIT)
├── src/
│   ├── main.cpp           ← ponto de entrada, orquestra as 4 tarefas
│   ├── models.h           ← structs: Estacionamento, Mobilidade, CicloVia, FreguesiaStat
│   ├── utils.h            ← funções utilitárias (trim, datas, projeção EPSG:3857→4326)
│   ├── csv_reader.h       ← leitura de CSV (tokenizador próprio, sem dependências)
│   ├── geojson_reader.h   ← leitura de GeoJSON com nlohmann/json
│   ├── statistics.h       ← agregação (std::map), ordenação (std::sort), médias, medianas
│   └── writer.h           ← escrita de CSV e GeoJSON de saída
├── data/                  ← criada por setup_data.py
└── output/                ← criada pelo executável
```

---

## Pré-requisitos

| Ferramenta | Versão mínima |
|---|---|
| g++ / clang++ | C++17 (GCC ≥ 7, Clang ≥ 5) |
| CMake | 3.16 |
| Python 3 + openpyxl | para o setup_data.py |

---

## Setup e Compilação

### 1. Preparar os dados

Coloca os ficheiros originais numa pasta e corre o script de setup:

```bash
python3 setup_data.py /caminho/para/ficheiros_originais
```

Isto converte o XLSX para CSV e copia os GeoJSON para `data/` com os nomes esperados.

### 2. Compilar com CMake

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### 3. (Alternativa) Compilação direta com g++

```bash
g++ -std=c++17 -O2 -Iinclude -Isrc -o mobilidade src/main.cpp
```

### 4. Executar

```bash
./mobilidade ../data ../output
# ou com caminhos custom:
./mobilidade /caminho/dados /caminho/saida
```

---

## Ficheiros de Entrada Esperados

| Nome em `data/` | Origem |
|---|---|
| `estacionamento_velocipedes.geojson` | GeoJSON pontos (WGS84) |
| `mobilidade_urbana.csv` | XLSX exportado por setup_data.py |
| `rede_ciclavel.geojson` | GeoJSON linhas (WGS84) — **opcional** |

---

## Campos Removidos

### Estacionamento GeoJSON
| Campo | Motivo |
|---|---|
| COD_SIG | Redundante com COD_SIG_ESTACIONAMENTO |
| MORADA_DETALHE | 201 nulos |
| BALIZADORES | 820 nulos (37%) |
| OBS | 1 775 nulos (80%) |
| EQUIPAMENTO_SERVIDO | 1 430 nulos (65%) |
| FONTE_LEVANTAMENTO | Metadado interno |
| GlobalID | Redundante com OBJECTID |

### Rede Ciclável GeoJSON
| Campo | Motivo |
|---|---|
| COD_SIG / COD_VIA | Identificadores de sistema |
| EIXO | Maioritariamente nulo |
| IDTIPO | Código interno |
| ZONAMENTO | Redundante face a FREGUESIA |
| GlobalID | Redundante com OBJECTID |

---

## Decisões de Normalização (Tarefa 2)

- **Datas** → ISO 8601 (`YYYY-MM-DD`), reconhece formatos Excel e PT
- **COBERTO** → `bool` (true = Sim)
- **MODELO** nulo → `"Desconhecido"`
- **Coordenadas XLSX** (EPSG:3857 Web Mercator) → WGS84 calculado em `utils::mercatorToWGS84()`
- **Coordenadas GeoJSON** → já em WGS84, lidas diretamente

---

## Ficheiros de Saída

| Ficheiro | Tarefa |
|---|---|
| `cleaned_estacionamento.csv` | T1 |
| `cleaned_rede_ciclavel.csv` | T1 |
| `estatisticas.csv` | T2 |
| `estatisticas_locais.csv` | T3 |
| `top_10_freguesias_por_capacidade.csv` | T3 |
| `distribuicao_modelos.csv` | T3 |
| `distribuicao_coberto.csv` | T3 |
| `histograma_capacidade.csv` | T3 |
| `distribuicao_tipologia_ciclavel.csv` | T3 |
| `normalized_estacionamento.geojson` | T3 |
| `normalized_rede_ciclavel.geojson` | T3 |
| `agregacao_freguesia.csv` | T4 |
| `ranking_freguesias.csv` | T4 |

---

## Estruturas de Dados e Algoritmos Utilizados

| Conceito | Onde |
|---|---|
| `std::vector<Estacionamento>` | armazenamento principal de registos |
| `std::map<string, FreguesiaStat>` | agregação por freguesia (BST ordenada) |
| `std::sort` com comparador lambda | ranking por capacidade (Tarefa 4) |
| `std::min_element` / `std::max_element` | estatísticas de capacidade |
| Mediana com vector ordenado | `utils::median()` |
| CSV tokenizador manual | `csv_reader::parseLine()` — trata aspas e vírgulas internas |
| Projeção cartográfica | `utils::mercatorToWGS84()` — fórmula analítica EPSG:3857→4326 |
