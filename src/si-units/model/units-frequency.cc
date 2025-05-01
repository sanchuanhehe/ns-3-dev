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
#include "units-frequency.h"

namespace ns3
{

// User defined literals
Hz operator"" _Hz(unsigned long long val)
{
    return Hz{static_cast<double>(val)};
}

Hz operator"" _Hz(long double val)
{
    return Hz{static_cast<double>(val)};
}

Hz operator"" _kHz(unsigned long long val)
{
    return Hz{static_cast<double>(val * ONE_KILO)};
}

Hz operator"" _kHz(long double val)
{
    return Hz{static_cast<double>(val * ONE_KILO)};
}

Hz operator"" _MHz(unsigned long long val)
{
    return Hz{static_cast<double>(val * ONE_MEGA)};
}

Hz operator"" _MHz(long double val)
{
    return Hz{static_cast<double>(val * ONE_MEGA)};
}

Hz operator"" _GHz(unsigned long long val)
{
    return Hz{static_cast<double>(val * ONE_GIGA)};
}

Hz operator"" _GHz(long double val)
{
    return Hz{static_cast<double>(val * ONE_GIGA)};
}

Hz operator"" _THz(unsigned long long val)
{
    return Hz{static_cast<double>(val * ONE_TERA)};
}

Hz operator"" _THz(long double val)
{
    return Hz{static_cast<double>(val * ONE_TERA)};
}

// Output-input operators overloading
std::ostream&
operator<<(std::ostream& os, const Hz& rhs)
{
    return os << rhs.str();
}

std::istream&
operator>>(std::istream& is, Hz& rhs)
{
    is >> rhs.val;
    return is;
}

Hz
operator*(double lfs, const Hz& rhs)
{
    return rhs * lfs;
}

double
operator*(Time nstime, const Hz& rhs)
{
    return rhs * nstime;
}
} // namespace ns3
