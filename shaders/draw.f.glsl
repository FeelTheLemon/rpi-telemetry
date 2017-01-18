precision mediump float;
uniform vec4 color;

void main() {

	//vec3 ret = vec3(color.a) * gl_FragColor.xyz + vec3(1.0 - color.a) * color.xyz;

	//gl_FragColor = vec4(ret, 1.0);

	//vec4 ret = vec4(gl_FragColor.a) * gl_FragColor + vec4(1.0 - gl_FragColor.a) * color;
	//ret.w = 1.0;

	//vec4 ret = vec4(1.0) * gl_FragColor + vec4(1.0) * color;

	//vec3 ret = vec3(0);

	//ret = (gl_FragColor.rgb + color.rgb) * vec3(0.5);

	//gl_FragColor = vec4(ret, 1.0);

	gl_FragColor = color;
	//gl_FragColor = vec4(gl_FragColor.a) * gl_FragColor + vec4(1.0 - gl_FragColor.a) * color;
	//gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);

	//vec3 ret = vec3(color.a) * color.rgb + vec3(1.0 - color.a) * gl_FragColor.rgb;

	//gl_FragColor = vec4(ret, 0.5);


}
