#include <math.h>
#include <stdio.h>
#include <assert.h>

/* #include "util.h" */
#include "vec3.h"


extern inline
double vec3_dist(vec3 v1, vec3 v2) {
    return sqrt (
        (v1.x-v2.x) * (v1.x-v2.x) +
        (v1.y-v2.y) * (v1.y-v2.y) +
        (v1.z-v2.z) * (v1.z-v2.z) );
}

extern inline
double dot(vec3 v1, vec3 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

extern inline
vec3 sub(vec3 v1, vec3 v2) {
    vec3 result = {v1.x - v2.x, v1.y - v2.y, v1.z -v2.z};
    return result;
}

extern inline
vec3 add(vec3 v1, vec3 v2) {
    vec3 result = {v1.x + v2.x, v1.y + v2.y, v1.z +v2.z};
    return result;

}

extern inline
vec3 scale(vec3 v, double s) {
    vec3 res = {v.x * s, v.y * s, v.z * s};
    return res;
}

extern inline
vec3 normalize(vec3 v) {
    double s = sqrt(dot(v,v));
    vec3 res = scale(v, 1.0/s);
    return res;
}

extern inline
vec3 reflect(vec3 v, vec3 n) {
    vec3 tmp = scale(n, 2* dot(n,v));
    return sub(v,tmp);
}

extern inline
vec3 cross(vec3 u, vec3 v) {
    vec3 res = {
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    };
    return res;
}

extern inline
double length(vec3 v) {
    return sqrt(dot(v,v));
}

extern inline
void print_vec3(vec3 v) {
    printf("[%.3f,%.3f,%.3f]", v.x, v.y, v.z);
}

