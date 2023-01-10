
#version 330 core
#define FOV 1.0
#define SAMPLES 50
#define MAX_BOUNCES 7

out vec4 FragColor;
in vec4 gl_FragCoord;
uniform vec2 iResolution;
uniform float frameCount;

float seed = 0.0;
float rand() { return fract(sin(seed++)*43758.5453123); }
float randd(float low, float high) { return rand() * (high-low) + low; }

struct ray {
    vec3 o;
    vec3 d;
};
struct sphere {
    vec3 c;
    float r;
    bool is_metal;
    vec3 albedo;
    float metal_fuzziness;
};
struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    sphere s;
};

const int sphere_count = 50;
sphere spheres [sphere_count];
void initSpheres() {
    spheres[0] = sphere(vec3(0.0, -1000.5,-1.0), 1000.0, true, vec3(0.5, 0.5, 0.5), 0.1);
    for (int i = 1; i < sphere_count; i++) {
        float z = randd(-20.0,-1.0);
        //float r = randd(0.05, 0.2 - z*0.1);
        float r = rand() > 0.97 ? randd(0.5,2.5) : randd(0.1,0.5);
        spheres[i] = sphere(vec3(randd(-3.0+z*0.5, 3.0-z*0.5), -0.5 + r, z), r, rand() > 0.5, vec3(rand(), rand(), rand()), randd(0.0,0.1));
    }
    // spheres[0] = sphere(vec3(0.0, 0.0, -1.0), 0.5, false, vec3(0.8, 0.3, 0.3), 0);
    // spheres[1] = sphere(vec3(0.0, -10000.5,-1.0), 10000.0, false, vec3(0.8, 0.8, 0.0), 0);
    // spheres[2] = sphere(vec3(1.0, 0.0,-1.0), 0.5, true, vec3(0.8, 0.8, 0.8), 0.2);

}
vec3 point_at_parameter(ray r, float t) { return r.o + t*r.d; }
vec3 reflect(vec3 v, vec3 n) { return v-2*dot(v,n)*n; }
vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.0*vec3(rand(), rand(), rand())-vec3(1.0,1.0,1.0);
    } while (length(p) >= 1.0);
    return p;
}
bool scatter(ray r_in, hit_record rec, out vec3 attenuation, out ray scattered, sphere s) {
    if (s.is_metal) {
        vec3 reflected = reflect(normalize(r_in.d), rec.normal);
        scattered = ray(rec.p, reflected + s.metal_fuzziness * random_in_unit_sphere());
        attenuation = s.albedo;
        return (dot(scattered.d, rec.normal) > 0);
    } 
    vec3 target = rec.p + rec.normal + random_in_unit_sphere();
    scattered = ray(rec.p, target-rec.p);
    attenuation = s.albedo;
    return true;
}
bool hit_sphere(ray r, sphere s, float t_min, float t_max, out hit_record rec) {
    vec3 oc = r.o - s.c;
    float a = dot(r.d, r.d);
    float b = dot(oc, r.d);
    float c = dot(oc, oc) - s.r*s.r;
    float discriminant = b*b - a*c;
    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant))/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = point_at_parameter(r, rec.t);
            rec.normal = (rec.p-s.c)/s.r;
            rec.s = s;
            return true;
        }
        temp = (-b + sqrt(discriminant))/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = point_at_parameter(r, rec.t);
            rec.normal = (rec.p-s.c)/s.r;
            rec.s = s;
            return true;
        }
    }
    return false;
}
bool hit_all(ray r, float t_min, float t_max, out hit_record rec) {
    hit_record temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;
    for (int i = 0; i < sphere_count; i++) {
        if (hit_sphere(r, spheres[i], t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

vec3 recursive_color(ray r) {
    hit_record rec;
    ray rr = ray(r.o, r.d);
    bool is_hit = hit_all(rr, 0.0, 1e7, rec);
    vec3 cc = vec3(1.0,1.0,1.0);
    int recursion_stop = 0;
    while(is_hit && recursion_stop < MAX_BOUNCES) {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        //ray scattered;
        vec3 attenuation;
        bool is_scattered = scatter(r, rec, attenuation, rr, rec.s);
        cc *= is_scattered ? attenuation : vec3(0);
        //rr = ray(rec.p, target-rec.p);
        is_hit = hit_all(rr, 0.001, 1e7, rec);
        recursion_stop++;

    }
    vec3 unit_direction = normalize(r.d);
    float t = 0.5 * (unit_direction.x + 1.0);
    float tt = 0.5 * (unit_direction.y + 1.0);
    vec3 cx = (1.0-t)*vec3(0.2,0.3,0.8) + t*vec3(0.5, 0.7, 1.0);
    vec3 c = (1.0-tt)*vec3(1.0,1.0,1.0) + tt*cx;
    return cc * c;
}

vec3 color (ray r) {
    hit_record rec;
    
    if (hit_all(r, 0.0, 1e7, rec)) {

        //vec3 target = rec.p + rec.normal + random_in_unit_sphere();

        return 0.5*(rec.normal+1.0);
    }
    vec3 unit_direction = normalize(r.d);
    float t = 0.5 * (unit_direction.y + 1.0);
    return (1.0-t)*vec3(1.0,1.0,1.0) + t*vec3(0.5, 0.7, 1.0);
}
void main() {
    initSpheres();
    vec3 col = vec3(0.0,0.0,0.0);
    for (int s = 0; s < SAMPLES; s++) {
        vec3 uv = FOV * vec3((gl_FragCoord.xy + vec2(rand(), rand()) - .5*iResolution.xy)/iResolution.y, 0);
        //vec3 col = vec3(gl_FragCoord.xy / iResolution.xy, 0.2);
        vec3 camera_origin = vec3(0,1,1);
        ray r = ray(camera_origin, vec3(uv.xy, -1.0));
        col += recursive_color(r);
    }
    col /= SAMPLES;
    col = vec3(sqrt(col.x), sqrt(col.y), sqrt(col.z));
    // col = pow(col, vec3(.4545));
    FragColor = vec4(col, 1.0);
}