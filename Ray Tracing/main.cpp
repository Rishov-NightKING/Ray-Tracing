//
//  main.cpp
//  Ray Tracing
//
//  Created by Rishov Paul on 26/6/21.
//

#include "1605084_classes.h"
#include "bitmap_image.hpp"

#define pi (2*acos(0.0))
#define MOVE_CONSTANT 2.0
#define ROTATION_CONSTANT 0.25

/* ****************** global variables ******************** */

//u -- up vector
//r -- right vector
//l - look vector
Point3D pos, u, r, l;

double cameraHeight;
double cameraAngle;
int drawgrid;
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

void drawSquare(double a, vector<double> c)
{
    glColor3f(c[0], c[1], c[2]);
    glBegin(GL_QUADS);{
        glVertex3f( a, a,2);
        glVertex3f( a,-a,2);
        glVertex3f(-a,-a,2);
        glVertex3f(-a, a,2);
    }glEnd();
}

void drawCircle(double radius,int segments)
{
    int i;
    vector<Point3D> points(100);
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
            glVertex3f(points[i].x,points[i].y,0);
            glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawSphere(double radius, int slices, int stacks)
{
    //Point3D points[100][100];
    vector<vector<Point3D>> points( 100 , vector<Point3D> (100));
    double h, r;
    //generate points
    for(int i = 0; i <= stacks; i++)
    {
        h = radius * sin(((double)i/(double)stacks)*(pi/2));
        r = radius * cos(((double)i/(double)stacks)*(pi/2));
        //r = 2*radius - r;
        for(int j = 0;j <= slices; j++)
        {
            points[i][j].x = r*sin(((double)j/(double)slices)*2*pi);
            points[i][j].y = h;
            points[i][j].z = r*cos(((double)j/(double)slices)*2*pi);
        }
    }
    //draw quads using generated points
    for(int i = 0; i < stacks; i++)
    {
        for(int j = 0; j < slices; j++)
        {
            if(j % 2 == 0)
            {
                glColor3f(1,1,1);
            }
            else
            {
                glColor3f(0,0,0);
            }
            glBegin(GL_QUADS);
            {
                //upper hemisphere
                glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                glVertex3f(points[i][j+1].x, points[i][j+1].y, points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x, points[i+1][j+1].y, points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x, points[i+1][j].y, points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x, -points[i][j].y, points[i][j].z);
                glVertex3f(points[i][j+1].x, -points[i][j+1].y, points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x, -points[i+1][j+1].y, points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x, -points[i+1][j].y, points[i+1][j].z);
            }
            glEnd();
        }
    }
}

void look_left(double angle)
{
    //up fixed
    Point3D temp = l ;
    l.x = temp.x * cos((double)angle) + (u.y * temp.z - u.z * temp.y) * sin((double)angle);
    l.y = temp.y * cos((double)angle) + (u.z * temp.x - u.x * temp.z) * sin((double)angle);
    l.z = temp.z * cos((double)angle) + (u.x * temp.y - u.y * temp.x) * sin((double)angle);

    temp = r ;
    r.x = temp.x * cos((double)angle) + (u.y * temp.z - u.z * temp.y) * sin((double)angle);
    r.y = temp.y * cos((double)angle) + (u.z * temp.x - u.x * temp.z) * sin((double)angle);
    r.z = temp.z * cos((double)angle) + (u.x * temp.y - u.y * temp.x) * sin((double)angle);

}

void look_right(double angle)
{
    angle = (-1) * angle;
    look_left(angle);
}

void look_up(double angle)
{
    //r fixed
    Point3D temp = u ;
    u.x = temp.x * cos((double)angle) + (r.y * temp.z - r.z * temp.y) * sin((double)angle);
    u.y = temp.y * cos((double)angle) + (r.z * temp.x - r.x * temp.z) * sin((double)angle);
    u.z = temp.z * cos((double)angle) + (r.x * temp.y - r.y * temp.x) * sin((double)angle);

    temp = l ;
    l.x = temp.x * cos((double)angle) + (r.y * temp.z - r.z * temp.y) * sin((double)angle);
    l.y = temp.y * cos((double)angle) + (r.z * temp.x - r.x * temp.z) * sin((double)angle);
    l.z = temp.z * cos((double)angle) + (r.x * temp.y - r.y * temp.x) * sin((double)angle);
}

void look_down(double angle)
{
    angle = (-1)*angle;
    look_up(angle);
}

void tilt_clockwise(double angle)
{
    //l fixed
    angle = (-1)*angle;
    Point3D temp = u ;
    u.x = temp.x * cos((double)angle) + (l.y * temp.z - l.z * temp.y) * sin((double)angle);
    u.y = temp.y * cos((double)angle) + (l.z * temp.x - l.x * temp.z) * sin((double)angle);
    u.z = temp.z * cos((double)angle) + (l.x * temp.y - l.y * temp.x) * sin((double)angle);

    temp = r ;
    r.x = temp.x * cos((double)angle) + (l.y * temp.z - l.z * temp.y) * sin((double)angle);
    r.y = temp.y * cos((double)angle) + (l.z * temp.x - l.x * temp.z) * sin((double)angle);
    r.z = temp.z * cos((double)angle) + (l.x * temp.y - l.y * temp.x) * sin((double)angle);

}

void tilt_anticlockwise(double angle)
{
    angle = (-1)*angle;
    tilt_clockwise(angle);
}

void keyboardListener(unsigned char key, int x,int y){
    switch(key){
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
            pos = pos - l * MOVE_CONSTANT;
            break;
            
        case GLUT_KEY_UP:        // up arrow key
            cameraHeight += 3.0;
            pos = pos + l * MOVE_CONSTANT;
            break;

        case GLUT_KEY_RIGHT:
            pos = pos + r * MOVE_CONSTANT;
            break;
            
        case GLUT_KEY_LEFT:
            pos = pos - r * MOVE_CONSTANT;
            break;

        case GLUT_KEY_PAGE_UP:
            pos = pos + u * MOVE_CONSTANT;
            break;
            
        case GLUT_KEY_PAGE_DOWN:
            pos = pos - u * MOVE_CONSTANT;
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

    gluLookAt(pos.x, pos.y, pos.z,     pos.x+l.x, pos.y+l.y, pos.z+l.z,    u.x, u.y, u.z);


    //again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);


        /****************************
        / Add your objects from here
        ****************************/
        //add objects

    drawAxes();

    //draw Objects
    for(int i = 0; i < objects.size(); i++)
    {
        objects[i]->draw();
    }
    
    //draw Light Sources
    for(int i = 0; i < lights.size(); i++)
    {
        lights[i].draw_light_source();
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
    drawgrid=0;
    drawaxes = 1;
    cameraHeight = 150.0;
    cameraAngle = 1.0;
    angle = 0;

    //initialization of pos, u, r, l vectors
    pos.x = pos.y = 120;
    pos.z = 20;

    u.x = u.y = 0;
    u.z = 1;

    r.x = (double)-1/sqrt(2);
    r.y = (double)1/sqrt(2);
    r.z = 0;

    l.x = (double)-1/sqrt(2);
    l.y = (double)-1/sqrt(2);
    l.z = 0;



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
    gluPerspective(80,    1,    1,    1000.0);
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
                cin >> object->gen_obj_coefficients_array[i];
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
}

int main(int argc, char **argv) {
    /* *************** File Read **********************************/
    
    freopen("/Users/rishovpaul/Desktop/scene.txt", "r", stdin);
    load_data();
    
    //project location ---> cd Documents/Academics/4-1/"Computer Graphics Sessional"/Offline3/"Ray Tracing"
    
    /* ***********************************************************/
    glutInit(&argc,argv);
    glutInitWindowSize(600, 600);
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
