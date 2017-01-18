attribute vec4 coord;
varying vec2 texpos;
uniform float rotation;
uniform vec2 scrsize;

uniform vec2 translation;
uniform vec2 offset;

void main(void) {
	float cosR = cos(radians(rotation));
	float sinR = sin(radians(rotation));

	vec2 vert = coord.xy;
	vert += offset;

	vec2 ret = vec2(0,0);

	ret.x = vert.x * cosR + vert.y * sinR;
	ret.y = vert.y * cosR - vert.x * sinR;

	ret = ret + translation;

	gl_Position = vec4(ret / scrsize * vec2(2,2) - vec2(1,1), 0, 1);
	texpos = coord.zw;
}
