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
#ifndef UNITS_FREQUENCY_H
#define UNITS_FREQUENCY_H

#include "format-string.h"
#include "units-aliases.h"

#include <ns3/nstime.h>

#include <algorithm>
#include <assert.h>
#include <cinttypes>
#include <cmath>
#include <math.h> // To support Linux. <cmath> lacks log10l().
#include <ostream>

namespace ns3
{

struct Hz
{
    // TODO(porce): Investigate to change `double` to `int64_t` if there is no need of sub-Hertz.
    // If there is a need of sub-Hertz, such as mHz, define struct mHz and build Hz on top of it.
    double val{};

    /// \brief Stringfy with metric prefix
    /// Sub-Hertz not supported
    std::string str() const // NOLINT(readability-identifier-naming)
    {
        const std::vector<std::string> WHOLE_UNIT_PREFIX = {"", "k", "M", "G", "T"};

        auto idx = 0;
        auto valInt = static_cast<int64_t>(val); // No support of sub-Hertz
        while (((idx + 1) < WHOLE_UNIT_PREFIX.size()) && ((valInt % ONE_KILO) == 0))
        {
            valInt /= ONE_KILO;
            ++idx;
        }

        return sformat("%lld %sHz", valInt, WHOLE_UNIT_PREFIX[idx].c_str());
    }

    static std::vector<Hz> from_doubles(
        std::vector<double>& input) // NOLINT(readability-identifier-naming)
    {
        std::vector<Hz> output(input.size());
        std::transform(input.cbegin(), input.cend(), output.begin(), [](double f) {
            return Hz{f};
        });
        return output;
    }

    static std::vector<double> to_doubles(
        std::vector<Hz>& input) // NOLINT(readability-identifier-naming)
    {
        std::vector<double> output(input.size());
        std::transform(input.cbegin(), input.cend(), output.begin(), [](Hz f) { return f.val; });
        return output;
    }

    inline bool operator==(const Hz& rhs) const
    {
        return val == rhs.val;
    }

    inline bool operator!=(const Hz& rhs) const
    {
        return !(operator==(rhs));
    }

    inline bool operator<(const Hz& rhs) const
    {
        return val < rhs.val;
    }

    inline bool operator>(const Hz& rhs) const
    {
        return val > rhs.val;
    }

    inline bool operator<=(const Hz& rhs) const
    {
        return val <= rhs.val;
    }

    inline bool operator>=(const Hz& rhs) const
    {
        return val >= rhs.val;
    }

    inline Hz operator-() const // Negation
    {
        return Hz{-val};
    }

    inline Hz operator+(const Hz& rhs) const
    {
        return Hz{val + rhs.val};
    }

    inline Hz operator-(const Hz& rhs) const
    {
        return Hz{val - rhs.val};
    }

    inline Hz& operator+=(const Hz& rhs)
    {
        val += rhs.val;
        return *this;
    }

    inline Hz& operator-=(const Hz& rhs)
    {
        val -= rhs.val;
        return *this;
    }

    inline Hz operator/(double rhs) const
    {
        return Hz{val / rhs};
    }

    inline double operator/(const Hz& rhs) const
    {
        return val / rhs.val;
    }

    inline Hz operator*(double rhs) const
    {
        return Hz{val * rhs};
    }

    inline double operator*(Time nstime) const
    {
        // 64-bit double storage type is large enough to support Time's storage type and its
        // metric prefixes. 64-bit double supports upto 1.7e308. Time supports up to 2^63 - 1,
        // regardless of its metric prefix. The supported range of the return value large enough
        // for ns-3 This means upto 1.8e289 Hz is supported for this multiplication operation
        // This range does not qualify anything about the precision and accuracy.
        return (val * nstime.GetNanoSeconds()) / ONE_GIGA;
    }

    inline double in_Hz() const // NOLINT(readability-identifier-naming)
    {
        return val;
    }

    inline double in_kHz() const // NOLINT(readability-identifier-naming)
    {
        return val / ONE_KILO;
    }

    inline double in_MHz() const // NOLINT(readability-identifier-naming)
    {
        return val / ONE_MEGA;
    }

    inline double in_GHz() const // NOLINT(readability-identifier-naming)
    {
        return val / ONE_GIGA;
    }
};

// User defined literals
Hz operator"" _Hz(unsigned long long val);
Hz operator"" _Hz(long double val);
Hz operator"" _kHz(unsigned long long val);
Hz operator"" _kHz(long double val);
Hz operator"" _MHz(unsigned long long val);
Hz operator"" _MHz(long double val);
Hz operator"" _GHz(unsigned long long val);
Hz operator"" _GHz(long double val);
Hz operator"" _THz(unsigned long long val);
Hz operator"" _THz(long double val);

std::ostream& operator<<(std::ostream& os, const Hz& rhs);
std::istream& operator>>(std::istream& is, Hz& rhs);

Hz operator*(double lfs, const Hz& rhs);
double operator*(Time nstime, const Hz& rhs);

inline Hz
kHz(double val)
{
    return Hz{val * ONE_KILO};
}

inline Hz
MHz(double val)
{
    return Hz{val * ONE_MEGA};
}

inline Hz
GHz(double val)
{
    return Hz{val * ONE_GIGA};
}

inline Hz
THz(double val)
{
    return Hz{val * ONE_TERA};
}

} // namespace ns3

#endif // UNITS_FREQUENCY_H
