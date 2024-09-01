#include "ObjectList.h"

const int thisMonth = 2678400;
const int thisHalfYear = 15638400;
const int thisYear = 31536000;

const size_t minObjType = 2;


namespace groupLabel
{
    namespace prefix
    {
        const char* dist = "~ Group distance: ";
        const char* name = "~ Group name: ";
        const char* type = "~ Group type: ";
        const char* time = "~ Group creation time: ";
    }
    namespace name
    {
        const char* distClose = "1) From 0 to 30";
        const char* distMid = "2) From 30 to 60";
        const char* distFar = "3) From 60 to 100";
        const char* distVeryFar = "4) From 100";

        const char* nameOther = "#";

        const char* typeMisc = "Misc";

        const char* timeMonth = "1) <1 month";
        const char* timeHalfYear = "2) From 1 to 6 months";
        const char* timeYear = "3) From 6 to 12 months";
        const char* timeOld = "4) >12 months";
    }
}


float Object::calculateDistance() const
{
    float distance = 0.0f;
    std::stringstream coordStr;
    float coordX, coordY;
    coordStr << m_CoordX << " " << m_CoordY;
    coordStr >> coordX;
    coordStr >> coordY;
    distance = std::abs(coordX - coordY);
    return distance;
}

uint32_t Object::calculateTimeDifference(const uint32_t& now) const
{
    return now - m_Time;
}

std::string Object::getAsString() const
{
    return std::string(m_Name + " "
        + m_CoordX + " "
        + m_CoordY + " "
        + m_Type + " "
        + std::to_string(m_Time));
}

std::string Object::getReadableTime() const
{
    return std::string();
}




ObjectsList::ObjectsList(const std::string& fileName) : m_FileName(fileName)
{
	readFile(m_FileName);
}

bool ObjectsList::readFile(const std::string& fileName)
{
    std::ifstream file;

    file.open(fileName);
    if (!file.is_open())
    {
        return false;
    }

    m_Objects.clear();
    m_GroupedObjects.clear();
    m_CurrentSortState = NOSORT;

    std::string line;
    while (std::getline(file, line))
    {
        if (line[0] == '~')
            continue;
        std::stringstream tempStream(line);
        m_Objects.push_back(getObjectFromString(tempStream));
    }
    file.close();

    return true;
}

void ObjectsList::addObject(const Object& obj)
{
    m_Objects.push_back(obj);
    if (m_CurrentSortState != NOSORT)
        groupAndSort(m_CurrentSortState);
}

void ObjectsList::groupAndSort(const SORTSTATE sortChoice)
{
    m_GroupedObjects.clear();
    switch (sortChoice)
    {
    case BY_DIST:
        groupAndSortByDistance();
        break;
    case BY_NAME:
        groupAndSortByName();
        break;
    case BY_TYPE:
        groupAndSortByType();
        break;
    case BY_TIME:
        groupAndSortByTime();
        break;
    default:
        break;
    }

    m_CurrentSortState = sortChoice;
}

void ObjectsList::saveFile(const std::string& fileName) const
{
    std::ofstream outFile;
    outFile.open(fileName);
    if (!outFile.is_open())
    {
        std::cout << "Error!\n";
        return;
    }

    if (m_CurrentSortState == NOSORT)
    {
        for (const auto& obj : m_Objects)
        {
            outFile << obj.getAsString() << '\n';
        }
    }
    else
    {
        for (const auto& [groupName, objects] : m_GroupedObjects)
        {
            if (objects.size() == 0)
                continue;
            switch (m_CurrentSortState)
            {
            case BY_DIST:
                outFile << groupLabel::prefix::dist;
                break;
            case BY_NAME:
                outFile << groupLabel::prefix::name;
                break;
            case BY_TYPE:
                outFile << groupLabel::prefix::type;
                break;
            case BY_TIME:
                outFile << groupLabel::prefix::time;
                break;
            default:
                break;
            }
            outFile << groupName << '\n';
            for (const auto& obj : objects)
            {
                outFile << obj.getAsString() << '\n';
            }
        }
    }

    outFile.close();
}

void ObjectsList::groupAndSortByDistance()
{
    for (const auto& obj : m_Objects)
    {
        float dist = obj.calculateDistance();

        if (dist < 30)
            m_GroupedObjects[groupLabel::name::distClose].push_back(obj);
        else if (dist < 60)
            m_GroupedObjects[groupLabel::name::distMid].push_back(obj);
        else if (dist < 100)
            m_GroupedObjects[groupLabel::name::distFar].push_back(obj);
        else
            m_GroupedObjects[groupLabel::name::distVeryFar].push_back(obj);
    }
    for (auto& [group, objects] : m_GroupedObjects)
    {
        std::sort(objects.begin(), objects.end(), [&](Object& lhs, Object& rhs)
        {
            float lhsDist = lhs.calculateDistance();
            float rhsDist = rhs.calculateDistance();

            return lhsDist < rhsDist;
        });
    }
}

void ObjectsList::groupAndSortByName()
{    
    for (const auto& obj : m_Objects)
    {
        char firstLet = obj.m_Name.front();
        if ((firstLet >= 'A' && firstLet <= 'Z') || (firstLet >= 'a' && firstLet <= 'z'))
            m_GroupedObjects[std::string{firstLet}].push_back(obj);
        else
            m_GroupedObjects[groupLabel::name::nameOther].push_back(obj);
            
    }
    for (auto& [group, objects] : m_GroupedObjects)
    {
        std::sort(objects.begin(), objects.end(), [&](Object& lhs, Object& rhs)
            {
                return lhs.m_Name < rhs.m_Name;
            });
    }
}

void ObjectsList::groupAndSortByType()
{
    for (const auto& obj : m_Objects)
    {
        m_GroupedObjects[obj.m_Type].push_back(obj);
    }
    for (auto& [group, objects] : m_GroupedObjects)
    {
        if (objects.size() == minObjType)
        {
            m_GroupedObjects[groupLabel::name::typeMisc] = std::move(objects);
            continue;
        }
        std::sort(objects.begin(), objects.end(), [&](Object& lhs, Object& rhs)
            {
                return lhs.m_Name < rhs.m_Name;
            });
    }
}

void ObjectsList::groupAndSortByTime()
{
    auto nowDate = std::chrono::system_clock::now();
    uint32_t nowSec = static_cast<uint32_t>(std::chrono::time_point_cast<std::chrono::seconds>(nowDate).time_since_epoch().count());
    for (const auto& obj : m_Objects)
    {
        uint32_t timeDiff= obj.calculateTimeDifference(nowSec);
        if (timeDiff <= thisMonth)
            m_GroupedObjects[groupLabel::name::timeMonth].push_back(obj);
        else if (timeDiff <= thisHalfYear)
            m_GroupedObjects[groupLabel::name::timeHalfYear].push_back(obj);
        else if (timeDiff <= thisYear)
            m_GroupedObjects[groupLabel::name::timeYear].push_back(obj);
        else
            m_GroupedObjects[groupLabel::name::timeOld].push_back(obj);
    }
    for (auto& [group, objects] : m_GroupedObjects)
    {
        std::sort(objects.begin(), objects.end(), [&](Object& lhs, Object& rhs)
            {
                return lhs.m_Time > rhs.m_Time;
            });
    }
}

Object ObjectsList::getObjectFromString(std::stringstream& str) const
{
    Object ret;
    str >> ret.m_Name;
    str >> ret.m_CoordX;
    str >> ret.m_CoordY;
    str >> ret.m_Type;
    str >> ret.m_Time;
    return ret;
}