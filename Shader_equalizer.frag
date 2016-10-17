#version 150 core
uniform vec3  iResolution;
uniform float iGlobalTime;
uniform float Spectrum[256];
uniform float Wavedata[256];
in vec4 gl_FragCoord;
out vec4 gl_FragColor;

void InterpolateValue(in float index, out float value) {
	float norm = 255.0/iResolution.x*index;
	int Floor = int(floor(norm));
    value = Wavedata[Floor];
}

#define THICKNESS 0.02
void main()
{
	float time = iGlobalTime;
	float x = gl_FragCoord.x / iResolution.x;
	float y = gl_FragCoord.y / iResolution.y ;

	float wave = 0;
	InterpolateValue(x*iResolution.x, wave);

	if (gl_FragCoord.y < wave * 1000) {
	    gl_FragColor = vec4(0.6 + 0.4*sin(time + 3.14), 0.6 + 0.4*sin(time + 3.14 * 2), 0.6 + 0.4*sin(time + 0.7) , 0);
	} else {
		gl_FragColor = vec4(0, 0, 0, 0);
	};
}