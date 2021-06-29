//
//  main.cpp
//  Ray Tracing
//
//  Created by Rishov Paul on 26/6/21.
//
#include <limits>
#include "1605084_classes.h"
#include "bitmap_image.hpp"

#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 600

#define FOVY 80
#define ASPECT_RATIO 1
#define Z_NEAR_DISTANCE 1
#define Z_FAR_DISTANCE 1000

#define pi (2*acos(0.0))
#define MOVE_CONSTANT 2.0
#define ROTATION_CONSTANT 0.25

/* ****************** global variables ******************** */

//u -- up vector
//r -- right vector
//l - look vector
Point3D eye_pos, up, rght, look;

double cameraHeight;
double cameraAngle;
int drawaxes;
double angle;


//global variables
int level_of_recursion;
int image_height, image_width;
int num_of_objects;
int num_of_light_sources;

vector<Object*> objects;
vector<Light> lights;


using namespace std;

/* **************** OPENGL Functions ****************** */
void drawAxes()
{
    if(drawaxes == 1)
    {
        glBegin(GL_LINES);
        {
            //X - axis -- RED
            glColor3f(1.0, 0, 0);
            glVertex3f(150, 0, 0);
            glVertex3f(-150, 0, 0);

            //Y - axis --GREEN
            glColor3f(0, 1, 0);
            glVertex3f(0, -150, 0);
            glVertex3f(0, 150, 0);

            //Z - axis -- BLUE
            glColor3f(0, 0, 1);
            glVertex3f(0, 0, 150);
            glVertex3f(0, 0, -150);
        }
        glEnd();
    }
}

void look_left(double angle)
{
    //up fixed
    Point3D temp = look ;
    look = temp * cos((double)angle) + vector_cross_product(up, temp) * sin((double)angle);

    temp = rght;
    rght = temp * cos((double)angle) + vector_cross_product(up, temp) * sin((double)angle);
}

void look_right(double angle)
{
    angle = (-1) * angle;
    look_left(angle);
}

void look_up(double angle)
{
    //right fixed
    Point3D temp = up ;
    up = temp * cos((double)angle) + vector_cross_product(rght, temp) * sin((double)angle);

    temp = look ;
    look = temp * cos((double)angle) + vector_cross_product(rght, temp) * sin((double)angle);
}

void look_down(double angle)
{
    angle = (-1)*angle;
    look_up(angle);
}

void tilt_clockwise(double angle)
{
    //look fixed
    angle = (-1)*angle;
    Point3D temp = up ;
    up = temp * cos((double)angle) + vector_cross_product(look, temp) * sin((double)angle);

    temp = rght ;
    rght = temp * cos((double)angle) + vector_cross_product(look, temp) * sin((double)angle);
}

void tilt_anticlockwise(double angle)
{
    angle = (-1)*angle;
    tilt_clockwise(angle);
}

double degreeToRadianAngle(double degree)
{
    return pi / 180 * degree;
}

void capture()
{
    //initialize bitmap image and set background color to black
    bitmap_image image(image_width, image_height); //col x row
    for(int i = 0; i < image_height; i++)
    {
        for(int j = 0; j < image_width; j++)
        {
            image.set_pixel(j, i, 0, 0, 0);
        }
    }

    double plane_distance = (WINDOW_HEIGHT / 2.0) / tan(degreeToRadianAngle(FOVY / 2.0));
    Point3D top_left = eye_pos + look * plane_distance - rght * (WINDOW_HEIGHT / 2.0) + up * (WINDOW_HEIGHT / 2.0);
    double du = (double) WINDOW_WIDTH / image_width;
    double dv = (double) WINDOW_HEIGHT / image_height;

    // Choose middle of the grid cell
    top_left = top_left + rght * (0.5 * du) - up * (0.5 * dv);

    for(int i = 0; i < image_height; i++)
    {
        for(int j = 0; j < image_width; j++)
        {
            int nearest = -1; //stores the nearest object index
            double t;
            double t_min = numeric_limits<double>::max();
            vector<double> dummy_color(3);
            
            Point3D current_pixel = top_left + rght * (j * du) - up * (i * dv);
            
            //cast ray from eye to (curPixel-eye) direction
            Ray ray(eye_pos, current_pixel - eye_pos);
            
            for(int k = 0; k < objects.size(); k++)
            {
                t = objects[k]->intersect(ray, dummy_color, 0);
                
                if(t < t_min && t > 0)
                {
                    t_min = t;
                    nearest = k;
                }
            }
            
            if(nearest != -1)
            {
                t_min = objects[nearest]->intersect(ray, dummy_color, 1);
            }
            
            //update image pixel (i,j)
            image.set_pixel(j, i, dummy_color[0] * 255, dummy_color[1] * 255, dummy_color[2] * 255);
        }
    }
    image.save_image("1605084_ray_tracing.bmp");
    image.clear();
}

void keyboardListener(unsigned char key, int x,int y){
    switch(key){
        case '0':
            capture();
            break;
        case '1':
            look_left(pi / 18 * ROTATION_CONSTANT);
            break;
        case '2':
            look_right(pi / 18 * ROTATION_CONSTANT);
            break;
        case '3':
            look_up(pi / 18 * ROTATION_CONSTANT);
            break;
        case '4':
            look_down(pi / 18 * ROTATION_CONSTANT);
            break;
        case '5':
            tilt_clockwise(pi / 18 * ROTATION_CONSTANT);
            break;
        case '6':
            tilt_anticlockwise(pi / 18 * ROTATION_CONSTANT);
            break;
        default:
            break;
    }
}

void specialKeyListener(int key, int x, int y){
    switch(key){
        case GLUT_KEY_DOWN:        //down arrow key
            cameraHeight -= 3.0;
            eye_pos = eye_pos - look * MOVE_CONSTANT;
            break;
            
        case GLUT_KEY_UP:        // up arrow key
            cameraHeight += 3.0;
            eye_pos = eye_pos + look * MOVE_CONSTANT;
            break;

        case GLUT_KEY_RIGHT:
            eye_pos = eye_pos + rght * MOVE_CONSTANT;
            break;
            
        case GLUT_KEY_LEFT:
            eye_pos = eye_pos - rght * MOVE_CONSTANT;
            break;

        case GLUT_KEY_PAGE_UP:
            eye_pos = eye_pos + up * MOVE_CONSTANT;
            break;
            
        case GLUT_KEY_PAGE_DOWN:
            eye_pos = eye_pos - up * MOVE_CONSTANT;
            break;
        default:
            break;
    }
}

void mouseListener(int button, int state, int x, int y)
{  //x, y is the x-y of the screen(2D)
    switch(button){
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN){        // 2 times?? in ONE click? -- solution is checking DOWN or UP
                drawaxes = 1 - drawaxes;
            }
            break;
            
        case GLUT_RIGHT_BUTTON:
            //........
            break;
            
        case GLUT_MIDDLE_BUTTON:
            //........
            break;
            
        default:
            break;
    }
}

void display(){

    //clear the display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);    //color black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /********************
        / set-up camera here ********************/
    //load the correct matrix -- MODEL-VIEW matrix
    glMatrixMode(GL_MODELVIEW);

    //initialize the matrix
    glLoadIdentity();

    //now give three info
    //1. where is the camera (viewer)?
    //2. where is the camera looking?
    //3. Which direction is the camera's UP direction?

    gluLookAt(eye_pos.x, eye_pos.y, eye_pos.z,     eye_pos.x+look.x, eye_pos.y+look.y, eye_pos.z+look.z,    up.x, up.y, up.z);


    //again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);


        /****************************
        / Add your objects from here
        ****************************/
        //add objects

    drawAxes();
    
    //draw Light Sources
    if(lights.size() == 0) return;
    for(int i = 0; i < lights.size(); i++)
    {
        lights[i].draw_light_source();
    }

    //draw Objects
    for(int i = 0; i < objects.size(); i++)
    {
        objects[i]->draw();
    }

    //ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
    glutSwapBuffers();
}

void animate(){
    angle += 0.05;
    //codes for any changes in Models, Camera
    glutPostRedisplay();
}

void init()
{
    //codes for initialization
    drawaxes = 1;
    cameraHeight = 150.0;
    cameraAngle = 1.0;
    angle = 0;

    //initialization of pos, u, r, l vectors
    eye_pos.x = eye_pos.y = 120;
    eye_pos.z = 20;

    up.x = up.y = 0;
    up.z = 1;

    rght.x = (double)-1/sqrt(2);
    rght.y = (double)1/sqrt(2);
    rght.z = 0;

    look.x = (double)-1/sqrt(2);
    look.y = (double)-1/sqrt(2);
    look.z = 0;



    //clear the screen
    glClearColor(0, 0, 0, 0);

    /* ***********************
    / set-up projection here
    ************************/
    
    //load the PROJECTION matrix
    glMatrixMode(GL_PROJECTION);

    //initialize the matrix
    glLoadIdentity();

    //give PERSPECTIVE parameters
    gluPerspective(FOVY, ASPECT_RATIO, Z_NEAR_DISTANCE, Z_FAR_DISTANCE);
    //field of view in the Y (vertically)
    //aspect ratio that determines the field of view in the X direction (horizontally)
    //near distance
    //far distance
}

void load_data()
{
    Object *object = nullptr;
    string str;
    double R, G, B;
    double amb, dif, spec, rec_ref;
    double light_x, light_y, light_z;
    int shininess;

    cin >> level_of_recursion;
    cin >> image_height;
    cin >> num_of_objects;
    
    image_width = image_height;
    
    for(int i = 0; i < num_of_objects; i++)
    {
        cin >> str;
        if(str == "sphere")
        {
            Point3D center;
            double radius;

            cin >> center.x >> center.y >> center.z;
            cin >> radius;

            object = new Sphere(center, radius);
        }
        else if(str == "triangle")
        {
            Point3D a, b, c;

            cin >> a.x >> a.y >> a.z;
            cin >> b.x >> b.y >> b.z;
            cin >> c.x >> c.y >> c.z;

            object = new Triangle(a, b, c);
        }
        else if(str == "general")
        {
            object = new GeneralObject();

            for(int i = 0; i < 10; i++)
            {
                cin >> object->gen_obj_coefficients[i];
            }

            Point3D cube_reference_point;
            cin >> cube_reference_point.x >> cube_reference_point.y >> cube_reference_point.z;
            object->reference_point = cube_reference_point;

            cin >> object->length >> object->width >> object->height;
        }

        //possible refactor by directly inputting to array
        cin >> R >> G >> B;
        cin >> amb >> dif >> spec >> rec_ref;
        cin >> shininess;

        object->set_color(R, G, B);
        object->set_reflection_coefficients(amb, dif, spec, rec_ref);
        object->set_shininess(shininess);

        //object->print_object();

        objects.push_back(object);
    }
    
    cin >> num_of_light_sources;
    
    
    for(int i = 0; i < num_of_light_sources; i++)
    {
        cin >> light_x >> light_y >> light_z;
        cin >> R >> G >> B;
        
        Point3D source = Point3D(light_x, light_y, light_z);
        Light light = Light(source);
        light.set_color(R, G, B);
        
        lights.push_back(light);
    }
    
    //Floor Push at last
    object = new Floor(1000, 20);
    object->set_reflection_coefficients(0.5, 0.3, 0.3, 0.3);
    object->set_shininess(5);
    objects.push_back(object);
    
    //objects[objects.size() - 1]->print_object();
}


int main(int argc, char **argv) {
    /* *************** File Read **********************************/
    
    freopen("scene.txt", "r", stdin);
    load_data();
    
    //project location ---> cd Documents/Academics/4-1/"Computer Graphics Sessional"/Offline3/"Ray Tracing"
    
    /* ***********************************************************/
    glutInit(&argc,argv);
    glutInitWindowSize(WINDOW_WIDTH , WINDOW_HEIGHT);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);  //Depth, Double buffer, RGB color

    glutCreateWindow("My OpenGL Ray Tracing Program");

    init();

    glEnable(GL_DEPTH_TEST);    //enable Depth Testing

    glutDisplayFunc(display);    //display callback function
    glutIdleFunc(animate);        //what you want to do in the idle time (when no drawing is occuring)

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMouseFunc(mouseListener);

    glutMainLoop();        //The main loop of OpenGL

    /* ****************** Free Memory **************************** */
    lights.clear();
    objects.clear();
    
    return 0;
}
