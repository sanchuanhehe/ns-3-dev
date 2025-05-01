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
#include "units-angle.h"

namespace ns3
{

degree
degree::from_radian(const radian& input)
{
    return degree{input.val * 180.0 / M_PI};
}

radian
degree::to_radian() const
{
    return radian{val / 180.0 * M_PI};
}

double
degree::in_radian() const
{
    return to_radian().val;
}

double
degree::in_degree() const
{
    return val;
}

degree
operator*(const degree& lhs, double rhs)
{
    return degree{lhs.val * rhs};
}

degree
operator*(double lhs, const degree& rhs)
{
    return rhs * lhs;
}

degree
operator/(const degree& lhs, double rhs)
{
    return degree{lhs.val / rhs};
}

radian
radian::from_degree(const degree& input)
{
    return radian{input.val / 180.0 * M_PI};
}

degree
radian::to_degree() const
{
    return degree{val * 180.0 / M_PI};
}

double
radian::in_degree() const
{
    return to_degree().val;
}

double
radian::in_radian() const
{
    return val;
}

radian
operator*(const radian& lhs, double rhs)
{
    return radian{lhs.val * rhs};
}

radian
operator*(double lhs, const radian& rhs)
{
    return rhs * lhs;
}

radian
operator/(const radian& lhs, double rhs)
{
    return radian{lhs.val / rhs};
}

// User defined literals
degree operator"" _degree(long double val)
{
    return degree{static_cast<double>(val)};
}

degree operator"" _degree(unsigned long long val)
{
    return degree{static_cast<double>(val)};
}

radian operator"" _radian(long double val)
{
    return radian{static_cast<double>(val)};
}

radian operator"" _radian(unsigned long long val)
{
    return radian{static_cast<double>(val)};
}

// Output-input operator overloading
std::ostream&
operator<<(std::ostream& os, const degree& rhs)
{
    return os << rhs.str();
}

std::ostream&
operator<<(std::ostream& os, const radian& rhs)
{
    return os << rhs.str();
}

std::istream&
operator>>(std::istream& is, degree& rhs)
{
    is >> rhs.val;
    return is;
}

std::istream&
operator>>(std::istream& is, radian& rhs)
{
    is >> rhs.val;
    return is;
}

} // namespace ns3
