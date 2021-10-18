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
        double x[3], y[3], z[3]; // ToDo: Need initialization?
        for (int j_point = 0; j_point < 3; j_point++)
        {
            std::cin >> x[j_point] >> y[j_point] >> z[j_point];

            // ToDo: delete copypaste?

            if (std::isnan (area_max.x_)) area_max.x_ = x[j_point];
            if (std::isnan (area_max.y_)) area_max.y_ = y[j_point];
            if (std::isnan (area_max.z_)) area_max.z_ = z[j_point];

            if (std::isnan (area_min.x_)) area_min.x_ = x[j_point];
            if (std::isnan (area_min.y_)) area_min.y_ = y[j_point];
            if (std::isnan (area_min.z_)) area_min.z_ = z[j_point];
            
            area_max.x_ = std::max (area_max.x_, x[j_point]);
            area_max.y_ = std::max (area_max.y_, y[j_point]);
            area_max.z_ = std::max (area_max.z_, z[j_point]);

            area_min.x_ = std::min (area_min.x_, x[j_point]);
            area_min.y_ = std::min (area_min.y_, y[j_point]);
            area_min.z_ = std::min (area_min.z_, z[j_point]);
        }

        geo::Vector A (x[0], y[0], z[0]);
        geo::Vector B (x[1], y[1], z[1]);
        geo::Vector C (x[2], y[2], z[2]);
        
        trs.push_back ({i_elem, {A, B, C}});
    }

    geo::Area area {area_max + geo::EPS_Vec,
                    area_min - geo::EPS_Vec, 
                    (int) (log2 ((double) N) / 3) + 1};

    for (auto i_tr = trs.begin(); i_tr != trs.end(); i_tr++)
        area.AddTriangle (i_tr);

    // area.PrintLog (0);
    area.IntersectTriangles(intersect_set);
    
    for (auto i_elem: intersect_set)
        std::cout << i_elem << std::endl;

    return 0;
}
