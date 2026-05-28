#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include "models.h"
#include "utils.h"

namespace csv_reader {

// ─────────────────────────────────────────────────────────────────────────────
//  Tokenizador de linha CSV — trata campos com aspas e vírgulas internas
// ─────────────────────────────────────────────────────────────────────────────
inline std::vector<std::string> parseLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (inQuotes) {
            if (c == '"') {
                // aspa dupla dentro de campo → aspa literal
                if (i + 1 < line.size() && line[i+1] == '"') {
                    field += '"'; ++i;
                } else {
                    inQuotes = false;
                }
            } else {
                field += c;
            }
        } else {
            if (c == '"') {
                inQuotes = true;
            } else if (c == ',') {
                fields.push_back(field);
                field.clear();
            } else {
                field += c;
            }
        }
    }
    fields.push_back(field);  // último campo
    return fields;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Lê um ficheiro CSV e devolve vector de maps {coluna → valor}
// ─────────────────────────────────────────────────────────────────────────────
using Row = std::unordered_map<std::string, std::string>;

inline std::vector<Row> readCSV(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[ERRO] Nao foi possivel abrir: " << path << "\n";
        return {};
    }

    std::vector<Row> rows;
    std::string line;

    // Cabeçalho
    if (!std::getline(file, line)) return {};
    // Remove BOM UTF-8 se presente (0xEF 0xBB 0xBF)
    if (line.size() >= 3 &&
        (unsigned char)line[0] == 0xEF &&
        (unsigned char)line[1] == 0xBB &&
        (unsigned char)line[2] == 0xBF)
        line = line.substr(3);

    std::vector<std::string> header = parseLine(line);

    // Linhas de dados
    while (std::getline(file, line)) {
        if (utils::trim(line).empty()) continue;
        std::vector<std::string> values = parseLine(line);
        Row row;
        for (size_t i = 0; i < header.size(); ++i)
            row[header[i]] = (i < values.size()) ? values[i] : "";
        rows.push_back(std::move(row));
    }
    return rows;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Constrói vector<Mobilidade> a partir do CSV normalizado
//  (XLSX deve ser exportado para CSV antes — ver README)
// ─────────────────────────────────────────────────────────────────────────────
inline std::vector<Mobilidade> loadMobilidade(const std::string& path) {
    auto rows = readCSV(path);
    std::vector<Mobilidade> result;
    result.reserve(rows.size());

    for (auto& row : rows) {
        Mobilidade m;
        m.objectid             = utils::toInt(row["OBJECTID"]);
        m.cod_sig              = utils::toInt(row["COD_SIG_ESTACIONAMENTO"]);
        m.morada               = utils::trim(row["MORADA"]);
        m.freguesia            = utils::trim(row["FREGUESIA"]);
        m.localizacao          = utils::trim(row["LOCALIZACAO"]);
        m.tipo_estacionamento  = utils::trim(row["TIPO_ESTACIONAMENTO"]);
        m.modelo               = utils::orDefault(utils::trim(row["MODELO"]), "Desconhecido");
        m.num_suportes         = utils::toInt(row["NUM_SUPORTES"]);
        m.capacidade           = utils::toInt(row["CAPACIDADE"]);
        m.coberto              = utils::parseBool(row["COBERTO"]);
        m.dominialidade        = utils::trim(row["DOMINIALIDADE"]);
        m.estado               = utils::trim(row["ESTACIONAMENTO_ESTADO"]);
        m.data_instalacao      = utils::normalizeDate(row["DATA_INSTALACAO"]);

        double x = utils::toDouble(row["x"]);
        double y = utils::toDouble(row["y"]);
        if (x != 0.0 || y != 0.0) {
            auto [lon, lat]  = utils::mercatorToWGS84(x, y);
            m.lon = lon; m.lat = lat;
        }
        result.push_back(std::move(m));
    }
    std::cout << "[T1] Mobilidade CSV: " << result.size() << " registos\n";
    return result;
}

} // namespace csv_reader
