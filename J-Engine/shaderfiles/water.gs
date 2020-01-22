#version 460 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

noperspective out vec3 edgeDistance;

in gl_PerVertex 
{
    vec4 gl_Position;
} gl_in[];

out gl_PerVertex 
{
    vec4 gl_Position;
};

layout (location = 1) in block 
{
    vec3 vertex;
    vec3 vertex_viewspace;
    vec3 normal;
    vec2 texCoords;
} In[];

layout (location = 1) out block 
{
    vec3 vertex;
    vec3 vertex_viewspace;
    vec3 normal;
    vec2 texCoords;
} Out;


void emitVertex(int i)
{
    Out.vertex = In[i].vertex;
    Out.vertex_viewspace = In[i].vertex_viewspace;
    Out.normal = In[i].normal;
    Out.texCoords = In[i].texCoords;
    gl_Position = gl_in[i].gl_Position;
}

void main()
{
    float a = abs(length(gl_in[1].gl_Position.xyz - gl_in[2].gl_Position.xyz));
    float b = abs(length(gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz));
    float c = abs(length(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz));

    float alpha = acos((b*b + c*c - a*a) / (2.0*b*c));
    float beta = acos((a*a + c*c - b*b) / (2.0*a*c));
    float ha = abs(c * sin( beta ));
    float hb = abs(c * sin( alpha ));
    float hc = abs(b * sin( alpha ));

    emitVertex(0);
    edgeDistance = vec3(ha, 0, 0);
    EmitVertex();
    emitVertex(1);
    edgeDistance = vec3(0, hb, 0);
    EmitVertex();
    edgeDistance = vec3(0, 0, hc);
    emitVertex(2);
    EmitVertex();
}