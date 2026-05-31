#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>

namespace utils {

// ── Texto ────────────────────────────────────────────────────────────────────

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
 * Normaliza vários formatos de data para formato português: DD/MM/YYYY
 * Formatos reconhecidos:
 *   MM/DD/YYYY HH:MM:SS AM/PM   (formato Excel americano)
 *   YYYY-MM-DD
 *   DD/MM/YYYY                  (formato português)
 */
inline std::string normalizeDate(const std::string& raw) {
    std::string s = trim(raw);
    if (s.empty()) return "";

    auto formatPT = [](int day, int month, int year) {
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << day << "/"
            << std::setfill('0') << std::setw(2) << month << "/"
            << year;
        return oss.str();
    };

    // YYYY-MM-DD ou YYYY-MM-DD HH:MM:SS
    if (s.size() >= 10 && s[4] == '-' && s[7] == '-') {
        int year = toInt(s.substr(0, 4));
        int month = toInt(s.substr(5, 2));
        int day = toInt(s.substr(8, 2));
        if (day > 0 && day <= 31 && month > 0 && month <= 12 && year > 1900)
            return formatPT(day, month, year);
    }

    // DD/MM/YYYY ... em contexto PT, aceitando dias/meses sem zero inicial.
    // Se o segundo campo for > 12, cai para MM/DD/YYYY porque DD/MM seria inválido.
    size_t slash1 = s.find('/');
    size_t slash2 = (slash1 == std::string::npos) ? std::string::npos
                                                   : s.find('/', slash1 + 1);
    if (slash1 != std::string::npos && slash2 != std::string::npos) {
        int p1 = toInt(s.substr(0, slash1));
        int p2 = toInt(s.substr(slash1 + 1, slash2 - slash1 - 1));
        int p3 = toInt(s.substr(slash2 + 1, 4));

        if (p1 > 0 && p2 > 0 && p3 > 1900) {
            int day = p1;
            int month = p2;

            if (p2 > 12) {
                month = p1;
                day = p2;
            }

            if (day <= 31 && month <= 12)
                return formatPT(day, month, p3);
        }
    }

    // Formato GeoJSON/RFC usado nos ficheiros originais:
    // Wed, 09 Jan 2019 00:00:00 GMT
    if (s.size() >= 16 && s[3] == ',' && s[7] == ' ' && s[11] == ' ') {
        int day = toInt(s.substr(5, 2));
        std::string mon = s.substr(8, 3);
        int year = toInt(s.substr(12, 4));

        int month = 0;
        if      (mon == "Jan") month = 1;
        else if (mon == "Feb") month = 2;
        else if (mon == "Mar") month = 3;
        else if (mon == "Apr") month = 4;
        else if (mon == "May") month = 5;
        else if (mon == "Jun") month = 6;
        else if (mon == "Jul") month = 7;
        else if (mon == "Aug") month = 8;
        else if (mon == "Sep") month = 9;
        else if (mon == "Oct") month = 10;
        else if (mon == "Nov") month = 11;
        else if (mon == "Dec") month = 12;

        if (day > 0 && month > 0 && year > 1900)
            return formatPT(day, month, year);
    }

    return s;  // devolve como está se não reconhecer
}

// ── Projeção de coordenadas ───────────────────────────────────────────────────

/*
 * Converte coordenadas Web Mercator (EPSG:3857) para WGS84 (EPSG:4326)
 * Equivalente ao transformador do pyproj.
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

// ── Escrita segura em CSV ────────────────────────────────────────────────────

// Envolve valor em aspas se contiver vírgula, aspas ou quebra de linha
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

// Formata número decimal com n casas decimais
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
