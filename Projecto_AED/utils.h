#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>

namespace utils {

// ── String ───────────────────────────────────────────────────────────────────

// Remove espaços iniciais e finais (equivalente ao strip() do Python)
inline std::string trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// Converte string para minúsculas
inline std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

// Verifica se string é vazia ou apenas espaços
inline bool isEmpty(const std::string& s) {
    return trim(s).empty();
}

// Substitui valor vazio por um valor por defeito
inline std::string orDefault(const std::string& s, const std::string& def) {
    return isEmpty(s) ? def : s;
}

// Converte string "Sim"/"sim" para bool
inline bool parseBool(const std::string& s) {
    return toLower(trim(s)) == "sim";
}

// Tenta converter string para int; devolve 0 em caso de falha
inline int toInt(const std::string& s) {
    try {
        if (isEmpty(s)) return 0;
        return std::stoi(trim(s));
    } catch (...) { return 0; }
}

// Tenta converter string para double; devolve 0.0 em caso de falha
inline double toDouble(const std::string& s) {
    try {
        if (isEmpty(s)) return 0.0;
        return std::stod(trim(s));
    } catch (...) { return 0.0; }
}

// ── Data ─────────────────────────────────────────────────────────────────────

/*
 * Normaliza vários formatos de data para ISO 8601: YYYY-MM-DD
 * Formatos reconhecidos:
 *   MM/DD/YYYY HH:MM:SS AM/PM   (formato Excel americano)
 *   YYYY-MM-DD                  (já normalizado)
 *   DD/MM/YYYY                  (formato português)
 */
inline std::string normalizeDate(const std::string& raw) {
    std::string s = trim(raw);
    if (s.empty()) return "";

    // Já está em YYYY-MM-DD
    if (s.size() >= 10 && s[4] == '-' && s[7] == '-')
        return s.substr(0, 10);

    // MM/DD/YYYY ... (Excel americano — primeiro campo ≤ 12 e separador '/')
    if (s.size() >= 10 && s[2] == '/') {
        int p1 = toInt(s.substr(0, 2));
        int p2 = toInt(s.substr(3, 2));
        int p3 = toInt(s.substr(6, 4));
        // Heurística: se p1 <= 12 assumimos MM/DD/YYYY
        if (p1 <= 12 && p3 > 1900) {
            std::ostringstream oss;
            oss << p3 << "-"
                << std::setfill('0') << std::setw(2) << p1 << "-"
                << std::setfill('0') << std::setw(2) << p2;
            return oss.str();
        }
        // DD/MM/YYYY
        if (p3 > 1900) {
            std::ostringstream oss;
            oss << p3 << "-"
                << std::setfill('0') << std::setw(2) << p2 << "-"
                << std::setfill('0') << std::setw(2) << p1;
            return oss.str();
        }
    }
    return s;  // devolve como está se não reconhecer
}

// ── Projeção de coordenadas ───────────────────────────────────────────────────

/*
 * Converte coordenadas Web Mercator (EPSG:3857) para WGS84 (EPSG:4326)
 * Equivalente ao Transformer do pyproj.
 *
 * Fórmulas:
 *   lon = x / EARTH_SEMI_MAJOR * (180 / π)
 *   lat = (2 * atan(exp(y / EARTH_SEMI_MAJOR)) - π/2) * (180 / π)
 */
inline std::pair<double,double> mercatorToWGS84(double x, double y) {
    constexpr double R   = 6378137.0;   // semi-eixo maior WGS84 (metros)
    constexpr double PI  = 3.14159265358979323846;
    double lon = x / R * (180.0 / PI);
    double lat = (2.0 * std::atan(std::exp(y / R)) - PI / 2.0) * (180.0 / PI);
    return {lon, lat};
}

// ── CSV escaping ─────────────────────────────────────────────────────────────

// Envolve valor em aspas se contiver vírgula, aspas ou newline
inline std::string csvEscape(const std::string& s) {
    if (s.find_first_of(",\"\n\r") == std::string::npos) return s;
    std::string out = "\"";
    for (char c : s) {
        if (c == '"') out += "\"\"";
        else out += c;
    }
    out += '"';
    return out;
}

// Formata double com n casas decimais
inline std::string fmtDouble(double v, int precision = 3) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << v;
    return oss.str();
}

// ── Estatísticas básicas (operam sobre std::vector<int>) ─────────────────────

inline double mean(const std::vector<int>& v) {
    if (v.empty()) return 0.0;
    double sum = 0;
    for (int x : v) sum += x;
    return sum / static_cast<double>(v.size());
}

// Mediana: requer cópia ordenada (não altera o original)
inline double median(std::vector<int> v) {
    if (v.empty()) return 0.0;
    std::sort(v.begin(), v.end());
    size_t n = v.size();
    if (n % 2 == 0) return (v[n/2 - 1] + v[n/2]) / 2.0;
    return v[n/2];
}

inline int sum(const std::vector<int>& v) {
    int s = 0;
    for (int x : v) s += x;
    return s;
}

} // namespace utils
