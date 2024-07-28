varying vec4 oNormal;
varying vec4 oPosition;

vec2 map(vec3 pos) {
     vec2 p;
     float pos_norm = length(pos);
     p.x = acos(pos.x / pos_norm);
     p.y = atan(pos.y / pos.z);

     return p;
}

void main(){	     
     vec3 brickColor = vec3(1.0, 0.3, 0.2);
     vec3 mortarColor = vec3(0.85, 0.86, 0.84);
     vec2 brickSize = vec2(0.1, 0.05);
     vec2 brickPct = vec2(0.9, 0.85);

     vec3 color;
     vec2 position = map(oPosition.xyz);
     vec2 useBrick;

     position = position / brickSize;

     if (fract(position.y * 0.5) > 0.5){
     	position.x += 0.5;
     }
     
     position = fract(position);
     useBrick = step(position, brickPct);

     color = mix(mortarColor, brickColor, useBrick.x*useBrick.y);
	     
     gl_FragColor = vec4(color, 1.0);
}
	