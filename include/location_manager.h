#pragma once

#include <string>
#include <vector>
#include <map>

namespace Astro {

struct Location {
    std::string name;
    std::string country;
    std::string region;
    double latitude;
    double longitude;
    double timezone;
    std::string timezoneStr;
    int population = 0; // For ranking results
};

class LocationManager {
public:
    LocationManager();

    // Search for locations by name
    std::vector<Location> searchLocations(const std::string& query, int maxResults = 10) const;

    // Get location by exact name match
    Location getLocationByName(const std::string& name) const;

    // List all available locations
    std::vector<Location> getAllLocations() const;

    // List locations by country
    std::vector<Location> getLocationsByCountry(const std::string& country) const;

    // Get popular/major cities
    std::vector<Location> getPopularLocations() const;

    // Check if location exists
    bool hasLocation(const std::string& name) const;

private:
    std::vector<Location> locations;
    std::map<std::string, std::vector<int>> nameIndex;

    void initializeDatabase();
    void addLocation(const std::string& name, const std::string& country,
                    const std::string& region, double lat, double lon,
                    double tz, const std::string& tzStr, int population = 0);
    void buildIndex();
    std::vector<std::string> tokenize(const std::string& text) const;
    double calculateRelevance(const Location& location, const std::string& query) const;
};

} // namespace Astro
