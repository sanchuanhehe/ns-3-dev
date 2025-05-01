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
#ifndef UNITS_ENERGY_H
#define UNITS_ENERGY_H

#include "format-string.h"
#include "units-aliases.h"
#include "units-frequency.h"

#include <algorithm>
#include <assert.h>
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <math.h> // To support Linux. <cmath> lacks log10l().

namespace ns3
{

struct dB;
struct dBm;
struct mWatt;
struct Watt;

/// Convert energy value in linear scale into log scale.
inline double
ToLogScale(double val)
{
    assert(val > 0.);
    return 10.0 * log10l(val); // NOLINT
}

/// Convert energy value in log scale into linear scale.
inline double
ToLinearScale(double val)
{
    return std::pow(10.0, val / 10.0); // NOLINT
}

struct dB
{
    double val{};

    dB() = default;

    dB(double val)
        : val(val) // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
    {
    }

    std::string str() const // NOLINT(readability-identifier-naming)
    {
        return sformat("%.1f dB", val);
    }

    static dB from_linear(double input) // NOLINT(readability-identifier-naming)
    {
        return dB{ToLogScale(input)};
    }

    double to_linear() const // NOLINT(readability-identifier-naming)
    {
        return ToLinearScale(val);
    }

    double in_dB() const // NOLINT(readability-identifier-naming)
    {
        return val;
    }

    double in_linear() const // NOLINT(readability-identifier-naming)
    {
        return to_linear();
    }

    static std::vector<dB> from_doubles(
        std::vector<double>& input) // NOLINT(readability-identifier-naming)
    {
        std::vector<dB> output(input.size());
        std::transform(input.cbegin(), input.cend(), output.begin(), [](double f) {
            return dB{f};
        });
        return output;
    }

    static std::vector<double> to_doubles(
        std::vector<dB>& input) // NOLINT(readability-identifier-naming)
    {
        std::vector<double> output(input.size());
        std::transform(input.cbegin(), input.cend(), output.begin(), [](dB f) {
            return static_cast<double>(f.val);
        });
        return output;
    }

    inline bool operator==(const dB& rhs) const
    {
        return val == rhs.val;
    }

    inline bool operator!=(const dB& rhs) const
    {
        return !(operator==(rhs));
    }

    inline bool operator<(const dB& rhs) const
    {
        return val < rhs.val;
    }

    inline bool operator>(const dB& rhs) const
    {
        return val > rhs.val;
    }

    inline bool operator<=(const dB& rhs) const
    {
        return val <= rhs.val;
    }

    inline bool operator>=(const dB& rhs) const
    {
        return val >= rhs.val;
    }

    inline dB operator-() const // Negation
    {
        return dB{-val};
    }

    inline dB operator+(const dB& rhs) const
    {
        return dB{val + rhs.val};
    }

    inline dB operator-(const dB& rhs) const
    {
        return dB{val - rhs.val};
    }

    inline dB& operator+=(const dB& rhs)
    {
        val += rhs.val;
        return *this;
    }

    inline dB& operator-=(const dB& rhs)
    {
        val -= rhs.val;
        return *this;
    }

    dBm operator+(const dBm& rhs) const;
    dBm operator-(const dBm& rhs) const;
};

struct dBm
{
    double val{};

    dBm() = default;

    dBm(double val)
        : val(val) // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
    {
    }

    static dBm from_mWatt(const mWatt& input); // NOLINT(readability-identifier-naming)
    mWatt to_mWatt() const;                    // NOLINT(readability-identifier-naming)
    double in_mWatt() const; // NOLINT(readability-identifier-naming). Return quantity only

    /// Sets power unit in dBm from Watt
    /// \param input input power struct in Watt
    /// \returns power unit struct in dBm
    static dBm from_Watt(const Watt& input); // NOLINT(readability-identifier-naming)

    /// Converts a dBm power unit struct to Watt
    /// \returns power unit struct in Watt
    Watt to_Watt() const; // NOLINT(readability-identifier-naming)

    /// Returns a dBm power unit struct value in Watt
    /// \returns a power unit value in Watt
    double in_Watt() const; // NOLINT(readability-identifier-naming). Return quantity only

    /// Returns a dBm power unit struct value in dBm
    /// \returns a power unit value in dBm
    double in_dBm() const; // NOLINT(readability-identifier-naming). Return quantity only

    std::string str() const // NOLINT(readability-identifier-naming)
    {
        return sformat("%.1f dBm", val);
    }

    static std::vector<dBm> from_doubles(
        std::vector<double>& input) // NOLINT(readability-identifier-naming)
    {
        std::vector<dBm> output(input.size());
        std::transform(input.cbegin(), input.cend(), output.begin(), [](double f) {
            return dBm{f};
        });
        return output;
    }

    static std::vector<double> to_doubles(
        std::vector<dBm>& input) // NOLINT(readability-identifier-naming)
    {
        std::vector<double> output(input.size());
        std::transform(input.cbegin(), input.cend(), output.begin(), [](dBm f) {
            return static_cast<double>(f.val);
        });
        return output;
    }

    inline bool operator==(const dBm& rhs) const
    {
        return val == rhs.val;
    }

    inline bool operator!=(const dBm& rhs) const
    {
        return !(operator==(rhs));
    }

    inline bool operator<(const dBm& rhs) const
    {
        return val < rhs.val;
    }

    inline bool operator>(const dBm& rhs) const
    {
        return val > rhs.val;
    }

    inline bool operator<=(const dBm& rhs) const
    {
        return val <= rhs.val;
    }

    inline bool operator>=(const dBm& rhs) const
    {
        return val >= rhs.val;
    }

    inline dBm operator-() const
    {
        return dBm{-val};
    }

    inline dBm operator+(const dB& rhs) const
    {
        return dBm{val + rhs.val};
    }

    inline dBm& operator+=(const dB& rhs)
    {
        val += rhs.val;
        return *this;
    }

    inline dBm operator-(const dB& rhs) const
    {
        return dBm{val - rhs.val};
    }

    inline dBm& operator-=(const dB& rhs)
    {
        val -= rhs.val;
        return *this;
    }

    // Disallow addition/subtraction between dBm and mWatt.
    // Convert operands to the same unit first before addition or subtraction operation

    // Energy's addition and subtraction in dBm
    // are prohibited due to the fact that those operators have conflicting meanings.
    // While energy may be added or separated/consumed, + and - have the meanings of
    // multiplication and division respectively in log scale. In order to avoid this confusion
    // and potential harm, addition and subtraction of energy is allowed only in the linear scale.
    inline dBm operator+(const dBm& rhs) const
    {
        return dBm{ToLogScale(ToLinearScale(val) + ToLinearScale(rhs.val))};
    }

    // Subtraction in energy may mean consumption
    inline dBm operator-(const dBm& rhs) const
    {
        return dBm{ToLogScale(ToLinearScale(val) - ToLinearScale(rhs.val))};
    }

    inline dBm& operator+=(const dBm& rhs)
    {
        val = ToLogScale(ToLinearScale(val) + ToLinearScale(rhs.val));
        return *this;
    }

    inline dBm& operator-=(const dBm& rhs)
    {
        val = ToLogScale(ToLinearScale(val) - ToLinearScale(rhs.val));
        return *this;
    }
};

struct mWatt
{
    double val{};

    mWatt() = default;

    mWatt(double val)
        : val(val) // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
    {
    }

    // Note these are exceptions to method naming rules in order output be consistent. dBm, mWatt,
    // and Watt conversion method names.
    static mWatt from_dBm(const dBm& input); // NOLINT(readability-identifier-naming)
    dBm to_dBm() const;                      // NOLINT(readability-identifier-naming)
    double in_dBm() const; // NOLINT(readability-identifier-naming). Return quantity only

    /// Sets power unit in mWatt from Watt
    /// \param input input power struct in Watt
    /// \returns power unit struct in mWatt
    static mWatt from_Watt(const Watt& input); // NOLINT(readability-identifier-naming)

    /// Converts a mWatt power unit struct to Watt
    /// \returns power unit struct in Watt
    Watt to_Watt() const; // NOLINT(readability-identifier-naming)

    /// Returns a mWatt power unit struct value in Watt
    /// \returns a power unit value in Watt
    double in_Watt() const; // NOLINT(readability-identifier-naming). Return quantity only

    /// Returns a mWatt power unit struct value in mWatt
    /// \returns a power unit value in mWatt
    double in_mWatt() const; // NOLINT(readability-identifier-naming). Return quantity only

    std::string str() const // NOLINT(readability-identifier-naming)
    {
        return sformat("%.1f mWatt", val);
    }

    static std::vector<mWatt> from_doubles(
        std::vector<double>& input) // NOLINT(readability-identifier-naming)
    {
        std::vector<mWatt> output(input.size());
        std::transform(input.cbegin(), input.cend(), output.begin(), [](double f) {
            return mWatt{f};
        });
        return output;
    }

    static std::vector<double> to_doubles(
        std::vector<mWatt>& input) // NOLINT(readability-identifier-naming)
    {
        std::vector<double> output(input.size());
        std::transform(input.cbegin(), input.cend(), output.begin(), [](mWatt f) {
            return static_cast<double>(f.val);
        });
        return output;
    }

    inline bool operator==(const mWatt& rhs) const
    {
        return val == rhs.val;
    }

    inline bool operator!=(const mWatt& rhs) const
    {
        return !(operator==(rhs));
    }

    inline bool operator<(const mWatt& rhs) const
    {
        return val < rhs.val;
    }

    inline bool operator>(const mWatt& rhs) const
    {
        return val > rhs.val;
    }

    inline bool operator<=(const mWatt& rhs) const
    {
        return val <= rhs.val;
    }

    inline bool operator>=(const mWatt& rhs) const
    {
        return val >= rhs.val;
    }

    inline mWatt operator-() const
    {
        return mWatt{-val};
    }

    inline mWatt operator+(const mWatt& rhs) const
    {
        return mWatt{val + rhs.val};
    }

    inline mWatt operator-(const mWatt& rhs) const
    {
        return mWatt{val - rhs.val};
    }

    inline mWatt& operator+=(const mWatt& rhs)
    {
        val += rhs.val;
        return *this;
    }

    inline mWatt& operator-=(const mWatt& rhs)
    {
        val -= rhs.val;
        return *this;
    }

    /// Multiplier Operator
    /// \param rhs right-hand side operand in double type
    /// \returns a mWatt power unit struct with value equals to val * rhs
    inline mWatt operator*(const double& rhs) const
    {
        return mWatt{val * rhs};
    }

    /// Divider Operator
    /// \param rhs right-hand side operand in double type
    /// \returns a mWatt power unit struct with value equals to val / rhs
    inline mWatt operator/(const double& rhs) const
    {
        return mWatt{val / rhs};
    }

    // mWatt-Watt Cross Operations
    // Keeping classes simple, even if duplication is present. Preferring faster runtime.

    /// Equality Operator
    /// \param rhs right-hand side operand
    /// \returns true if the two power values are equal
    bool operator==(const Watt& rhs) const;

    /// Inequality Operator
    /// \param rhs right-hand side operand
    /// \returns true if the two power values are not equal
    bool operator!=(const Watt& rhs) const;

    /// Less Than Comparison Operator
    /// \param rhs right-hand side operand
    /// \returns true if the power value is less than the rhs value
    bool operator<(const Watt& rhs) const;

    /// Greater Than Comparison Operator
    /// \param rhs input Watt power unit struct to compare to
    /// \returns true if the power value is greater than the rhs value
    bool operator>(const Watt& rhs) const;

    /// Less Than or Equal Comparison Operator
    /// \param rhs right-hand side operand
    /// \returns true if the power value is less than or equal rhs value
    bool operator<=(const Watt& rhs) const;

    /// Greater Than or Equal Comparison Operator
    /// \param rhs right-hand side operand
    /// \returns true if the power value is greater than or equal the rhs value
    bool operator>=(const Watt& rhs) const;

    /// Addition Operator
    /// \param rhs right-hand side operand
    /// \returns a mWatt power unit struct with value equals the sum of the two
    mWatt operator+(const Watt& rhs) const;

    /// Subtraction Operator
    /// \param rhs right-hand side operand
    /// \returns a mWatt power unit struct with value equals val - rhs.val
    mWatt operator-(const Watt& rhs) const;

    /// Addition Assignment Operator
    /// \param rhs right-hand side operand
    /// \returns a mWatt power unit struct with value equals to val + rhs.val
    mWatt& operator+=(const Watt& rhs);

    /// Subtraction Assignment Operator
    /// \param rhs right-hand side operand
    /// \returns a mWatt power unit struct with value equals to val - rhs.val
    mWatt& operator-=(const Watt& rhs);
};

/// Multiplier Operator for mWatt
/// \param lfs left-hand side operand in double type
/// \param rhs right-hand side operand in mWatt type
/// \returns a mWatt power unit struct with value equals to lfs * rhs.val
mWatt operator*(const double& lfs, const mWatt& rhs);

/// Watt power unit structure
struct Watt
{
    double val{};     /// Value in Watt
    Watt() = default; /// Default Empty Constructor

    /// Constructor to set the power value in Watt
    /// \param val the power value in Watt to set
    Watt(double val)
        : val(val) // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
    {
    }

    // mWatt-Watt-dBm Conversion Operators
    /// Sets power unit in Watt from dBm
    /// \param input input power struct in dBm
    /// \returns power unit struct in Watt
    static Watt from_dBm(const dBm& input); // NOLINT(readability-identifier-naming)

    /// Converts a Watt power unit struct to dBm
    /// \returns power unit struct in dBm
    dBm to_dBm() const; // NOLINT(readability-identifier-naming)

    /// Returns a Watt power unit struct value in dBm
    /// \returns a power unit value in dBm
    double in_dBm() const; // NOLINT(readability-identifier-naming). Return quantity only

    /// Sets power unit in Watt from mWatt
    /// \param input input power struct in mWatt
    /// \returns power unit struct in Watt
    static Watt from_mWatt(const mWatt& input); // NOLINT(readability-identifier-naming)

    /// Converts a Watt power unit struct to mWatt
    /// \returns power unit struct in mWatt
    mWatt to_mWatt() const; // NOLINT(readability-identifier-naming)

    /// Returns a Watt power unit struct value in mWatt
    /// \returns a power unit value in mWatt
    double in_mWatt() const; // NOLINT(readability-identifier-naming). Return quantity only

    /// Returns Watt power unit struct value
    /// \returns power unit value in Watt
    double in_Watt() const; // NOLINT(readability-identifier-naming). Return quantity only

    /// Returns Watt power unit value in floating point string representations
    /// \returns a string of format "%.1f Watt" representing the power value in Watt
    std::string str() const // NOLINT(readability-identifier-naming)
    {
        return sformat("%.1f Watt", val);
    }

    /// Rerturns a vector of Watt power unit structs from an input vector values
    /// \param input a vector of input power values in Watt
    /// \returns  a vector of Watt power unit structs
    static std::vector<Watt> from_doubles(
        std::vector<double>& input) // NOLINT(readability-identifier-naming)
    {
        std::vector<Watt> output(input.size());
        std::transform(input.cbegin(), input.cend(), output.begin(), [](double f) {
            return Watt{f};
        });
        return output;
    }

    /// Converts a vector of Watt power unit structs to a output vector values
    /// \param input a vector of Watt power unit structs
    /// \returns a vector of output power values in Watt
    static std::vector<double> to_doubles(
        std::vector<Watt>& input) // NOLINT(readability-identifier-naming)
    {
        std::vector<double> output(input.size());
        std::transform(input.cbegin(), input.cend(), output.begin(), [](Watt f) {
            return static_cast<double>(f.val);
        });
        return output;
    }

    // Watt-Watt Operators
    // Keeping classes simple, even if duplication is present. Preferring faster runtime.

    /// Equality Operator
    /// \param rhs right-hand side operand
    /// \returns true if the two power values are equal
    inline bool operator==(const Watt& rhs) const
    {
        return val == rhs.val;
    }

    /// Inequality Operator
    /// \param rhs right-hand side operand
    /// \returns true if the two power values are not equal
    inline bool operator!=(const Watt& rhs) const
    {
        return !(operator==(rhs));
    }

    /// Less Than Comparison Operator
    /// \param rhs right-hand side operand
    /// \returns true if the power value is less than the rhs value
    inline bool operator<(const Watt& rhs) const
    {
        return val < rhs.val;
    }

    /// Greater Than Comparison Operator
    /// \param rhs right-hand side operand
    /// \returns true if the power value is greater than the rhs value
    inline bool operator>(const Watt& rhs) const
    {
        return val > rhs.val;
    }

    /// Less Than or Equal Comparison Operator
    /// \param rhs right-hand side operand
    /// \returns true if the power value is less than or equal the rhs value
    inline bool operator<=(const Watt& rhs) const
    {
        return val <= rhs.val;
    }

    /// Greater Than or Equal Comparison Operator
    /// \param rhs right-hand side operand
    /// \returns true if the power value is greater than or equal the rhs value
    inline bool operator>=(const Watt& rhs) const
    {
        return val >= rhs.val;
    }

    /// Arithmetic Negation Operator
    /// \returns negated power value
    inline Watt operator-() const
    {
        return Watt{-val};
    }

    /// Addition Operator
    /// \param rhs right-hand side operand
    /// \returns a Watt power unit struct with value equals the sum of the two
    inline Watt operator+(const Watt& rhs) const
    {
        return Watt{val + rhs.val};
    }

    /// Subtraction Operator
    /// \param rhs right-hand side operand
    /// \returns a Watt power unit struct with value equals val - rhs.val
    inline Watt operator-(const Watt& rhs) const
    {
        return Watt{val - rhs.val};
    }

    /// Addition Assignment Operator
    /// \param rhs right-hand side operand
    /// \returns a Watt power unit struct with value equals to val + rhs.val
    inline Watt& operator+=(const Watt& rhs)
    {
        val += rhs.val;
        return *this;
    }

    /// Subtraction Assignment Operator
    /// \param rhs right-hand side operand
    /// \returns a Watt power unit struct with value equals to val - rhs.val
    inline Watt& operator-=(const Watt& rhs)
    {
        val -= rhs.val;
        return *this;
    }

    /// Multiplication Assignment Operator
    /// \param rhs right-hand side operand
    /// \returns a Watt power unit struct with value equals to val * rhs.val
    inline Watt operator*(const Watt& rhs) const
    {
        return Watt{val * rhs.val};
    }

    // Watt-mWatt Cross-operators
    // Keeping classes simple, even if duplication is present. Preferring faster runtime.

    /// Equality Operator
    /// \param rhs right-hand side operand
    /// \returns true if the two power values are equal
    bool operator==(const mWatt& rhs) const;

    /// Inequality Operator
    /// \param rhs right-hand side operand
    /// \returns true if the two power values are not equal
    bool operator!=(const mWatt& rhs) const;

    /// Less Than Comparison Operator
    /// \param rhs right-hand side operand
    /// \returns true if the power value is less than the rhs value
    bool operator<(const mWatt& rhs) const;

    /// Greater Than Comparison Operator
    /// \param rhs right-hand side operand
    /// \returns true if the power value is greater than the rhs value
    bool operator>(const mWatt& rhs) const;

    /// Less Than or Equal Comparison Operator
    /// \param rhs right-hand side operand
    /// \returns true if the power value is less than or equal the rhs value
    bool operator<=(const mWatt& rhs) const;

    /// Greater Than or Equal Comparison Operator
    /// \param rhs right-hand side operand
    /// \returns true if the power value is greater than or equal the rhs value
    bool operator>=(const mWatt& rhs) const;

    /// Addition Operator
    /// \param rhs right-hand side operand
    /// \returns a mWatt power unit struct with value equals the sum of the two values
    mWatt operator+(const mWatt& rhs) const;

    /// Subtraction Operator
    /// \param rhs right-hand side operand
    /// \returns a mWatt power unit struct with value equals val - rhs.val
    mWatt operator-(const mWatt& rhs) const;

    /// Addition Assignment Operator
    /// \param rhs right-hand side operand
    /// \returns a Watt power unit struct with value equals to val + rhs.val
    Watt& operator+=(const mWatt& rhs);

    /// Subtraction Assignment Operator
    /// \param rhs right-hand side operand
    /// \returns a Watt power unit struct with value equals to val - rhs.val
    Watt& operator-=(const mWatt& rhs);
};

// Power spectral density
struct dBm_per_Hz // NOLINT(readability-identifier-naming)
{
    double val{};

    dBm_per_Hz() = default;

    dBm_per_Hz(double val)
        : val(val) // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
    {
    }

    double in_dBm() const; // NOLINT(readability-identifier-naming). Return quantity only

    dBm to_dBm() const // NOLINT(readability-identifier-naming)
    {
        return dBm{val};
    }

    static std::vector<dBm_per_Hz> from_doubles(
        std::vector<double>& input) // NOLINT(readability-identifier-naming)
    {
        std::vector<dBm_per_Hz> output(input.size());
        std::transform(input.cbegin(), input.cend(), output.begin(), [](double f) {
            return dBm_per_Hz{f};
        });
        return output;
    }

    static std::vector<double> to_doubles(
        std::vector<dBm_per_Hz>& input) // NOLINT(readability-identifier-naming)
    {
        std::vector<double> output(input.size());
        std::transform(input.cbegin(), input.cend(), output.begin(), [](dBm_per_Hz f) {
            return static_cast<double>(f.val);
        });
        return output;
    }

    std::string str() const // NOLINT(readability-identifier-naming)
    {
        return sformat("%.1Lf dBm/Hz", val);
    }

    static dBm_per_Hz AveragePsd(dBm power, Hz bandwidth)
    {
        return dBm_per_Hz{power.val - ToLogScale(static_cast<double>(bandwidth.val))};
    }

    inline dBm OverBandwidth(const Hz& rhs) const
    {
        return dBm{val + ToLogScale(static_cast<double>(rhs.val))};
    }

    inline bool operator==(const dBm_per_Hz& rhs) const
    {
        return val == rhs.val;
    }

    inline bool operator!=(const dBm_per_Hz& rhs) const
    {
        return !(operator==(rhs));
    }

    inline bool operator<(const dBm_per_Hz& rhs) const
    {
        return val < rhs.val;
    }

    inline bool operator>(const dBm_per_Hz& rhs) const
    {
        return val > rhs.val;
    }

    inline bool operator<=(const dBm_per_Hz& rhs) const
    {
        return val <= rhs.val;
    }

    inline bool operator>=(const dBm_per_Hz& rhs) const
    {
        return val >= rhs.val;
    }

    inline dBm_per_Hz operator-() const
    {
        return dBm_per_Hz{-val};
    }

    inline dBm_per_Hz operator+(const dB& rhs) const
    {
        return dBm_per_Hz{val + rhs.val};
    }

    inline dBm_per_Hz& operator+=(const dB& rhs)
    {
        val += rhs.val;
        return *this;
    }

    inline dBm_per_Hz operator-(const dB& rhs) const
    {
        return dBm_per_Hz{val - rhs.val};
    }

    inline dBm_per_Hz& operator-=(const dB& rhs)
    {
        val -= rhs.val;
        return *this;
    }
};

dB operator"" _dB(long double val);
dB operator"" _dB(unsigned long long val);
dBm operator"" _dBm(long double val);
dBm operator"" _dBm(unsigned long long val);
mWatt operator"" _mWatt(long double val);
mWatt operator"" _mWatt(unsigned long long val);
mWatt operator"" _pWatt(long double val);
mWatt operator"" _pWatt(unsigned long long val);
Watt operator"" _Watt(long double val);
Watt operator"" _Watt(unsigned long long val);
dBm_per_Hz operator"" _dBm_per_Hz(long double val);

std::ostream& operator<<(std::ostream& os, const dB& rhs);
std::ostream& operator<<(std::ostream& os, const dBm& rhs);
std::ostream& operator<<(std::ostream& os, const mWatt& rhs);
std::ostream& operator<<(std::ostream& os, const Watt& rhs);
std::ostream& operator<<(std::ostream& os, const dBm_per_Hz& rhs);

std::istream& operator>>(std::istream& is, dB& rhs);
std::istream& operator>>(std::istream& is, dBm& rhs);
std::istream& operator>>(std::istream& is, mWatt& rhs);
std::istream& operator>>(std::istream& is, Watt& rhs);
std::istream& operator>>(std::istream& is, dBm_per_Hz& rhs);

} // namespace ns3

#endif // UNITS_ENERGY_H
