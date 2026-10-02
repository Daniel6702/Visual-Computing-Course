#include "cube_renderer.hpp"

#include "shader.hpp"
#include "transformations.hpp"

#include <cmath>

using namespace cv;
using namespace std;

//cube edges
static const int edges[][2] = {
    {0,1},{1,2},{2,3},{3,0}, //front
    {4,5},{5,6},{6,7},{7,4}, //back
    {0,4},{1,5},{2,6},{3,7}  //connect them
};

static vector<Point3f> create_cube_vertices()
{
    //create the 8 cube corners
    return {
        {-1,-1, 0}, //front
        { 1,-1, 0},
        { 1, 1, 0},
        {-1, 1, 0},

        {-1,-1, 2}, //back
        { 1,-1, 2},
        { 1, 1, 2},
        {-1, 1, 2}
    };
}

bool setup_cube(
    CubeRenderer& cube,
    const char* vertex_shader_path,
    const char* fragment_shader_path
)
{
    //load cube shader
    cube.shader = load_shader_program(vertex_shader_path, fragment_shader_path);

    if (!cube.shader)
        return false;

    cube.points = create_cube_vertices();

    //move cube in front of the camera
    cube.points = translate(cube.points, -1, 0, 10);

    //create VAO and VBO
    glGenVertexArrays(1, &cube.vao);
    glGenBuffers(1, &cube.vbo);

    glBindVertexArray(cube.vao);
    glBindBuffer(GL_ARRAY_BUFFER, cube.vbo);

    //define vertex position layout
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    return true;
}

void update_cube(CubeRenderer& cube)
{
    float speed = 2.0f;

    cube.time += 0.05f * speed;

    //rotate cube slightly each frame
    cube.points = rotate(cube.points, 0.04f, -0.06f, 0.0f);

    //move cube using sine waves
    cube.points = translate(
        cube.points,
        sin(cube.time) * 0.05f * speed,
        sin(cube.time + 2 * CV_PI / 3) * 0.05f * speed,
        sin(cube.time + 4 * CV_PI / 3) * 0.2f * speed
    );
}

void draw_cube(const CubeRenderer& cube, const Matx44f& view, const Matx44f& projection) {
    //12 edges with 2 vertices each
    float vertices[72]; //each edge consist of two points, with three coordinates. i.e 2*3*12 = 72

    for (int i = 0; i < 12; i++) {
        Point3f A = cube.points[edges[i][0]];
        Point3f B = cube.points[edges[i][1]];

        int size = 6; //6 floats per edge

        vertices[i * size + 0] = A.x;
        vertices[i * size + 1] = A.y;
        vertices[i * size + 2] = A.z;

        vertices[i * size + 3] = B.x;
        vertices[i * size + 4] = B.y;
        vertices[i * size + 5] = B.z;
    }

    glEnable(GL_DEPTH_TEST); //enable depth testing for 3D rendering

    glUseProgram(cube.shader);
    
    //get transformation matrix locations
    GLint view_location = glGetUniformLocation(cube.shader, "view");
    GLint projection_location = glGetUniformLocation(cube.shader, "projection");
    
    //send matrices to shader for projection and view transformations
    glUniformMatrix4fv(view_location, 1, GL_TRUE, view.val);
    glUniformMatrix4fv(projection_location, 1, GL_TRUE, projection.val);

    glBindVertexArray(cube.vao);
    glBindBuffer(GL_ARRAY_BUFFER, cube.vbo);

    //upload current cube vertices
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_LINES, 0, 24); //draw the 12 cube edges. starting index, total vertices, 12 lines = 24 points
}

void cleanup_cube(CubeRenderer& cube)
{
    //delete OpenGL resources
    glDeleteBuffers(1, &cube.vbo);
    glDeleteVertexArrays(1, &cube.vao);
    glDeleteProgram(cube.shader);
}