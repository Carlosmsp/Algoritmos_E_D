/*
 * Estudo sobre Dados de Mobilidade de Lisboa
 * ───────────────────────────────────────────
 * Disciplina: Algoritmos e Estruturas de Dados
 * Licenciatura em Engenharia Informática — IADE / Universidade Europeia
 *
 * Estrutura do projeto:
 *   src/main.cpp          — ponto de entrada, orquestra as 4 tarefas
 *   src/models.h          — estruturas de dados (Estacionamento, CicloVia, …)
 *   src/utils.h           — funções utilitárias (trim, parse, projeção)
 *   src/csv_reader.h      — leitura de CSVs
 *   src/geojson_reader.h  — leitura de GeoJSON (nlohmann/json)
 *   src/statistics.h      — agregação, ordenação, estatísticas
 *   src/writer.h          — escrita de CSV e GeoJSON de saída
 *
 * Compilação (ver CMakeLists.txt ou README):
 *   g++ -std=c++17 -O2 -Iinclude -o mobilidade src/main.cpp
 *
 * Utilização:
 *   ./mobilidade [dir_dados] [dir_output]
 *   (por omissão: ./data  e  ./output)
 */

#include <iostream>
#include <string>
#include <filesystem>
#include "models.h"
#include "utils.h"
#include "csv_reader.h"
#include "geojson_reader.h"
#include "statistics.h"
#include "writer.h"

namespace fs = std::filesystem;

// ─────────────────────────────────────────────────────────────────────────────
//  Imprime os 5 primeiros registos de cada conjunto de dados (Tarefa 1)
// ─────────────────────────────────────────────────────────────────────────────
void printSample(const std::vector<Estacionamento>& estac,
                 const std::vector<Mobilidade>&     mob,
                 const std::vector<CicloVia>&       cic) {
    std::cout << "\n  Primeiros 5 — Estacionamento:\n";
    for (size_t i = 0; i < std::min(estac.size(), size_t(5)); ++i)
        std::cout << "    [" << estac[i].objectid << "] "
                  << estac[i].freguesia << " | "
                  << estac[i].tipo_estacionamento
                  << " | cap=" << estac[i].capacidade << "\n";

    std::cout << "  Primeiros 5 — Mobilidade CSV:\n";
    for (size_t i = 0; i < std::min(mob.size(), size_t(5)); ++i)
        std::cout << "    [" << mob[i].objectid << "] "
                  << mob[i].freguesia << " | "
                  << mob[i].tipo_estacionamento
                  << " | cap=" << mob[i].capacidade << "\n";

    if (!cic.empty()) {
        std::cout << "  Primeiros 3 — Rede Ciclável:\n";
        for (size_t i = 0; i < std::min(cic.size(), size_t(3)); ++i)
            std::cout << "    [" << cic[i].objectid << "] "
                      << cic[i].freguesia << " | "
                      << cic[i].tipologia
                      << " | " << utils::fmtDouble(cic[i].comp_km,2) << " km\n";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {

    // Diretórios configuráveis por argumento (ou por defeito)
    std::string dataDir   = (argc > 1) ? argv[1] : "./data";
    std::string outputDir = (argc > 2) ? argv[2] : "./output";
    fs::create_directories(outputDir);

    // Caminhos de entrada
    const std::string PATH_ESTAC  = dataDir + "/estacionamento_velocipedes.geojson";
    const std::string PATH_MOB    = dataDir + "/mobilidade_urbana.csv";
    const std::string PATH_CIC    = dataDir + "/rede_ciclavel.geojson";  // opcional

    // ═════════════════════════════════════════════════════════════════════════
    //  TAREFA 1 — Leitura e exploração inicial
    // ═════════════════════════════════════════════════════════════════════════
    std::cout << "\n════════════════════════════════════════════\n";
    std::cout << " TAREFA 1 — Leitura e exploração inicial\n";
    std::cout << "════════════════════════════════════════════\n";

    // Carrega os três conjuntos de dados para vetores de estruturas em memória
    std::vector<Estacionamento> estac  = geojson_reader::loadEstacionamento(PATH_ESTAC);
    std::vector<Mobilidade>     mob    = csv_reader::loadMobilidade(PATH_MOB);
    std::vector<CicloVia>       cic    = geojson_reader::loadCiclovias(PATH_CIC);

    printSample(estac, mob, cic);

    // CSV limpos — campos selecionados, sem colunas desnecessárias
    writer::writeCleaned_Estacionamento(estac, outputDir + "/cleaned_estacionamento.csv");
    writer::writeCleaned_Mobilidade(mob,       outputDir + "/cleaned_mobilidade.csv");
    writer::writeCleaned_CicloVia(cic,         outputDir + "/cleaned_rede_ciclavel.csv");

    // ═════════════════════════════════════════════════════════════════════════
    //  TAREFA 2 — Normalização e estatísticas descritivas
    //  (normalização já feita no momento da leitura em geojson_reader /
    //   csv_reader — datas → DD/MM/YYYY, COBERTO → bool, coords → WGS84)
    // ═════════════════════════════════════════════════════════════════════════
    std::cout << "\n════════════════════════════════════════════\n";
    std::cout << " TAREFA 2 — Normalização e estatísticas\n";
    std::cout << "════════════════════════════════════════════\n";

    statistics::printDescriptiveStats(estac, cic);
    writer::writeEstatisticas(estac, mob, cic, outputDir + "/estatisticas.csv");
    writer::writeNormalizedEstacionamento(estac, outputDir + "/normalized_estacionamento.csv");
    writer::writeNormalizedMobilidade(mob, outputDir + "/normalized_mobilidade.csv");
    if (!cic.empty())
        writer::writeNormalizedCicloVia(cic, outputDir + "/normalized_rede_ciclavel.csv");

    // ═════════════════════════════════════════════════════════════════════════
    //  TAREFA 3 — Estatísticas e análises auxiliares por local
    // ═════════════════════════════════════════════════════════════════════════
    std::cout << "\n════════════════════════════════════════════\n";
    std::cout << " TAREFA 3 — Estatísticas por local\n";
    std::cout << "════════════════════════════════════════════\n";

    // Agrega por freguesia (std::map + std::sort internamente)
    std::vector<FreguesiaStat> stats     = statistics::aggregateByFreguesia(estac, mob, cic);
    std::vector<FreguesiaStat> ranked    = statistics::rankByCapacidade(stats);

    writer::writeEstatisticasLocais(stats,  outputDir + "/estatisticas_locais.csv");
    writer::writeTop10(ranked,              outputDir + "/top_10_freguesias_por_capacidade.csv");
    writer::writeDistribuicaoModelos(estac, outputDir + "/distribuicao_modelos.csv");
    writer::writeDistribuicaoCoberto(estac, outputDir + "/distribuicao_coberto.csv");
    writer::writeHistograma(estac,          outputDir + "/histograma_capacidade.csv");

    if (!cic.empty())
        writer::writeDistribuicaoTipologiaCiclavel(cic,
            outputDir + "/distribuicao_tipologia_ciclavel.csv");

    // GeoJSON normalizados (prontos para QGIS / kepler.gl)
    writer::writeGeoJSON_Estacionamento(estac, outputDir + "/normalized_estacionamento.geojson");
    if (!cic.empty())
        writer::writeGeoJSON_CicloVia(cic, outputDir + "/normalized_rede_ciclavel.geojson");
    writer::writeGeoJSON_AgregacaoFreguesia(stats, outputDir + "/agregacao_freguesia.geojson");

    // ═════════════════════════════════════════════════════════════════════════
    //  TAREFA 4 — Agregação e ranking por freguesia
    // ═════════════════════════════════════════════════════════════════════════
    std::cout << "\n════════════════════════════════════════════\n";
    std::cout << " TAREFA 4 — Agregação por freguesia\n";
    std::cout << "════════════════════════════════════════════\n";

    writer::writeAgregacaoFreguesia(stats,  outputDir + "/agregacao_freguesia.csv");
    writer::writeRankingFreguesias(ranked,  outputDir + "/ranking_freguesias.csv");

    std::cout << "\n✓ Pipeline concluído. Ficheiros em: " << outputDir << "\n\n";
    return 0;
}
