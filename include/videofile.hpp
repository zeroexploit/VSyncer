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
 * File:   videofile.hpp
 * Author: Jörn Roddelkopf
 * Version: 1.0 23. Februar 2017
 * Purpose: Store all required Information about a Video File
 */

#ifndef VIDEOFILE_HPP
#define VIDEOFILE_HPP

#include <cstring>
#include <vector>
#include "xmlparser.hpp"

class VideoFile
{
    private:
        int duration;
        std::string offsetStart;
        std::string offsetEnd;
        std::string path;
        std::string videoCodec;
        std::string audioCodec;
        std::string videoEncoder;
        std::string audioEncoder;
        
    public:
        VideoFile(void);
        void setEncoders(std::string videoEncoder, std::string audioEncoder);
        int getDuration(void);
        std::string getOffsetStart(void);
        std::string getOffsetEnd(void);
        std::string getPath(void);
        std::string getVideoCodec(void);
        std::string getAudioCodec(void);
        bool parseFile(std::string path);
        bool calculateOffsets(std::string startTime, std::string endTime, std::vector<std::string> patterns);
        bool encodeToNewFile(std::vector<std::string> videoCodec, std::vector<std::string> audioCodec, std::string fileType);
        
    private:
        void parseFromXML(std::string xml);
};

#endif /* VIDEOFILE_HPP */

