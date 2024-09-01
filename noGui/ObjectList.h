#pragma once

#include "inc.h"

enum SORTSTATE {
    NOSORT,
    BY_DIST,
    BY_NAME,
    BY_TYPE,
    BY_TIME,
};

struct Object {
    std::string     m_Name;
    std::string     m_CoordX;
    std::string     m_CoordY;
    std::string     m_Type;
    uint32_t        m_Time;

    float calculateDistance() const;
    uint32_t calculateTimeDifference(const uint32_t& now) const;
    std::string getAsString() const;
};

class ObjectsList {
public:
    ObjectsList() = default;
    ObjectsList(const std::string& fileName);
    void readFile(const std::string& fileName);
    void addObject();
    void addObject(const Object& obj);
    void groupAndSort(const SORTSTATE sortChoice);
    void saveFile(const std::string& fileName) const;

private:
    void groupAndSortByDistance();
    void groupAndSortByName();
    void groupAndSortByTime();
    void groupAndSortByType();

    Object getObjectFromString(std::istringstream& str) const;

private:
    SORTSTATE m_CurrentSortState = NOSORT;
    std::string m_FileName;
    std::fstream m_File;
    std::vector<Object> m_Objects;    
    std::map<std::string, std::vector<Object>> m_GroupedObjects;
};