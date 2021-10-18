#include "geometry.h"
#include <set>
#include <iostream>
#include <vector>

int main()
{
    std::set <size_t> intersect_set;

    size_t N = 0;
    std::cin >> N;
    std::vector<geo::Triangle> trs;     

    for (size_t i_elem = 0; i_elem < N; i_elem++)
    {
        double x[3], y[3], z[3];
        for (int j_point = 0; j_point < 3; j_point++)
            std::cin >> x[j_point] >> y[j_point] >> z[j_point];
        
        trs.push_back ({(geo::Vector) {x[0], y[0], z[0]},
                        (geo::Vector) {x[1], y[1], z[1]},
                        (geo::Vector) {x[2], y[2], z[2]}});
    }

    for (size_t i_elem = 0; i_elem < N; i_elem++)
        for (size_t j_elem = i_elem + 1; j_elem < N; j_elem++)
            if (trs[i_elem].CheckGeneralIntersect(trs[j_elem]))
            {
                intersect_set.insert (i_elem);
                intersect_set.insert (j_elem);
            }

    for (auto i_elem: intersect_set)
        std::cout << i_elem << std::endl;

    return 0;
}
