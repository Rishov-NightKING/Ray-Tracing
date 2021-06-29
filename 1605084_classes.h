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
#define GL_SILENCE_DEPRECATION
#else

#include <windows.h>
#include <GL/glut.h>

#endif

#define epsilon 0.0000001

using namespace std;

class Point3D{

public:
    double x, y, z;

    Point3D()
    {
        this->x = this->y = this->z = 0.0;
    }

    Point3D(double x, double y, double z) : x(x), y(y), z(z) {}

    Point3D operator + (const Point3D& rhs) const
    {
        return Point3D(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Point3D operator - (const Point3D& rhs) const
    {
        return Point3D(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    template<typename T>
    Point3D operator * (T constant) const
    {
        return Point3D(x * constant, y * constant, z * constant);
    }

    bool operator == (const Point3D& rhs) const
    {
        return (x == rhs.x && y == rhs.y && z == rhs.z);
    }

    template<typename T>
    friend Point3D operator*(T constant, Point3D const &rhs);

    void normalize_point()
    {
        double value = sqrt(x * x + y * y + z * z);
        x /= value;
        y /= value;
        z /= value;
    }

    void printPoint() const
    {
        cout << "(" << setprecision(5) << fixed << this->x << ", "
             <<setprecision(5) << fixed << this->y << ", "
             << setprecision(5) << fixed << this->z << ")" << endl;
    }

    virtual ~Point3D()
    {
        this->x = this->y = this->z = 0;
    }
};

template<typename T>
inline Point3D operator*(T constant, Point3D const &rhs) {
    return rhs * constant;
}

double vector_dot_product(Point3D const &a, Point3D const &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Point3D vector_cross_product(Point3D const &a, Point3D const &b)
{
    return Point3D(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
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
    Point3D start, direction;
    Ray()
    {
        this->start = this->direction = Point3D(0, 0, 0);
    }

    Ray(Point3D start, Point3D direction)
    {
        this->start = start;
        this->direction = direction; // normalize for easier calculations
        this->direction.normalize_point();
    }

    void print_ray()
    {
        cout << "Start or Origin of the ray: ";
        start.printPoint();
        cout << "Direction of the ray: ";
        direction.printPoint();
    }

    virtual ~Ray() {
        this->start = this->direction = Point3D();
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
    
    virtual Point3D get_normal_vector()
    {
        return Point3D(); //origin
    }
    
    virtual double get_intersection_point_t_value(Ray ray)
    {
        return -1;
    }

    virtual double intersect(Ray ray, vector<double> &changed_color, int level)
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
    
    double get_intersection_point_t_value(Ray ray)
    {
        //Geometric Ray-Sphere Intersection
        Point3D Ro = Point3D(ray.start - reference_point); // ro = ro - center(stored in reference point)
        double radius = height;

        double Ro_dot_Ro = vector_dot_product(Ro, Ro);
        double tp = vector_dot_product((-1) * Ro, ray.direction);
        double d_square = vector_dot_product(Ro, Ro) - tp * tp;
        double r_square = radius * radius;
        

        if(tp < 0 || d_square > r_square) return -1.0; //tp < 0 ---> object is beside the eye.  d^2 > r^2 --->ray is going away from circle

        double t_prime = sqrt(r_square - d_square); // d^2 <= r^2
        double t = -1.0;

        if(Ro_dot_Ro < r_square) //ray origin(eye) inside sphere
        {
            t = tp + t_prime;
        }
        else if(Ro_dot_Ro >= r_square)//ray origin(eye) outside or on sphere
        {
            t = tp - t_prime;
        }
        
        return t;
    }

    double intersect(Ray ray, vector<double> &changed_color, int level)
    {
        double t = get_intersection_point_t_value(ray);

        if(t <= 0 ) return -1.0;

        //near and far plane check needed??

        if(level == 0) return t;

        for (int i = 0; i < 3; i++)
        {
            changed_color[i] = color[i] * reflection_coefficients[0];
        }
        return t;
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
    
    double get_intersection_point_t_value(Ray ray)
    {
        //Moller–Trumbore ray-triangle intersection algorithm
        Point3D edge1 = triangle_end_points[1] - triangle_end_points[0];
        Point3D edge2 = triangle_end_points[2] - triangle_end_points[0];
        Point3D h = vector_cross_product(ray.direction, edge2);
        double a = vector_dot_product(edge1, h);
        
        if(a > -epsilon && a < epsilon) return -1.0;// This ray is parallel to this triangle.
        
        double f = 1.0 / a;
        Point3D s = ray.start - triangle_end_points[0];
        double u = f * vector_dot_product(s, h);
        
        if(u < 0.0 || u > 1.0) return -1.0;
        
        Point3D q = vector_cross_product(s, edge1);
        double v = f * vector_dot_product(ray.direction, q);
        
        if(v < 0.0 || u + v > 1.0) return -1.0;
        
        /* At this stage we can compute t to find out where the intersection point is on the line. */
        double t = f * vector_dot_product(edge2, q);
        
        if(t > epsilon) return t;
        else return -1.0;
    }
    
    double intersect(Ray ray, vector<double> &changed_color, int level)
    {
        double t = get_intersection_point_t_value(ray);
        if(t <= 0 ) return -1.0;
        
        //near and far plane check needed??
        
        if(level == 0) return t;
        
        for (int i = 0; i < 3; i++)
        {
            changed_color[i] = color[i] * reflection_coefficients[0];
        }
        return t;
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
    
//    double intersect(Ray ray, vector<double> &changed_color, int level)
//    {
//        double t = get_intersection_point_t_value(ray);
//
//        if(t <= 0 ) return -1.0;
//
//        //near and far plane check needed??
//
//        if(level == 0) return t;
//
//        for (int i = 0; i < 3; i++)
//        {
//            changed_color[i] = color[i] * reflection_coefficients[0];
//        }
//        return t;
//    }

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
    
    Point3D get_normal_vector()
    {
        return Point3D(0, 0, 1); //In XY plane normal is Z axis
    }
    
    bool is_within_boundary(Point3D const &point)
    {
        if(point.x < reference_point.x || point.x > -reference_point.x || point.y < reference_point.y || point.y > -reference_point.y)
        {
            return false;
        }
        else return true;
    }
    
    double get_intersection_point_t_value(Ray ray)
    {
        /*
         ray : P(t) = Ro + t * Rd
         plane: H(P) = n·P + D = 0   here n = normal
         n·(Ro + t * Rd) + D = 0
         t = -(D + n·Ro) / n·Rd
         
         for floor: D = 0 and t = - ray.start.z / ray.direction.z
         */
        double t = -1.0;
        if(ray.direction.z != 0)//denom check
        {
            t = (double) -(ray.start.z / ray.direction.z);
        }
    
        return t;
    }
    
    double intersect(Ray ray, vector<double> &changed_color, int level)
    {
        double t = get_intersection_point_t_value(ray);
        
        Point3D intersecting_vector = ray.start + t * ray.direction;
        
        if(!is_within_boundary(intersecting_vector)) return -1.0;
        
        //near and far plane check needed??

        if(level == 0) return t;
        
        int tile_pixel_x = intersecting_vector.x - reference_point.x;
        int tile_pixel_y = intersecting_vector.y - reference_point.y;
        
        int tile_x_index = tile_pixel_x / length;
        int tile_y_index = tile_pixel_y / length;
        
        for (int i = 0; i < 3; i++)
        {
            changed_color[i] = (tile_x_index + tile_y_index + 1) % 2;
        }
        return t;
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
                    int c = (i + j + 1) % 2;
                    glColor3f(c, c, c); //odd(c = 1) - white tile  even(c = 0) - black tile
                    
                    glVertex3f(reference_point.x + i * length, reference_point.y + j * length, reference_point.z);
                    glVertex3f(reference_point.x + (i + 1) * length, reference_point.y + j * length, reference_point.z);
                    glVertex3f(reference_point.x + (i + 1) * length, reference_point.y + (j + 1) * length, reference_point.z);
                    glVertex3f(reference_point.x + i * length, reference_point.y + (j + 1) * length, reference_point.z);
                }
            }
        }
        glEnd();
    }
    
    void print_object()
    {
        cout << "Floor Info:" << endl;

        cout << "Reference Point: ";
        reference_point.printPoint();

        cout << "Floor Width: " << width << "   Tile Width: " << length << endl;
        

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

    ~Floor()
    {

    }
};

#endif //RAYTRACING_1605084_CLASSES_H


