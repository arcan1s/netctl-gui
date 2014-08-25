/***************************************************************************
 *  Copyright (C) 2014  Evgeniy Alekseev                                      *
 *                                                                         *
 *  This library is free software; you can redistribute it and/or          *
 *  modify it under the terms of the GNU Lesser General Public             *
 *  License as published by the Free Software Foundation; either           *
 *  version 3.0 of the License, or (at your option) any later version.     *
 *                                                                         *
 *  This library is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU       *
 *  Lesser General Public License for more details.                        *
 *                                                                         *
 *  You should have received a copy of the GNU Lesser General Public       *
 *  License along with this library.                                       *
 ***************************************************************************/


#ifndef PRETTY_DEBUG_H
#define PRETTY_DEBUG_H


inline const char *pDebug(const std::string prettyFunction)
{
    return prettyFunction.c_str();
}


inline std::string pFuncInfo(const std::string prettyFunction)
{
    size_t colons = prettyFunction.find("::");
    // workaround for functions which are not belong to any class
    if (colons == std::string::npos)
        colons = prettyFunction.rfind("(");
    size_t begin = prettyFunction.substr(0, colons).rfind(" ") + 1;
    size_t end = prettyFunction.rfind("(") - begin;

    return "[" + prettyFunction.substr(begin, end) + "]";
}


#define PDEBUG pDebug(pFuncInfo(__PRETTY_FUNCTION__))


#endif /* PRETTY_DEBUG_H */
