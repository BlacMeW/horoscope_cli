#include <iostream>
#include <cmath>
using namespace std;

int main() {
    // Constants from yan9a/mmcal
    double SOLAR_YEAR = 1577917828.0 / 4320000.0;    // 365.2587565
    double MYANMAR_EPOCH = 1954168.050623;           // Beginning of 0 ME

    // Julian Day for July 19, 2025, 12:00 UTC
    double jd = 2460876.0;

    cout << "Julian Day: " << jd << endl;
    cout << "Solar Year: " << SOLAR_YEAR << endl;
    cout << "Myanmar Epoch: " << MYANMAR_EPOCH << endl;

    // Myanmar year calculation
    long my = static_cast<long>(floor((jd - 0.5 - MYANMAR_EPOCH) / SOLAR_YEAR));
    cout << "Myanmar Year: " << my << endl;

    // Should be around 1387, not negative

    return 0;
}
