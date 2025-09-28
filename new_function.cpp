void HinduCalendar::calculateSunMoonTimes(PanchangaData& panchanga, double latitude, double longitude, double timezone) const {
    // BRAND NEW CLEAN IMPLEMENTATION USING SWISS EPHEMERIS RISE/SET FUNCTIONS
    try {
        // Setup Swiss Ephemeris parameters - using exact same approach as working Drik sunrise
        AtmosphericModel atmosphere = getSeasonalAtmosphere(panchanga.julianDay, latitude, longitude);
        double geopos[3] = {longitude, latitude, 0.0}; // Sea level
        double tret[10];  // Swiss Ephemeris result array
        char serr[AS_MAXCH]; // Error string
        int flags = SEFLG_SWIEPH | SEFLG_TOPOCTR; // Same flags as Drik
        
        // Use provided timezone or calculate from longitude
        double timezoneOffset = (timezone != 0.0) ? timezone : round(longitude / 15.0);
        
        // SUNRISE CALCULATION - Using Swiss Ephemeris swe_rise_trans
        int result = swe_rise_trans(panchanga.julianDay, SE_SUN, nullptr, flags, SE_CALC_RISE,
                                   geopos, atmosphere.pressure, atmosphere.temperature, tret, serr);
        if (result != ERR) {
            // Convert to local time using exact Drik method
            panchanga.sunriseTime = (tret[0] - floor(tret[0])) * 24.0 + timezoneOffset;
            if (panchanga.sunriseTime >= 24.0) panchanga.sunriseTime -= 24.0;
            if (panchanga.sunriseTime < 0.0) panchanga.sunriseTime += 24.0;
        } else {
            panchanga.sunriseTime = 6.0; // Default fallback
        }

        // SUNSET CALCULATION - Using Swiss Ephemeris swe_rise_trans
        result = swe_rise_trans(panchanga.julianDay, SE_SUN, nullptr, flags, SE_CALC_SET,
                               geopos, atmosphere.pressure, atmosphere.temperature, tret, serr);
        if (result != ERR) {
            // Convert to local time using exact Drik method
            panchanga.sunsetTime = (tret[0] - floor(tret[0])) * 24.0 + timezoneOffset;
            if (panchanga.sunsetTime >= 24.0) panchanga.sunsetTime -= 24.0;
            if (panchanga.sunsetTime < 0.0) panchanga.sunsetTime += 24.0;
        } else {
            panchanga.sunsetTime = 18.0; // Default fallback
        }

        // MOONRISE CALCULATION - Using Swiss Ephemeris swe_rise_trans
        result = swe_rise_trans(panchanga.julianDay, SE_MOON, nullptr, flags, SE_CALC_RISE,
                               geopos, atmosphere.pressure, atmosphere.temperature, tret, serr);
        if (result != ERR) {
            // Convert to local time using exact Drik method
            panchanga.moonriseTime = (tret[0] - floor(tret[0])) * 24.0 + timezoneOffset;
            if (panchanga.moonriseTime >= 24.0) panchanga.moonriseTime -= 24.0;
            if (panchanga.moonriseTime < 0.0) panchanga.moonriseTime += 24.0;
        } else {
            panchanga.moonriseTime = 7.0; // Default fallback
        }

        // MOONSET CALCULATION - Using Swiss Ephemeris swe_rise_trans
        result = swe_rise_trans(panchanga.julianDay, SE_MOON, nullptr, flags, SE_CALC_SET,
                               geopos, atmosphere.pressure, atmosphere.temperature, tret, serr);
        if (result != ERR) {
            // Convert to local time using exact Drik method
            panchanga.moonsetTime = (tret[0] - floor(tret[0])) * 24.0 + timezoneOffset;
            if (panchanga.moonsetTime >= 24.0) panchanga.moonsetTime -= 24.0;
            if (panchanga.moonsetTime < 0.0) panchanga.moonsetTime += 24.0;
        } else {
            panchanga.moonsetTime = 19.0; // Default fallback
        }

        // Calculate derived time periods
        panchanga.dayLength = panchanga.sunsetTime - panchanga.sunriseTime;
        if (panchanga.dayLength < 0) panchanga.dayLength += 24.0;
        panchanga.nightLength = 24.0 - panchanga.dayLength;

        // Calculate muhurta times
        panchanga.brahmaMuhurtaStart = calculateBrahmaMuhurta(panchanga.sunriseTime, true);
        panchanga.brahmaMuhurtaEnd = calculateBrahmaMuhurta(panchanga.sunriseTime, false);
        panchanga.abhijitStart = calculateAbhijitMuhurta(panchanga.sunriseTime, panchanga.sunsetTime, true);
        panchanga.abhijitEnd = calculateAbhijitMuhurta(panchanga.sunriseTime, panchanga.sunsetTime, false);
        double nextSunrise = panchanga.sunriseTime + 24.0;
        panchanga.nishitaMuhurtaStart = calculateNishitaMuhurta(panchanga.sunsetTime, nextSunrise, true);
        panchanga.nishitaMuhurtaEnd = calculateNishitaMuhurta(panchanga.sunsetTime, nextSunrise, false);

    } catch (const std::exception& e) {
        // Set safe default values in case of any error
        panchanga.sunriseTime = 6.0;
        panchanga.sunsetTime = 18.0;
        panchanga.moonriseTime = 7.0;
        panchanga.moonsetTime = 19.0;
        panchanga.dayLength = 12.0;
        panchanga.nightLength = 12.0;
    }
}
