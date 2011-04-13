#include <iostream>
#include <cstdlib>
#include <cmath>
using namespace std;
const double pi = 3.141592653589793238;

void sphere(double r)
{
  cout << "Sphere"<< endl;
  
  double tf=0;
  double to=0;
  double x, y, z;
  int num=0;
  for(tf=0; tf<2*pi; tf+=2*pi/8)
  {
    for(to=0; to<=pi; to+=pi/8)
    {
      num++;
      x=r*sin(to)*cos(tf);
      y=r*sin(to)*sin(tf);
      z=r*cos(to);
      cout << num<<": ("<<x<<",\t"<<y<<",\t"<<z<<")" << endl;
    }
  }
  
  cout << "Sphere End" << endl;
}

// segs1 - количество сегментов в торе
// segs2 - количество подсегментов в каждом сегменте тора
void torus(double R1, double R2, int segs1, int segs2)
{
  double x, y, z;
  double a1, a2;
  int num=0;
  int vnum=0;
  cout << "// Torus begin"<< endl;
  cout << "bigtorus_vertexes = new_array(8000)"<< endl;  
  for(int i=0; i<segs1; i++)
  {
    for(int j=0; j<segs2; j++)
    {
      num++;
      a1=i*(2*pi/segs1);
      a2=j*(2*pi/segs2);
      //x = (R1 + R2 * cos(a2)) * cos(a1);
      //y = (R1 + R2 * cos(a2)) * sin(a1);
      //z = R2 * sin(a2);

      x = (R1 + R2 * cos(a2)) * cos(a1);
      y = (R1 + R2 * cos(a2)) * sin(a1);
      z = R2 * sin(a2);

      //cout << num<<": ("<<x<<",\t"<<y<<",\t"<<z<<")" << endl;
      cout << "vertexes["<<(vnum)<<"]="<<(int)(x*256+0.5)<<"  \t// x"<<endl;
      cout << "bigtorus_vertexes["<<(vnum)<<"+1]="<<(int)(y*256+0.5)<< "\t// y" <<endl;
      cout << "vertexes["<<(vnum)<<"+2]="<<(int)(z*256+0.5)<< "\t// z" <<endl;
      cout << endl;
      vnum+=3;
    }
  }
  
  cout << "// Torus End" << endl;
}


void torus_knot(double p, double q)
{
  double x, y, z;
  cout << "// torus_knot begin"<< endl;
  cout << "knot_vertexes = new_array(8000)"<< endl;  
  int vnum=0;
  for(double fi=0; fi<=6*pi; fi+=2*pi/200)
  {
      x = ( 2+cos( (q * fi) / p) ) * cos(fi);
      y = ( 2+cos( (q * fi) / p) ) * sin(fi);
      z = sin( (q * fi) / p );

      //cout << num<<": ("<<x<<",\t"<<y<<",\t"<<z<<")" << endl;
      cout << "knot_vertexes["<<(vnum)<<"]="<<(int)((x*256+0.5)*32)<<"  \t// x"<<endl;
      cout << "knot_vertexes["<<(vnum)<<"+1]="<<(int)((y*256+0.5)*32)<< "\t// y" <<endl;
      cout << "knot_vertexes["<<(vnum)<<"+2]="<<(int)((z*256+0.5)*32)<< "\t// z" <<endl;
      cout << endl;
      vnum+=3;
  }
  
  cout << "// torus_knot End" << endl;
}

int main(int argc, char *argv[])
{
  //sphere(60);
  //torus(60, 20, 12, 12);
  torus_knot(7,4);
//  system("pause");
  return EXIT_SUCCESS;
}
