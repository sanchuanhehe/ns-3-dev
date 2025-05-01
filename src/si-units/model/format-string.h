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
#ifndef FORMAT_STRING_H
#define FORMAT_STRING_H

// String formatter
// A stop gap for C++20 std::format and C++23 std::print
// TODO(porce): Deprecate by std::format when ready

#include <memory>
#include <stdexcept>
#include <string>

namespace ns3
{

// https://stackoverflow.com/a/26221725
// License: CC0 1.0
template <typename... Args>
std::string
sformat(const std::string& format_str, Args... args)
{
    int size = snprintf(nullptr, 0, format_str.c_str(), args...) + 1; // Extra space for '\0'
    if (size <= 0)
    {
        throw std::runtime_error("Error during format string.");
    }
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format_str.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

} // namespace ns3

#endif // FORMAT_STRING_H
