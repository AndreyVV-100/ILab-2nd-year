#pragma once
#include <cmath>

namespace geo
{
    const double EPS = 1e-5; // precision of calculations

    // point is equivalent to vector
    struct Vector
    {
        double x_ = NAN,
               y_ = NAN,
               z_ = NAN;
        double len_ = NAN; // Warning: len - optimization, you should sure, that vector wasn't changed!

        // Vector () = default;
        Vector (double x, double y, double z);
        bool CheckCorrect() const;
        bool CheckZero()    const;
        void Print()  const;
        
        double Len();
        Vector operator+ (const Vector& a) const;
        Vector operator- (const Vector& a) const;
        Vector operator* (const Vector& a) const; // vector multiplication
        double operator^ (const Vector& a) const; // scalar multiplication
        Vector operator* (double num) const;
    };

    Vector operator* (double num, const Vector& a);

    bool CheckCollinear (const Vector& a, const Vector& b);

    struct Section
    {
        Vector A_, B_;
        Section (const Vector& A, const Vector& B);

        bool CheckIntersect (const Section& sec) const;
        bool CheckIntersect (const Vector& vec)  const;
    };
    

    class Triangle
    {
        Vector A_, B_, C_, n_; // normal vector for check intersections

        public:

        enum TrStat
        {
            INCORRECT = 0,
            NORMAL    = 1,
            SECTION   = 2,
            POINT     = 3
        };
        
        TrStat status = INCORRECT;
        TrStat GetStatus() const;

        // Triangle() = default;
        Triangle (const Vector& A, const Vector& B , const Vector& C);
        void SortVertexes(); // Correct only if status == SECTION, after function AC --- largest section 
        bool CheckCorrect() const;

        bool CheckGeneralIntersect (const Triangle& tr) const;
        void Print() const;

        private:

        // This functions think, that status == TRIANGLE
        bool CheckIntersect (const Triangle& tr) const;
        bool CheckIntersect (const Section& sec) const;
        bool CheckIntersect (const Vector& vec)  const;
    };

    bool IsZero (double num);
    bool In0_1  (double num);
    bool In_m1_1 (double num);
    
} // end of namespace geo
