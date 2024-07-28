varying vec4 oNormal;
varying vec4 oPosition;

vec2 dispHash(int i, int j) 
{
    float x = float(i);
    float y = float(j);
    
    for (int i=0; i<2; ++i) {
    	x = mod(87.0*x + 23.0*y, 257.0);
    	y = mod(87.0*x + 23.0*y, 1009.0);
    }
    
    return vec2(x/257.0, y/1009.0);
}

// map a pair of int (i,j) to a color in [0,1]x[0,1]x[0,1]
vec3 colHash(int i, int j)
{
    float r = float(i);
    float g = float(j);
    float b = float(i+j);
    
    for (int i=0; i<2; ++i) {
    	r = mod(87.0*r + 23.0*g + 125.0*b, 257.0);
    	g = mod(87.0*r + 23.0*g + 125.0*b, 1009.0);
        b = mod(87.0*r + 23.0*g + 125.0*b, 21001.0);
    }

    return vec3(r/257.0,g/1009.0,b/21001.0);
}

vec3 vor2d(vec2 pos) 
{
    float step = 10.0;
    
    int xi = int(floor(pos.x/step));
    int yj = int(floor(pos.y/step));
    
    ivec2 nearest;
    float min_dist = 1e5;
    
    for (int i = xi-1; i <= xi+1; ++i) {
        for (int j = yj-1; j <= yj+1; ++j) {
            vec2 disp = dispHash(i,j);
            vec2 seed = vec2((float(i)+disp.x)*step, (float(j)+disp.y)*step);
            float dist = length(pos-seed);
            if (dist<min_dist) {
                min_dist = dist;
                nearest = ivec2(i, j);
            }
        }
    }
    
    // color of the cell
    vec3 col = colHash(nearest.x, nearest.y);
    
    return col;
}

vec2 map(vec3 pos) {
     vec2 p;
     float pos_norm = length(pos);
     p.x = acos(pos.x / pos_norm);
     p.y = atan(pos.y / pos.z);

     return p;
}

void main(){
  vec2 p = map(oPosition.xyz); 
  vec3 col = vor2d(100.0 * p); 

  gl_FragColor = vec4(col, 1.0);

  // light position in eye space
  vec4 light[2];
  light[0] = vec4(10.0, 5.0, 1.0, 1.0);
  light[1] = vec4(-10.0, -5.0, 1.0,1.0);
  vec4 lin = vec4(0.0);

  for (int i = 0; i < 2; i++) {

  // vertex in eye space
  vec4 V = oPosition;

  // vertex to light direction
  vec4 L = normalize(light[i] - V);

  // normal in eye space
  vec4 N = oNormal;

  // apply a (pseudo-random) perturbation to the normal
  N.w = 0.0;
  N = normalize(N);

  // material
  vec4 amb = vec4(0.01, 0.01, 0.01, 1.0);
  vec4 diff = vec4(1.0, 1.0, 1.0, 1.0);
  vec4 spec = vec4(2.0, 2.0, 2.0, 1.0);
  float shiny = 16.0;
  
  // light color
  vec4 lcol = vec4(1.0, 1.0, 1.0, 1.0);

  // Phong shading model
  //
  // reflected light vector:
  vec4 R = reflect(-L, N);
  vec4 View = normalize(-V);

  // lighting
  float ndl = clamp(dot(N, L), 0.0, 1.0);
  float vdr = pow(clamp(dot(View, R), 0.0, 1.0), shiny);

  
  lin += amb * lcol;
  lin += ndl * diff * lcol;
  lin += vdr * spec * lcol;
  }
  
  gl_FragColor *= lin;
}