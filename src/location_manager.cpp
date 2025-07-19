#include "location_manager.h"
#include <algorithm>
#include <sstream>
#include <cctype>
#include <cmath>
#include <set>

namespace Astro {

LocationManager::LocationManager() {
    initializeDatabase();
    buildIndex();
}

void LocationManager::initializeDatabase() {
    // Major world cities with accurate coordinates and timezones
    // Format: addLocation(name, country, region, lat, lon, timezone_offset, timezone_string, population)

    // United States
    addLocation("New York", "USA", "New York", 40.7128, -74.0060, -5, "America/New_York", 8400000);
    addLocation("Los Angeles", "USA", "California", 34.0522, -118.2437, -8, "America/Los_Angeles", 4000000);
    addLocation("Chicago", "USA", "Illinois", 41.8781, -87.6298, -6, "America/Chicago", 2700000);
    addLocation("Houston", "USA", "Texas", 29.7604, -95.3698, -6, "America/Chicago", 2300000);
    addLocation("Phoenix", "USA", "Arizona", 33.4484, -112.0740, -7, "America/Phoenix", 1600000);
    addLocation("Philadelphia", "USA", "Pennsylvania", 39.9526, -75.1652, -5, "America/New_York", 1500000);
    addLocation("San Antonio", "USA", "Texas", 29.4241, -98.4936, -6, "America/Chicago", 1500000);
    addLocation("San Diego", "USA", "California", 32.7157, -117.1611, -8, "America/Los_Angeles", 1400000);
    addLocation("Dallas", "USA", "Texas", 32.7767, -96.7970, -6, "America/Chicago", 1300000);
    addLocation("San Jose", "USA", "California", 37.3382, -121.8863, -8, "America/Los_Angeles", 1000000);
    addLocation("Austin", "USA", "Texas", 30.2672, -97.7431, -6, "America/Chicago", 950000);
    addLocation("San Francisco", "USA", "California", 37.7749, -122.4194, -8, "America/Los_Angeles", 875000);
    addLocation("Miami", "USA", "Florida", 25.7617, -80.1918, -5, "America/New_York", 470000);
    addLocation("Seattle", "USA", "Washington", 47.6062, -122.3321, -8, "America/Los_Angeles", 750000);
    addLocation("Denver", "USA", "Colorado", 39.7392, -104.9903, -7, "America/Denver", 715000);
    addLocation("Boston", "USA", "Massachusetts", 42.3601, -71.0589, -5, "America/New_York", 685000);
    addLocation("Las Vegas", "USA", "Nevada", 36.1699, -115.1398, -8, "America/Los_Angeles", 650000);
    addLocation("Atlanta", "USA", "Georgia", 33.7490, -84.3880, -5, "America/New_York", 500000);

    // Canada
    addLocation("Toronto", "Canada", "Ontario", 43.6532, -79.3832, -5, "America/Toronto", 2930000);
    addLocation("Montreal", "Canada", "Quebec", 45.5017, -73.5673, -5, "America/Montreal", 1700000);
    addLocation("Vancouver", "Canada", "British Columbia", 49.2827, -123.1207, -8, "America/Vancouver", 675000);
    addLocation("Calgary", "Canada", "Alberta", 51.0447, -114.0719, -7, "America/Edmonton", 1336000);
    addLocation("Ottawa", "Canada", "Ontario", 45.4215, -75.6972, -5, "America/Toronto", 995000);
    addLocation("Edmonton", "Canada", "Alberta", 53.5461, -113.4938, -7, "America/Edmonton", 980000);
    addLocation("Quebec City", "Canada", "Quebec", 46.8139, -71.2080, -5, "America/Montreal", 540000);
    addLocation("Winnipeg", "Canada", "Manitoba", 49.8951, -97.1384, -6, "America/Winnipeg", 750000);

    // United Kingdom
    addLocation("London", "UK", "England", 51.5074, -0.1278, 0, "Europe/London", 9000000);
    addLocation("Birmingham", "UK", "England", 52.4862, -1.8904, 0, "Europe/London", 1140000);
    addLocation("Manchester", "UK", "England", 53.4808, -2.2426, 0, "Europe/London", 550000);
    addLocation("Glasgow", "UK", "Scotland", 55.8642, -4.2518, 0, "Europe/London", 635000);
    addLocation("Edinburgh", "UK", "Scotland", 55.9533, -3.1883, 0, "Europe/London", 540000);
    addLocation("Liverpool", "UK", "England", 53.4084, -2.9916, 0, "Europe/London", 500000);
    addLocation("Leeds", "UK", "England", 53.8008, -1.5491, 0, "Europe/London", 790000);
    addLocation("Sheffield", "UK", "England", 53.3811, -1.4701, 0, "Europe/London", 580000);
    addLocation("Bristol", "UK", "England", 51.4545, -2.5879, 0, "Europe/London", 470000);
    addLocation("Cardiff", "UK", "Wales", 51.4816, -3.1791, 0, "Europe/London", 365000);
    addLocation("Belfast", "UK", "Northern Ireland", 54.5973, -5.9301, 0, "Europe/London", 340000);

    // France
    addLocation("Paris", "France", "Île-de-France", 48.8566, 2.3522, 1, "Europe/Paris", 11000000);
    addLocation("Lyon", "France", "Auvergne-Rhône-Alpes", 45.7640, 4.8357, 1, "Europe/Paris", 1700000);
    addLocation("Marseille", "France", "Provence-Alpes-Côte d'Azur", 43.2965, 5.3698, 1, "Europe/Paris", 1600000);
    addLocation("Toulouse", "France", "Occitanie", 43.6047, 1.4442, 1, "Europe/Paris", 970000);
    addLocation("Nice", "France", "Provence-Alpes-Côte d'Azur", 43.7102, 7.2620, 1, "Europe/Paris", 350000);
    addLocation("Nantes", "France", "Pays de la Loire", 47.2184, -1.5536, 1, "Europe/Paris", 650000);
    addLocation("Montpellier", "France", "Occitanie", 43.6110, 3.8767, 1, "Europe/Paris", 300000);
    addLocation("Strasbourg", "France", "Grand Est", 48.5734, 7.7521, 1, "Europe/Paris", 480000);
    addLocation("Bordeaux", "France", "Nouvelle-Aquitaine", 44.8378, -0.5792, 1, "Europe/Paris", 760000);
    addLocation("Lille", "France", "Hauts-de-France", 50.6292, 3.0573, 1, "Europe/Paris", 1200000);

    // Germany
    addLocation("Berlin", "Germany", "Berlin", 52.5200, 13.4050, 1, "Europe/Berlin", 3700000);
    addLocation("Hamburg", "Germany", "Hamburg", 53.5511, 9.9937, 1, "Europe/Berlin", 1900000);
    addLocation("Munich", "Germany", "Bavaria", 48.1351, 11.5820, 1, "Europe/Berlin", 1500000);
    addLocation("Cologne", "Germany", "North Rhine-Westphalia", 50.9375, 6.9603, 1, "Europe/Berlin", 1100000);
    addLocation("Frankfurt", "Germany", "Hesse", 50.1109, 8.6821, 1, "Europe/Berlin", 760000);
    addLocation("Stuttgart", "Germany", "Baden-Württemberg", 48.7758, 9.1829, 1, "Europe/Berlin", 630000);
    addLocation("Düsseldorf", "Germany", "North Rhine-Westphalia", 51.2277, 6.7735, 1, "Europe/Berlin", 620000);
    addLocation("Leipzig", "Germany", "Saxony", 51.3397, 12.3731, 1, "Europe/Berlin", 600000);
    addLocation("Dortmund", "Germany", "North Rhine-Westphalia", 51.5136, 7.4653, 1, "Europe/Berlin", 590000);
    addLocation("Essen", "Germany", "North Rhine-Westphalia", 51.4556, 7.0116, 1, "Europe/Berlin", 580000);

    // Italy
    addLocation("Rome", "Italy", "Lazio", 41.9028, 12.4964, 1, "Europe/Rome", 2870000);
    addLocation("Milan", "Italy", "Lombardy", 45.4642, 9.1900, 1, "Europe/Rome", 1400000);
    addLocation("Naples", "Italy", "Campania", 40.8518, 14.2681, 1, "Europe/Rome", 970000);
    addLocation("Turin", "Italy", "Piedmont", 45.0703, 7.6869, 1, "Europe/Rome", 870000);
    addLocation("Palermo", "Italy", "Sicily", 38.1157, 13.3613, 1, "Europe/Rome", 670000);
    addLocation("Genoa", "Italy", "Liguria", 44.4056, 8.9463, 1, "Europe/Rome", 580000);
    addLocation("Bologna", "Italy", "Emilia-Romagna", 44.4949, 11.3426, 1, "Europe/Rome", 390000);
    addLocation("Florence", "Italy", "Tuscany", 43.7696, 11.2558, 1, "Europe/Rome", 380000);
    addLocation("Venice", "Italy", "Veneto", 45.4408, 12.3155, 1, "Europe/Rome", 260000);

    // Spain
    addLocation("Madrid", "Spain", "Community of Madrid", 40.4168, -3.7038, 1, "Europe/Madrid", 6600000);
    addLocation("Barcelona", "Spain", "Catalonia", 41.3874, 2.1686, 1, "Europe/Madrid", 5500000);
    addLocation("Valencia", "Spain", "Valencian Community", 39.4699, -0.3763, 1, "Europe/Madrid", 1600000);
    addLocation("Seville", "Spain", "Andalusia", 37.3891, -5.9845, 1, "Europe/Madrid", 690000);
    addLocation("Zaragoza", "Spain", "Aragon", 41.6488, -0.8891, 1, "Europe/Madrid", 680000);
    addLocation("Málaga", "Spain", "Andalusia", 36.7213, -4.4214, 1, "Europe/Madrid", 580000);
    addLocation("Murcia", "Spain", "Region of Murcia", 37.9922, -1.1307, 1, "Europe/Madrid", 460000);
    addLocation("Palma", "Spain", "Balearic Islands", 39.5696, 2.6502, 1, "Europe/Madrid", 420000);
    addLocation("Las Palmas", "Spain", "Canary Islands", 28.1248, -15.4300, 0, "Atlantic/Canary", 380000);

    // Japan
    addLocation("Tokyo", "Japan", "Kantō", 35.6762, 139.6503, 9, "Asia/Tokyo", 37000000);
    addLocation("Osaka", "Japan", "Kansai", 34.6937, 135.5023, 9, "Asia/Tokyo", 19000000);
    addLocation("Nagoya", "Japan", "Chūbu", 35.1815, 136.9066, 9, "Asia/Tokyo", 10000000);
    addLocation("Sapporo", "Japan", "Hokkaidō", 43.0642, 141.3469, 9, "Asia/Tokyo", 2700000);
    addLocation("Fukuoka", "Japan", "Kyūshū", 33.5904, 130.4017, 9, "Asia/Tokyo", 2600000);
    addLocation("Kobe", "Japan", "Kansai", 34.6901, 135.1955, 9, "Asia/Tokyo", 1500000);
    addLocation("Kyoto", "Japan", "Kansai", 35.0116, 135.7681, 9, "Asia/Tokyo", 1500000);
    addLocation("Yokohama", "Japan", "Kantō", 35.4437, 139.6380, 9, "Asia/Tokyo", 3700000);

    // China
    addLocation("Beijing", "China", "Beijing", 39.9042, 116.4074, 8, "Asia/Shanghai", 21500000);
    addLocation("Shanghai", "China", "Shanghai", 31.2304, 121.4737, 8, "Asia/Shanghai", 24200000);
    addLocation("Guangzhou", "China", "Guangdong", 23.1291, 113.2644, 8, "Asia/Shanghai", 13100000);
    addLocation("Shenzhen", "China", "Guangdong", 22.5431, 114.0579, 8, "Asia/Shanghai", 12400000);
    addLocation("Chengdu", "China", "Sichuan", 30.5728, 104.0668, 8, "Asia/Shanghai", 16300000);
    addLocation("Chongqing", "China", "Chongqing", 29.5630, 106.5516, 8, "Asia/Shanghai", 31200000);
    addLocation("Tianjin", "China", "Tianjin", 39.3434, 117.3616, 8, "Asia/Shanghai", 13900000);
    addLocation("Hangzhou", "China", "Zhejiang", 30.2741, 120.1551, 8, "Asia/Shanghai", 10400000);
    addLocation("Nanjing", "China", "Jiangsu", 32.0603, 118.7969, 8, "Asia/Shanghai", 8500000);
    addLocation("Xi'an", "China", "Shaanxi", 34.3416, 108.9398, 8, "Asia/Shanghai", 12900000);
    addLocation("Wuhan", "China", "Hubei", 30.5928, 114.3055, 8, "Asia/Shanghai", 11000000);
    addLocation("Dongguan", "China", "Guangdong", 23.0489, 113.7447, 8, "Asia/Shanghai", 8300000);
    addLocation("Foshan", "China", "Guangdong", 23.0218, 113.1064, 8, "Asia/Shanghai", 7200000);
    addLocation("Shenyang", "China", "Liaoning", 41.8057, 123.4315, 8, "Asia/Shanghai", 6300000);
    addLocation("Harbin", "China", "Heilongjiang", 45.7536, 126.6460, 8, "Asia/Shanghai", 5900000);
    addLocation("Suzhou", "China", "Jiangsu", 31.2989, 120.5853, 8, "Asia/Shanghai", 10700000);
    addLocation("Qingdao", "China", "Shandong", 36.0986, 120.3719, 8, "Asia/Shanghai", 9000000);
    addLocation("Dalian", "China", "Liaoning", 38.9140, 121.6147, 8, "Asia/Shanghai", 5900000);
    addLocation("Zhengzhou", "China", "Henan", 34.7466, 113.6254, 8, "Asia/Shanghai", 10100000);
    addLocation("Shantou", "China", "Guangdong", 23.3681, 116.7081, 8, "Asia/Shanghai", 5500000);
    addLocation("Jinan", "China", "Shandong", 36.6512, 117.1201, 8, "Asia/Shanghai", 8700000);
    addLocation("Changchun", "China", "Jilin", 43.8171, 125.3235, 8, "Asia/Shanghai", 7700000);
    addLocation("Kunming", "China", "Yunnan", 25.0389, 102.7183, 8, "Asia/Shanghai", 6600000);
    addLocation("Changsha", "China", "Hunan", 28.2282, 112.9388, 8, "Asia/Shanghai", 7000000);
    addLocation("Taiyuan", "China", "Shanxi", 37.8570, 112.5489, 8, "Asia/Shanghai", 4200000);
    addLocation("Xiamen", "China", "Fujian", 24.4798, 118.0819, 8, "Asia/Shanghai", 4000000);
    addLocation("Hefei", "China", "Anhui", 31.8639, 117.2808, 8, "Asia/Shanghai", 9370000);
    addLocation("Urumqi", "China", "Xinjiang", 43.8256, 87.6168, 8, "Asia/Shanghai", 3500000);
    addLocation("Fuzhou", "China", "Fujian", 26.0745, 119.2965, 8, "Asia/Shanghai", 7700000);
    addLocation("Wuxi", "China", "Jiangsu", 31.5804, 120.2936, 8, "Asia/Shanghai", 6600000);
    addLocation("Ningbo", "China", "Zhejiang", 29.8683, 121.5440, 8, "Asia/Shanghai", 8200000);

    // India
    addLocation("Mumbai", "India", "Maharashtra", 19.0760, 72.8777, 5.5, "Asia/Kolkata", 20400000);
    addLocation("Delhi", "India", "Delhi", 28.7041, 77.1025, 5.5, "Asia/Kolkata", 32900000);
    addLocation("Kolkata", "India", "West Bengal", 22.5726, 88.3639, 5.5, "Asia/Kolkata", 14800000);
    addLocation("Chennai", "India", "Tamil Nadu", 13.0827, 80.2707, 5.5, "Asia/Kolkata", 11000000);
    addLocation("Bangalore", "India", "Karnataka", 12.9716, 77.5946, 5.5, "Asia/Kolkata", 12300000);
    addLocation("Hyderabad", "India", "Telangana", 17.3850, 78.4867, 5.5, "Asia/Kolkata", 10000000);
    addLocation("Pune", "India", "Maharashtra", 18.5204, 73.8567, 5.5, "Asia/Kolkata", 7400000);
    addLocation("Ahmedabad", "India", "Gujarat", 23.0225, 72.5714, 5.5, "Asia/Kolkata", 8300000);
    addLocation("Surat", "India", "Gujarat", 21.1702, 72.8311, 5.5, "Asia/Kolkata", 6600000);
    addLocation("Jaipur", "India", "Rajasthan", 26.9124, 75.7873, 5.5, "Asia/Kolkata", 3900000);

    // Australia
    addLocation("Sydney", "Australia", "New South Wales", -33.8688, 151.2093, 10, "Australia/Sydney", 5300000);
    addLocation("Melbourne", "Australia", "Victoria", -37.8136, 144.9631, 10, "Australia/Melbourne", 5000000);
    addLocation("Brisbane", "Australia", "Queensland", -27.4698, 153.0251, 10, "Australia/Brisbane", 2500000);
    addLocation("Perth", "Australia", "Western Australia", -31.9505, 115.8605, 8, "Australia/Perth", 2100000);
    addLocation("Adelaide", "Australia", "South Australia", -34.9285, 138.6007, 9.5, "Australia/Adelaide", 1400000);
    addLocation("Gold Coast", "Australia", "Queensland", -28.0167, 153.4000, 10, "Australia/Brisbane", 680000);
    addLocation("Newcastle", "Australia", "New South Wales", -32.9283, 151.7817, 10, "Australia/Sydney", 490000);
    addLocation("Canberra", "Australia", "Australian Capital Territory", -35.2809, 149.1300, 10, "Australia/Sydney", 430000);
    addLocation("Sunshine Coast", "Australia", "Queensland", -26.6500, 153.0667, 10, "Australia/Brisbane", 350000);
    addLocation("Wollongong", "Australia", "New South Wales", -34.4244, 150.8931, 10, "Australia/Sydney", 300000);

    // Russia
    addLocation("Moscow", "Russia", "Moscow", 55.7558, 37.6176, 3, "Europe/Moscow", 12500000);
    addLocation("Saint Petersburg", "Russia", "Northwestern Federal District", 59.9311, 30.3609, 3, "Europe/Moscow", 5400000);
    addLocation("Novosibirsk", "Russia", "Siberian Federal District", 55.0084, 82.9357, 7, "Asia/Novosibirsk", 1600000);
    addLocation("Yekaterinburg", "Russia", "Ural Federal District", 56.8431, 60.6454, 5, "Asia/Yekaterinburg", 1500000);
    addLocation("Kazan", "Russia", "Volga Federal District", 55.8304, 49.0661, 3, "Europe/Moscow", 1300000);
    addLocation("Nizhny Novgorod", "Russia", "Volga Federal District", 56.2965, 43.9361, 3, "Europe/Moscow", 1200000);
    addLocation("Chelyabinsk", "Russia", "Ural Federal District", 55.1644, 61.4368, 5, "Asia/Yekaterinburg", 1200000);
    addLocation("Samara", "Russia", "Volga Federal District", 53.2001, 50.1500, 4, "Europe/Samara", 1200000);
    addLocation("Omsk", "Russia", "Siberian Federal District", 54.9884, 73.3242, 6, "Asia/Omsk", 1200000);
    addLocation("Rostov-on-Don", "Russia", "Southern Federal District", 47.2357, 39.7015, 3, "Europe/Moscow", 1100000);

    // Brazil
    addLocation("São Paulo", "Brazil", "São Paulo", -23.5505, -46.6333, -3, "America/Sao_Paulo", 22000000);
    addLocation("Rio de Janeiro", "Brazil", "Rio de Janeiro", -22.9068, -43.1729, -3, "America/Sao_Paulo", 13300000);
    addLocation("Brasília", "Brazil", "Federal District", -15.8267, -47.9218, -3, "America/Sao_Paulo", 3100000);
    addLocation("Salvador", "Brazil", "Bahia", -12.9714, -38.5014, -3, "America/Bahia", 2900000);
    addLocation("Fortaleza", "Brazil", "Ceará", -3.7319, -38.5267, -3, "America/Fortaleza", 2700000);
    addLocation("Belo Horizonte", "Brazil", "Minas Gerais", -19.9191, -43.9386, -3, "America/Sao_Paulo", 2500000);
    addLocation("Manaus", "Brazil", "Amazonas", -3.1190, -60.0217, -4, "America/Manaus", 2200000);
    addLocation("Curitiba", "Brazil", "Paraná", -25.4284, -49.2733, -3, "America/Sao_Paulo", 1900000);
    addLocation("Recife", "Brazil", "Pernambuco", -8.0476, -34.8770, -3, "America/Recife", 1650000);
    addLocation("Porto Alegre", "Brazil", "Rio Grande do Sul", -30.0346, -51.2177, -3, "America/Sao_Paulo", 1500000);

    // Mexico
    addLocation("Mexico City", "Mexico", "Mexico City", 19.4326, -99.1332, -6, "America/Mexico_City", 21800000);
    addLocation("Guadalajara", "Mexico", "Jalisco", 20.6597, -103.3496, -6, "America/Mexico_City", 5200000);
    addLocation("Monterrey", "Mexico", "Nuevo León", 25.6866, -100.3161, -6, "America/Monterrey", 4700000);
    addLocation("Puebla", "Mexico", "Puebla", 19.0414, -98.2063, -6, "America/Mexico_City", 3200000);
    addLocation("Tijuana", "Mexico", "Baja California", 32.5149, -117.0382, -8, "America/Tijuana", 2100000);
    addLocation("León", "Mexico", "Guanajuato", 21.1619, -101.6921, -6, "America/Mexico_City", 1900000);
    addLocation("Juárez", "Mexico", "Chihuahua", 31.6904, -106.4245, -7, "America/Ojinaga", 1500000);
    addLocation("Zapopan", "Mexico", "Jalisco", 20.7214, -103.3918, -6, "America/Mexico_City", 1400000);
    addLocation("Nezahualcóyotl", "Mexico", "México", 19.4007, -99.0146, -6, "America/Mexico_City", 1100000);

    // South Korea
    addLocation("Seoul", "South Korea", "Seoul Capital Area", 37.5665, 126.9780, 9, "Asia/Seoul", 25600000);
    addLocation("Busan", "South Korea", "South Gyeongsang Province", 35.1796, 129.0756, 9, "Asia/Seoul", 3400000);
    addLocation("Incheon", "South Korea", "Seoul Capital Area", 37.4563, 126.7052, 9, "Asia/Seoul", 2900000);
    addLocation("Daegu", "South Korea", "North Gyeongsang Province", 35.8714, 128.6014, 9, "Asia/Seoul", 2500000);
    addLocation("Daejeon", "South Korea", "South Chungcheong Province", 36.3504, 127.3845, 9, "Asia/Seoul", 1500000);
    addLocation("Gwangju", "South Korea", "South Jeolla Province", 35.1595, 126.8526, 9, "Asia/Seoul", 1500000);
    addLocation("Suwon", "South Korea", "Seoul Capital Area", 37.2636, 127.0286, 9, "Asia/Seoul", 1200000);
    addLocation("Ulsan", "South Korea", "South Gyeongsang Province", 35.5384, 129.3114, 9, "Asia/Seoul", 1200000);

    // Argentina
    addLocation("Buenos Aires", "Argentina", "Buenos Aires", -34.6118, -58.3960, -3, "America/Argentina/Buenos_Aires", 15200000);
    addLocation("Córdoba", "Argentina", "Córdoba", -31.4201, -64.1888, -3, "America/Argentina/Cordoba", 1700000);
    addLocation("Rosario", "Argentina", "Santa Fe", -32.9442, -60.6505, -3, "America/Argentina/Buenos_Aires", 1400000);
    addLocation("Mendoza", "Argentina", "Mendoza", -32.8895, -68.8458, -3, "America/Argentina/Mendoza", 1200000);
    addLocation("San Miguel de Tucumán", "Argentina", "Tucumán", -26.8083, -65.2176, -3, "America/Argentina/Tucuman", 900000);
    addLocation("La Plata", "Argentina", "Buenos Aires", -34.9215, -57.9545, -3, "America/Argentina/Buenos_Aires", 900000);
    addLocation("Mar del Plata", "Argentina", "Buenos Aires", -38.0055, -57.5426, -3, "America/Argentina/Buenos_Aires", 650000);
    addLocation("Salta", "Argentina", "Salta", -24.7821, -65.4232, -3, "America/Argentina/Salta", 620000);

    // Egypt
    addLocation("Cairo", "Egypt", "Cairo Governorate", 30.0444, 31.2357, 2, "Africa/Cairo", 20900000);
    addLocation("Alexandria", "Egypt", "Alexandria Governorate", 31.2001, 29.9187, 2, "Africa/Cairo", 5200000);
    addLocation("Giza", "Egypt", "Giza Governorate", 30.0131, 31.2089, 2, "Africa/Cairo", 4400000);
    addLocation("Shubra El Kheima", "Egypt", "Qalyubia Governorate", 30.1217, 31.2444, 2, "Africa/Cairo", 1200000);
    addLocation("Port Said", "Egypt", "Port Said Governorate", 31.2653, 32.3019, 2, "Africa/Cairo", 750000);
    addLocation("Suez", "Egypt", "Suez Governorate", 29.9668, 32.5498, 2, "Africa/Cairo", 750000);
    addLocation("Luxor", "Egypt", "Luxor Governorate", 25.6872, 32.6396, 2, "Africa/Cairo", 500000);
    addLocation("Mansoura", "Egypt", "Dakahlia Governorate", 31.0409, 31.3785, 2, "Africa/Cairo", 500000);

    // South Africa
    addLocation("Johannesburg", "South Africa", "Gauteng", -26.2041, 28.0473, 2, "Africa/Johannesburg", 5600000);
    addLocation("Cape Town", "South Africa", "Western Cape", -33.9249, 18.4241, 2, "Africa/Johannesburg", 4600000);
    addLocation("Durban", "South Africa", "KwaZulu-Natal", -29.8587, 31.0218, 2, "Africa/Johannesburg", 3700000);
    addLocation("Pretoria", "South Africa", "Gauteng", -25.7479, 28.2293, 2, "Africa/Johannesburg", 2500000);
    addLocation("Port Elizabeth", "South Africa", "Eastern Cape", -33.9608, 25.6022, 2, "Africa/Johannesburg", 1300000);
    addLocation("Pietermaritzburg", "South Africa", "KwaZulu-Natal", -29.6094, 30.3781, 2, "Africa/Johannesburg", 680000);
    addLocation("East London", "South Africa", "Eastern Cape", -33.0153, 27.9116, 2, "Africa/Johannesburg", 480000);
    addLocation("Bloemfontein", "South Africa", "Free State", -29.0852, 26.1596, 2, "Africa/Johannesburg", 370000);

    // Turkey
    addLocation("Istanbul", "Turkey", "Istanbul Province", 41.0082, 28.9784, 3, "Europe/Istanbul", 15500000);
    addLocation("Ankara", "Turkey", "Ankara Province", 39.9334, 32.8597, 3, "Europe/Istanbul", 5600000);
    addLocation("Izmir", "Turkey", "İzmir Province", 38.4192, 27.1287, 3, "Europe/Istanbul", 4400000);
    addLocation("Bursa", "Turkey", "Bursa Province", 40.1826, 29.0665, 3, "Europe/Istanbul", 3100000);
    addLocation("Adana", "Turkey", "Adana Province", 37.0000, 35.3213, 3, "Europe/Istanbul", 2200000);
    addLocation("Gaziantep", "Turkey", "Gaziantep Province", 37.0662, 37.3833, 3, "Europe/Istanbul", 2100000);
    addLocation("Konya", "Turkey", "Konya Province", 37.8667, 32.4833, 3, "Europe/Istanbul", 1400000);
    addLocation("Antalya", "Turkey", "Antalya Province", 36.8969, 30.7133, 3, "Europe/Istanbul", 1350000);

    // Myanmar (Burma)
    addLocation("Yangon", "Myanmar", "Yangon Region", 16.8661, 96.1951, 6.5, "Asia/Yangon", 5200000);
    addLocation("Mandalay", "Myanmar", "Mandalay Region", 21.9588, 96.0891, 6.5, "Asia/Yangon", 1400000);
    addLocation("Naypyidaw", "Myanmar", "Naypyidaw Union Territory", 19.7633, 96.0785, 6.5, "Asia/Yangon", 1200000);
    addLocation("Mawlamyine", "Myanmar", "Mon State", 16.4919, 97.6278, 6.5, "Asia/Yangon", 300000);
    addLocation("Taunggyi", "Myanmar", "Shan State", 20.7833, 97.0333, 6.5, "Asia/Yangon", 280000);
    addLocation("Bago", "Myanmar", "Bago Region", 17.3356, 96.4797, 6.5, "Asia/Yangon", 250000);
    addLocation("Pathein", "Myanmar", "Ayeyarwady Region", 16.7831, 94.7325, 6.5, "Asia/Yangon", 240000);
    addLocation("Monywa", "Myanmar", "Sagaing Region", 22.1083, 95.1356, 6.5, "Asia/Yangon", 200000);
    addLocation("Meiktila", "Myanmar", "Mandalay Region", 20.8781, 95.8578, 6.5, "Asia/Yangon", 180000);
    addLocation("Myitkyina", "Myanmar", "Kachin State", 25.3839, 97.3956, 6.5, "Asia/Yangon", 150000);
    addLocation("Sittwe", "Myanmar", "Rakhine State", 20.1500, 92.8833, 6.5, "Asia/Yangon", 140000);
    addLocation("Dawei", "Myanmar", "Tanintharyi Region", 14.0833, 98.2000, 6.5, "Asia/Yangon", 130000);
    addLocation("Pyay", "Myanmar", "Bago Region", 18.8264, 95.2236, 6.5, "Asia/Yangon", 125000);
    addLocation("Hpa-An", "Myanmar", "Kayin State", 16.8836, 97.6342, 6.5, "Asia/Yangon", 120000);
    addLocation("Lashio", "Myanmar", "Shan State", 22.9333, 97.7500, 6.5, "Asia/Yangon", 115000);
    addLocation("Magway", "Myanmar", "Magway Region", 20.1486, 94.9306, 6.5, "Asia/Yangon", 100000);
    addLocation("Pakokku", "Myanmar", "Magway Region", 21.3378, 95.0831, 6.5, "Asia/Yangon", 95000);
    addLocation("Sagaing", "Myanmar", "Sagaing Region", 21.8786, 95.9794, 6.5, "Asia/Yangon", 78000);
    addLocation("Myeik", "Myanmar", "Tanintharyi Region", 12.4500, 98.6000, 6.5, "Asia/Yangon", 75000);
    addLocation("Hakha", "Myanmar", "Chin State", 22.6431, 93.6103, 6.5, "Asia/Yangon", 20000);

    // Thailand
    addLocation("Bangkok", "Thailand", "Bangkok", 13.7563, 100.5018, 7, "Asia/Bangkok", 10500000);
    addLocation("Chiang Mai", "Thailand", "Chiang Mai", 18.7883, 98.9853, 7, "Asia/Bangkok", 1200000);
    addLocation("Pattaya", "Thailand", "Chonburi", 12.9236, 100.8825, 7, "Asia/Bangkok", 320000);
    addLocation("Phuket", "Thailand", "Phuket", 7.8804, 98.3923, 7, "Asia/Bangkok", 600000);
    addLocation("Hat Yai", "Thailand", "Songkhla", 7.0167, 100.4667, 7, "Asia/Bangkok", 800000);
    addLocation("Nonthaburi", "Thailand", "Nonthaburi", 13.8621, 100.5144, 7, "Asia/Bangkok", 270000);
    addLocation("Pak Kret", "Thailand", "Nonthaburi", 13.9092, 100.5048, 7, "Asia/Bangkok", 200000);
    addLocation("Chiang Rai", "Thailand", "Chiang Rai", 19.9105, 99.8406, 7, "Asia/Bangkok", 200000);
    addLocation("Lamphun", "Thailand", "Lamphun", 18.5804, 99.0165, 7, "Asia/Bangkok", 150000);
    addLocation("Udon Thani", "Thailand", "Udon Thani", 17.4138, 102.7875, 7, "Asia/Bangkok", 220000);
    addLocation("Surat Thani", "Thailand", "Surat Thani", 9.1382, 99.3215, 7, "Asia/Bangkok", 130000);
    addLocation("Khon Kaen", "Thailand", "Khon Kaen", 16.4419, 102.8359, 7, "Asia/Bangkok", 220000);
    addLocation("Nakhon Ratchasima", "Thailand", "Nakhon Ratchasima", 14.9799, 102.0977, 7, "Asia/Bangkok", 280000);
    addLocation("Rayong", "Thailand", "Rayong", 12.6807, 101.2539, 7, "Asia/Bangkok", 170000);
    addLocation("Hua Hin", "Thailand", "Prachuap Khiri Khan", 12.5683, 99.9576, 7, "Asia/Bangkok", 85000);

    // More cities can be added as needed...
}

void LocationManager::addLocation(const std::string& name, const std::string& country,
                                 const std::string& region, double lat, double lon,
                                 double tz, const std::string& tzStr, int population) {
    Location loc;
    loc.name = name;
    loc.country = country;
    loc.region = region;
    loc.latitude = lat;
    loc.longitude = lon;
    loc.timezone = tz;
    loc.timezoneStr = tzStr;
    loc.population = population;

    locations.push_back(loc);
}

void LocationManager::buildIndex() {
    for (size_t i = 0; i < locations.size(); ++i) {
        const Location& loc = locations[i];

        // Index by full name
        std::string lowerName = loc.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        nameIndex[lowerName].push_back(i);

        // Index by tokens in name
        std::vector<std::string> tokens = tokenize(loc.name);
        for (const std::string& token : tokens) {
            std::string lowerToken = token;
            std::transform(lowerToken.begin(), lowerToken.end(), lowerToken.begin(), ::tolower);
            if (nameIndex[lowerToken].empty() || nameIndex[lowerToken].back() != i) {
                nameIndex[lowerToken].push_back(i);
            }
        }

        // Index by country
        std::string lowerCountry = loc.country;
        std::transform(lowerCountry.begin(), lowerCountry.end(), lowerCountry.begin(), ::tolower);
        nameIndex[lowerCountry].push_back(i);

        // Index by region
        std::string lowerRegion = loc.region;
        std::transform(lowerRegion.begin(), lowerRegion.end(), lowerRegion.begin(), ::tolower);
        nameIndex[lowerRegion].push_back(i);
    }
}

std::vector<std::string> LocationManager::tokenize(const std::string& text) const {
    std::vector<std::string> tokens;
    std::istringstream stream(text);
    std::string token;

    while (stream >> token) {
        // Remove common separators
        token.erase(std::remove_if(token.begin(), token.end(), [](char c) {
            return c == ',' || c == '-' || c == '(' || c == ')';
        }), token.end());

        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    return tokens;
}

double LocationManager::calculateRelevance(const Location& location, const std::string& query) const {
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

    std::string lowerName = location.name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    std::string lowerCountry = location.country;
    std::transform(lowerCountry.begin(), lowerCountry.end(), lowerCountry.begin(), ::tolower);

    double score = 0.0;

    // Exact name match gets highest score
    if (lowerName == lowerQuery) {
        score += 100.0;
    }
    // Name starts with query
    else if (lowerName.find(lowerQuery) == 0) {
        score += 80.0;
    }
    // Name contains query
    else if (lowerName.find(lowerQuery) != std::string::npos) {
        score += 60.0;
    }

    // Country match
    if (lowerCountry == lowerQuery) {
        score += 40.0;
    } else if (lowerCountry.find(lowerQuery) != std::string::npos) {
        score += 20.0;
    }

    // Population bonus (log scale)
    if (location.population > 0) {
        score += std::log10(location.population) * 2.0;
    }

    return score;
}

std::vector<Location> LocationManager::searchLocations(const std::string& query, int maxResults) const {
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

    std::vector<std::pair<double, const Location*>> results;

    // Use index for initial filtering
    std::set<int> candidateIndices;

    // Check direct matches in index
    for (const auto& entry : nameIndex) {
        if (entry.first.find(lowerQuery) != std::string::npos) {
            for (int idx : entry.second) {
                candidateIndices.insert(idx);
            }
        }
    }

    // If no index matches, do a full search only if query is very short
    if (candidateIndices.empty()) {
        // Only do full search for short queries (like single letters)
        if (lowerQuery.length() <= 2) {
            for (size_t i = 0; i < locations.size(); ++i) {
                candidateIndices.insert(i);
            }
        }
    }

    // Calculate relevance for candidates
    for (int idx : candidateIndices) {
        const Location& loc = locations[idx];
        double relevance = calculateRelevance(loc, query);
        if (relevance > 0) {
            results.push_back({relevance, &loc});
        }
    }

    // Sort by relevance (highest first)
    std::sort(results.begin(), results.end(),
              [](const std::pair<double, const Location*>& a, const std::pair<double, const Location*>& b) {
                  return a.first > b.first;
              });

    // Return top results
    std::vector<Location> topResults;
    int count = std::min(maxResults, static_cast<int>(results.size()));
    for (int i = 0; i < count; ++i) {
        topResults.push_back(*results[i].second);
    }

    return topResults;
}

Location LocationManager::getLocationByName(const std::string& name) const {
    auto results = searchLocations(name, 1);
    if (!results.empty()) {
        return results[0];
    }
    return Location{}; // Return empty location if not found
}

std::vector<Location> LocationManager::getAllLocations() const {
    return locations;
}

std::vector<Location> LocationManager::getLocationsByCountry(const std::string& country) const {
    std::vector<Location> result;
    std::string lowerCountry = country;
    std::transform(lowerCountry.begin(), lowerCountry.end(), lowerCountry.begin(), ::tolower);

    for (const Location& loc : locations) {
        std::string locCountry = loc.country;
        std::transform(locCountry.begin(), locCountry.end(), locCountry.begin(), ::tolower);
        if (locCountry == lowerCountry) {
            result.push_back(loc);
        }
    }

    // Sort by population (descending)
    std::sort(result.begin(), result.end(),
              [](const Location& a, const Location& b) {
                  return a.population > b.population;
              });

    return result;
}

std::vector<Location> LocationManager::getPopularLocations() const {
    std::vector<Location> popular = locations;

    // Sort by population (descending)
    std::sort(popular.begin(), popular.end(),
              [](const Location& a, const Location& b) {
                  return a.population > b.population;
              });

    // Return top 50 most populated cities
    if (popular.size() > 50) {
        popular.resize(50);
    }

    return popular;
}

bool LocationManager::hasLocation(const std::string& name) const {
    return !getLocationByName(name).name.empty();
}

} // namespace Astro
