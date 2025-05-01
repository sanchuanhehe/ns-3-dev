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
#include "units-energy.h"

#include "units-aliases.h"

namespace ns3
{

dBm
dB::operator+(const dBm& rhs) const
{
    return dBm{val + rhs.val};
}

dBm
dB::operator-(const dBm& rhs) const
{
    return dBm{val - rhs.val};
}

bool
mWatt::operator==(const Watt& rhs) const
{
    return val == (rhs.val * ONE_KILO);
}

bool
mWatt::operator!=(const Watt& rhs) const
{
    return !(operator==(rhs));
}

bool
mWatt::operator<(const Watt& rhs) const
{
    return val < (rhs.val * ONE_KILO);
}

bool
mWatt::operator>(const Watt& rhs) const
{
    return val > (rhs.val * ONE_KILO);
}

bool
mWatt::operator<=(const Watt& rhs) const
{
    return val <= (rhs.val * ONE_KILO);
}

bool
mWatt::operator>=(const Watt& rhs) const
{
    return val >= (rhs.val * ONE_KILO);
}

mWatt
mWatt::operator+(const Watt& rhs) const
{
    return mWatt{val + (rhs.val * ONE_KILO)};
}

mWatt
mWatt::operator-(const Watt& rhs) const
{
    return mWatt{val - (rhs.val * ONE_KILO)};
}

mWatt&
mWatt::operator+=(const Watt& rhs)
{
    val += (rhs.val * ONE_KILO);
    return *this;
}

mWatt&
mWatt::operator-=(const Watt& rhs)
{
    val -= (rhs.val * ONE_KILO);
    return *this;
}

mWatt
operator*(const double& lfs, const mWatt& rhs)
{
    return mWatt{lfs * rhs.val};
}

bool
Watt::operator==(const mWatt& rhs) const
{
    return val == (rhs.val / ONE_KILO);
}

bool
Watt::operator!=(const mWatt& rhs) const
{
    return !(operator==(rhs));
}

bool
Watt::operator<(const mWatt& rhs) const
{
    return val < (rhs.val / ONE_KILO);
}

bool
Watt::operator>(const mWatt& rhs) const
{
    return val > (rhs.val / ONE_KILO);
}

bool
Watt::operator<=(const mWatt& rhs) const
{
    return val <= (rhs.val / ONE_KILO);
}

bool
Watt::operator>=(const mWatt& rhs) const
{
    return val >= (rhs.val / ONE_KILO);
}

mWatt
Watt::operator+(const mWatt& rhs) const
{
    return mWatt{(val * ONE_KILO) + rhs.val};
}

mWatt
Watt::operator-(const mWatt& rhs) const
{
    return mWatt{(val * ONE_KILO) - rhs.val};
}

Watt&
Watt::operator+=(const mWatt& rhs)
{
    val += (rhs.val / ONE_KILO);
    return *this;
}

Watt&
Watt::operator-=(const mWatt& rhs)
{
    val -= (rhs.val / ONE_KILO);
    return *this;
}

dBm
dBm::from_mWatt(const mWatt& input)
{
    return dBm{ToLogScale(input.val)};
}

mWatt
dBm::to_mWatt() const
{
    return mWatt{ToLinearScale(val)};
}

double
dBm::in_mWatt() const
{
    return to_mWatt().val;
}

dBm
dBm::from_Watt(const Watt& input)
{
    return dBm{ToLogScale(input.val) + 30.0}; // 30 == 10log10(1000)
}

Watt
dBm::to_Watt() const
{
    return Watt{ToLinearScale(val - 30.0)}; // -30 == 10log10(1/1000)
}

double
dBm::in_Watt() const
{
    return to_Watt().val;
}

double
dBm::in_dBm() const
{
    return val;
}

mWatt
mWatt::from_dBm(const dBm& from)
{
    return mWatt{ToLinearScale(from.val)};
}

dBm
mWatt::to_dBm() const
{
    return dBm{ToLogScale(val)};
}

double
mWatt::in_dBm() const
{
    return to_dBm().val;
}

mWatt
mWatt::from_Watt(const Watt& from)
{
    return mWatt{from.val * ONE_KILO};
}

Watt
mWatt::to_Watt() const
{
    return Watt{val / ONE_KILO};
}

double
mWatt::in_Watt() const
{
    return to_Watt().val;
}

double
mWatt::in_mWatt() const
{
    return val;
}

Watt
Watt::from_dBm(const dBm& from)
{
    return Watt{ToLinearScale(from.val - 30.0)}; // -30 == 10log10(1/1000)
}

dBm
Watt::to_dBm() const
{
    return dBm{ToLogScale(val) + 30.0}; // 30 == 10log10(1000)
}

double
Watt::in_dBm() const
{
    return to_dBm().val;
}

Watt
Watt::from_mWatt(const mWatt& from)
{
    return Watt{(from.val / ONE_KILO)};
}

mWatt
Watt::to_mWatt() const
{
    return mWatt{val * ONE_KILO};
}

double
Watt::in_mWatt() const
{
    return to_mWatt().val;
}

double
Watt::in_Watt() const
{
    return val;
}

double
dBm_per_Hz::in_dBm() const
{
    return val;
}

// User defined literals
dB operator"" _dB(long double val)
{
    return dB{static_cast<double>(val)};
}

dB operator"" _dB(unsigned long long val)
{
    return dB{static_cast<double>(val)};
}

dBm operator"" _dBm(long double val)
{
    return dBm{static_cast<double>(val)};
}

dBm operator"" _dBm(unsigned long long val)
{
    return dBm{static_cast<double>(val)};
}

mWatt operator"" _mWatt(long double val)
{
    return mWatt{static_cast<double>(val)};
}

mWatt operator"" _mWatt(unsigned long long val)
{
    return mWatt{static_cast<double>(val)};
}

mWatt operator"" _pWatt(long double val)
{
    return mWatt{static_cast<double>(val * 1e-9)}; // NOLINT
}

mWatt operator"" _pWatt(unsigned long long val)
{
    return mWatt{val * 1e-9}; // NOLINT
}

Watt operator"" _Watt(long double val)
{
    return Watt{static_cast<double>(val)}; // NOLINT
}

Watt operator"" _Watt(unsigned long long val)
{
    return Watt{static_cast<double>(val)}; // NOLINT
}

dBm_per_Hz operator"" _dBm_per_Hz(long double val)
{
    return dBm_per_Hz{static_cast<double>(val)};
}

// Output-input operators overloading
std::ostream&
operator<<(std::ostream& os, const dB& rhs)
{
    return os << rhs.str();
}

std::ostream&
operator<<(std::ostream& os, const dBm& rhs)
{
    return os << rhs.str();
}

std::ostream&
operator<<(std::ostream& os, const mWatt& rhs)
{
    return os << rhs.str();
}

std::ostream&
operator<<(std::ostream& os, const Watt& rhs)
{
    return os << rhs.str();
}

std::ostream&
operator<<(std::ostream& os, const dBm_per_Hz& rhs)
{
    return os << rhs.str();
}

std::istream&
operator>>(std::istream& is, dB& rhs)
{
    is >> rhs.val;
    return is;
}

std::istream&
operator>>(std::istream& is, dBm& rhs)
{
    is >> rhs.val;
    return is;
}

std::istream&
operator>>(std::istream& is, mWatt& rhs)
{
    is >> rhs.val;
    return is;
}

std::istream&
operator>>(std::istream& is, Watt& rhs)
{
    is >> rhs.val;
    return is;
}

std::istream&
operator>>(std::istream& is, dBm_per_Hz& rhs)
{
    is >> rhs.val;
    return is;
}

} // namespace ns3
