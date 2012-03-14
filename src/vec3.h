#ifndef VEC3_H_
#define VEC3_H_
typedef struct {
    double x,y,z;
}vec3;


double vec3_dist(vec3 v1, vec3 v2);
double dot(vec3 v1, vec3 v2);
vec3 sub(vec3 v1, vec3 v2);
vec3 add(vec3 v1, vec3 v2);
vec3 scale(vec3 v, double s);
vec3 normalize(vec3 v);
vec3 reflect(vec3 v, vec3 n);
vec3 cross(vec3 u, vec3 v);
double length(vec3 v);
void print_vec3(vec3 v);
vec3 *make_vec3(void);


#endif /* !VEC3_H_ */
