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
#include <limits>

#ifdef __APPLE__

#include <GLUT/glut.h>
#define GL_SILENCE_DEPRECATION

#else

#include <windows.h>
#include <GL/glut.h>

#endif

#define epsilon 0.0000001
#define Z_NEAR_DISTANCE 1
#define Z_FAR_DISTANCE 1000

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
    Point3D operator * (const T constant) const
    {
        return Point3D(x * constant, y * constant, z * constant);
    }

    bool operator == (const Point3D& rhs) const
    {
        return (x == rhs.x && y == rhs.y && z == rhs.z);
    }

    template<typename T>
    friend Point3D operator * (const T constant, const Point3D &rhs);

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
inline Point3D operator * (const T constant, const Point3D &rhs) {
    return rhs * constant;
}

double vector_dot_product(const Point3D &a, const Point3D &b)
{
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

Point3D vector_cross_product(const Point3D &a, const Point3D &b)
{
    return Point3D(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

double distance_between_points(const Point3D &a, const Point3D &b)
{
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
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

    Light(const Point3D &source)
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

    virtual ~Light()
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

    Ray(const Point3D &start, const Point3D &direction)
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

    virtual ~Ray()
    {
        this->start = this->direction = Point3D();
    }
};

class Object{

public:
    Point3D reference_point;
    vector<Point3D> triangle_end_points;
    vector<double> gen_obj_coefficients;

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

    Point3D get_reflection_vector(Point3D const &incident_vector, Point3D const &normal)
    {
        //r = a - 2 * (a . n) * n.   here, a = incident ray, n = normal, r = reflected ray
        Point3D reflection = incident_vector - 2 * vector_dot_product(incident_vector, normal) * normal;
        reflection.normalize_point();
        
        return reflection;
    }
    
    virtual void draw(){}
    
    virtual Point3D get_normal_vector(const Point3D &intersection_point)
    {
        return Point3D();
    }
    
    
    virtual double get_intersection_point_t_value(const Ray &ray)
    {
        return -1.0;
    }

    virtual double intersect(const Ray &ray, vector<double> &changed_color, int level)
    {
        return -1.0;
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

vector<Object*> objects;
vector<Light> lights;
int level_of_recursion;

void coloring_illumination_reflection(Object *object, const Ray &ray, double t, vector<double> &changed_color, int level)
{
    //intersection point equation --> (ro + t * rd)
    Point3D intersection_point = ray.start + t * ray.direction;
    Point3D normal = object->get_normal_vector(intersection_point);
    Point3D reflection = object->get_reflection_vector(ray.direction, normal);
    
    /* ********************************* ILLUMINATION START ********************************* */
    
    // set ambient color
    for(int i = 0; i < 3; i++)
    {
        changed_color[i] = object->color[i] * object->reflection_coefficients[0];
    }
    
    for(int i = 0; i < lights.size(); i++)
    {
        /* Construct L ray like in the picture. direction = (lightSource - intersectionPoint) then normalize it */
        Point3D light_ray_direction = lights[i].source_light_position - intersection_point;
        light_ray_direction.normalize_point();
        
        Point3D light_ray_start = intersection_point +  0.001 * light_ray_direction;// 0.001 is for taking slightly above the point so it doesn’t again intersect with same object due to precision
        
        Ray light_ray(light_ray_start, light_ray_direction);
        
        // For each object now check whether this L ray obscured by any object or not.
        bool is_obscured = false;
        double dist_from_light_to_intersection = distance_between_points(lights[i].source_light_position, intersection_point);
        
        for(int j = 0; j < objects.size(); j++)
        {
            double t_value = objects[j]->get_intersection_point_t_value(light_ray);
            
            if(t_value > 0.0 && t_value <= dist_from_light_to_intersection)
            {
                is_obscured = true;
                break;
            }
        }
        
        // If it is not obscured that means light falls onto the intersection point, I have to update current_color
        if(!is_obscured)
        {
            // calculate lambert diffuse value
            double L_dot_N = vector_dot_product(light_ray.direction, normal); //(L_dot_N)=> L = light source incident ray
            L_dot_N = max(0.0, L_dot_N); //when theta is negative
            double phong_diffuse = object->reflection_coefficients[1] * L_dot_N;
            
            // calculate phong specular value
            double R_dot_V = vector_dot_product(reflection, ray.direction); //(R_dot_V)=>V = eye ray direction
            R_dot_V = max(0.0, R_dot_V);
            double phong_specular = object->reflection_coefficients[2] * pow(R_dot_V, object->shininess); // I_spec = K_spec * (R . V)^shininess
            
            //set diffuse and specular color. Formula from schaums's outline book
            for(int j = 0; j < 3; j++)
            {
                changed_color[j] += lights[i].color[j] * (phong_diffuse + phong_specular) * object->color[j];
            }
        }
    }
    /* ********************************* ILLUMINATION END ********************************* */
    
    /* ********************************* REFLECTION START ********************************* */
    
    if(level < level_of_recursion)
    {
        Point3D reflection_ray_start = intersection_point + 0.001 * reflection; //slight up to avoid own intersection
        
        Ray reflection_ray(reflection_ray_start, reflection);
        
        // Like capture method, find the nearest intersecting object, using intersect function
        int nearest_reflection = -1;
        double t_reflection;
        double t_min_reflection = numeric_limits<double>::max();
        vector<double> reflection_color(3);
        
        for(int k = 0; k < objects.size(); k++)
        {
            t_reflection = objects[k]->intersect(reflection_ray, reflection_color, 0);
            
            if(t_reflection < t_min_reflection && t_reflection > 0)
            {
                t_min_reflection = t_reflection;
                nearest_reflection = k;
            }
        }
        
        if(nearest_reflection != -1)
        {
            t_min_reflection = objects[nearest_reflection]->intersect(reflection_ray, reflection_color, level + 1);
            
            for(int k = 0; k < 3; k++)
            {
                changed_color[k] += reflection_color[k] * object->reflection_coefficients[3];
            }
        }
        reflection_color.clear();
    }
    
    /* ********************************* REFLECTION END ********************************* */
}

class Sphere : public Object{

public:
    Sphere(const Point3D &center, double radius)
    {
        this->reference_point = center;
        this->height = this->width = this->length = radius;
    }

    void draw() override
    {
        glPushMatrix();
        glTranslatef(reference_point.x, reference_point.y, reference_point.z);
        glColor3f(color[0], color[1], color[2]);
        glutSolidSphere(height, 200, 200);
        glPopMatrix();
    }
    
    Point3D get_normal_vector(const Point3D &intersection_point) override
    {
        Point3D normal = intersection_point - reference_point;
        normal.normalize_point();

        return normal;
    }
    
    double get_intersection_point_t_value(const Ray &ray) override
    {
        //Geometric Ray-Sphere Intersection
        Point3D Ro = ray.start - reference_point; // ro = ro - center(stored in reference point)
        double radius = height;

        double Ro_dot_Ro = vector_dot_product(Ro, Ro);
        double tp = vector_dot_product((-1) * Ro, ray.direction);
        double d_square = vector_dot_product(Ro, Ro) - tp * tp;
        double r_square = radius * radius;

        if(tp <= 0 || d_square > r_square) return -1.0; //tp < 0 ---> object is beside the eye.  d^2 > r^2 --->ray is going away from circle

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
        
        /*                        Normal Procedure
        double a = 1.0;
        double b = 2 * vector_dot_product(ray.direction, ray.start - reference_point);
        double c = vector_dot_product(ray.start - reference_point, ray.start - reference_point) - height * height;
        
        double d = (b * b) - (4 * a * c);
        
        if(d < 0) return -1.0;
        
        d = sqrt(d);
        double t1 = (- b - d) / (2 * a);
        double t2 = (- b + d) / (2 * a);
        
        if(t1 > 0.0 && t2 > 0.0) return min(t1, t2);
        else if(t1 < 0.0 && t2 > 0.0) return t2;
        else return -1.0;   */
        
        return t;
    }

    double intersect(const Ray &ray, vector<double> &changed_color, int level) override
    {
        double t = get_intersection_point_t_value(ray);
        
        if(t <= 0) return -1.0;

        //between near and far plane check
        if(t < Z_NEAR_DISTANCE || t > Z_FAR_DISTANCE) return -1.0;

        if(level == 0) return t; //When level is 0, the purpose of the method is to determine the nearest object only.

        coloring_illumination_reflection(this, ray, t, changed_color, level);
        
        return t;
    }

    void print_object() override
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
    Triangle(const Point3D &a, const Point3D &b, const Point3D &c)
    {
        triangle_end_points.resize(3);
        triangle_end_points[0] = a;
        triangle_end_points[1] = b;
        triangle_end_points[2] = c;
    }
    
    Point3D get_normal_vector(const Point3D &intersection_point) override
    {
        Point3D edge1 = triangle_end_points[1] - triangle_end_points[0];
        Point3D edge2 = triangle_end_points[2] - triangle_end_points[0];
        Point3D normal = vector_cross_product(edge1, edge2);
        normal.normalize_point();
        
        return normal;
    }
    
    double get_intersection_point_t_value(const Ray &ray) override
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
    
    double intersect(const Ray &ray, vector<double> &changed_color, int level) override
    {
        double t = get_intersection_point_t_value(ray);
        if(t <= 0 ) return -1.0;
        
        // between near and far plane check
        if(t < Z_NEAR_DISTANCE || t > Z_FAR_DISTANCE) return -1.0;
        
        if(level == 0) return t; //When level is 0, the purpose of the method is to determine the nearest object only.
        
        coloring_illumination_reflection(this, ray, t, changed_color, level);
        
        return t;
    }
    
    void draw() override
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

    void print_object() override
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
        gen_obj_coefficients.resize(10);
    }
    
    void draw() override
    {
        
    }
    
    Point3D get_normal_vector(const Point3D &intersection_point) override
    {
        //del F / del x = 2Ax + Dy + Ez + G
        double normal_x = 2 * gen_obj_coefficients[0] * intersection_point.x + gen_obj_coefficients[3] * intersection_point.y + gen_obj_coefficients[4] * intersection_point.z + gen_obj_coefficients[6];
        
        //del F / del y = 2By + Dx + Fz + H
        double normal_y = 2 * gen_obj_coefficients[1] * intersection_point.y + gen_obj_coefficients[3] * intersection_point.x + gen_obj_coefficients[5] * intersection_point.z + gen_obj_coefficients[7];
        
        //del F / del z = 2Cz + Ex + Fy + I
        double normal_z = 2 * gen_obj_coefficients[2] * intersection_point.z + gen_obj_coefficients[4] * intersection_point.x + gen_obj_coefficients[5] * intersection_point.y + gen_obj_coefficients[8];
        
        Point3D normal(normal_x, normal_y, normal_z);
        normal.normalize_point();
        
        return normal;
    }
    
    bool is_within_cube(const Point3D &intersection_point)
    {
        bool is_within = true;
        if(length != 0)
        {
            if(intersection_point.x < reference_point.x || intersection_point.x > reference_point.x + length)
            {
                is_within = false;
            }
        }
        
        if(width != 0)
        {
            if(intersection_point.y < reference_point.y || intersection_point.y > reference_point.y + width)
            {
                is_within = false;
            }
        }
        
        if(height != 0)
        {
            if(intersection_point.z < reference_point.z || intersection_point.z > reference_point.z + height)
            {
                is_within = false;
            }
        }
        return is_within;
    }
    
    double get_intersection_point_t_value(const Ray &ray) override
    {
        double a = gen_obj_coefficients[0] * ray.direction.x * ray.direction.x + gen_obj_coefficients[1] * ray.direction.y * ray.direction.y + gen_obj_coefficients[2] * ray.direction.z * ray.direction.z + gen_obj_coefficients[3] * ray.direction.x * ray.direction.y + gen_obj_coefficients[4] * ray.direction.x * ray.direction.z + gen_obj_coefficients[5] * ray.direction.y * ray.direction.z ;
        
        double b = 2 * gen_obj_coefficients[0] * ray.start.x * ray.direction.x + 2 * gen_obj_coefficients[1] * ray.start.y * ray.direction.y + 2 * gen_obj_coefficients[2] * ray.start.z * ray.direction.z + gen_obj_coefficients[3] * ray.start.x * ray.direction.y + gen_obj_coefficients[3] * ray.start.y * ray.direction.x + gen_obj_coefficients[4] * ray.start.x * ray.direction.z + gen_obj_coefficients[4] * ray.start.z * ray.direction.x + gen_obj_coefficients[5] * ray.start.y * ray.direction.z + gen_obj_coefficients[5] * ray.start.z * ray.direction.y + gen_obj_coefficients[6] * ray.direction.x + gen_obj_coefficients[7] * ray.direction.y + gen_obj_coefficients[8] * ray.direction.z;
        
        double c = gen_obj_coefficients[0] * ray.start.x * ray.start.x + gen_obj_coefficients[1] * ray.start.y * ray.start.y + gen_obj_coefficients[2] * ray.start.z * ray.start.z + gen_obj_coefficients[3] * ray.start.x * ray.start.y + gen_obj_coefficients[4] * ray.start.x * ray.start.z + gen_obj_coefficients[5] * ray.start.y * ray.start.z + gen_obj_coefficients[6] * ray.start.x + gen_obj_coefficients[7] * ray.start.y + gen_obj_coefficients[8] * ray.start.z + gen_obj_coefficients[9];
        
        //double t = -1.0;
        
        double D = b * b - 4 * a * c;
        if(D < 0) return -1.0;
        
        double t_min = (-b - sqrt(D)) / (2 * a);
        double t_max = (-b + sqrt(D)) / (2 * a);
        
        Point3D intersection_point_1 = ray.start + t_min * ray.direction;
        Point3D intersection_point_2 = ray.start + t_max * ray.direction;
        
        if(is_within_cube(intersection_point_1))
        {
            return t_min;
        }
        else if(is_within_cube(intersection_point_2))
        {
            return t_max;
        }
        else return -1.0;
    }
    
    double intersect(const Ray &ray, vector<double> &changed_color, int level) override
    {
        double t = get_intersection_point_t_value(ray);

        if(t <= 0 ) return -1.0;

        // between near and far plane check
        if(t < Z_NEAR_DISTANCE || t > Z_FAR_DISTANCE) return -1.0;

        if(level == 0) return t; //When level is 0, the purpose of the method is to determine the nearest object only.

        coloring_illumination_reflection(this, ray, t, changed_color, level);
        
        return t;
    }

    void print_object() override
    {
        cout << "General Object Info:" << endl;
        
        cout << "General Object 10 CoEfficients: " << endl;
        for(int i = 0; i < 10; i++)
        {
            cout << char(65 + i) << ": " << gen_obj_coefficients[i] << " ";
        }
        cout << endl;
        
        cout << "General Object Equation: ";
        string eq = "";
        vector<string> vec = {"x^2", "y^2", "z^2", "xy", "zx", "yz", "x", "y", "z", " = 0"};
        for(int i = 0; i < 9; i++)
        {
            eq += to_string(gen_obj_coefficients[i]);
            eq += vec[i];
            if(gen_obj_coefficients[i+1] >= 0) eq += "+";
            else eq += "";
            if(i == 8)
            {
                eq += to_string(gen_obj_coefficients[i+1]);
                eq += vec[i+1];
            }
        }
        cout << eq <<endl;

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
        gen_obj_coefficients.clear();
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
    
    Point3D get_normal_vector(const Point3D &intersection_point) override
    {
        return Point3D(0.0, 0.0, 1.0); //In XY plane normal is Z axis
    }
    
    bool is_within_boundary(const Point3D &point)
    {
        if(point.x < reference_point.x || point.x > -reference_point.x || point.y < reference_point.y || point.y > -reference_point.y)
        {
            return false;
        }
        else return true;
    }
    
    double get_intersection_point_t_value(const Ray &ray) override
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
    
    double intersect(const Ray &ray, vector<double> &changed_color, int level) override
    {
        double t = get_intersection_point_t_value(ray);
        
        Point3D intersecting_vector = ray.start + t * ray.direction;
        
        if(!is_within_boundary(intersecting_vector)) return -1.0;
        
        // between near and far plane check
        if(t < Z_NEAR_DISTANCE || t > Z_FAR_DISTANCE) return -1.0;

        if(level == 0) return t; //When level is 0, the purpose of the  method is to determine the nearest object only.
        
        int tile_pixel_x = intersecting_vector.x - reference_point.x;
        int tile_pixel_y = intersecting_vector.y - reference_point.y;
        
        int tile_x_index = tile_pixel_x / length;
        int tile_y_index = tile_pixel_y / length;
        
        for (int i = 0; i < 3; i++)
        {
            color[i] = (tile_x_index + tile_y_index + 1) % 2;
        }
        
        coloring_illumination_reflection(this, ray, t, changed_color, level);
        
        return t;
    }

    void draw() override
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
    
    void print_object() override
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


