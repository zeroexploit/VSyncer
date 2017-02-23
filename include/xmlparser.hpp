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
 * File:   xmlparser.hpp
 * Author: Jörn Roddelkopf
 * Version: 1.0 4. Oktober 2016
 * 
 * Purpose: Working with Xml Data
 */

#ifndef XMLPARSER_HPP
#define XMLPARSER_HPP

#include <iostream>
#include <vector>

struct NameValuePair
{
    std::string name;
    std::string value;
};

struct XMLEntry
{
    std::string tag;
    std::string value;
    std::vector<struct NameValuePair> attributes;
    std::vector<struct XMLEntry> subTags;
};

class XMLParser
{
    private:
        std::string xmlContent;
        std::vector<struct XMLEntry> xmlEntries;
        std::vector<struct XMLEntry> parseXML(std::string content);
        struct XMLEntry findTag(std::vector<struct XMLEntry> container, std::string name);
        std::vector<struct NameValuePair> getAttributes(std::string tag);        
        
    public:
        XMLParser(void);
        XMLParser(std::string xmlContent);
        std::vector<struct XMLEntry> getXmlEntries(void);
        std::string getXmlString(void);
        void startXML(void);
        void openTag(std::string name, std::vector<struct NameValuePair> attributes, bool noContent=false);
        void closeTag(std::string name);
        void insertValue(std::string value);
        struct XMLEntry getSingleTag(std::string name);
        static std::vector<std::string>split(std::string text, std::string delimiter, bool keepDelimiter = false);
};

#endif /* XMLPARSER_HPP */

