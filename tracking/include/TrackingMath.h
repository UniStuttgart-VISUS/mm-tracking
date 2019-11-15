/**
 * TrackingMath.h
 *
 * Copyright (C) 2014-2018 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */

#ifndef TRACKING_MATH_INCLUDED
#define TRACKING_MATH_INCLUDED

#include <cmath>
#include <cstdlib>
#include <limits>


namespace tracking
{
    /** Point2D - 2D ***********************************************************/
    class Point2D {
    public:
        Point2D(void) {
            this->x = 0.0f;
            this->y = 0.0f;
        }
        Point2D(float xp, float yp) {
            this->x = xp;
            this->y = yp;
        }

        ~Point2D(void) {
            // nothing to do here
        }

        inline Point2D& operator =(const Point2D& rhs) {
            this->Set(rhs.X(), rhs.Y());
            return *this;
        }

        inline void SetX(float xp) {
            this->x = xp;
        }

        inline void SetY(float yp) {
            this->y = yp;
        }

        inline void Set(float xp, float yp) {
            this->x = xp;
            this->y = yp;
        }

        inline float X(void) const {
            return this->x;
        }

        inline float Y(void) const {
            return this->y;
        }

    private:

        float x;
        float y;
    };


    /** Vector3D - 3D **********************************************************/
    class Vector3D {
    public:
        Vector3D(void) {
            this->x = 0.0f;
            this->y = 0.0f;
            this->z = 0.0f;
        }
        Vector3D(float xp, float yp, float zp) {
            this->x = xp;
            this->y = yp;
            this->z = zp;
        }

        ~Vector3D(void) {
            // nothing to do here
        }

        inline Vector3D& operator =(const Vector3D& rhs) {
            this->Set(rhs.X(), rhs.Y(), rhs.Z());
            return *this;
        }

        inline bool operator ==(const Vector3D& rhs) {
            return ((this->x == rhs.X()) && (this->y == rhs.Y()) && (this->z == rhs.Z()));
        }

        inline bool operator !=(const Vector3D& rhs) {
            return ((this->x != rhs.X()) || (this->y != rhs.Y()) || (this->z != rhs.Z()));
        }

        inline Vector3D operator -(const Vector3D& rhs) const {
            Vector3D retval;
            retval.SetX(this->x - rhs.X());
            retval.SetY(this->y - rhs.Y());
            retval.SetZ(this->z - rhs.Z());
            return retval;
        }

        inline Vector3D operator +(const Vector3D& rhs) const {
            Vector3D retval;
            retval.SetX(this->x + rhs.X());
            retval.SetY(this->y + rhs.Y());
            retval.SetZ(this->z + rhs.Z());
            return retval;
        }

        inline Vector3D operator *(const float rhs) const {
            Vector3D retval;
            retval.SetX(this->x * rhs);
            retval.SetY(this->y * rhs);
            retval.SetZ(this->z * rhs);
            return retval;
        }

        inline Vector3D Cross(const Vector3D & rhs) const {
            Vector3D retval;
            retval.Set(
                this->y * (rhs.Z()) - this->z * (rhs.Y()),
                this->z * (rhs.X()) - this->x * (rhs.Z()),
                this->x * (rhs.Y()) - this->y * (rhs.X()));
            return retval;
        }

        inline float Dot(const Vector3D & rhs) const {
            float retval = 0.0f;
            retval += this->x * (rhs.X());
            retval += this->y * (rhs.Y());
            retval += this->z * (rhs.Z());
            return retval;
        }

        inline Vector3D& operator *=(const float rhs) {
            this->SetX(this->x * (rhs));
            this->SetY(this->y * (rhs));
            this->SetZ(this->z * (rhs));
            return *this;
        }

        float Length(void) const {
            float retval = 0.0f;
            retval += (this->x * this->x);
            retval += (this->y * this->y);
            retval += (this->z * this->z);
            return ::sqrtf(retval);
        }

        float Normalise(void) {
            float length = this->Length();
            if (length != 0.0f) {
                this->SetX(this->x / length);
                this->SetY(this->y / length);
                this->SetZ(this->z / length);
            }
            else {
                this->SetX(0.0f);
                this->SetY(0.0f);
                this->SetZ(0.0f);
            }
            return length;
        }

        inline void SetX(float xp) {
            this->x = xp;
        }

        inline void SetY(float yp) {
            this->y = yp;
        }

        inline void SetZ(float zp) {
            this->z = zp;
        }

        inline void Set(float xp, float yp, float zp) {
            this->x = xp;
            this->y = yp;
            this->z = zp;
        }

        inline float X(void) const {
            return this->x;
        }

        inline float Y(void) const {
            return this->y;
        }

        inline float Z(void) const {
            return this->z;
        }

    private:

        float x;
        float y;
        float z;

    };

    // Scalar multiplication from left.
    extern tracking::Vector3D operator *(const float lhs, const tracking::Vector3D& rhs);


    /** QUATERNION ************************************************************/
    class Quaternion {
    public:
        Quaternion(void) {
            this->x = 0.0f;
            this->y = 0.0f;
            this->z = 0.0f;
            this->w = 1.0f;
        }
        Quaternion(float xp, float yp, float zp, float wp) {
            this->x = xp;
            this->y = yp;
            this->z = zp;
            this->w = wp;
        }

        ~Quaternion(void) {
            // nothing to do here
        }

        inline Quaternion& operator =(const Quaternion& rhs) {
            this->Set(rhs.X(), rhs.Y(), rhs.Z(), rhs.W());
            return *this;
        }

        inline tracking::Vector3D operator *(const tracking::Vector3D& rhs) const {
            Vector3D u;
            u.Set(this->x, this->y, this->z);
            return (((2.0f) * ((u.Dot(rhs) * u)
                + (this->W() * u.Cross(rhs))))
                + (((this->W() *this->W()) - u.Dot(u)) * (rhs)));
        }

        inline Quaternion operator *(const Quaternion & rhs) const {
            return Quaternion(
                this->w * rhs.X()
                + rhs.W() * this->x
                + this->y * rhs.Z()
                - this->z * rhs.Y(),

                this->w * rhs.Y()
                + rhs.W() * this->y
                + this->z * rhs.X()
                - this->x * rhs.Z(),

                this->w * rhs.Z()
                + rhs.W() * this->z
                + this->x * rhs.Y()
                - this->y * rhs.X(),

                this->w * rhs.W()
                - (this->x * rhs.X()
                    + this->y * rhs.Y()
                    + this->z * rhs.Z()));
        }

        inline void Conjugate(void) {
            this->x *= (-1.0f);
            this->y *= (-1.0f);
            this->x *= (-1.0f);
        }

        inline void Invert(void) {
            float norm = this->Norm();
            if (norm != 0.0f) {
                this->x /= (-norm);
                this->y /= (-norm);
                this->z /= (-norm);
                this->w /= (norm);

            }
            else {
                this->x = 0.0f;
                this->y = 0.0f;
                this->z = 0.0f;
                this->w = 1.0f;
            }
        }

        inline Quaternion Inverse(void) const {
            Quaternion retval(this->X(), this->Y(), this->Z(), this->W());
            retval.Invert();
            return retval;
        }


        inline float Norm(void) const {
            return ::sqrtf((this->x * this->x) + (this->y * this->y) + (this->z * this->z) + (this->w * this->w));
        }

        inline float Normalise(void) {
            float norm = this->Norm();
            if (norm != 0.0f) {
                this->x /= (norm);
                this->y /= (norm);
                this->z /= (norm);
                this->w /= (norm);
            }
            else {
                this->x = 0.0f;
                this->y = 0.0f;
                this->z = 0.0f;
                this->w = 1.0f;
            }
            return norm;
        }

        inline void SetX(float xp) {
            this->x = xp;
        }

        inline void SetY(float yp) {
            this->y = yp;
        }

        inline void SetZ(float zp) {
            this->z = zp;
        }

        inline void SetW(float wp) {
            this->w = wp;
        }

        inline void Set(float xp, float yp, float zp, float wp) {
            this->x = xp;
            this->y = yp;
            this->z = zp;
            this->w = wp;
        }

        inline float X(void) const {
            return this->x;
        }

        inline float Y(void) const {
            return this->y;
        }

        inline float Z(void) const {
            return this->z;
        }

        inline float W(void) const {
            return this->w;
        }

    private:

        float x;
        float y;
        float z;
        float w;

    };

}

#endif /** TRACKING_MATH_INCLUDED */
