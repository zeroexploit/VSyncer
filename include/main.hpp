/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   main.hpp
 * Author: Jörn Roddelkopf
 * Version: 1.0 23. Februar 2017
 * Purpose:
 */

#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <vector>
#include <dirent.h>
#include <cstring>
#include <cstdlib>
#include "videofile.hpp"

std::vector<std::string> getFilesInDir(std::string dir);

#endif /* MAIN_HPP */

