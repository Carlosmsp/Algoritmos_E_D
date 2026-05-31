#pragma once
#include <string>
#include <vector>
#include <optional>

// ─────────────────────────────────────────────────────────────────────────────
//  Registo de estacionamento de velocípedes (GeoJSON → ponto WGS84)
// ─────────────────────────────────────────────────────────────────────────────
struct Estacionamento {
    int         objectid        = 0;
    int         cod_sig         = 0;
    std::string morada;
    std::string freguesia;
    std::string localizacao;
    std::string tipo_estacionamento;   // BICICLETA | TROTINETE | HOTSPOT
    std::string modelo;                // tipo de suporte físico
    int         num_suportes    = 0;
    int         capacidade      = 0;
    bool        coberto         = false;
    std::string dominialidade;
    std::string estado;
    std::string data_instalacao;       // normalizado para DD/MM/YYYY
    double      lon             = 0.0;
    double      lat             = 0.0;
};

// ─────────────────────────────────────────────────────────────────────────────
//  Registo de mobilidade (CSV / XLSX exportado como CSV)
//  Coordenadas em Web Mercator (EPSG:3857) → convertidas para WGS84
// ─────────────────────────────────────────────────────────────────────────────
struct Mobilidade {
    int         objectid        = 0;
    int         cod_sig         = 0;
    std::string morada;
    std::string freguesia;
    std::string localizacao;
    std::string tipo_estacionamento;
    std::string modelo;
    int         num_suportes    = 0;
    int         capacidade      = 0;
    bool        coberto         = false;
    std::string dominialidade;
    std::string estado;
    std::string data_instalacao;
    double      x               = 0.0;   // coordenada Web Mercator original (EPSG:3857)
    double      y               = 0.0;   // coordenada Web Mercator original (EPSG:3857)
    double      lon             = 0.0;   // convertido de x (EPSG:3857)
    double      lat             = 0.0;   // convertido de y (EPSG:3857)
};

// ─────────────────────────────────────────────────────────────────────────────
//  Segmento da rede ciclável (GeoJSON → LineString WGS84)
// ─────────────────────────────────────────────────────────────────────────────
struct CicloVia {
    int         objectid        = 0;
    std::string cod_ciclovia;
    std::string designacao;             // nome da via
    std::string nome_projeto;
    std::string hierarquia;             // Principal | Complementar | Local
    std::string tipologia;              // Pista Bidirecional | 30+Bici | …
    std::string nivel_segregacao;
    std::string tipo_intervencao;       // Definitiva | Provisória
    std::string situacao;               // Executado
    std::string ano;
    std::string entidade_resp;
    std::string freguesia;
    double      comprimento_m   = 0.0;
    double      comp_km         = 0.0;
    // Geometria: lista de pontos [lon, lat]
    std::vector<std::pair<double,double>> coords;
};

// ─────────────────────────────────────────────────────────────────────────────
//  Agregação por freguesia — produzida nas Tarefas 3 e 4
// ─────────────────────────────────────────────────────────────────────────────
struct FreguesiaStat {
    std::string freguesia;
    int         n_estacionamentos       = 0;
    int         capacidade_total        = 0;
    int         num_suportes_total      = 0;
    double      capacidade_media        = 0.0;
    int         n_mobilidade            = 0;
    int         capacidade_mobilidade   = 0;
    int         num_suportes_mobilidade = 0;
    double      capacidade_media_mob    = 0.0;
    double      km_rede_ciclavel        = 0.0;
    int         n_segmentos_ciclavel    = 0;
    double      estac_por_km_ciclavel   = 0.0;  // 0 se sem rede
    double      cap_por_km_ciclavel     = 0.0;  // 0 se sem rede
    double      lon_media               = 0.0;
    double      lat_media               = 0.0;

    // campos auxiliares — não escritos diretamente em CSV
    std::vector<int> capacidades;
    double      lon_soma                = 0.0;
    double      lat_soma                = 0.0;
    int         n_coords                = 0;
};
