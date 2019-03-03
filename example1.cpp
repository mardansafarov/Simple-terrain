//Cube, Pyramid, Sphere, Ortho and Perspective projections, Mouse interaction(rotation)
#include "Angel.h"
#include <vector>
using namespace std;

typedef Angel::vec4  point4;
typedef Angel::vec4  color4;

vec4 eye = vec4(0.6, 0.6, 0.6, 1.0);
vec4 at = vec4(0.0, 0.0, 0.0, 1.0);
vec4 up = vec4(0.0, 1.0, 0.0, 0.0);
    int w=512;
    int h=512;
double r = 0.5;
int currentShape = 0;   //for choosing which object to display
int Index = 0;
const int numI = 0;
const int numCube = 36;
const int numPyr = 18;
const int NumTimesToSubdivide = 5;
const int NumTriangles = 4096;  // (4 faces)^(NumTimesToSubdivide + 1)
const int NumVertices = 3 * NumTriangles;

//arrays for the coordinates of the sphere, cube, pyramid, normal(sphere), initial object->triangle
vec4 points_sphere[NumVertices];
vec4 normals[numI + numCube + numPyr + NumVertices];  //sphere
//----------------------------------------------------------------------------
float s=0.5;
//----------------------------------------------------------------------------
// Viewing transformation parameters
GLfloat radius = 1.0;
GLfloat theta = 0.0;
GLfloat phi = 0.0;
GLuint mLoc;


//----------------------------------------------------------------------------
bool mouseDown = false;
bool ortho = false;
bool frust = false;
//----------------------------------------------------------------------------
float r_x = 0.0;
float r_y = 0.0;

float Fx = 0.0;
float Fy = 0.0;
//----------------------------------------------------------------------------
vec4 points_initT[numI];
vec4 pointsCube[numCube];
vec4 pointsPyramid[numPyr];
//----------------------------------------------------------------------------
mat4 rotMat;
int numPoints = 0;
vec4 quad_data[342*16];

//----------------------------------------------------------------------------
//booleans for choosing  type of projection, scale or mouse interacton
bool sc = false;
bool ms = false;
bool p_proj = false;
bool o_proj = false;
bool mouse = false;


//------------------------------------
// Projection transformation parameters
GLfloat  left = -1.0, right = 1.0;
GLfloat  bottom = -1.0, top = 1.0;
GLfloat  zNear = 0.5, zFar = 3.0;
//------------------------------—
float rotX, rotY, rotZ;
float difX, difY;
float X, Y, Z, dX, dY, dZ;
//------------------------------—
GLuint model_view;  //for locations of the matrixes in vshader
GLuint  ModelView, Projection;
GLuint program ;
//---------------------------------------------------------------------------------—
void
 triangle( const point4& a, const point4& b, const point4& c )
{
 //vec4  normal = normalize( cross(b - a, c - b) );
 int offset = numI + numPyr + numCube;
 normals[offset+Index] = vec4(a.x, a.y, a.z, 0.0);  points_sphere[Index] = a;  Index++;
 normals[offset+Index] = vec4(b.x, b.y, b.z, 0.0);  points_sphere[Index] = b;  Index++;
 normals[offset+Index] = vec4(c.x, c.y, c.z, 0.0);  points_sphere[Index] = c;  Index++;
}
//------------------------------------------------------------------------------------------—
point4
 unit( const point4& p )
{
 float len = p.x*p.x + p.y*p.y + p.z*p.z;

 point4 t;
 if ( len > DivideByZeroTolerance ) {
  t = p / sqrt(len);
  t.w = 1.0;
 }
 return t;
}
 float lx=0.0;
 float ly=0.0;
 float lz=0.0;

//---------------------------------------------------------------------------------------------
void
 divide_triangle( const point4& a, const point4& b,
 const point4& c, int count )
{
 if ( count > 0 ) {
  point4 v1 = unit( a + b );
  point4 v2 = unit( a + c );
  point4 v3 = unit( b + c );
  divide_triangle(  a, v1, v2, count - 1 );
  divide_triangle(  c, v2, v3, count - 1 );
  divide_triangle(  b, v3, v1, count - 1 );
  divide_triangle( v1, v3, v2, count - 1 );
 }
 else {
  triangle( a, b, c );
 }
}
//---------------------------------------------------------------------------------—
void
 tetrahedron( int count )
{
 point4 v[4] = {
  vec4( 0.0, 0.0, 1.0, 1.0 ),
  vec4( 0.0, 0.942809, -0.333333, 1.0 ),
  vec4( -0.816497, -0.471405, -0.333333, 1.0 ),
  vec4( 0.816497, -0.471405, -0.333333, 1.0 )
 };
 divide_triangle( v[0], v[1], v[2], count );
 divide_triangle( v[3], v[2], v[1], count );
 divide_triangle( v[0], v[3], v[1], count );
 divide_triangle( v[0], v[2], v[3], count );
}
//------------------------------------------------------------------------------------------------
void
 init( void )
{
 vec4 points_initT[numI] = {                  //coordinates of initial object -> triangle
 };
 vec4 pointsCube[] = {   //cube points
    vec4(-0.5, -0.5, -0.5, 1.0),
    vec4(-0.5, -0.5, 0.5, 1.0),
    vec4(-0.5, 0.5, 0.5, 1.0),
    vec4(0.5, 0.5, -0.5, 1.0),
    vec4(-0.5, -0.5, -0.5, 1.0),
    vec4(-0.5, 0.5, -0.5, 1.0),
    vec4(0.5, -0.5, 0.5, 1.0),
    vec4(-0.5, -0.5, -0.5, 1.0),
    vec4(0.5, -0.5, -0.5, 1.0),
    vec4(0.5, 0.5, -0.5, 1.0),
    vec4(0.5, -0.5, -0.5, 1.0),
    vec4(-0.5, -0.5, -0.5, 1.0),
    vec4(-0.5, -0.5, -0.5, 1.0),
    vec4(-0.5, 0.5, 0.5, 1.0),
    vec4(-0.5, 0.5, -0.5, 1.0),
    vec4(0.5, -0.5, 0.5, 1.0),
    vec4(-0.5, -0.5, 0.5, 1.0),
    vec4(-0.5, -0.5, -0.5, 1.0),
    vec4(-0.5, 0.5, 0.5, 1.0),
    vec4(-0.5, -0.5, 0.5, 1.0),
    vec4(0.5, -0.5, 0.5, 1.0),
    vec4(0.5, 0.5, 0.5, 1.0),
    vec4(0.5, -0.5, -0.5, 1.0),
    vec4(0.5, 0.5, -0.5, 1.0),
    vec4(0.5, -0.5, -0.5, 1.0),
    vec4(0.5, 0.5, 0.5, 1.0),
    vec4(0.5, -0.5, 0.5, 1.0),
    vec4(0.5, 0.5, 0.5, 1.0),
    vec4(0.5, 0.5, -0.5, 1.0),
    vec4(-0.5, 0.5, -0.5, 1.0),
    vec4(0.5, 0.5, 0.5, 1.0),
    vec4(-0.5, 0.5, -0.5, 1.0),
    vec4(-0.5, 0.5, 0.5, 1.0),
    vec4(0.5, 0.5, 0.5, 1.0),
    vec4(-0.5, 0.5, 0.5, 1.0),
    vec4(0.5, -0.5, 0.5, 1.0)
        };
vec4 pointsPyramid[] = { //pyramid points
    vec4(0.0, 0.5, 0.0, 1.0),
    vec4(-0.5, -0.5, 0.5, 1.0),
    vec4(0.5, -0.5, 0.5, 1.0),
    vec4(0.0, 0.5, 0.0, 1.0),
    vec4(0.5, -0.5, 0.5, 1.0),
    vec4(0.5, -0.5, -0.5, 1.0),
    vec4(0.0, 0.5, 0.0, 1.0),
    vec4(0.5, -0.5, -0.5, 1.0),
    vec4(-0.5, -0.5, -0.5, 1.0),
    vec4( 0.0, 0.5, 0.0, 1.0),
    vec4(-0.5,-0.5,-0.5, 1.0),
    vec4(-0.5,-0.5, 0.5, 1.0),
    vec4(0.5, -0.5, 0.5, 1.0),
    vec4(0.5, -0.5, -0.5, 1.0),
    vec4(-0.5, -0.5, -0.5, 1.0),
    vec4(0.5, -0.5, 0.5, 1.0),
    vec4(-0.5, -0.5, -0.5, 1.0),
    vec4(-0.5, -0.5, 0.5, 1.0)
        };


 for(int i = 0; i < numI; i++){
  normals[i] = points_initT[i];
 }

 for (int i = 0; i < numCube; i++) {
  normals[numI + i] = pointsCube[i];
  normals[numI + i].w = 0.0;
 }

 for (int i = 0; i < numPyr; i++) {
  normals[numI + numCube + i] = pointsPyramid[i];
  normals[numI + numCube + i].w = 0.0;
 }

 tetrahedron( NumTimesToSubdivide );
//---------------------------------------------------------------------------------------------—

 // Create a vertex array object
 GLuint vao;
 glGenVertexArrays( 1, &vao );
 glBindVertexArray( vao );

 // Create and initialize a buffer object
 GLuint buffer;
 glGenBuffers( 1, &buffer );
 glBindBuffer( GL_ARRAY_BUFFER, buffer );

 //buffer data -> coordinates of all the objects above
 //each buffer subdata -> one object's coordinates

 glBufferData( GL_ARRAY_BUFFER, sizeof(points_initT) + sizeof(pointsCube) + sizeof(pointsPyramid) +
  + sizeof(points_sphere) + sizeof(normals), NULL, GL_STATIC_DRAW );

 glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points_initT), points_initT );

 glBufferSubData( GL_ARRAY_BUFFER, sizeof(points_initT),
  sizeof(pointsCube), pointsCube );

 glBufferSubData( GL_ARRAY_BUFFER, sizeof(points_initT) + sizeof(pointsCube),
  sizeof(pointsPyramid), pointsPyramid);

 glBufferSubData( GL_ARRAY_BUFFER, sizeof(points_initT) + sizeof(pointsCube) + sizeof(pointsPyramid),
  sizeof(points_sphere), points_sphere);

 glBufferSubData( GL_ARRAY_BUFFER, sizeof(points_initT) + sizeof(pointsCube) + sizeof(pointsPyramid)
  +sizeof(points_sphere), sizeof(normals), normals);


 // Load shaders and use the resulting shader program
 GLuint program = InitShader( "vshader21.glsl", "fshader21.glsl" );
 glUseProgram( program );

 // Initialize the vertex position attribute from the vertex shader
 GLuint loc = glGetAttribLocation( program, "vPosition" );
 glEnableVertexAttribArray( loc );
 glVertexAttribPointer( loc, 4, GL_FLOAT, GL_FALSE, 0,
  BUFFER_OFFSET(0) );

 int off_s = sizeof(points_initT) + sizeof(pointsCube) + sizeof(pointsPyramid) + sizeof(points_sphere);

 GLuint vNormal = glGetAttribLocation( program, "vNormal" );
 glEnableVertexAttribArray( vNormal );
 glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0,
  BUFFER_OFFSET(off_s));
  // Initialize shader lighting parameters
vec4 light_position ( 0.0, 5.0, 0.0, 1.0 );
 color4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
 color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
 color4 light_specular( 1.0, 1.0, 1.0, 1.0 );

 color4 material_ambient( 1.0, 0.0, 1.0, 1.0 );
 color4 material_diffuse( 1.0, 0.8, 0.0, 1.0 );
 color4 material_specular( 1.0, 0.0, 1.0, 1.0 );
 float  material_shininess = 5.0;

 color4 ambient_product = light_ambient * material_ambient;
 color4 diffuse_product = light_diffuse * material_diffuse;
 color4 specular_product = light_specular * material_specular;

 glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),
  1, ambient_product );

 glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),
  1, diffuse_product );
 glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),
  1, specular_product );



 glUniform1f( glGetUniformLocation(program, "Shininess"),
  material_shininess );

 //location of the matrix in vshader
 ModelView = glGetUniformLocation( program, "ModelView" );
 Projection = glGetUniformLocation( program, "Projection" );


 glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);

 glEnable(GL_DEPTH_TEST);
 glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background

}
//-------------------------------------------------------------------------------------------------

void
 display( void )
{
 glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    mat4 mtr, m_v;
 glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 m_v = LookAt(eye, at, up);

 mtr = Ortho(-1, 1, -1, 1, 1, 120) *RotateX(r_x)* RotateY(r_y)*Scale(s,s,s);

 if(o_proj == true){         // switches the projection to ortho
  mtr = Ortho(-1, 1, -1, 1, 1, 120) * RotateX(r_x)* RotateY(r_y)*Scale(s,s,s) * mtr;
 }

 if(p_proj == true){         // switches the projection to perspective
  mtr = Perspective(120, 1.0, 0.5, 3.0) * m_v * RotateX(r_x)* RotateY(r_y)*Scale(s,s,s)* mtr;

 }
 glUniformMatrix4fv(ModelView, 1, GL_TRUE, mtr);

 if (currentShape == 0){
  glDrawArrays(GL_TRIANGLES, 0, numI); //draw initial object - triangle
 }
 else if(currentShape == 1){
  glDrawArrays(GL_TRIANGLES, numI, numCube); //draw cube
 }
 else if(currentShape == 2){
  glDrawArrays(GL_TRIANGLES, numI + numCube, numPyr); // draw pyramid
 }
 else if (currentShape == 3){                // draw sphere
  glDrawArrays( GL_TRIANGLES, numI + numCube + numPyr, NumVertices );
 }
 glFlush();
 glutSwapBuffers();
}
//-------------------------------------------------------------------------------------------------

void myKey(unsigned char key, int x, int y)
{
 if(key == 'c' || key == 'C')    currentShape = 1;  //c -> cube
 if(key == 'p' || key == 'P')    currentShape = 2;  //p -> pyramid
 if(key == 's' || key == 'S') {  s=0.3; currentShape = 3;} //s -> sphere
 if(key == '-') {
  s*=0.5;
 }
 if(key == '+')
 {s*=1.5;
 }
 if(key == 'o' || key == 'O'){   //ortho projection call
        ortho=true;
        frust=false;
 }
 if(key == 'f' || key == 'F') {  //perspective projection call
        ortho=false;
        frust=true;
 }
 glutPostRedisplay();

}
//----------------------------------------------------------------------------

void MyMouse(int button, int state, int x, int y)

{
  if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {

    mouseDown = true;

    Fx = x - r_y;
    Fy = y - r_x;

  }
  else
    mouseDown = false;

  glutPostRedisplay();
}
//----------------------------------------------------------------------------
void Motion(int x, int y)
{

  if(mouseDown==true)

  {

    r_y = -(x - Fy)*0.3;
    r_x = -(y - Fx)*0.3;

  }
  glutPostRedisplay();
}
//----------------------------------------------------------------------------

void
reshape( int width, int height )
{
    glViewport( 0, 0, width, height );

    GLfloat aspect = GLfloat(width)/height;
    mat4  projection = Perspective( 45.0, aspect, 0.5, 3.0 );

    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
}
int
 main( int argc, char **argv )
{
 glutInit( &argc, argv );
 glutInitDisplayMode( GLUT_RGBA);

    glutInitWindowSize( w, h );
 glutInitContextVersion( 3, 2 );
 glutInitContextProfile( GLUT_CORE_PROFILE );
 glutCreateWindow( "ASSIGNMENT 3" );
 glewInit();
 init();
 glutDisplayFunc( display );  //display callback
     glutMouseFunc(MyMouse);
    glutMotionFunc(Motion);
 glutKeyboardFunc( myKey );  // calls keyboard funcion
 glutReshapeFunc(reshape);
 glutMainLoop();          //eventloop
 return 0;
}
