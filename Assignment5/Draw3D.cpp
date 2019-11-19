#include <windows.h>
#include "Sphere.h"
#include <bits/stdc++.h>
#define x_offset -250
#define y_offset -300
using namespace std;
//
//void setShapeData()
//{
//  setSphereData(0, 0, 0, 180.0);
////    setSphereData(100, 0, 0, 50);	
////	setSphereData(0, 0, 100, 50);
////		setSphereData(0, 100, 0, 50);	
//	setSphereData(-140, 140, 200, 50.0);
//}
void setShapeData(char inp[])
{

	stringstream ss(inp);
	double x, y, z, radius;
	while (ss >> x >> y >> z >> radius){
		setSphereData(x + x_offset, y + y_offset, z, radius);
	}

}

