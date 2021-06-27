//
// Created by Rishov Paul on 26/6/21.
//

#ifndef RAYTRACING_1605084_CLASSES_H
#define RAYTRACING_1605084_CLASSES_H

#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdio>
#include <vector>

#ifdef __APPLE__

#include <GLUT/glut.h>

#else

#include <windows.h>
#include <GL/glut.h>

#endif



using namespace std;

class Point3D{

public:
    double x, y, z;

    Point3D()
    {
        this->x = this->y = this->z = 0.0;
    }

    Point3D(double x, double y, double z) : x(x), y(y), z(z) {}

    Point3D operator + (const Point3D& p) const
    {
        return Point3D(x + p.x, y + p.y, z + p.z);
    }

    Point3D operator - (const Point3D& p) const
    {
        return Point3D(x - p.x, y - p.y, z - p.z);
    }

    template<typename T>
    Point3D operator * (T value) const
    {
        return Point3D(x * value, y * value, z * value);
    }

    bool operator == (const Point3D& p) const
    {
        return (x == p.x && y == p.y && z == p.z);
    }

    void normalize_point()
    {
        double value = sqrt(x * x + y * y + z * z);
        x /= value;
        y /= value;
        z /= value;
    }

    void printPoint() const
    {
        cout << "(" << setprecision(7) << fixed << this->x << ", "
             <<setprecision(7) << fixed << this->y << ", "
             << setprecision(7) << fixed << this->z << ")" << endl;
    }

    virtual ~Point3D()
    {
        this->x = this->y = this->z = 0;
    }
};

double vector_dot_product(Point3D a, Point3D b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Point3D vector_cross_product(Point3D a, Point3D b)
{
    Point3D temp;
    temp.x = a.y * b.z - a.z * b.y;
    temp.y = a.z * b.x - a.x * b.z;
    temp.z = a.x * b.y - a.y * b.x;
    return temp;
}

class Light{

public:
    Point3D source_light_position;
    vector<double> color;

    Light()
    {
        source_light_position = Point3D();
        color.resize(3);
    }

    Light(Point3D source)
    {
        source_light_position = source;
        color.resize(3);
    }

    void set_color(double r, double g, double b)
    {
        this->color[0] = r;
        this->color[1] = g;
        this->color[2] = b;
    }
    
    void draw_light_source()
    {
        glPushMatrix();
        glTranslatef(source_light_position.x, source_light_position.y, source_light_position.z);
        glColor3f(color[0], color[1], color[2]);
        glutSolidSphere(2, 100, 100);
        glPopMatrix();
    }

    void print_light_info()
    {
        cout << "position of the point light source: ";
        source_light_position.printPoint();
        cout << "RGB color value:  R: " << color[0] << "   G: " << color[1] << "   B: " << color[2] << endl;
    }

    ~Light()
    {
        source_light_position = Point3D();
        color.clear();
    }
};

class Ray{

public:
    Point3D start, dir;
    Ray()
    {
        this->start = this->dir = Point3D(0, 0, 0);
    }

    Ray(Point3D start, Point3D dir)
    {
        this->start = start;
        this->dir = dir; // normalize for easier calculations
        this->dir.normalize_point();
    }

    void print_ray()
    {
        cout << "Start: ";
        start.printPoint();
        cout << "Dir: ";
        dir.printPoint();
    }

    virtual ~Ray() {
        this->start = this->dir = Point3D();
    }
};

class Object{

public:
    Point3D reference_point;
    vector<Point3D> triangle_end_points;
    vector<double> gen_obj_coefficients_array;

    double height, width, length;
    vector<double> color;
    vector<double> reflection_coefficients; // reflection coefficients --> 0-ambient, 1-diffuse, 2-specular, 3-recursive reflection;
    int shininess; // exponent term of specular component

    Object()
    {
        color.resize(3);
        reflection_coefficients.resize(4);
    }

    void set_color(double r, double g, double b)
    {
        this->color[0] = r;
        this->color[1] = g;
        this->color[2] = b;
    }

    void set_shininess(int shine)
    {
        this->shininess = shine;
    }

    void set_reflection_coefficients(double amb, double dif, double spec, double rec_ref)
    {
        //0-ambient, 1-diffuse, 2-specular, 3-recursive reflection
        this->reflection_coefficients[0] = amb;
        this->reflection_coefficients[1] = dif;
        this->reflection_coefficients[2] = spec;
        this->reflection_coefficients[3] = rec_ref;
    }

    virtual void draw(){}

    virtual double intersect(Ray &ray, vector<double> current_color, int level)
    {
        return -1;
    }

    virtual void print_object()
    {

    }

    virtual ~Object()
    {
        reference_point = Point3D();
        color.clear();
        reflection_coefficients.clear();
        height = width = length = 0.0;
        shininess = 0;
    }
};

class Sphere : public Object{

public:
    Sphere(Point3D center, double radius)
    {
        this->reference_point = center;
        this->height = this->width = this->length = radius;
    }

    void draw()
    {
        glPushMatrix();
        glTranslatef(reference_point.x, reference_point.y, reference_point.z);
        glColor3f(color[0], color[1], color[2]);
        glutSolidSphere(height, 200, 200);
        glPopMatrix();
    }

    void print_object()
    {
        cout << "Sphere Info: " << endl;
        cout << "Center: ";
        reference_point.printPoint();

        cout << "radius: " << height << endl;

        cout << "color array: ";
        for(int i = 0; i < color.size(); i++)
        {
            cout << color[i] << "   ";
        }

        cout << "\nReflection CoEfficients array: ";
        for(int i = 0; i < reflection_coefficients.size(); i++)
        {
            cout << reflection_coefficients[i] << "   ";
        }
        cout << endl;

        cout << "Shininess: " << shininess << endl << endl;
    }

    ~Sphere()
    {

    }
};

class Triangle : public Object{

public:
    Triangle(Point3D a, Point3D b, Point3D c)
    {
        triangle_end_points.resize(3);
        triangle_end_points[0] = a;
        triangle_end_points[1] = b;
        triangle_end_points[2] = c;
    }
    
    void draw()
    {
        glColor3f(color[0], color[1], color[2]);
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(triangle_end_points[0].x, triangle_end_points[0].y, triangle_end_points[0].z);
            glVertex3f(triangle_end_points[1].x, triangle_end_points[1].y, triangle_end_points[1].z);
            glVertex3f(triangle_end_points[2].x, triangle_end_points[2].y, triangle_end_points[2].z);
        }
        glEnd();
    }

    void print_object()
    {
        cout << "\nTriangle Info" << endl;
        for(int i = 0; i < 3; i++)
        {
            cout << "Endpoint-" << (i + 1) << ":  ";
            triangle_end_points[i].printPoint();
        }

        cout << "color array: ";
        for(int i = 0; i < color.size(); i++)
        {
            cout << color[i] << "   ";
        }

        cout << "\nReflection CoEfficients array: ";
        for(int i = 0; i < reflection_coefficients.size(); i++)
        {
            cout << reflection_coefficients[i] << "   ";
        }
        cout << endl;

        cout << "Shininess: " << shininess << endl << endl;
    }

    ~Triangle()
    {
        triangle_end_points.clear();
    }
};

class GeneralObject : public Object{

public:
    GeneralObject()
    {
        gen_obj_coefficients_array.resize(10);
    }
    
    void draw()
    {
        
    }

    void print_object()
    {
        cout << "General Object Info:" << endl;
        cout << "General Object 10 CoEfficients: " << endl;
        for(int i = 0; i < 10; i++)
        {
            cout << char(65 + i) << ": " << gen_obj_coefficients_array[i] << " ";
        }
        cout << endl;

        cout << "Cube Reference Point: ";
        reference_point.printPoint();

        cout << "Length: " << length << "   Width: " << width << "  Height: " << height << endl;
        //(0 indicates no clipping along this dimension)

        cout << "color array: ";
        for(int i = 0; i < color.size(); i++)
        {
            cout << color[i] << "   ";
        }

        cout << "\nReflection CoEfficients array: ";
        for(int i = 0; i < reflection_coefficients.size(); i++)
        {
            cout << reflection_coefficients[i] << "   ";
        }
        cout << endl;

        cout << "Shininess: " << shininess << endl << endl;
    }

    ~GeneralObject()
    {
        gen_obj_coefficients_array.clear();
    }
};

class Floor : public Object{
    
public:
    Floor(double floor_width, double tile_width)
    {
        this->width = floor_width; // object class width = floor_width
        this->length = tile_width; // object class length = tile_width
        reference_point = Point3D(-floor_width/2, -floor_width/2, 0); //leftmost bottom corner of the XY plane
    }

    void draw()
    {
        int num_of_tiles = this->width / this->length; //In a row or in a column
        
        glBegin(GL_QUADS);
        {
            for(int i = 0; i < num_of_tiles; i++)
            {
                for (int j = 0; j < num_of_tiles; j++)
                {
                    int c = (i + j) % 2;
                    glColor3f(!c, !c, !c); //starts with white from leftmost bottom of the plane
                    
                    glVertex3f(reference_point.x + i * length, reference_point.y + j * length, reference_point.z);
                    glVertex3f(reference_point.x + (i + 1) * length, reference_point.y + j * length, reference_point.z);
                    glVertex3f(reference_point.x + (i + 1) * length, reference_point.y + (j + 1) * length, reference_point.z);
                    glVertex3f(reference_point.x + i * length, reference_point.y + (j + 1) * length, reference_point.z);
                }
            }
        }
        glEnd();
    }

    ~Floor()
    {

    }
};

#endif //RAYTRACING_1605084_CLASSES_H


