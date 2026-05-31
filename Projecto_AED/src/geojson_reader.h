#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "../include/nlohmann/json.hpp"
#include "models.h"
#include "utils.h"

namespace geojson_reader {

using json = nlohmann::json;

// ── Funções auxiliares para ler campos com segurança ─────────────────────────

inline std::string getString(const json& props, const std::string& key) {
    if (!props.contains(key) || props[key].is_null()) return "";
    return utils::trim(props[key].get<std::string>());
}

inline int getInt(const json& props, const std::string& key) {
    if (!props.contains(key) || props[key].is_null()) return 0;
    if (props[key].is_number()) return props[key].get<int>();
    try { return std::stoi(props[key].get<std::string>()); }
    catch (...) { return 0; }
}

inline double getDouble(const json& props, const std::string& key) {
    if (!props.contains(key) || props[key].is_null()) return 0.0;
    if (props[key].is_number()) return props[key].get<double>();
    try { return std::stod(props[key].get<std::string>()); }
    catch (...) { return 0.0; }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Carrega GeoJSON de estacionamento de velocípedes
//  Geometria: Point [lon, lat] — já em WGS84
// ─────────────────────────────────────────────────────────────────────────────
inline std::vector<Estacionamento> loadEstacionamento(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[ERRO] Nao foi possivel abrir: " << path << "\n";
        return {};
    }

    json root;
    try {
        file >> root;
    } catch (const json::parse_error& e) {
        std::cerr << "[ERRO] JSON invalido: " << e.what() << "\n";
        return {};
    }

    std::vector<Estacionamento> result;
    const auto& features = root["features"];
    result.reserve(features.size());

    for (const auto& feat : features) {
        // Ignorar features sem geometria
        if (feat["geometry"].is_null()) continue;

        const auto& props  = feat["properties"];
        const auto& coords = feat["geometry"]["coordinates"];

        Estacionamento e;
        e.objectid            = getInt(props, "OBJECTID");
        e.cod_sig             = getInt(props, "COD_SIG_ESTACIONAMENTO");
        e.morada              = getString(props, "MORADA");
        e.freguesia           = getString(props, "FREGUESIA");
        e.localizacao         = getString(props, "LOCALIZACAO");
        e.tipo_estacionamento = getString(props, "TIPO_ESTACIONAMENTO");
        e.modelo              = utils::orDefault(getString(props, "MODELO"), "Desconhecido");
        e.num_suportes        = getInt(props, "NUM_SUPORTES");
        e.capacidade          = getInt(props, "CAPACIDADE");
        e.coberto             = utils::parseBool(getString(props, "COBERTO"));
        e.dominialidade       = getString(props, "DOMINIALIDADE");
        e.estado              = getString(props, "ESTACIONAMENTO_ESTADO");
        e.data_instalacao     = utils::normalizeDate(getString(props, "DATA_INSTALACAO"));
        e.lon                 = coords[0].get<double>();
        e.lat                 = coords[1].get<double>();

        result.push_back(std::move(e));
    }

    std::cout << "[T1] Estacionamento GeoJSON: " << result.size() << " registos\n";
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Carrega GeoJSON da rede ciclável
//  Geometria: LineString [[lon,lat], [lon,lat], …] — já em WGS84
//  Campos opcionais removidos: COD_SIG, COD_VIA, EIXO (muitos nulos),
//  IDTIPO, ZONAMENTO, GlobalID
// ─────────────────────────────────────────────────────────────────────────────
inline std::vector<CicloVia> loadCiclovias(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[AVISO] Rede ciclavel nao encontrada: " << path << " (opcional)\n";
        return {};
    }

    json root;
    try {
        file >> root;
    } catch (const json::parse_error& e) {
        std::cerr << "[ERRO] JSON invalido: " << e.what() << "\n";
        return {};
    }

    std::vector<CicloVia> result;
    int skipped = 0;

    for (const auto& feat : root["features"]) {
        // Feature sem geometria — ignorar (acontece em 1 registo nos dados reais)
        if (feat["geometry"].is_null()) { ++skipped; continue; }

        const auto& props  = feat["properties"];
        const auto& coords = feat["geometry"]["coordinates"];

        CicloVia c;
        c.objectid         = getInt(props, "OBJECTID");
        c.cod_ciclovia     = getString(props, "COD_CICLOVIA");
        c.designacao       = getString(props, "DESIGNACAO");
        c.nome_projeto     = getString(props, "NOME_PROJETO");
        c.hierarquia       = getString(props, "HIERARQUIA");
        c.tipologia        = getString(props, "TIPOLOGIA");
        c.nivel_segregacao = getString(props, "NIVEL_SEGREGACAO");
        c.tipo_intervencao = getString(props, "TIPO_INTERVENCAO");
        c.situacao         = getString(props, "SITUACAO");
        c.ano              = getString(props, "ANO");
        c.entidade_resp    = getString(props, "ENTIDADE_RESP");
        c.freguesia        = getString(props, "FREGUESIA");
        c.comprimento_m    = getDouble(props, "COMPRIMENTO");
        c.comp_km          = getDouble(props, "COMP_KM");

        // Lê geometria: LineString ou MultiLineString
        // MultiLineString: [[line1_pt1, ...], [line2_pt1, ...]] → achata para uma só lista
        const std::string geomType = feat["geometry"]["type"].get<std::string>();
        if (geomType == "LineString") {
            c.coords.reserve(coords.size());
            for (const auto& pt : coords)
                c.coords.emplace_back(pt[0].get<double>(), pt[1].get<double>());
        } else if (geomType == "MultiLineString") {
            // Achata todas as sub-linhas numa única sequência de pontos
            for (const auto& line : coords)
                for (const auto& pt : line)
                    c.coords.emplace_back(pt[0].get<double>(), pt[1].get<double>());
        }

        result.push_back(std::move(c));
    }

    std::cout << "[T1] Rede ciclavel GeoJSON: " << result.size()
              << " segmentos";
    if (skipped > 0) std::cout << " (ignorados " << skipped << " sem geometria)";
    std::cout << "\n";
    return result;
}

} // namespace geojson_reader
