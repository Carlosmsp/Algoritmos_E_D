# Mobilidade Urbana Lisboa

Projeto em C++17 para o CBL de Algoritmos e Estruturas de Dados.

## Entradas

- `data/estacionamento_velocipedes.geojson`
- `data/mobilidade_urbana.csv`
- `data/rede_ciclavel.geojson` opcional

## Como compilar

```bash
cmake -S . -B build
cmake --build build
```

## Como executar

```bash
./build/mobilidade ./data ./output
```

No Windows:

```bash
.\build\mobilidade.exe .\data .\output
```

Executar novamente sobrescreve os ficheiros da pasta `output`.

## O que o programa gera

- Ficheiros `cleaned_*.csv` com colunas selecionadas.
- Ficheiros `normalized_*.csv` com campos padronizados.
- `estatisticas.csv` com totais, capacidade e histograma.
- `estatisticas_locais.csv` e `agregacao_freguesia.csv` por freguesia.
- `ranking_freguesias.csv` ordenado por capacidade total.
- Análises auxiliares: modelos, coberto, histograma e top 10.
- GeoJSON normalizados e `agregacao_freguesia.geojson`.

## Decisões de limpeza

- Foram removidos campos muito incompletos ou metadados internos, como `OBS`, `GlobalID`, `FONTE_LEVANTAMENTO`, `BALIZADORES` e campos redundantes.
- Datas são normalizadas para formato português `DD/MM/YYYY`, assumindo dia/mês/ano quando há ambiguidade.
- Coordenadas da mobilidade em EPSG:3857 são convertidas para WGS84.
- `MODELO` vazio passa para `Desconhecido`.
- `COBERTO` passa para booleano/0-1 nos ficheiros normalizados.
