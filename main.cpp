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

#include "include/main.hpp"

int main(int argc, char** argv)
{
    // Say Hello and parse the Command Line Arguments
    std::cout << "VSyncer v.1.0.0\n\nUsage: vsyncer [Filename Convention] [Video Folder] [Start Time] [End Time] [Output Format] [Required Video Codec] [Required Audio Codec]\n" << std::endl;
    
    if(argc < 8)
    {
        std::cout << "Not enough Parameters provided! Exiting..." << std::endl;
        return EXIT_FAILURE;
    }
    
    std::string convention= std::string(argv[1]);
    std::string folder = std::string(argv[2]);
    std::string timestringStart = std::string(argv[3]);
    std::string timestringEnd = std::string(argv[4]);
    std::string outputFormat = std::string(argv[5]);
    std::string videoCodec = std::string(argv[6]);
    std::string audioCodec = std::string(argv[7]);
    std::vector<std::string> fileList = getFilesInDir(folder);
    
    // Exit if there is no File to process
    if(fileList.size() == 0)
    {
        return EXIT_FAILURE;
    }   
    
    // Create the VideoFile Objects from the Pathes
    for(unsigned int i = 0; i < fileList.size(); i++)
    {
        VideoFile tmpVideo = VideoFile();        
        
        tmpVideo.parseFile(folder + fileList.at(i));
        if(tmpVideo.calculateOffsets(timestringStart, timestringEnd, XMLParser::split(convention, "|")))
        {
            std::cout << "File: " << fileList.at(i) << "\nStart: " << tmpVideo.getOffsetStart() + " | Duration: " + tmpVideo.getOffsetEnd() + " | Video Codec: " + tmpVideo.getVideoCodec() + " | Audio Codec: " + tmpVideo.getAudioCodec() + "\n" << std::endl;
        
            tmpVideo.encodeToNewFile(videoCodec, audioCodec, outputFormat);
        }
        else
        {
            std::cout << "--- Skipping: " << fileList.at(i) << std::endl;
        }
    }
    
    return EXIT_SUCCESS;
}

std::vector<std::string> getFilesInDir(std::string dir)
{
    DIR* dp;
    struct dirent* dirp;
    std::vector<std::string> fileList;
    
    if((dp  = opendir(dir.c_str())) == NULL) 
    {
        std::cout << "Error opening " + dir << std::endl;
        return fileList;
    }

    while ((dirp = readdir(dp)) != NULL) 
    {
        if(dirp->d_type == DT_REG)
        {
            fileList.push_back(std::string(dirp->d_name));
        }
    }
    
    closedir(dp);
    
    return fileList;
}