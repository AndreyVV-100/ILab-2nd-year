#include "geometry.h"
#include <iostream>

namespace geo
{

Vector :: Vector (double x, double y, double z):
    x_ (x),
    y_ (y),
    z_ (z),
    len_ (Len())
{
    
}

bool Vector :: CheckCorrect() const
{
    using std::isfinite;
    return isfinite (x_) && isfinite (y_) && isfinite (z_); 
}

bool Vector :: CheckZero() const
{
    return IsZero (x_) && IsZero (y_) && IsZero (z_);
}

void Vector :: Print() const
{
    std::cout << "{" << x_ << ", " << y_ << ", " << z_ << "}";
    return;
}

double Vector :: Len()
{
    return len_ = std::sqrt (x_*x_ + y_*y_ + z_*z_);
}

Vector Vector :: operator+ (const Vector& a) const
{
    return {x_ + a.x_, y_ + a.y_, z_ + a.z_};
}

Vector Vector :: operator- (const Vector& a) const
{
    return {x_ - a.x_, y_ - a.y_, z_ - a.z_};
}

Vector Vector :: operator* (const Vector& a) const // vector multiplication
{
    return {y_ * a.z_ - z_ * a.y_, 
            z_ * a.x_ - x_ * a.z_, 
            x_ * a.y_ - y_ * a.x_};
}

double Vector :: operator^ (const Vector& a) const // scalar multiplication
{
    return x_ * a.x_ + y_ * a.y_ + z_ * a.z_;
}

Vector Vector :: operator* (double num) const
{
    return {num * x_, num * y_, num * z_};
}

Vector operator* (double num, const Vector& a)
{
    return a * num;
}

bool CheckCollinear (const Vector& a, const Vector& b)
{
    return (a - b).CheckZero();
}

Section  :: Section (const Vector& A, const Vector& B):
    A_ (A),
    B_ (B)
{

}

Triangle :: Triangle (const Vector& A, const Vector& B, const Vector& C):
    A_ (A),
    B_ (B),
    C_ (C),
    n_ ((A - B) * (A - C)),
    status (GetStatus()) // ToDo: Is it correct?
{
    if (status == SECTION)
        SortVertexes();
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

    // ToDo: Use standart functions?

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

bool Triangle :: CheckCorrect() const
{
    return A_.CheckCorrect() && B_.CheckCorrect() && C_.CheckCorrect();
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
            return (Section) {A_, C_}.CheckIntersect ((Section) {tr.A_, tr.C_});

        case POINT:
            return (A_ - tr.A_).CheckZero();

        case INCORRECT:
        default:
            return false;
    }

    return false;
}

bool Triangle :: CheckIntersect (const Triangle& tr) const
{
    if (tr.status == SECTION)
        return CheckIntersect ((Section) {tr.A_, tr.B_});

    if (tr.status == POINT)
        return CheckIntersect (tr.A_);

    return CheckIntersect ((Section) {tr.A_, tr.B_}) ||
           CheckIntersect ((Section) {tr.B_, tr.C_}) ||
           CheckIntersect ((Section) {tr.A_, tr.C_}) ||
        tr.CheckIntersect ((Section) {   A_,    B_}) ||
        tr.CheckIntersect ((Section) {   B_,    C_}) ||
        tr.CheckIntersect ((Section) {   A_,    C_});
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
    // sum of angels = 2 pi
    Vector A = A_ - vec,
           B = B_ - vec,
           C = C_ - vec; 

    double AOB = (A ^ B) / A.len_ / B.len_,
           AOC = (A ^ C) / A.len_ / C.len_,
           BOC = (B ^ C) / B.len_ / C.len_;

    return IsZero (acos (AOB) + acos (AOC) + acos (BOC) - 2 * M_PI);
}

void Triangle :: Print() const
{
    std::cout << "{";
    A_.Print();
    std::cout << ", ";
    B_.Print();
    std::cout << ", ";
    C_.Print();
    std::cout << "}\n";
    return;
}

bool Section :: CheckIntersect (const Section& sec) const
{
    Vector a1 = B_ - A_,
           a2 = sec.B_ - sec.A_; // direct vectors
    Vector n = a1 * a2;

    // Here was magic. If you would understand it, try to solve alpha * a1 + B*a2 = r2 - r1
    if (n.CheckZero())
        return CheckIntersect (sec.A_) || CheckIntersect (sec.B_);

    Vector mnpn = sec.A_ - A_; // mne_nadoelo_pridumivat_names

    // ToDo: delete copypaste

    if (!IsZero (n.z_))
    {
        double alpha = (mnpn.x_ * a2.y_ - mnpn.y_ * a2.x_) / n.z_;
        double beta  = (mnpn.y_ * a1.x_ - mnpn.x_ * a1.y_) / n.z_;

        return alpha * a1.z_ + beta * a2.z_ == mnpn.z_ && In_m1_1 (alpha) && In_m1_1 (beta);
    }

    if (!IsZero (n.x_))
    {
        double alpha = (mnpn.y_ * a2.z_ - mnpn.z_ * a2.y_) / n.x_;
        double beta  = (mnpn.z_ * a1.y_ - mnpn.y_ * a1.z_) / n.x_;

        return alpha * a1.x_ + beta * a2.x_ == mnpn.x_ && In_m1_1 (alpha) && In_m1_1 (beta);
    }

    double alpha = (mnpn.x_ * a2.z_ - mnpn.z_ * a2.x_) / n.y_;
    double beta  = (mnpn.z_ * a1.x_ - mnpn.x_ * a1.z_) / n.y_;

    return alpha * a1.y_ + beta * a2.y_ == mnpn.y_ && In_m1_1 (alpha) && In_m1_1 (beta);
}

bool Section :: CheckIntersect (const Vector& vec) const
{
    Vector rA = vec - A_,
           rB = vec - B_; // from point to section's ends

    if (rA.CheckZero() || rB.CheckZero())
        return true;

    if (!(rB * rA).CheckZero()) // In one line?
        return false;

    // rA == -t rB ?
    if (!IsZero (rA.x_))
        return rA.x_ * rB.x_ < 0;
        
    if (!IsZero (rA.y_))
        return rA.y_ * rB.y_ < 0;
        
    return rA.z_ * rB.z_ < 0;
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
    int A_ind = GetVectorIndex (tr_it->second.A_);
    int B_ind = GetVectorIndex (tr_it->second.B_);
    int C_ind = GetVectorIndex (tr_it->second.C_);

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
        i_tr->second.Print();

    std::cout << std::endl;
    for (auto i_area: areas_)
        i_area.PrintLog (deep + 1);
}

bool IsZero (double num) // ToDo: overloading
{
    return std::abs (num) < EPS;
}

bool In0_1 (double num) // useful for parametric equations
{
    return -EPS <= num && num <= 1 + EPS; 
}

bool In_m1_1 (double num)
{
    return -1 - EPS <= num && num <= 1 + EPS; 
}

} // end of namespace geo
