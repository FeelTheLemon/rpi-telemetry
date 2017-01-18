

attribute vec2 vertex;

uniform float size;

uniform vec2 translation;
uniform vec2 offset;
uniform float rotation;
uniform vec2 scale;
uniform vec2 scrsize;

void main()
{
	

	float cosR = cos(radians(rotation));
	float sinR = sin(radians(rotation));

	vec2 vert = vertex;
	vert += offset;

	vec2 ret = vec2(0,0);

	ret.x = (vert.x * cosR + vert.y * sinR) * scale.x;
	ret.y = (vert.y * cosR - vert.x * sinR) * scale.y;

	ret = ret + translation;

	gl_PointSize = size;

	gl_Position = vec4(ret / scrsize * vec2(2,2) - vec2(1,1), 0, 1);
}