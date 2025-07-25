#include <iostream>
#include <string>

// Copy the parseBCDate function for testing
bool parseBCDate(const std::string& dateStr, int& year, int& month, int& day) {
    if (dateStr.empty()) {
        return false;
    }

    std::string processStr = dateStr;
    bool isBCEra = false;

    // Check for explicit BC suffix formats
    if (processStr.find(" BC") != std::string::npos) {
        isBCEra = true;
        size_t bcPos = processStr.find(" BC");
        processStr = processStr.substr(0, bcPos);
    } else if (processStr.find("BC") == processStr.length() - 2) {
        isBCEra = true;
        processStr = processStr.substr(0, processStr.length() - 2);
    }

    // Handle format like "45 BC-01-15" or "45-BC-01-15" 
    if (processStr.find("BC-") != std::string::npos) {
        isBCEra = true;
        size_t bcPos = processStr.find("BC-");
        if (bcPos != std::string::npos) {
            std::string yearPart = processStr.substr(0, bcPos);
            std::string datePart = processStr.substr(bcPos + 3); // Skip "BC-"

            // Remove any space before BC
            if (!yearPart.empty() && yearPart.back() == ' ') {
                yearPart.pop_back();
            }

            // Reconstruct as standard format: YYYY-MM-DD
            // Pad year to 4 digits
            while (yearPart.length() < 4) {
                yearPart = "0" + yearPart;
            }

            processStr = yearPart + "-" + datePart;
        }
    } else if (processStr.find("AD-") != std::string::npos) {
        // Handle AD suffix format
        size_t adPos = processStr.find("AD-");
        if (adPos != std::string::npos) {
            std::string yearPart = processStr.substr(0, adPos);
            std::string datePart = processStr.substr(adPos + 3); // Skip "AD-"

            // Remove any space before AD
            if (!yearPart.empty() && yearPart.back() == ' ') {
                yearPart.pop_back();
            }

            // Pad year to 4 digits if needed
            while (yearPart.length() < 4) {
                yearPart = "0" + yearPart;
            }

            processStr = yearPart + "-" + datePart;
        }
    }

    // Check for BC era with minus sign (original format)
    // Only treat as BC if it starts with minus and has proper length
    if (!isBCEra && processStr.length() >= 11 && processStr[0] == '-') {
        isBCEra = true;
        processStr = processStr.substr(1); // Remove leading minus
    }

    if (processStr.length() != 10 || processStr[4] != '-' || processStr[7] != '-') {
        return false;
    }

    try {
        year = std::stoi(processStr.substr(0, 4));
        month = std::stoi(processStr.substr(5, 2));
        day = std::stoi(processStr.substr(8, 2));

        // Convert to astronomical year numbering for BC dates ONLY
        // In astronomical year numbering: 1 BC = year 0, 2 BC = year -1, etc.
        // For AD dates, leave the year as-is
        if (isBCEra) {
            year = -(year - 1);
        }
        // For standard dates like "2025-07-01", year remains 2025 (no conversion)

        std::cout << "Input: " << dateStr << std::endl;
        std::cout << "Processed: " << processStr << std::endl;
        std::cout << "isBCEra: " << (isBCEra ? "true" : "false") << std::endl;
        std::cout << "Final year: " << year << ", month: " << month << ", day: " << day << std::endl;

        return true;
    } catch (const std::exception&) {
        return false;
    }
}

int main() {
    int year, month, day;
    
    std::cout << "Testing parseBCDate with '2025-07-01':" << std::endl;
    bool result1 = parseBCDate("2025-07-01", year, month, day);
    std::cout << "Result: " << (result1 ? "Success" : "Failed") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Testing parseBCDate with '2025-07-31':" << std::endl;
    bool result2 = parseBCDate("2025-07-31", year, month, day);
    std::cout << "Result: " << (result2 ? "Success" : "Failed") << std::endl;
    
    return 0;
}
