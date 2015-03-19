#ifndef CSV_H
#define CSV_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

class CData
{
private:
    long start, end;
    string state, city;
    double latitude, longitude;
public:
    CData() {start = end = latitude = longitude =0; state=city="";}
    void Load(ifstream &f);
    void Print();
    string &GetState() { return state; }
    string &GetCity() { return city; }
    long GetStart() { return start; }
    long GetEnd() { return end; }
    double GetLatitude() { return latitude; }
    double GetLongitude() { return longitude; }
};

void CData::Load(ifstream &f)
{
    char ch;
    f >> start >> ch >> end >> ch >> latitude >> ch >> longitude;
}

void CData::Print()
{
    
    cout << start << "\t" << end << "\t" <<latitude << "\t" <<longitude << endl;
}

#endif
