#version 330

in highp vec3 a_position;
in highp vec4 a_color;

uniform highp mat4 u_proj;
uniform highp mat4 u_view;
uniform highp mat4 u_trans;

out highp vec4 color;

void main(void)
{
   gl_Position = u_proj * u_view * u_trans * vec4(a_position, 1.0);
   color = a_color;
}
