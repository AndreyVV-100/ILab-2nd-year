#pragma once
#include <cmath>
#include <vector>
#include <set>
#include <iostream>

namespace geo
{
    const double EPS = 1e-5; // precision of calculations

    void StartIntersect(); // main function

    bool IsZero (double num);
    bool In0_1  (double num);
    bool In_m1_1 (double num);

    double MaxOrNonNAN (double val1, double val2);
    double MinOrNonNAN (double val1, double val2);

    // point is equivalent to vector
    struct Vector
    {
        double x_ = NAN,
               y_ = NAN,
               z_ = NAN;
        double len_ = NAN; // Warning: len - optimization, you should sure, that vector wasn't changed!

        Vector() = default;
        Vector (double x, double y, double z):
            x_ (x),
            y_ (y),
            z_ (z),
            len_ (Len())
        { }

        bool CheckCorrect() const
        {
            using std::isfinite;
            return isfinite (x_) && isfinite (y_) && isfinite (z_); 
        }

        bool CheckZero() const
        {
            return IsZero (x_) && IsZero (y_) && IsZero (z_);
        }
        
        double Len()
        {
            return len_ = std::sqrt (x_*x_ + y_*y_ + z_*z_);
        }

        Vector operator+ (const Vector& a) const
        {
            return {x_ + a.x_, y_ + a.y_, z_ + a.z_};
        }

        Vector operator- (const Vector& a) const
        {
            return {x_ - a.x_, y_ - a.y_, z_ - a.z_};
        }

        Vector operator* (const Vector& a) const // vector multiplication
        {
            return {y_ * a.z_ - z_ * a.y_, 
                    z_ * a.x_ - x_ * a.z_, 
                    x_ * a.y_ - y_ * a.x_};
        }

        double operator^ (const Vector& a) const // scalar multiplication
        {
            return x_ * a.x_ + y_ * a.y_ + z_ * a.z_;
        }

        Vector operator* (double num) const
        {
            return {num * x_, num * y_, num * z_};
        }

        bool operator== (const Vector& a) const
        {
            return (*this - a).CheckZero();
        }

        bool operator!= (const Vector& a) const
        {
            return !(*this - a).CheckZero();
        }

        bool operator|| (const Vector& a) const // Is collinear?
        {
            return (*this * a).CheckZero();
        }

        // ToDo: It's already copypaste?

        void Maximize (const Vector& vec)
        {
            x_ = MaxOrNonNAN (x_, vec.x_);
            y_ = MaxOrNonNAN (y_, vec.y_);
            z_ = MaxOrNonNAN (z_, vec.z_);
        }

        void Minimize (const Vector& vec)
        {
            x_ = MinOrNonNAN (x_, vec.x_);
            y_ = MinOrNonNAN (y_, vec.y_);
            z_ = MinOrNonNAN (z_, vec.z_);
        }
    };

    const Vector EPS_Vec = {EPS, EPS, EPS};

    Vector operator* (double num, const Vector& a);
    std::ostream& operator<< (std::ostream& out, const Vector& vec);
    std::istream& operator>> (std::istream& in, Vector& vec);

    struct Section
    {
        Vector A_, B_;
        Section (const Vector& A, const Vector& B):
            A_ (A),
            B_ (B)
        { }

        bool CheckIntersect (const Section& sec) const;
        bool CheckIntersect (const Vector& vec)  const;
    };

    std::ostream& operator<< (std::ostream& out, const Section& vec);
    std::istream& operator>> (std::istream& in, Section& vec);

    class Triangle
    {
        Vector A_, B_, C_, n_; // normal vector for check intersections

        enum TrStat
        {
            INCORRECT = 0,
            NORMAL    = 1,
            SECTION   = 2,
            POINT     = 3
        };
        
        TrStat status = INCORRECT;
        TrStat GetStatus() const;

        public:

        Triangle() = default;
        Triangle (const Vector& A, const Vector& B , const Vector& C);
        void UpdateDate(); // is equivalent to a part of ctor 
        void SortVertexes(); // Correct only if status == SECTION, after function AC --- largest section 
        
        bool CheckCorrect() const
        {
            return A_.CheckCorrect() && B_.CheckCorrect() && C_.CheckCorrect();
        }

        bool CheckGeneralIntersect (const Triangle& tr) const;

        friend std::ostream& operator<< (std::ostream& out, const Triangle& tr);
        friend std::istream& operator>> (std::istream& in, Triangle& tr);

        // ToDo: copypaste?
        const Vector& GetA() const {return A_;}
        const Vector& GetB() const {return B_;}
        const Vector& GetC() const {return C_;}

        private:

        // This functions think, that status == TRIANGLE
        bool CheckIntersect (const Triangle& tr) const;
        bool CheckIntersect (const Section& sec) const;
        bool CheckIntersect (const Vector& vec)  const;
    };

    // std::ostream& operator<< (std::ostream& out, const Triangle& tr);
    // std::istream& operator>> (std::istream& in, Triangle& tr);

    class Area
    {
        public: using SaveT = typename std::pair <size_t, Triangle>;

        private:

        enum Coords
        {
            X = 1,
            Y = 2,
            Z = 4
        };
        using TrIt = typename std::vector <SaveT> :: iterator;

        Vector max_point_;
        Vector min_point_;
        std::vector <Area> areas_;
        std::vector <TrIt> trs_;
        
        public:

        Area (const Vector& max, const Vector& min, int need_deep);

        void AddTriangle (TrIt tr_it);
        void IntersectTriangles (std::set <size_t>& intersect_set);
        void IntersectOneTriangle (TrIt tr_it, std::set <size_t>& intersect_set);
        int GetVectorIndex (const Vector& vec) const;

        void PrintLog (size_t deep) const;
    };
} // end of namespace geo
