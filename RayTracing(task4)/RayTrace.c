#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include <float.h>
#include <stdlib.h>

#include "Scene.h"
#include "RayTrace.h"
#include "Geometry.h"


// Clamp c's entries between low and high. 
static void clamp(Color * c, float low, float high) {
    c->_red = fminf(fmaxf(c->_red, low), high);
    c->_green = fminf(fmaxf(c->_green, low), high);
    c->_blue = fminf(fmaxf(c->_blue, low), high);
}


// Complete
// Given a ray (origin, direction), check if it intersects a given
// sphere.
// Return 1 if there is an intersection, 0 otherwise.
// *t contains the distance to the closest intersection point, if any.
static int
hitSphere(Vector3 origin, Vector3 direction, Sphere sphere, float* t)
{
    float norm_o = 0, norm_d = 0;
    float dot_do = 0;
    float rad = sphere._radius;
    Vector3 center = sphere._center;

    //o_c is a vector that origin vector - sphere's center vector
    Vector3 o_c;
    sub(origin, center, &o_c);

    computeNorm(direction, &norm_d);
    computeNorm(o_c, &norm_o);
    computeDotProduct(o_c, direction, &dot_do);

    float alpha = powf(norm_d, 2);
    float beta = 2 * dot_do;
    float ganma = powf(norm_o, 2) - powf(rad, 2);

    // dis is discriminant
    float dis = powf(beta, 2) - 4 * alpha * ganma;

    // if t exists
    if (dis >= 0) {
        float temp1 = (-beta + sqrtf(dis)) / 2 * alpha;
        float temp2 = (-beta - sqrtf(dis)) / 2 * alpha;

        if (temp1 > 0 && temp2 > 0) {
            *t = (temp1 < temp2) ? temp1 : temp2;
            return 1;
        }
        else if (temp1 > 0 || temp2 > 0) {
            *t = (temp1 > temp2) ? temp1 : temp2;
            return 1;
        }
    }

    return 0;
}


// Check if the ray defined by (scene._camera, direction) is intersecting
// any of the spheres defined in the scene.
// Return 0 if there is no intersection, and 1 otherwise.
//
// If there is an intersection:
// - the position of the intersection with the closest sphere is computed 
// in hit_pos
// - the normal to the surface at the intersection point in hit_normal
// - the diffuse color and specular color of the intersected sphere
// in hit_color and hit_spec
static int
hitScene(Vector3 origin, Vector3 direction, Scene scene,
    Vector3* hit_pos, Vector3* hit_normal,
    Color* hit_color, Color* hit_spec)
{
    Vector3 o = origin;
    Vector3 d = direction;

    float t_min = FLT_MAX;
    int hit_idx = -1;
    Sphere hit_sph;

    // For each sphere in the scene
    int i;
    for (i = 0; i < scene._number_spheres; ++i) {
        Sphere curr = scene._spheres[i];
        float t = 0.0f;
        if (hitSphere(o, d, curr, &t)) {
            if (t < t_min) {
                hit_idx = i;
                t_min = t;
                hit_sph = curr;
            }
        }
    }

    if (hit_idx == -1) return 0;

    Vector3 td;
    mulAV(t_min, d, &td);
    add(o, td, hit_pos);

    Vector3 n;
    sub(*hit_pos, hit_sph._center, &n);
    mulAV(1.0f / hit_sph._radius, n, hit_normal);

    // Save the color of the intersected sphere in hit_color and hit_spec
    *hit_color = hit_sph._color;
    *hit_spec = hit_sph._color_spec;

    return 1;
}


// Save the image in a raw buffer (texture)
// The memory for texture is allocated in this function. It needs to 
// be freed in the caller.
static void saveRaw(Color** image, int width, int height, GLubyte** texture) {
    int count = 0;
    int i;
    int j;
    *texture = (GLubyte*)malloc(sizeof(GLubyte) * 3 * width * height);

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            unsigned char red = (unsigned char)(image[i][j]._red * 255.0f);
            unsigned char green = (unsigned char)(image[i][j]._green * 255.0f);
            unsigned char blue = (unsigned char)(image[i][j]._blue * 255.0f);

            (*texture)[count] = red;
            count++;

            (*texture)[count] = green;
            count++;

            (*texture)[count] = blue;
            count++;
        }
    }
}


// Complete
// Given an intersection point (hit_pos),
// the normal to the surface at the intersection point (hit_normal),
// and the color (diffuse and specular) terms at the intersection point,
// compute the colot intensity at the point by applying the Phong
// shading model.
// Return the color intensity in *color.
static void
shade(Vector3 hit_pos, Vector3 hit_normal,
    Color hit_color, Color hit_spec, Scene scene, Color* color)
{
    // Complete
    // ambient component

    color->_red += scene._ambient._red * hit_color._red;
    color->_green += scene._ambient._green * hit_color._green;
    color->_blue += scene._ambient._blue * hit_color._blue;

    // v is view vector
    Vector3 v;
    sub(scene._camera, hit_pos, &v);
    normalize(v, &v);

    int l;
    for (l = 0; l < scene._number_lights; ++l) {
        Vector3 l_pos = scene._lights[l]._light_pos;

        // l_dir is light vector
        Vector3 l_dir;
        sub(l_pos, hit_pos, &l_dir);
        normalize(l_dir, &l_dir);

        // shadow's ray property
        Color shadow_color, shadow_spec;
        Vector3 shadow_origin, shadow_normal, shadow_pos;

        mulAV(0.001f, hit_normal, &shadow_normal);
        add(hit_pos, shadow_normal, &shadow_origin);

        if (hitScene(shadow_origin, l_dir, scene, &shadow_pos, &shadow_normal, &shadow_color, &shadow_spec)) {
            continue; 
        }

        // diffuse component
        Vector3 n;
        normalize(hit_normal, &n);
        float dot_nl;
        computeDotProduct(n, l_dir, &dot_nl);
        dot_nl = fmaxf(0.0f, dot_nl);
        color->_red += hit_color._red * dot_nl * scene._lights[l]._light_color._red;
        color->_green += hit_color._green * dot_nl * scene._lights[l]._light_color._green;
        color->_blue += hit_color._blue * dot_nl * scene._lights[l]._light_color._blue;
        
        // specular component

        // r is reflected light
        Vector3 r = { 2 * dot_nl * n._x - l_dir._x, 2 * dot_nl * n._y - l_dir._y, 2 * dot_nl * n._z - l_dir._z };
        float dot_rv;
        computeDotProduct(v, r, &dot_rv);
        dot_rv = powf(fmaxf(0.0f, dot_rv), 64.0f);
        color->_red += hit_spec._red * dot_rv * scene._lights[l]._light_color._red;
        color->_green +=  hit_spec._green * dot_rv * scene._lights[l]._light_color._green;
        color->_blue += hit_spec._blue * dot_rv * scene._lights[l]._light_color._blue;
    }

    color->_red = fminf(fmaxf(color->_red, 0.0f), 1.f);
    color->_green = fminf(fmaxf(color->_green, 0.0f), 1.f);
    color->_blue = fminf(fmaxf(color->_blue, 0.0f), 1.f);
}


static void rayTrace(Vector3 origin, Vector3 direction_normalized,
    Scene scene, Color* color)
{
    Vector3 hit_pos;
    Vector3 hit_normal;
    Color hit_color;
    Color hit_spec;
    int hit;

    // does the ray intersect an object in the scene?
    hit =
        hitScene(origin, direction_normalized, scene,
            &hit_pos, &hit_normal, &hit_color,
            &hit_spec);

    // no hit
    if (!hit) {
        color->_red = scene._background_color._red;
        color->_green = scene._background_color._green;
        color->_blue = scene._background_color._blue;
        return;
    }

    // otherwise, apply the shading model at the intersection point
    shade(hit_pos, hit_normal, hit_color, hit_spec, scene, color);
}


void rayTraceScene(Scene scene, int width, int height, GLubyte** texture) {
    Color** image;
    int i;
    int j;
    int k;

    Vector3 camera_pos;
    float screen_scale;

    image = (Color**)malloc(height * sizeof(Color*));
    for (i = 0; i < height; i++) {
        image[i] = (Color*)malloc(width * sizeof(Color));
    }

    // get parameters for the camera position and the screen fov
    camera_pos._x = scene._camera._x;
    camera_pos._y = scene._camera._y;
    camera_pos._z = scene._camera._z;

    screen_scale = scene._scale;

    int ray_num = 10;

    srand(64);

    // go through each pixel
    // and check for intersection between the ray and the scene
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            // finally color
            Color color_ave = { 0.0f, 0.0f, 0.0f };

            for (k = 0; k < ray_num; k++) {
                // Compute (x,y) coordinates for the current pixel 
                // in scene space
                float x = screen_scale * j - 0.5f * screen_scale * width;
                float y = screen_scale * i - 0.5f * screen_scale * height;

                // epsilon x and y
                // range from -screen_scale/2 to screen_scale/2
                float eps_x = screen_scale * ((float)rand() / RAND_MAX - 0.5f);
                float eps_y = screen_scale * ((float)rand() / RAND_MAX - 0.5f);

                x += eps_x;
                y += eps_y;

                // Form the vector camera to current pixel
                Vector3 direction;
                Vector3 direction_normalized;

                direction._x = x - camera_pos._x;
                direction._y = y - camera_pos._y;
                direction._z = -camera_pos._z;

                normalize(direction, &direction_normalized);

                Vector3 origin = scene._camera;
                Color color;
                color._red = 0.f;
                color._green = 0.f;
                color._blue = 0.f;
                rayTrace(origin, direction_normalized, scene, &color);

                color_ave._red += color._red;
                color_ave._green += color._green;
                color_ave._blue += color._blue;
            }

            // average color
            color_ave._red = color_ave._red / ray_num;
            color_ave._green = color_ave._green / ray_num;
            color_ave._blue = color_ave._blue / ray_num;

            // Gamma 
            color_ave._red = color_ave._red * 1.1f - 0.02f;
            color_ave._green = color_ave._green * 1.1f - 0.02f;
            color_ave._blue = color_ave._blue * 1.1f - 0.02f;
            clamp(&color_ave, 0.f, 1.f);
            color_ave._red = powf(color_ave._red, 0.4545f);
            color_ave._green = powf(color_ave._green, 0.4545f);
            color_ave._blue = powf(color_ave._blue, 0.4545f);

            // Contrast 
            color_ave._red = color_ave._red * color_ave._red * (3.f - 2.f * color_ave._red);
            color_ave._green = color_ave._green * color_ave._green * (3.f - 2.f * color_ave._green);
            color_ave._blue = color_ave._blue * color_ave._blue * (3.f - 2.f * color_ave._blue);

            image[i][j] = color_ave;
        }
    }

    // save image to texture buffer
    saveRaw(image, width, height, texture);

    for (i = 0; i < height; i++) {
        free(image[i]);
    }

    free(image);
}