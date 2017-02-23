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

#include <string>

#include "include/videofile.hpp"

VideoFile::VideoFile(void)
{
    this->duration = 0;
    this->offsetEnd = "";
    this->offsetStart = "";
    this->path = "";
    this->audioCodec = "";
    this->videoCodec = "";
}

int VideoFile::getDuration(void)
{
    return this->duration;
}

std::string VideoFile::getOffsetEnd(void)
{
    return this->offsetEnd;
}

std::string VideoFile::getOffsetStart(void)
{
    return this->offsetStart;
}

std::string VideoFile::getPath(void)
{
    return this->path;
}

std::string VideoFile::getVideoCodec(void)
{
    return this->videoCodec;
}

std::string VideoFile::getAudioCodec(void)
{
    return this->audioCodec;
}

bool VideoFile::parseFile(std::string path)
{
    char buffer[2048];
    std::string xmlRaw = "";
    this->path = path;
    
    // Using ffprobe here with a XML Output
    std::string command = "ffprobe -v quiet -print_format xml -show_format -show_streams -unit \"" + path + "\"";

    FILE* file = popen(command.c_str(), "r");
    
    if(file == NULL)
    {
        return false;
    }
    
    while(fgets(buffer, sizeof(buffer), file))
    {
        xmlRaw.append(buffer);
        memset(buffer, '\0', sizeof(buffer));
    }
    
    pclose(file);

    // Check if any information could be read
    if(xmlRaw.length() <= 0 || xmlRaw.find("format") == std::string::npos || xmlRaw.find("streams") == std::string::npos)
    {
        return false;
    }
    
    // Parse XML
    parseFromXML(xmlRaw);
    
    return true;
}

void VideoFile::parseFromXML(std::string xml)
{
    XMLParser xmlParser = XMLParser(xml);
    struct XMLEntry xmlEntry = xmlParser.getSingleTag("format");
    
    for(unsigned int i = 0; i < xmlEntry.attributes.size(); i++)
    {
        if(xmlEntry.attributes.at(i).name.compare("duration") == 0)
        {
            this->duration = std::stod(xmlEntry.attributes.at(i).value);
            break;
        }
    }

    struct XMLEntry streams = xmlParser.getSingleTag("streams");
    
    for(unsigned int i = 0; i < streams.subTags.size(); i++)
    {
        std::string codecName = "";
        std::string codecLongName = "";
        std::string codecType = "";

        struct XMLEntry xmlStream = streams.subTags.at(i);
        
        for(unsigned int x = 0; x < xmlStream.attributes.size(); x++)
        {
            if(xmlStream.attributes.at(x).name.compare("codec_name") == 0)
                codecName = xmlStream.attributes.at(x).value;
            else if(xmlStream.attributes.at(x).name.compare("codec_long_name") == 0)
                codecLongName = xmlStream.attributes.at(x).value;
            else if(xmlStream.attributes.at(x).name.compare("codec_type") == 0)
                codecType = xmlStream.attributes.at(x).value;
        }
        
        if(codecType.compare("video") == 0)
        {
            this->videoCodec = codecName;
        }
        else if(codecType.compare("audio") == 0)
        {
            this->audioCodec = codecName;
        }
    }
}

bool VideoFile::calculateOffsets(std::string startTime, std::string endTime, std::vector<std::string> patterns)
{
    std::string filename = this->path.substr(path.find_last_of("/") + 1);
    
    for(unsigned int i = 0; i < patterns.size(); i++)
    {
        std::string pattern = patterns.at(i);
        
        if(filename.length() != pattern.length() - 12)
            continue;
        
        int startDay = pattern.find("%d");
        int endDay = pattern.find("d%");

        int startMonth = pattern.find("%m") - 2;
        int endMonth = pattern.find("m%") - 2;

        int startYear = pattern.find("%y") - 4;
        int endYear = pattern.find("y%") - 4;   

        int startHour = pattern.find("%h") - 6;
        int endHour = pattern.find("h%") - 6;

        int startMinute = pattern.find("%M") - 8;
        int endMinute = pattern.find("M%") - 8;

        int startSecond = pattern.find("%s") - 10;
        int endSecond = pattern.find("s%") - 10;

        int day = std::stoi(filename.substr(startDay, endDay - startDay));
        int month = std::stoi(filename.substr(startMonth, endMonth - startMonth));
        int year = std::stoi(filename.substr(startYear, endYear - startYear));
        int hour = std::stoi(filename.substr(startHour, endHour - startHour));
        int minute = std::stoi(filename.substr(startMinute, endMinute - startMinute));
        int second = std::stoi(filename.substr(startSecond, endSecond - startSecond));

        int hourTimeStart = std::stoi(startTime.substr(0, 2));
        int minuteTimeStart = std::stoi(startTime.substr(3, 2));
        int secondTimeStart = std::stoi(startTime.substr(6, 2));

        int hourTimeEnd = std::stoi(endTime.substr(0, 2));
        int minuteTimeEnd = std::stoi(endTime.substr(3, 2));
        int secondTimeEnd = std::stoi(endTime.substr(6, 2));

        ///----------------------

        // Calculate the start Time
        int hourDiffStart = hourTimeStart - hour;
        int minuteDiffStart = minuteTimeStart - minute;
        int secondDiffStart = secondTimeStart - second;

        // File starts after Start Time        
        if(hourDiffStart < 0)
        {
            hourDiffStart = 0;
            minuteDiffStart = 0;
            secondDiffStart = 0;
        }

        if(minuteDiffStart < 0)
        {
            if(hourDiffStart > 0)
            {
                hourDiffStart--;            
                minuteDiffStart = 60 + minuteDiffStart;
            }
            else
            {
                minuteDiffStart = 0;
            }
        }

        if(secondDiffStart < 0)
        {
            if(minuteDiffStart > 0)
            {
                minuteDiffStart--;
                secondDiffStart = 60 + secondDiffStart;
            }
            else
            {
                secondDiffStart = 0;
            }
        }

        // Calculate the Duration
        int hourDuraDiff = hourTimeEnd - hour - hourDiffStart;
        int minuteDuraDiff = minuteTimeEnd - minute - minuteDiffStart;
        int secondDuraDiff = secondTimeEnd - second - secondDiffStart;

        // File Ends after End Time
        if(hourDuraDiff < 0)
        {
            hourDuraDiff = 0;
            minuteDuraDiff = 0;
            secondDuraDiff = 0;
        }

        if(minuteDuraDiff < 0)
        {
            hourDuraDiff--;
            minuteDuraDiff = 60 + minuteDuraDiff;
        }

        if(secondDuraDiff < 0)
        {
            minuteDuraDiff--;
            secondDuraDiff = 60 + secondDuraDiff;
        }

        // If everything is 0 than the file is not inside the Time
        if(hourDiffStart == 0 && minuteDiffStart == 0 && secondDiffStart == 0)
        {
            if(hourDuraDiff == 0 && minuteDuraDiff == 0 && secondDuraDiff == 0)
            {
                return false;
            }
        }

        // Set the Values for the start Time
        if(hourDiffStart < 10)
        {
            this->offsetStart = "0" + std::to_string(hourDiffStart);
        }
        else
        {
            this->offsetStart = std::to_string(hourDiffStart);
        }

        if(minuteDiffStart < 10)
        {
            this->offsetStart += ":0" + std::to_string(minuteDiffStart);
        }
        else
        {
            this->offsetStart += ":" + std::to_string(minuteDiffStart);
        }

        if(secondDiffStart < 10)
        {
            this->offsetStart += ":0" + std::to_string(secondDiffStart);
        }
        else
        {
            this->offsetStart += ":" + std::to_string(secondDiffStart);
        }

        // Set the Values for the End time
        if(hourDuraDiff < 10)
        {
            this->offsetEnd = "0" + std::to_string(hourDuraDiff);
        }
        else
        {
            this->offsetEnd = std::to_string(hourDuraDiff);
        }

        if(minuteDuraDiff < 10)
        {
            this->offsetEnd += ":0" + std::to_string(minuteDuraDiff);
        }
        else
        {
            this->offsetEnd += ":" + std::to_string(minuteDuraDiff);
        }

        if(secondDuraDiff < 10)
        {
            this->offsetEnd += ":0" + std::to_string(secondDuraDiff);
        }
        else
        {
            this->offsetEnd += ":" + std::to_string(secondDuraDiff);
        }

        return true;
    }
    
    return false;
}

bool VideoFile::encodeToNewFile(std::string videoCodec, std::string audioCodec, std::string fileType)
{
    std::string outputPath = this->path;
    outputPath = outputPath.substr(0, outputPath.find_last_of(".") - 1) + "_enc" + outputPath.substr(outputPath.find("."));
    
    std::string ffmpeg = "ffmpeg -y -loglevel quiet -ss " + this->offsetStart + " -i " + this->path;
    
    if(this->videoCodec.compare(videoCodec) == 0 || this->videoCodec.length() == 0)
    {
        ffmpeg += " -c:v copy";
    }
    else
    {
        ffmpeg += " -c:v libx264 -crf 25";
    }
    
    if(this->audioCodec.compare(audioCodec) == 0)
    {
        ffmpeg += " -c:a copy";
    }    
    else if(this->audioCodec.length() == 0)
    {
        ffmpeg += " -an";
    }
    else
    {
        ffmpeg += " -c:a libmp3lame -q:a 5";
    }
    
    ffmpeg += " -t " + this->offsetEnd + " -f " + fileType + " \"" + outputPath + "\"";
    
    system(ffmpeg.c_str());
}