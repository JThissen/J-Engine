#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

//uniform sampler2D screenTexture;
//uniform sampler2D textureColorBuffer;
uniform sampler2D textureHdrBuffer;
uniform bool horizontal;
float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{
	//FragColor = vec4(texture(textureHdrBuffer, TexCoords).rgb, 1.0);
	vec2 tex_offset = (1.0 / textureSize(textureHdrBuffer, 0)) * 1.0;
    vec3 result = texture(textureHdrBuffer, TexCoords).rgb * weight[0];
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
        result += texture(textureHdrBuffer, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        result += texture(textureHdrBuffer, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(textureHdrBuffer, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(textureHdrBuffer, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }

    //FragColor = vec4(result + texture(textureColorBuffer, TexCoords).rgb, 1.0);
    FragColor = vec4(result, 1.0);
}