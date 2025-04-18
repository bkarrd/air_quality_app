#include "DataAnalyzer.h"
#include <iostream>
#include <limits>

/**
 * @brief Analizuje dane o jakości powietrza z formatu JSON
 *
 * Funkcja przetwarza dane pomiarowe o jakości powietrza dostarczone w formacie JSON,
 * oblicza wartości minimalne i maksymalne oraz zapisuje wyniki do wewnętrznych struktur danych.
 *
 * @param data Dane wejściowe w formacie JSON zawierające pomiary jakości powietrza
 * @throws Nie zgłasza wyjątków, ale wyświetla błędy na standardowym wyjściu błędów
 */
void DataAnalyzer::analyzeData(const nlohmann::json& data) {
    results.clear();
    analysisResults.clear();

    // Sprawdź, czy dane zawierają wartości
    if (!data.contains("values") || !data["values"].is_array()) {
        std::cerr << "Błąd: Dane nie zawierają wartości pomiarowych" << std::endl;
        return;
    }

    // Zmienne do śledzenia min/max wartości
    double minVal = std::numeric_limits<double>::max();
    double maxVal = std::numeric_limits<double>::lowest();
    minDate = "";
    maxDate = "";

    // Przetwarzanie danych pomiarowych
    for (const auto& item : data["values"]) {
        if (item.is_object() && item.contains("date") && item.contains("value")) {
            std::string dateStr = item["date"];

            // Obsługa wartości null
            double value = 0.0;
            if (!item["value"].is_null()) {
                // Konwersja wartości tekstowej na liczbową
                try {
                    if (item["value"].is_string()) {
                        value = std::stod(item["value"].get<std::string>());
                    } else if (item["value"].is_number()) {
                        value = item["value"].get<double>();
                    }
                }
                catch (const std::exception& e) {
                    std::cerr << "Błąd konwersji wartości: " << e.what() << std::endl;
                    continue;
                }
            }

            // Dodanie wartości do mapy wyników
            results[dateStr] = value;

            // Aktualizacja statystyk
            analysisResults[dateStr] = std::make_tuple(value, 0.0, 0.0); // wartość, min, max

            // Sprawdzenie, czy to wartość minimalna lub maksymalna
            if (value > 0.0 && value < minVal) {  // Pomijamy wartości null (zamienione na 0.0)
                minVal = value;
                minDate = dateStr;
            }
            if (value > maxVal) {
                maxVal = value;
                maxDate = dateStr;
            }
        }
    }

    // Sortowanie wyników po dacie (jeśli potrzebne)
    // Aktualizacja wartości trendów itp. może być dodana tutaj
}

/**
 * @brief Zwraca wyniki analizy danych
 *
 * @return Referencja do stałej mapy zawierającej pary data-wartość pomiarowa
 */
const std::map<std::string, double>& DataAnalyzer::getResults() const {
    return results;
}

/**
 * @brief Zwraca szczegółowe wyniki analizy danych
 *
 * @return Referencja do stałej mapy zawierającej pary data-krotka z wartościami (aktualna wartość, min, max)
 */
const std::map<std::string, std::tuple<double, double, double>>& DataAnalyzer::getAnalysisResults() const {
    return analysisResults;
}
