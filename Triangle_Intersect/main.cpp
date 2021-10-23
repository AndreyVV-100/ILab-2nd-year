#include "geometry.h"
#include <iostream>

int main()
{
    std::set <size_t> intersect_set;

    size_t N = 0;
    std::cin >> N;
    if (!N)
        return 0;

    std::vector <geo :: Area :: SaveT> trs;
    geo::Vector area_max, area_min;

    for (size_t i_elem = 0; i_elem < N; i_elem++)
    {
        geo::Vector tr_vecs[3] = {};

        for (int j_point = 0; j_point < 3; j_point++)
        {
            std::cin >> tr_vecs[j_point];

            area_max.Maximize (tr_vecs[j_point]);
            area_min.Minimize (tr_vecs[j_point]);
        }

        trs.push_back ({i_elem, {tr_vecs[0], tr_vecs[1], tr_vecs[2]}});
    }

    geo::Area area {area_max + geo::EPS_Vec,
                    area_min - geo::EPS_Vec, 
                    static_cast<int> (log2 (static_cast <double> (N)) / 3)};

    for (auto i_tr = trs.begin(); i_tr != trs.end(); i_tr++)
        area.AddTriangle (i_tr);

    // area.PrintLog (0);
    area.IntersectTriangles(intersect_set);
    
    for (auto i_elem: intersect_set)
        std::cout << i_elem << std::endl;
    return 0;
}
