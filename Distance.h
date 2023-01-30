#ifndef DISTANCE_H
#define DISTANCE_H

// Eugene
#include <QString>
#define _PI_   3.14159265358979323846264338327950288
//

class Distance
{
    public:
        enum Type { MM, MU, NM, INCH, MINCH };

        friend Distance operator"" _mm(long double val);
        friend Distance operator"" _mu(long double val);// micro
        friend Distance operator"" _nm(long double val);// nano
        friend Distance operator"" _inch(long double val);
        friend Distance operator"" _minch(long double val);
        friend Distance operator"" _mm(unsigned long long val);
        friend Distance operator"" _mu(unsigned long long val);
        friend Distance operator"" _nm(unsigned long long val);
        friend Distance operator"" _inch(unsigned long long val);
        friend Distance operator"" _minch(unsigned long long val);

        Distance operator+(Distance other) { return Distance(nm_ + other.nm_); }
        Distance operator-(Distance other) { return Distance(nm_ - other.nm_); }
        Distance operator-() { return Distance(-nm_); }
        Distance operator=(Distance other) { return Distance(other.nm_); }
        Distance operator=(long double d) { return Distance(d); }
        Distance operator+=(long double d) { return Distance(nm_ + d); }
        Distance operator-=(long double d) { return Distance(nm_ - d); }

        operator long double() const { return nm_; }

        Distance()
        {
            nm_ = 0;
        }

        Distance(long double value, Type type)
        {
            switch (type) {
                case MM: nm_ = value * 1000000.0; return;
                case MU: nm_ = value * 1000.0; return;
                case NM: nm_ = value; return;
                case INCH: nm_ = value * 25.4 * 1000000.0; return;
                case MINCH: nm_ = value * 25.4 * 1000.0; return;
            }
            /* Should never get here */
        }
        
        double operator()(Type type)
        {
            switch (type) {
                case MM: return nm_ / 1000000.0;
                case MU: return nm_ / 1000.0;
                case NM: return nm_;
                case INCH: return nm_ / (25.4 * 1000000.0);
                case MINCH: return nm_ / (25.4 * 1000.0);
            }
            /* Should never get here */
            return -1;
        }

        // Eugene
        //
        void move_motor(long double degree_value, const QString &arg)
        {
            degree_to_nm(degree_value);

            demention_choice(arg);
        }

        void degree_to_nm(long double degree_value)
        {
            long double rotor_radius = 16.0; // radius value in mm
            nm_ = (_PI_ * rotor_radius * 1e-6 * degree_value) / 180.0;
        }

private:
        void demention_choice(const QString &arg)
        {
            if(arg == "mm") {
                nm_ *= 1000000.0;
                return;
            } else if(arg == "mu") {
                nm_ *= 1000.0;
                return;
            }  else if(arg == "nm") {
                return;
            }  else if(arg == "inch") {
                nm_ *= 25.4 * 1000000.0;
                return;
            }  else if(arg == "minch") {
                nm_ *= 25.4 * 1000.0;
                return;
            }
        }
        //
        //Eugene

        explicit Distance(long double nm) : nm_(nm) { }

        long double nm_;
};

Distance operator"" _mm(long double val);
Distance operator"" _mu(long double val);
Distance operator"" _nm(long double val);
Distance operator"" _inch(long double val);
Distance operator"" _minch(long double val);

Distance operator"" _mm(unsigned long long val);
Distance operator"" _mu(unsigned long long val);
Distance operator"" _nm(unsigned long long val);
Distance operator"" _inch(unsigned long long val);
Distance operator"" _minch(unsigned long long val);

#if 0

def convertUnitsToEncoder(self, value, units):
    elif units == Units.mrad:
    return round(value * 10 ** 3 * 1 / self.stage.encoderResolution)
    elif units == Units.rad:
    return round(value * 10 ** 6 * 1 / self.stage.encoderResolution)
    elif units == Units.deg:
    return round(value * (2 * math.pi) / 360 * 10 ** 6 / self.stage.encoderResolution)
    else:
        self.xeryon_object.stop()
        raise ("Unexpected unit")

        def convertEncoderUnitsToUnits(self, value, units):
            elif units == Units.mrad:
            return value / (10 ** 3 * 1 / self.stage.encoderResolution)
            elif units == Units.rad:
            return value / (10 ** 6 * 1 / self.stage.encoderResolution)
            elif units == Units.deg:
            return value / ((2 * math.pi) / 360 * 10 ** 6 / self.stage.encoderResolution)
            else:
                self.xeryon_object.stop()
                raise ("Unexpected unit")
#endif

#endif

