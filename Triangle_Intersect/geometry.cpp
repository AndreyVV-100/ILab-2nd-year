#include "geometry.h"

namespace geo
{

Vector operator* (double num, const Vector& a)
{
    return a * num;
}

std::ostream& operator<< (std::ostream& out, const Vector& vec)
{
    out << "{" << vec.x_ << ", " << vec.y_ << ", " << vec.z_ << "}";
    return out;
}

std::istream& operator>> (std::istream& in, Vector& vec)
{
    in >> vec.x_ >> vec.y_ >> vec.z_;
    vec.Len();
    return in;
}

bool Section :: CheckIntersect (const Section& sec) const
{
    Vector a1 = B_ - A_,
           a2 = sec.B_ - sec.A_; // direct vectors
    Vector n = a1 * a2;

    // Here was magic. If you would understand it, try to solve: alpha * a1 + beta * a2 = r2 - r1
    // Solving of equation is easy in matrix by Cramer's rule.

    if (n.CheckZero())
        return CheckIntersect (sec.A_) || CheckIntersect (sec.B_);

    Vector r2_r1  = sec.A_ - A_, // it's r2 - r1
           det_a1 = r2_r1 * a2,
           det_a2 = a1 * r2_r1;

    double alpha = NAN, beta = NAN;

    for (int i_coord = 0; i_coord < 3; i_coord++)
    {
        if (!IsZero (n[i_coord]))
        {
            alpha = det_a1[i_coord] / n[i_coord];
            beta  = det_a2[i_coord] / n[i_coord];

            if (!IsZero (alpha * a1[i_coord] + beta * a2[i_coord] - r2_r1[i_coord]))
                return false;
            break;
        }
    }
    return In0_1 (alpha) && In0_1 (-beta);
}

bool Section :: CheckIntersect (const Vector& vec) const
{
    Vector rA = vec - A_,
           rB = vec - B_; // from point to section's ends

    if (rA.CheckZero() || rB.CheckZero())
        return true;

    if (!(rB.CheckCollinear (rA))) // In one line?
        return false;

    // rA == -t rB ?
    if (!IsZero (rA.x_))
        return rA.x_ * rB.x_ < 0;
        
    if (!IsZero (rA.y_))
        return rA.y_ * rB.y_ < 0;
        
    return rA.z_ * rB.z_ < 0;
}

std::ostream& operator<< (std::ostream& out, const Section& sec)
{
    out << "{" << sec.A_ << ", " << sec.B_ << "}" << std::endl;
    return out;
}

std::istream& operator>> (std::istream& in, Section& sec)
{
    in >> sec.A_ >> sec.B_;
    return in;
}

Triangle :: TrStat Triangle :: GetStatus () const
{
    if (!CheckCorrect())
        return INCORRECT;

    Vector A_B = A_ - B_;
    Vector A_C = A_ - C_;

    if (A_B.CheckZero() && A_C.CheckZero())
        return POINT;

    if (n_.CheckZero())
        return SECTION;

    return NORMAL;
}

void Triangle :: SortVertexes()
{
    // Correct only if status == SECTION, after function AC --- largest section
    if (status != SECTION)
        return;

    double AB = (A_ - B_).len_;
    double BC = (B_ - C_).len_;
    double AC = (A_ - C_).len_;

    if (AC >= AB && AC >= BC)
        return;

    if (AB >= AC && AB >= BC)
    {
        std::swap (B_, C_);
        return;
    }
    
    // BC >= AC && BC >= AB
    std::swap (A_, B_);
    return;
}

bool Triangle :: CheckGeneralIntersect (const Triangle& tr) const
{
    if (status > tr.status)
        return tr.CheckGeneralIntersect (*this);

    switch (status)
    {
        case NORMAL:
            return CheckIntersect (tr);

        case SECTION:
            return Section {A_, C_}.CheckIntersect ({tr.A_, tr.C_});

        case POINT:
            return A_ == tr.A_;

        case INCORRECT:
        default:
            return false;
    }

    return false;
}

bool Triangle :: CheckIntersect (const Triangle& tr) const
{
    if (tr.status == SECTION)
        return CheckIntersect ({tr.A_, tr.B_});

    if (tr.status == POINT)
        return CheckIntersect (tr.A_);

    return CheckIntersect ({tr.A_, tr.B_}) ||
           CheckIntersect ({tr.B_, tr.C_}) ||
           CheckIntersect ({tr.A_, tr.C_}) ||
        tr.CheckIntersect ({   A_,    B_}) ||
        tr.CheckIntersect ({   B_,    C_}) ||
        tr.CheckIntersect ({   A_,    C_});
}

bool Triangle :: CheckIntersect (const Section& sec) const
{
    // in 3D: solving x = A + t (A - B) - in triangle flat, 0 <= t <= 1, after check x and flat
    // in 2D or parallel: check sections

    Vector trA = sec.A_ - A_,
           trB = sec.B_ - A_;

    double n_m_AB = n_ ^ (trB - trA);
    if (IsZero (n_m_AB)) // 2D
    {
        if (sec.CheckIntersect ((Section) {A_, B_}))
            return true;

        if (sec.CheckIntersect ((Section) {A_, C_}))
            return true;

        return sec.CheckIntersect ((Section) {B_, C_});
    }

    double t = -(n_ ^ trA) / n_m_AB;
    // sec.A_.Print();
    // sec.B_.Print();
    // std::cout << " t = " << t << std::endl;
    if (!In0_1 (t))
        return false;
    
    return CheckIntersect (sec.A_ + t * (sec.B_ - sec.A_));
}

bool Triangle :: CheckIntersect (const Vector& vec) const
{
    // sum of angles = 2 pi
    Vector A = A_ - vec,
           B = B_ - vec,
           C = C_ - vec; 

    if (A.CheckZero() || B.CheckZero() || C.CheckZero())
        return true;

    double AOB = (A ^ B) / A.len_ / B.len_,
           AOC = (A ^ C) / A.len_ / C.len_,
           BOC = (B ^ C) / B.len_ / C.len_;

    return IsZero (acos (AOB) + acos (AOC) + acos (BOC) - 2 * M_PI);
}

std::ostream& operator<< (std::ostream& out, const Triangle& tr)
{
    out << "{" << tr.A_ << ", " << tr.B_ << ", " << tr.C_ << "}" << std::endl;
    return out;
}

std::istream& operator>> (std::istream& in, Triangle& tr)
{
    in >> tr.A_ >> tr.B_ >> tr.C_;
    tr.UpdateDate();
    return in;
}

Area :: Area (const Vector& max, const Vector& min, int need_deep):
    max_point_ (max),
    min_point_ (min)
{
    if (need_deep)
    {
        for (int i_area = 0; i_area < 8; i_area++)
        {
            Vector mid = (max + min) * 0.5;
            Vector max_now ((i_area & X) ? max.x_ : mid.x_, 
                            (i_area & Y) ? max.y_ : mid.y_,
                            (i_area & Z) ? max.z_ : mid.z_),
                              
                   min_now ((i_area & X) ? mid.x_ : min.x_,
                            (i_area & Y) ? mid.y_ : min.y_,
                            (i_area & Z) ? mid.z_ : min.z_);

            areas_.push_back ((Area) {max_now, min_now, need_deep - 1});
        }
    }
}

void Area :: AddTriangle (TrIt tr_it)
{
    int A_ind = GetVectorIndex (tr_it->second.GetA());
    int B_ind = GetVectorIndex (tr_it->second.GetB());
    int C_ind = GetVectorIndex (tr_it->second.GetC());

    if (A_ind == B_ind && A_ind == C_ind && areas_.size() > 0)
    {
        areas_[A_ind].AddTriangle (tr_it);
        return;
    }

    trs_.push_back (tr_it);
    return;
}

void Area :: IntersectTriangles (std::set <size_t>& intersect_set)
{
    for (auto i_tr = trs_.begin(); i_tr != trs_.end(); i_tr++)
    {
        for (auto j_tr = i_tr + 1; j_tr != trs_.end(); j_tr++)
        {
            if ((*i_tr)->second.CheckGeneralIntersect ((*j_tr)->second))
            {
                intersect_set.insert ((*i_tr)->first);
                intersect_set.insert ((*j_tr)->first);
            }
        }

        for (auto i_area = areas_.begin(); i_area != areas_.end(); i_area++)
            i_area->IntersectOneTriangle (*i_tr, intersect_set);
    }

    for (auto i_area = areas_.begin(); i_area != areas_.end(); i_area++)
        i_area->IntersectTriangles (intersect_set);
        
    return;
}

void Area :: IntersectOneTriangle (TrIt tr_it, std::set <size_t>& intersect_set)
{
    for (auto i_tr = trs_.begin(); i_tr != trs_.end(); i_tr++)
    {
        if (tr_it->second.CheckGeneralIntersect ((*i_tr)->second))
        {
            intersect_set.insert ( tr_it->first);
            intersect_set.insert ((*i_tr)->first);
        }
    }

    for (auto i_area = areas_.begin(); i_area != areas_.end(); i_area++)
        i_area->IntersectOneTriangle (tr_it, intersect_set);
    return;
}

int Area :: GetVectorIndex (const Vector& vec) const
{
    int ret = 0;
    Vector mid = (max_point_ + min_point_) * 0.5;

    ret |= (vec.x_ >= mid.x_) * X;
    ret |= (vec.y_ >= mid.y_) * Y;
    ret |= (vec.z_ >= mid.z_) * Z;
    return ret;
}

void Area :: PrintLog (size_t deep) const
{
    std::cout << "Area (" << deep << "):\n";
    for (auto i_tr: trs_)
        std::cout << i_tr->second;

    std::cout << std::endl;
    for (auto i_area: areas_)
        i_area.PrintLog (deep + 1);
}

bool IsZero (double num)
{
    return std::abs (num) < EPS;
}

bool In0_1 (double num) // useful for parametric equations
{
    return -EPS <= num && num <= 1 + EPS; 
}

double MaxOrNonNAN (double val1, double val2)
{
    if (!std::isnan (val1) && !std::isnan (val2))
        return std::max (val1, val2);

    return std::isnan (val1) ? val2 : val1;
} 

double MinOrNonNAN (double val1, double val2)
{
    if (!std::isnan (val1) && !std::isnan (val2))
        return std::min (val1, val2);

    return std::isnan (val1) ? val2 : val1;
} 

} // end of namespace geo
