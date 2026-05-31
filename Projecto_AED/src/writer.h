#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <map>
#include "../include/nlohmann/json.hpp"
#include "models.h"
#include "utils.h"
#include "statistics.h"

namespace writer {

using json = nlohmann::json;

// Macro interna para escrever uma linha CSV de forma legível
#define CSV_ROW(os, ...) { \
    std::vector<std::string> _f = {__VA_ARGS__}; \
    for (size_t _i=0;_i<_f.size();++_i) { \
        if (_i) os << ','; \
        os << utils::csvEscape(_f[_i]); \
    } \
    os << '\n'; \
}

// ─────────────────────────────────────────────────────────────────────────────
//  Tarefa 1 — CSV limpos
// ─────────────────────────────────────────────────────────────────────────────

inline void writeCleaned_Estacionamento(const std::vector<Estacionamento>& data,
                                        const std::string& path) {
    std::ofstream f(path);
    CSV_ROW(f, "OBJECTID","COD_SIG_ESTACIONAMENTO","MORADA","FREGUESIA","LOCALIZACAO",
               "TIPO_ESTACIONAMENTO","MODELO","NUM_SUPORTES","CAPACIDADE",
               "COBERTO","DOMINIALIDADE","ESTACIONAMENTO_ESTADO",
               "DATA_INSTALACAO","lon","lat");
    for (const auto& e : data)
        CSV_ROW(f,
            std::to_string(e.objectid), std::to_string(e.cod_sig),
            e.morada, e.freguesia, e.localizacao,
            e.tipo_estacionamento, e.modelo,
            std::to_string(e.num_suportes), std::to_string(e.capacidade),
            e.coberto?"Sim":"Não", e.dominialidade, e.estado,
            e.data_instalacao,
            utils::fmtDouble(e.lon,7), utils::fmtDouble(e.lat,7));
    std::cout << "[T1] cleaned_estacionamento.csv (" << data.size() << " linhas)\n";
}

inline void writeCleaned_Mobilidade(const std::vector<Mobilidade>& data,
                                    const std::string& path) {
    std::ofstream f(path);
    CSV_ROW(f, "OBJECTID","COD_SIG_ESTACIONAMENTO","MORADA","FREGUESIA","LOCALIZACAO",
               "TIPO_ESTACIONAMENTO","MODELO","NUM_SUPORTES","CAPACIDADE",
               "COBERTO","DOMINIALIDADE","ESTACIONAMENTO_ESTADO",
               "DATA_INSTALACAO","x","y");
    for (const auto& e : data)
        CSV_ROW(f,
            std::to_string(e.objectid), std::to_string(e.cod_sig),
            e.morada, e.freguesia, e.localizacao,
            e.tipo_estacionamento, e.modelo,
            std::to_string(e.num_suportes), std::to_string(e.capacidade),
            e.coberto?"Sim":"Não", e.dominialidade, e.estado,
            e.data_instalacao,
            utils::fmtDouble(e.x,7), utils::fmtDouble(e.y,7));
    std::cout << "[T1] cleaned_mobilidade.csv (" << data.size() << " linhas)\n";
}

inline void writeCleaned_CicloVia(const std::vector<CicloVia>& data,
                                   const std::string& path) {
    std::ofstream f(path);
    CSV_ROW(f,"OBJECTID","COD_CICLOVIA","DESIGNACAO","NOME_PROJETO","HIERARQUIA",
              "TIPOLOGIA","NIVEL_SEGREGACAO","TIPO_INTERVENCAO","SITUACAO","ANO",
              "ENTIDADE_RESP","FREGUESIA","COMPRIMENTO","COMP_KM");
    for (const auto& c : data)
        CSV_ROW(f,
            std::to_string(c.objectid), c.cod_ciclovia, c.designacao,
            c.nome_projeto, c.hierarquia, c.tipologia, c.nivel_segregacao,
            c.tipo_intervencao, c.situacao, c.ano, c.entidade_resp,
            c.freguesia,
            utils::fmtDouble(c.comprimento_m,2), utils::fmtDouble(c.comp_km,5));
    std::cout << "[T1] cleaned_rede_ciclavel.csv (" << data.size() << " linhas)\n";
}

inline void writeNormalizedEstacionamento(const std::vector<Estacionamento>& data,
                                          const std::string& path) {
    std::ofstream f(path);
    CSV_ROW(f,"objectid","cod_sig","morada","freguesia","localizacao",
              "tipo_estacionamento","modelo","num_suportes","capacidade",
              "coberto","dominialidade","estado","data_instalacao","lon","lat");
    for (const auto& e : data)
        CSV_ROW(f,
            std::to_string(e.objectid), std::to_string(e.cod_sig),
            e.morada, e.freguesia, e.localizacao,
            e.tipo_estacionamento, e.modelo,
            std::to_string(e.num_suportes), std::to_string(e.capacidade),
            e.coberto ? "1" : "0", e.dominialidade, e.estado,
            e.data_instalacao,
            utils::fmtDouble(e.lon,7), utils::fmtDouble(e.lat,7));
    std::cout << "[T2] normalized_estacionamento.csv (" << data.size() << " linhas)\n";
}

inline void writeNormalizedMobilidade(const std::vector<Mobilidade>& data,
                                      const std::string& path) {
    std::ofstream f(path);
    CSV_ROW(f,"objectid","cod_sig","morada","freguesia","localizacao",
              "tipo_estacionamento","modelo","num_suportes","capacidade",
              "coberto","dominialidade","estado","data_instalacao","lon","lat");
    for (const auto& e : data)
        CSV_ROW(f,
            std::to_string(e.objectid), std::to_string(e.cod_sig),
            e.morada, e.freguesia, e.localizacao,
            e.tipo_estacionamento, e.modelo,
            std::to_string(e.num_suportes), std::to_string(e.capacidade),
            e.coberto ? "1" : "0", e.dominialidade, e.estado,
            e.data_instalacao,
            utils::fmtDouble(e.lon,7), utils::fmtDouble(e.lat,7));
    std::cout << "[T2] normalized_mobilidade.csv (" << data.size() << " linhas)\n";
}

inline void writeNormalizedCicloVia(const std::vector<CicloVia>& data,
                                    const std::string& path) {
    std::ofstream f(path);
    CSV_ROW(f,"objectid","cod_ciclovia","designacao","nome_projeto","hierarquia",
              "tipologia","nivel_segregacao","tipo_intervencao","situacao","ano",
              "entidade_resp","freguesia","comprimento_m","comp_km");
    for (const auto& c : data)
        CSV_ROW(f,
            std::to_string(c.objectid), c.cod_ciclovia, c.designacao,
            c.nome_projeto, c.hierarquia, c.tipologia, c.nivel_segregacao,
            c.tipo_intervencao, c.situacao, c.ano, c.entidade_resp,
            c.freguesia,
            utils::fmtDouble(c.comprimento_m,2), utils::fmtDouble(c.comp_km,5));
    std::cout << "[T2] normalized_rede_ciclavel.csv (" << data.size() << " linhas)\n";
}

// ─────────────────────────────────────────────────────────────────────────────
//  Tarefa 2 — Estatísticas
// ─────────────────────────────────────────────────────────────────────────────

inline void writeEstatisticas(const std::vector<Estacionamento>& estac,
                               const std::vector<Mobilidade>& mob,
                               const std::vector<CicloVia>& ciclovias,
                               const std::string& path) {
    std::vector<int> caps, suportes;
    std::map<std::string, int> freq_count;
    for (const auto& e : estac) {
        if (e.capacidade  > 0) caps.push_back(e.capacidade);
        if (e.num_suportes > 0) suportes.push_back(e.num_suportes);
        freq_count[e.freguesia.empty()?"Desconhecida":e.freguesia]++;
    }
    double km_total = 0.0;
    for (const auto& c : ciclovias) km_total += c.comp_km;

    std::ofstream f(path);
    f << "metrica,valor\n";
    f << "total_estacionamentos," << estac.size() << "\n";
    f << "total_mobilidade,"      << mob.size() << "\n";
    f << "capacidade_soma,"       << utils::sum(caps) << "\n";
    f << "capacidade_media,"      << utils::fmtDouble(utils::mean(caps),2) << "\n";
    f << "capacidade_mediana,"    << utils::fmtDouble(utils::median(caps),1) << "\n";
    f << "capacidade_min,"        << *std::min_element(caps.begin(),caps.end()) << "\n";
    f << "capacidade_max,"        << *std::max_element(caps.begin(),caps.end()) << "\n";
    f << "suportes_soma,"         << utils::sum(suportes) << "\n";
    f << "suportes_media,"        << utils::fmtDouble(utils::mean(suportes),2) << "\n";
    f << "total_segmentos_ciclavel," << ciclovias.size() << "\n";
    f << "km_ciclavel_total,"     << utils::fmtDouble(km_total,2) << "\n";
    f << ",\ncontagem_por_freguesia,\n";
    for (const auto& [freg, cnt] : freq_count) f << freg << "," << cnt << "\n";

    // Histograma
    f << ",\nhistograma_capacidade,count\n";
    auto hist = statistics::buildHistogram(estac);
    for (const auto& b : hist) f << b.label << "," << b.count << "\n";

    std::cout << "[T2] estatisticas.csv\n";
}

// ─────────────────────────────────────────────────────────────────────────────
//  Tarefa 3 — Análises auxiliares
// ─────────────────────────────────────────────────────────────────────────────

inline void writeEstatisticasLocais(const std::vector<FreguesiaStat>& stats,
                                    const std::string& path) {
    std::ofstream f(path);
    CSV_ROW(f,"freguesia","n_estacionamentos","capacidade_total",
              "num_suportes_total","capacidade_media",
              "n_mobilidade","capacidade_mobilidade",
              "num_suportes_mobilidade","capacidade_media_mobilidade",
              "km_rede_ciclavel","n_segmentos_ciclavel",
              "estac_por_km_ciclavel","capacidade_por_km_ciclavel");
    for (const auto& s : stats)
        CSV_ROW(f,
            s.freguesia,
            std::to_string(s.n_estacionamentos),
            std::to_string(s.capacidade_total),
            std::to_string(s.num_suportes_total),
            utils::fmtDouble(s.capacidade_media,2),
            std::to_string(s.n_mobilidade),
            std::to_string(s.capacidade_mobilidade),
            std::to_string(s.num_suportes_mobilidade),
            utils::fmtDouble(s.capacidade_media_mob,2),
            utils::fmtDouble(s.km_rede_ciclavel,3),
            std::to_string(s.n_segmentos_ciclavel),
            s.estac_por_km_ciclavel>0 ? utils::fmtDouble(s.estac_por_km_ciclavel,2) : "",
            s.cap_por_km_ciclavel>0   ? utils::fmtDouble(s.cap_por_km_ciclavel,2)   : "");
    std::cout << "[T3] estatisticas_locais.csv\n";
}

inline void writeTop10(const std::vector<FreguesiaStat>& ranked,
                       const std::string& path) {
    std::ofstream f(path);
    CSV_ROW(f,"rank","freguesia","capacidade_total","n_estacionamentos","km_ciclavel");
    int rank = 1;
    for (const auto& s : ranked) {
        if (rank > 10) break;
        CSV_ROW(f,
            std::to_string(rank++), s.freguesia,
            std::to_string(s.capacidade_total),
            std::to_string(s.n_estacionamentos),
            utils::fmtDouble(s.km_rede_ciclavel,2));
    }
    std::cout << "[T3] top_10_freguesias_por_capacidade.csv\n";
}

inline void writeDistribuicaoModelos(const std::vector<Estacionamento>& data,
                                     const std::string& path) {
    std::map<std::string,int> cnt;
    for (const auto& e : data) cnt[e.modelo]++;
    int total = static_cast<int>(data.size());

    // Ordenar por contagem descendente usando vetor + sort
    std::vector<std::pair<std::string,int>> sorted(cnt.begin(), cnt.end());
    std::sort(sorted.begin(), sorted.end(),
        [](const auto& a, const auto& b){ return a.second > b.second; });

    std::ofstream f(path);
    f << "modelo,count,pct\n";
    for (const auto& [mod, n] : sorted)
        f << utils::csvEscape(mod) << "," << n << ","
          << utils::fmtDouble(100.0*n/total,1) << "\n";
    std::cout << "[T3] distribuicao_modelos.csv\n";
}

inline void writeDistribuicaoCoberto(const std::vector<Estacionamento>& data,
                                     const std::string& path) {
    int coberto = 0, descoberto = 0;
    for (const auto& e : data) e.coberto ? coberto++ : descoberto++;
    int total = static_cast<int>(data.size());

    std::ofstream f(path);
    f << "tipo,count,pct\n";
    f << "Coberto,"    << coberto    << "," << utils::fmtDouble(100.0*coberto/total,1)    << "\n";
    f << "Descoberto," << descoberto << "," << utils::fmtDouble(100.0*descoberto/total,1) << "\n";
    std::cout << "[T3] distribuicao_coberto.csv\n";
}

inline void writeHistograma(const std::vector<Estacionamento>& data,
                            const std::string& path) {
    auto hist = statistics::buildHistogram(data);
    std::ofstream f(path);
    f << "intervalo_capacidade,count\n";
    for (const auto& b : hist) f << b.label << "," << b.count << "\n";
    std::cout << "[T3] histograma_capacidade.csv\n";
}

inline void writeDistribuicaoTipologiaCiclavel(const std::vector<CicloVia>& data,
                                               const std::string& path) {
    auto tip = statistics::countByTipologia(data);
    double km_total = 0.0;
    for (const auto& [_, p] : tip) km_total += p.second;

    // Ordenar por km descendente
    std::vector<std::pair<std::string,std::pair<int,double>>> sorted(tip.begin(),tip.end());
    std::sort(sorted.begin(), sorted.end(),
        [](const auto& a, const auto& b){ return a.second.second > b.second.second; });

    std::ofstream f(path);
    f << "tipologia,n_segmentos,km_total,pct_km\n";
    for (const auto& [tip_name, p] : sorted)
        f << utils::csvEscape(tip_name) << "," << p.first << ","
          << utils::fmtDouble(p.second,3) << ","
          << utils::fmtDouble(100.0*p.second/km_total,1) << "\n";
    std::cout << "[T3] distribuicao_tipologia_ciclavel.csv\n";
}

// ─────────────────────────────────────────────────────────────────────────────
//  Tarefa 4 — Agregação e ranking
// ─────────────────────────────────────────────────────────────────────────────

inline void writeAgregacaoFreguesia(const std::vector<FreguesiaStat>& stats,
                                    const std::string& path) {
    std::ofstream f(path);
    CSV_ROW(f,"freguesia","n_estacionamentos","capacidade_total",
              "num_suportes_total","capacidade_media",
              "n_mobilidade","capacidade_mobilidade",
              "num_suportes_mobilidade","capacidade_media_mobilidade",
              "km_rede_ciclavel","n_segmentos_ciclavel",
              "estac_por_km_ciclavel","capacidade_por_km_ciclavel");
    for (const auto& s : stats)
        CSV_ROW(f,
            s.freguesia,
            std::to_string(s.n_estacionamentos),
            std::to_string(s.capacidade_total),
            std::to_string(s.num_suportes_total),
            utils::fmtDouble(s.capacidade_media,2),
            std::to_string(s.n_mobilidade),
            std::to_string(s.capacidade_mobilidade),
            std::to_string(s.num_suportes_mobilidade),
            utils::fmtDouble(s.capacidade_media_mob,2),
            utils::fmtDouble(s.km_rede_ciclavel,3),
            std::to_string(s.n_segmentos_ciclavel),
            s.estac_por_km_ciclavel>0 ? utils::fmtDouble(s.estac_por_km_ciclavel,2) : "",
            s.cap_por_km_ciclavel>0   ? utils::fmtDouble(s.cap_por_km_ciclavel,2)   : "");
    std::cout << "[T4] agregacao_freguesia.csv (" << stats.size() << " freguesias)\n";
}

inline void writeRankingFreguesias(const std::vector<FreguesiaStat>& ranked,
                                   const std::string& path) {
    std::ofstream f(path);
    CSV_ROW(f,"rank","freguesia","capacidade_total","n_estacionamentos",
              "capacidade_media","n_mobilidade","capacidade_mobilidade",
              "km_rede_ciclavel","capacidade_por_km_ciclavel");
    int rank = 1;
    for (const auto& s : ranked)
        CSV_ROW(f,
            std::to_string(rank++), s.freguesia,
            std::to_string(s.capacidade_total),
            std::to_string(s.n_estacionamentos),
            utils::fmtDouble(s.capacidade_media,2),
            std::to_string(s.n_mobilidade),
            std::to_string(s.capacidade_mobilidade),
            utils::fmtDouble(s.km_rede_ciclavel,2),
            s.cap_por_km_ciclavel>0 ? utils::fmtDouble(s.cap_por_km_ciclavel,2) : "");
    std::cout << "[T4] ranking_freguesias.csv\n";
}

// ─────────────────────────────────────────────────────────────────────────────
//  GeoJSON Estacionamento normalizado — Tarefa 3
// ─────────────────────────────────────────────────────────────────────────────
inline void writeGeoJSON_Estacionamento(const std::vector<Estacionamento>& data,
                                        const std::string& path) {
    json root;
    root["type"] = "FeatureCollection";
    root["features"] = json::array();

    for (const auto& e : data) {
        json feat;
        feat["type"] = "Feature";
        feat["geometry"]["type"] = "Point";
        feat["geometry"]["coordinates"] = {e.lon, e.lat};
        feat["properties"] = {
            {"objectid",            e.objectid},
            {"cod_sig",             e.cod_sig},
            {"morada",              e.morada},
            {"freguesia",           e.freguesia},
            {"tipo_estacionamento", e.tipo_estacionamento},
            {"modelo",              e.modelo},
            {"num_suportes",        e.num_suportes},
            {"capacidade",          e.capacidade},
            {"coberto",             e.coberto ? 1 : 0},
            {"dominialidade",       e.dominialidade},
            {"estado",              e.estado},
            {"data_instalacao",     e.data_instalacao},
        };
        root["features"].push_back(std::move(feat));
    }

    std::ofstream f(path);
    f << root.dump(2);
    std::cout << "[T3] normalized_estacionamento.geojson (" << data.size() << " pontos)\n";
}

// ─────────────────────────────────────────────────────────────────────────────
//  GeoJSON Rede Ciclável normalizada — Tarefa 3
// ─────────────────────────────────────────────────────────────────────────────
inline void writeGeoJSON_CicloVia(const std::vector<CicloVia>& data,
                                   const std::string& path) {
    json root;
    root["type"] = "FeatureCollection";
    root["features"] = json::array();

    for (const auto& c : data) {
        json coordsArr = json::array();
        for (const auto& [lon, lat] : c.coords)
            coordsArr.push_back({lon, lat});

        json feat;
        feat["type"] = "Feature";
        feat["geometry"]["type"] = "LineString";
        feat["geometry"]["coordinates"] = coordsArr;
        feat["properties"] = {
            {"objectid",         c.objectid},
            {"cod_ciclovia",     c.cod_ciclovia},
            {"designacao",       c.designacao},
            {"hierarquia",       c.hierarquia},
            {"tipologia",        c.tipologia},
            {"nivel_segregacao", c.nivel_segregacao},
            {"tipo_intervencao", c.tipo_intervencao},
            {"situacao",         c.situacao},
            {"ano",              c.ano},
            {"freguesia",        c.freguesia},
            {"comprimento_m",    c.comprimento_m},
            {"comp_km",          c.comp_km},
        };
        root["features"].push_back(std::move(feat));
    }

    std::ofstream f(path);
    f << root.dump(2);
    std::cout << "[T3] normalized_rede_ciclavel.geojson (" << data.size() << " segmentos)\n";
}

// ─────────────────────────────────────────────────────────────────────────────
//  GeoJSON Agregado por freguesia — Tarefas 3/4
//  Sem polígonos oficiais no conjunto de dados, usa o centro médio dos pontos como geometria.
// ─────────────────────────────────────────────────────────────────────────────
inline void writeGeoJSON_AgregacaoFreguesia(const std::vector<FreguesiaStat>& stats,
                                            const std::string& path) {
    json root;
    root["type"] = "FeatureCollection";
    root["features"] = json::array();

    for (const auto& s : stats) {
        json feat;
        feat["type"] = "Feature";
        feat["geometry"]["type"] = "Point";
        feat["geometry"]["coordinates"] = {s.lon_media, s.lat_media};
        feat["properties"] = {
            {"freguesia",                   s.freguesia},
            {"n_estacionamentos",           s.n_estacionamentos},
            {"capacidade_total",            s.capacidade_total},
            {"num_suportes_total",          s.num_suportes_total},
            {"capacidade_media",            s.capacidade_media},
            {"n_mobilidade",                s.n_mobilidade},
            {"capacidade_mobilidade",       s.capacidade_mobilidade},
            {"num_suportes_mobilidade",     s.num_suportes_mobilidade},
            {"capacidade_media_mobilidade", s.capacidade_media_mob},
            {"km_rede_ciclavel",            s.km_rede_ciclavel},
            {"n_segmentos_ciclavel",        s.n_segmentos_ciclavel},
            {"estac_por_km_ciclavel",       s.estac_por_km_ciclavel},
            {"capacidade_por_km_ciclavel",  s.cap_por_km_ciclavel}
        };
        root["features"].push_back(std::move(feat));
    }

    std::ofstream f(path);
    f << root.dump(2);
    std::cout << "[T4] agregacao_freguesia.geojson (" << stats.size() << " freguesias)\n";
}

#undef CSV_ROW

} // namespace writer
