#pragma once
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include "models.h"
#include "utils.h"

namespace statistics {

// ─────────────────────────────────────────────────────────────────────────────
//  Tarefa 2 — Imprime estatísticas descritivas básicas no ecrã
// ─────────────────────────────────────────────────────────────────────────────
inline void printDescriptiveStats(const std::vector<Estacionamento>& data,
                                  const std::vector<CicloVia>& ciclovias) {
    // Recolhe capacidades válidas
    std::vector<int> caps;
    std::vector<int> suportes;
    caps.reserve(data.size());
    suportes.reserve(data.size());

    for (const auto& e : data) {
        if (e.capacidade > 0) caps.push_back(e.capacidade);
        if (e.num_suportes > 0) suportes.push_back(e.num_suportes);
    }

    double km_total = 0.0;
    for (const auto& c : ciclovias) km_total += c.comp_km;

    std::cout << "\n--- Estatísticas Descritivas ---\n";
    std::cout << "  Total estacionamentos : " << data.size() << "\n";
    std::cout << "  Capacidade soma       : " << utils::sum(caps) << "\n";
    std::cout << "  Capacidade média      : " << utils::fmtDouble(utils::mean(caps), 2) << "\n";
    std::cout << "  Capacidade mediana    : " << utils::fmtDouble(utils::median(caps), 1) << "\n";
    std::cout << "  Capacidade min/max    : "
              << *std::min_element(caps.begin(), caps.end()) << " / "
              << *std::max_element(caps.begin(), caps.end()) << "\n";
    std::cout << "  Suportes soma         : " << utils::sum(suportes) << "\n";
    std::cout << "  Suportes média        : " << utils::fmtDouble(utils::mean(suportes), 2) << "\n";
    std::cout << "  Segmentos ciclável    : " << ciclovias.size() << "\n";
    std::cout << "  Km rede ciclável      : " << utils::fmtDouble(km_total, 2) << " km\n";
}

// ─────────────────────────────────────────────────────────────────────────────
//  Tarefa 4 — Agrega métricas por freguesia
//  Usa std::map (árvore BST) para acumular automaticamente por chave-freguesia
// ─────────────────────────────────────────────────────────────────────────────
inline std::vector<FreguesiaStat>
aggregateByFreguesia(const std::vector<Estacionamento>& estac,
                     const std::vector<CicloVia>& ciclovias) {

    // ── 1. Acumular estacionamentos por freguesia ─────────────────────────────
    std::map<std::string, FreguesiaStat> agg;  // ordenado automaticamente por nome

    for (const auto& e : estac) {
        const std::string& freg = e.freguesia.empty() ? "Desconhecida" : e.freguesia;
        FreguesiaStat& s = agg[freg];
        s.freguesia = freg;
        s.n_estacionamentos++;
        s.capacidade_total   += e.capacidade;
        s.num_suportes_total += e.num_suportes;
        if (e.capacidade > 0) s.capacidades.push_back(e.capacidade);
    }

    // ── 2. Acumular km da rede ciclável por freguesia ─────────────────────────
    for (const auto& c : ciclovias) {
        const std::string& freg = c.freguesia.empty() ? "Desconhecida" : c.freguesia;
        // só conta se a freguesia já tem estacionamentos (join natural)
        if (agg.find(freg) != agg.end()) {
            agg[freg].km_rede_ciclavel     += c.comp_km;
            agg[freg].n_segmentos_ciclavel += 1;
        }
    }

    // ── 3. Calcular médias e densidades ──────────────────────────────────────
    for (auto& [freg, s] : agg) {
        s.capacidade_media = utils::mean(s.capacidades);
        if (s.km_rede_ciclavel > 0.0) {
            s.estac_por_km_ciclavel = s.n_estacionamentos / s.km_rede_ciclavel;
            s.cap_por_km_ciclavel   = s.capacidade_total  / s.km_rede_ciclavel;
        }
    }

    // ── 4. Converter map → vector (mantém ordem alfabética do map) ───────────
    std::vector<FreguesiaStat> result;
    result.reserve(agg.size());
    for (auto& [_, s] : agg) result.push_back(std::move(s));

    std::cout << "[T4] Agregadas " << result.size() << " freguesias\n";
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Histograma de capacidade — intervalos fixos
// ─────────────────────────────────────────────────────────────────────────────
struct HistBin { std::string label; int lo, hi, count; };

inline std::vector<HistBin> buildHistogram(const std::vector<Estacionamento>& data) {
    std::vector<HistBin> bins = {
        {"1-2",   1,   2,  0},
        {"3-4",   3,   4,  0},
        {"5-10",  5,  10,  0},
        {"11-20", 11, 20,  0},
        {"21-50", 21, 50,  0},
        {"51+",   51, 9999, 0},
    };
    for (const auto& e : data) {
        for (auto& b : bins)
            if (e.capacidade >= b.lo && e.capacidade <= b.hi) { b.count++; break; }
    }
    return bins;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Contagem por tipologia de ciclovia (km acumulados)
// ─────────────────────────────────────────────────────────────────────────────
inline std::map<std::string, std::pair<int,double>>
countByTipologia(const std::vector<CicloVia>& ciclovias) {
    std::map<std::string, std::pair<int,double>> result;
    for (const auto& c : ciclovias) {
        result[c.tipologia].first++;
        result[c.tipologia].second += c.comp_km;
    }
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Ordena as freguesias por capacidade_total (maior → menor)
//  Demonstra std::sort com comparador lambda — tópico central do curso
// ─────────────────────────────────────────────────────────────────────────────
inline std::vector<FreguesiaStat>
rankByCapacidade(std::vector<FreguesiaStat> stats) {
    std::sort(stats.begin(), stats.end(),
        [](const FreguesiaStat& a, const FreguesiaStat& b) {
            return a.capacidade_total > b.capacidade_total;  // descendente
        });
    return stats;
}

} // namespace statistics
