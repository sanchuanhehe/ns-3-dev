/*
 * Copyright (c) 2024 Jiwoong Lee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef UNITS_ANGLE_H
#define UNITS_ANGLE_H

// Angles are dimensionless but still mentioned in the SI as an accepted unit.
// This intricacy belongs to SI not to implementation here.
//
// M_PI is 3.141.. in numeric value and is defined either in <math.h> or <cmath>.
// Note it is of radian not of a degree.

#include "format-string.h"

#include <algorithm>
#include <assert.h>
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <vector>

namespace ns3
{

struct radian;

struct degree
{
    double val{};

    static degree from_radian(const radian& input); // NOLINT(readability-identifier-naming)
    radian to_radian() const;                       // NOLINT(readability-identifier-naming)
    double in_radian() const;                       // NOLINT(readability-identifier-naming)
    double in_degree() const;                       // NOLINT(readability-identifier-naming)

    std::string str() const // NOLINT(readability-identifier-naming)
    {
        return sformat("%.1f degree", val);
    }

    static std::vector<degree> from_doubles(
        std::vector<double>& input) // NOLINT(readability-identifier-naming)
    {
        std::vector<degree> output(input.size());
        std::transform(input.cbegin(), input.cend(), output.begin(), [](double f) {
            return degree{f};
        });
        return output;
    }

    static std::vector<double> to_doubles(
        std::vector<degree>& input) // NOLINT(readability-identifier-naming)
    {
        std::vector<double> output(input.size());
        std::transform(input.cbegin(), input.cend(), output.begin(), [](degree f) {
            return static_cast<double>(f.val);
        });
        return output;
    }

    inline bool operator==(const degree& rhs) const
    {
        return val == rhs.val;
    }

    inline bool operator!=(const degree& rhs) const
    {
        return !(operator==(rhs));
    }

    inline bool operator<(const degree& rhs) const
    {
        return val < rhs.val;
    }

    inline bool operator>(const degree& rhs) const
    {
        return val > rhs.val;
    }

    inline bool operator<=(const degree& rhs) const
    {
        return val <= rhs.val;
    }

    inline bool operator>=(const degree& rhs) const
    {
        return val >= rhs.val;
    }

    inline degree operator-() const
    {
        return degree{-val};
    }

    inline degree operator+(const degree& rhs) const
    {
        return degree{val + rhs.val};
    }

    inline degree operator-(const degree& rhs) const
    {
        return degree{val - rhs.val};
    }

    inline degree& operator+=(const degree& rhs)
    {
        val += rhs.val;
        return *this;
    }

    inline degree& operator-=(const degree& rhs)
    {
        val -= rhs.val;
        return *this;
    }

    // Returns [0.0, 360.0)
    inline degree& normalize() // NOLINT(readability-identifier-naming)
    {
        val = std::remainder(val + 180.0, 360.0);      // NOLINT
        val = (val < 0.0) ? val + 180.0 : val - 180.0; // NOLINT
        return *this;
    }
};

struct radian
{
    double val;
    static radian from_degree(const degree& input); // NOLINT(readability-identifier-naming)
    degree to_degree() const;                       // NOLINT(readability-identifier-naming)
    double in_degree() const;                       // NOLINT(readability-identifier-naming)
    double in_radian() const;                       // NOLINT(readability-identifier-naming)

    std::string str() const // NOLINT(readability-identifier-naming)
    {
        return sformat("%.1f radian", val);
    }

    static std::vector<radian> from_doubles(
        std::vector<double>& input) // NOLINT(readability-identifier-naming)
    {
        std::vector<radian> output(input.size());
        std::transform(input.cbegin(), input.cend(), output.begin(), [](double f) {
            return radian{f};
        });
        return output;
    }

    static std::vector<double> to_doubles(
        std::vector<radian>& input) // NOLINT(readability-identifier-naming)
    {
        std::vector<double> output(input.size());
        std::transform(input.cbegin(), input.cend(), output.begin(), [](radian f) {
            return static_cast<double>(f.val);
        });
        return output;
    }

    inline bool operator==(const radian& rhs) const
    {
        return val == rhs.val;
    }

    inline bool operator!=(const radian& rhs) const
    {
        return !(operator==(rhs));
    }

    inline bool operator<(const radian& rhs) const
    {
        return val < rhs.val;
    }

    inline bool operator>(const radian& rhs) const
    {
        return val > rhs.val;
    }

    inline bool operator<=(const radian& rhs) const
    {
        return val <= rhs.val;
    }

    inline bool operator>=(const radian& rhs) const
    {
        return val >= rhs.val;
    }

    inline radian operator-() const
    {
        return radian{-val};
    }

    inline radian operator+(const radian& rhs) const
    {
        return radian{val + rhs.val};
    }

    inline radian operator-(const radian& rhs) const
    {
        return radian{val - rhs.val};
    }

    inline radian& operator+=(const radian& rhs)
    {
        val += rhs.val;
        return *this;
    }

    inline radian& operator-=(const radian& rhs)
    {
        val -= rhs.val;
        return *this;
    }

    /// Returns between [-M_PI, +M_PI)
    inline radian& normalize() // NOLINT(readability-identifier-naming)
    {
        val = std::remainder(val + M_PI, 2 * M_PI);
        val = (val < 0.0) ? val + M_PI : val - M_PI;
        return *this;
    }
};

degree operator*(const degree& lhs, double rhs);
degree operator*(double lhs, const degree& rhs);
degree operator/(const degree& lhs, double rhs);
radian operator*(const radian& lhs, double rhs);
radian operator*(double lhs, const radian& rhs);
radian operator/(const radian& lhs, double rhs);

degree operator"" _degree(long double val);
degree operator"" _degree(unsigned long long val);
radian operator"" _radian(long double val);
radian operator"" _radian(unsigned long long val);

std::ostream& operator<<(std::ostream& os, const degree& rhs);
std::ostream& operator<<(std::ostream& os, const radian& rhs);
std::istream& operator>>(std::istream& is, degree& rhs);
std::istream& operator>>(std::istream& is, radian& rhs);

const auto ZERO_RADIAN = 0_radian;
const auto PI_RADIANS = radian{M_PI};

} // namespace ns3

#endif // UNITS_ANGLE_H
