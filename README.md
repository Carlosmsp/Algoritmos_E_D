# Mobilidade Urbana Lisboa

Projeto desenvolvido em C++17 para o CBL de **Algoritmos e Estruturas de Dados**.

O objetivo é ler, limpar, normalizar e analisar dados de mobilidade urbana de Lisboa, com foco em estacionamento de velocípedes, dados de mobilidade associados e, de forma opcional, rede ciclável.

## Estrutura Principal

```text
Algoritmos_E_D/
├── Documentos/
│   └── Briefing - Mobilidade Urbana - CBL.pdf
└── Projecto_AED/
    ├── CMakeLists.txt
    ├── setup_data.py
    ├── data/
    ├── output/
    ├── include/
    │   └── nlohmann/json.hpp
    └── src/
        ├── main.cpp
        ├── models.h
        ├── utils.h
        ├── csv_reader.h
        ├── geojson_reader.h
        ├── statistics.h
        └── writer.h
```

## Requisitos

- Compilador C++ com suporte a C++17
- CMake 3.16 ou superior
- Python 3 com `openpyxl`, apenas se for necessário regenerar a pasta `data/` a partir dos ficheiros originais

## Como Compilar

A partir da pasta `Algoritmos_E_D/Projecto_AED`:

```bash
cmake -S . -B build
cmake --build build
```

No Windows, o executável fica em:

```text
build/mobilidade.exe
```

## Como Executar

A partir da pasta `Algoritmos_E_D/Projecto_AED`:

```bash
./build/mobilidade ./data ./output
```

No Windows:

```powershell
.\build\mobilidade.exe .\data .\output
```

Executar novamente sobrescreve os ficheiros da pasta `output/`.

## Dados de Entrada

O programa lê os ficheiros preparados em `Projecto_AED/data/`:

```text
data/estacionamento_velocipedes.geojson
data/mobilidade_urbana.csv
data/rede_ciclavel.geojson
```

A rede ciclável é opcional no briefing, mas está incluída no projeto como análise adicional.

## Preparação dos Dados

O script `Projecto_AED/setup_data.py` é auxiliar. Ele converte o ficheiro XLSX original para CSV e copia os GeoJSON para a pasta `data/` com os nomes esperados pelo programa C++.

Exemplo:

```bash
python setup_data.py ficheiros_originais
```

O projeto final, porém, é executado pelo programa C++.

## Tarefas do Briefing

### Tarefa 1: Exploração, seleção de campos e leitura inicial

O programa:

- lê os ficheiros de estacionamento, mobilidade e rede ciclável;
- guarda os registos em `std::vector` de estruturas;
- imprime o número de registos e os primeiros registos resumidos;
- gera ficheiros `cleaned_*.csv` com as colunas selecionadas.

### Tarefa 2: Limpeza, normalização e estatísticas

O programa:

- normaliza datas para formato português `DD/MM/YYYY`;
- converte coordenadas da mobilidade de EPSG:3857 para WGS84;
- normaliza campos como `COBERTO` e `MODELO`;
- gera ficheiros `normalized_*.csv`;
- gera `estatisticas.csv` com totais, capacidade, média, mediana, contagem por freguesia e histograma.

### Tarefa 3: Estatísticas sobre locais de estacionamento

O programa gera:

- `estatisticas_locais.csv`;
- `top_10_freguesias_por_capacidade.csv`;
- `distribuicao_modelos.csv`;
- `distribuicao_coberto.csv`;
- `histograma_capacidade.csv`;
- `distribuicao_tipologia_ciclavel.csv`;
- GeoJSON normalizados e agregado por freguesia.

### Tarefa 4: Agregação por freguesia

O programa gera:

- `agregacao_freguesia.csv`;
- `ranking_freguesias.csv`;
- `agregacao_freguesia.geojson`.

O ranking é ordenado por `capacidade_total`, da maior para a menor.

## Ficheiros Gerados

Os principais resultados ficam em `Projecto_AED/output/`:

```text
cleaned_estacionamento.csv
cleaned_mobilidade.csv
cleaned_rede_ciclavel.csv
normalized_estacionamento.csv
normalized_mobilidade.csv
normalized_rede_ciclavel.csv
estatisticas.csv
estatisticas_locais.csv
top_10_freguesias_por_capacidade.csv
distribuicao_modelos.csv
distribuicao_coberto.csv
histograma_capacidade.csv
distribuicao_tipologia_ciclavel.csv
agregacao_freguesia.csv
ranking_freguesias.csv
normalized_estacionamento.geojson
normalized_rede_ciclavel.geojson
agregacao_freguesia.geojson
```

## Estruturas e Algoritmos Utilizados

- `std::vector` para armazenar os registos lidos.
- `std::map` para agregação por freguesia.
- `std::sort` para ordenar o ranking.
- Funções auxiliares para média, mediana, soma e conversão de coordenadas.
- Leitura de GeoJSON com `nlohmann/json`.
- Escrita de CSV e GeoJSON pelo próprio programa C++.

