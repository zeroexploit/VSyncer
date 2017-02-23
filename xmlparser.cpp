#include "include/xmlparser.hpp"

XMLParser::XMLParser(void)
{
    this->xmlContent = "";
}

XMLParser::XMLParser(std::string xmlContent)
{
    this->xmlContent = xmlContent;
    this->xmlEntries = parseXML(this->xmlContent);
}

std::vector<struct XMLEntry> XMLParser::parseXML(std::string content)
{
    size_t startPosition = 0;
    size_t endPosition = content.length();
    size_t tagEndPosition = 0;
    
    std::vector<struct XMLEntry> resultList;
    XMLEntry result;
    
    if(content.length() == 0)
        return resultList;

    // Find the beginning of a new Tag
    while((startPosition = content.find("<", startPosition)) != std::string::npos)
    {
        // If this is the End Tag return the Result
        if(content.at(startPosition + 1) == '/')
        {
            return resultList;
        }
        
        // If this is the <?xml start skip it
        if(content.at(startPosition + 1) == '?')
        {
            startPosition = content.find("?>", startPosition) + 2;
            continue;
        }
        
        // Find the enclosing Point
        endPosition = content.find(">", startPosition);
        
        // Split on whitespaces to get tag name and attributes
        std::string subContent = content.substr(startPosition, endPosition - startPosition);

        // First item should be the Tag itself
        result.tag = split(subContent, " ").at(0).substr(1);
        result.attributes = getAttributes(subContent);
        
        // Find the end of this Tag
        tagEndPosition = content.find("</" + result.tag, endPosition);
        
        if(tagEndPosition == std::string::npos)
        {
            tagEndPosition = content.find("/>", startPosition);
            startPosition = tagEndPosition + 2;
        }
        else
        {
            tagEndPosition -= 2;
            
            // Get the Sub Tags
            result.subTags = parseXML(content.substr(endPosition + 1, tagEndPosition - endPosition ));
            
            // If there ar none put in a Value
            if(result.subTags.size() == 0)
                result.value = content.substr(endPosition + 1, tagEndPosition - endPosition + 1);
            
            startPosition = tagEndPosition + 3 + result.tag.length();
        }
        
        resultList.push_back(result);
    }
    
    return resultList;
}

std::vector<struct NameValuePair> XMLParser::getAttributes(std::string tag)
{
    std::vector<struct NameValuePair> result;
    
    // Remove the leading "<"
    tag = tag.substr(tag.find("<") + 1);
    
    // Remove the tailing ">" or "/>"
    tag = tag.substr(0, tag.find(">"));
    if(tag.at(tag.length() - 1) == '/')
        tag = tag.substr(0, tag.length() - 1);
    
    // Remove the Tags Name
    tag = tag.substr(tag.find(" ") + 1);
    
    // Split the Attributes
    std::vector<std::string> partList = split(tag, "=\"");
       
    // Get the Attributes
    for(unsigned int i = 0; i < partList.size() - 1; i++)
    {
        // Split at = and create Name-Value Pair
        struct NameValuePair nameValuePair;
        // Set the Name (removing the enclosing "")
        std::vector<std::string> nameList = split(partList.at(i), " ");
        nameValuePair.name = nameList.at(nameList.size() - 1);
        // Set the Value (removing the enclosing "")
        nameValuePair.value = partList.at(i + 1).substr(0, partList.at(i + 1).find("\""));
            
        result.push_back(nameValuePair);
    }
    
    return result;
}

std::vector<struct XMLEntry> XMLParser::getXmlEntries(void)
{
    return this->xmlEntries;
}

std::string XMLParser::getXmlString(void)
{
    return this->xmlContent;
}

void XMLParser::startXML(void)
{
    this->xmlContent = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
}

void XMLParser::insertValue(std::string value)
{
    this->xmlContent += value;
}

void XMLParser::openTag(std::string name, std::vector<NameValuePair> attributes, bool noContent)
{
    this->xmlContent +=
            "<" + name;
    
    if(attributes.size() > 0)
        this->xmlContent += " ";
    
    for(unsigned int i = 0; i < attributes.size(); i++)
    {
        this->xmlContent += attributes.at(i).name + "=\"" + attributes.at(i).value + "\"";
        this->xmlContent += " ";
    }
    
    if(noContent)
        this->xmlContent += "/>";
    else
        this->xmlContent += ">";
}

void XMLParser::closeTag(std::string name)
{
    this->xmlContent += "</" + name + ">";
}

struct XMLEntry XMLParser::findTag(std::vector<struct XMLEntry> container, std::string name)
{
    XMLEntry tmpEntry;
    
    for(unsigned int i = 0; i < container.size(); i++)
    {
        if(container.at(i).tag.compare(name) == 0)
        {
            return container.at(i);
        }
        else
        {
            tmpEntry = findTag(container.at(i).subTags, name);
            
            if(tmpEntry.tag.length() > 0)
                return tmpEntry;
        }
    }
    
    return tmpEntry;
}

struct XMLEntry XMLParser::getSingleTag(std::string name)
{
    return findTag(this->xmlEntries, name);
}

std::vector<std::string> XMLParser::split(std::string text, std::string delimiter, bool keepDelimiter)
{
    const int delimLength = delimiter.length();
    size_t position = 0;
    size_t oldPosition = 0;
    std::string tmpString = "";
    std::vector<std::string> partList;
    
    if(delimLength == 0)
    {
        partList.push_back(text);
        return partList;
    }
    
    while((position = text.find(delimiter, position)) != std::string::npos)
    {
        tmpString = text.substr(oldPosition, position - oldPosition);
        
        if(tmpString.length() > 0)
            partList.push_back(tmpString.c_str());
        
        oldPosition = position;
        position += delimLength;
        
        if(!keepDelimiter)
            oldPosition += delimLength;
    }
    
    if(partList.size() == 0)
    {
        partList.push_back(text);
    }
    else
    {
        tmpString = text.substr(oldPosition).c_str();
        
        if(tmpString.length() > 0)
        {
            partList.push_back(text.substr(oldPosition).c_str());
        }
    }
    
    return partList;
}