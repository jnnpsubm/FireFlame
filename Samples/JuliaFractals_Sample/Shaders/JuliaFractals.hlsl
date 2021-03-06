
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj; 
    float4   gC;
    int      gMaxIterations;
};

static const float3 palette[256] = {
    float3(0,0,1),
    float3(0.054902,0.0117647,1),
    float3(0.109804,0.027451,1),
    float3(0.164706,0.0392157,1),
    float3(0.219608,0.054902,1),
    float3(0.27451,0.0705882,1),
    float3(0.329412,0.0823529,1),
    float3(0.384314,0.0980392,1),
    float3(0.443137,0.113725,1),
    float3(0.498039,0.12549,1),
    float3(0.533333,0.133333,1),
    float3(0.572549,0.145098,1),
    float3(0.611765,0.152941,1),
    float3(0.65098,0.164706,1),
    float3(0.690196,0.172549,1),
    float3(0.729412,0.184314,1),
    float3(0.768627,0.192157,1),
    float3(0.807843,0.203922,1),
    float3(0.847059,0.211765,1),
    float3(0.886275,0.223529,1),
    float3(0.92549,0.231373,1),
    float3(0.964706,0.243137,1),
    float3(1,0.25098,1),
    float3(0.996078,0.25098,0.972549),
    float3(0.996078,0.25098,0.945098),
    float3(0.996078,0.254902,0.917647),
    float3(0.992157,0.254902,0.890196),
    float3(0.992157,0.254902,0.862745),
    float3(0.992157,0.258824,0.839216),
    float3(0.988235,0.258824,0.811765),
    float3(0.988235,0.258824,0.784314),
    float3(0.988235,0.262745,0.756863),
    float3(0.984314,0.262745,0.729412),
    float3(0.984314,0.262745,0.705882),
    float3(0.984314,0.266667,0.678431),
    float3(0.980392,0.266667,0.65098),
    float3(0.980392,0.270588,0.623529),
    float3(0.980392,0.270588,0.596078),
    float3(0.976471,0.270588,0.572549),
    float3(0.976471,0.27451,0.545098),
    float3(0.976471,0.27451,0.517647),
    float3(0.972549,0.27451,0.490196),
    float3(0.972549,0.27451,0.462745),
    float3(0.972549,0.278431,0.435294),
    float3(0.972549,0.278431,0.407843),
    float3(0.968627,0.278431,0.380392),
    float3(0.968627,0.282353,0.352941),
    float3(0.968627,0.282353,0.32549),
    float3(0.964706,0.282353,0.298039),
    float3(0.964706,0.286275,0.270588),
    float3(0.964706,0.286275,0.243137),
    float3(0.964706,0.290196,0.215686),
    float3(0.960784,0.290196,0.188235),
    float3(0.960784,0.290196,0.160784),
    float3(0.960784,0.294118,0.133333),
    float3(0.960784,0.294118,0.105882),
    float3(0.956863,0.294118,0.0784314),
    float3(0.956863,0.298039,0.0509804),
    float3(0.956863,0.301961,0.0235294),
    float3(0.945098,0.317647,0.0156863),
    float3(0.913725,0.333333,0.0509804),
    float3(0.882353,0.34902,0.0862745),
    float3(0.85098,0.364706,0.121569),
    float3(0.819608,0.380392,0.156863),
    float3(0.788235,0.396078,0.192157),
    float3(0.756863,0.411765,0.227451),
    float3(0.72549,0.427451,0.258824),
    float3(0.694118,0.443137,0.294118),
    float3(0.662745,0.458824,0.329412),
    float3(0.631373,0.47451,0.364706),
    float3(0.6,0.490196,0.4),
    float3(0.568627,0.505882,0.435294),
    float3(0.537255,0.52549,0.470588),
    float3(0.505882,0.541176,0.501961),
    float3(0.478431,0.556863,0.533333),
    float3(0.447059,0.572549,0.564706),
    float3(0.415686,0.588235,0.596078),
    float3(0.384314,0.603922,0.631373),
    float3(0.352941,0.619608,0.662745),
    float3(0.321569,0.635294,0.694118),
    float3(0.290196,0.65098,0.729412),
    float3(0.258824,0.666667,0.760784),
    float3(0.227451,0.682353,0.792157),
    float3(0.196078,0.698039,0.827451),
    float3(0.164706,0.713726,0.858824),
    float3(0.133333,0.729412,0.890196),
    float3(0.101961,0.745098,0.921569),
    float3(0.0705882,0.760784,0.956863),
    float3(0.0392157,0.776471,0.988235),
    float3(0.00784314,0.792157,0.960784),
    float3(0.0352941,0.807843,0.901961),
    float3(0.0941176,0.819608,0.847059),
    float3(0.152941,0.835294,0.792157),
    float3(0.211765,0.847059,0.733333),
    float3(0.270588,0.862745,0.678431),
    float3(0.329412,0.878431,0.619608),
    float3(0.384314,0.890196,0.564706),
    float3(0.435294,0.901961,0.509804),
    float3(0.486275,0.917647,0.443137),
    float3(0.537255,0.933333,0.380392),
    float3(0.588235,0.94902,0.317647),
    float3(0.639216,0.960784,0.25098),
    float3(0.690196,0.976471,0.188235),
    float3(0.741176,0.992157,0.12549),
    float3(0.890196,1,0.0588235),
    float3(0.913725,1,0.00392157),
    float3(0.933333,1,0.00392157),
    float3(0.956863,1,0.00392157),
    float3(0.980392,1,0),
    float3(1,1,0),
    float3(1,1,0),
    float3(1,1,0.0392157),
    float3(1,1,0.0823529),
    float3(1,1,0.12549),
    float3(1,1,0.168627),
    float3(1,1,0.211765),
    float3(1,1,0.254902),
    float3(1,1,0.298039),
    float3(1,1,0.341176),
    float3(1,1,0.384314),
    float3(1,1,0.427451),
    float3(1,1,0.470588),
    float3(1,1,0.505882),
    float3(1,1,0.541176),
    float3(1,1,0.572549),
    float3(1,1,0.603922),
    float3(1,1,0.639216),
    float3(1,1,0.670588),
    float3(1,1,0.705882),
    float3(1,1,0.737255),
    float3(1,1,0.768627),
    float3(1,1,0.803922),
    float3(1,1,0.835294),
    float3(1,1,0.866667),
    float3(1,1,0.901961),
    float3(1,1,0.933333),
    float3(1,1,0.968627),
    float3(1,1,1),
    float3(0.976471,1,1),
    float3(0.952941,1,1),
    float3(0.929412,1,1),
    float3(0.905882,1,1),
    float3(0.882353,1,1),
    float3(0.858824,1,1),
    float3(0.835294,1,1),
    float3(0.811765,1,1),
    float3(0.792157,1,1),
    float3(0.768627,1,1),
    float3(0.745098,1,1),
    float3(0.721569,1,1),
    float3(0.698039,1,1),
    float3(0.67451,1,1),
    float3(0.65098,1,1),
    float3(0.627451,1,1),
    float3(0.607843,1,1),
    float3(0.588235,1,1),
    float3(0.564706,1,1),
    float3(0.545098,1,1),
    float3(0.52549,1,1),
    float3(0.505882,1,1),
    float3(0.482353,1,1),
    float3(0.462745,1,1),
    float3(0.443137,1,1),
    float3(0.419608,1,1),
    float3(0.4,1,1),
    float3(0.380392,1,1),
    float3(0.360784,1,1),
    float3(0.337255,1,1),
    float3(0.317647,1,1),
    float3(0.298039,1,1),
    float3(0.278431,1,1),
    float3(0.254902,0.976471,1),
    float3(0.25098,0.941176,1),
    float3(0.25098,0.909804,1),
    float3(0.25098,0.87451,1),
    float3(0.25098,0.843137,1),
    float3(0.25098,0.811765,1),
    float3(0.25098,0.776471,1),
    float3(0.25098,0.745098,1),
    float3(0.25098,0.709804,1),
    float3(0.25098,0.678431,1),
    float3(0.25098,0.643137,1),
    float3(0.25098,0.611765,1),
    float3(0.25098,0.584314,1),
    float3(0.25098,0.552941,1),
    float3(0.25098,0.52549,1),
    float3(0.25098,0.494118,1),
    float3(0.25098,0.466667,1),
    float3(0.25098,0.435294,1),
    float3(0.25098,0.407843,1),
    float3(0.25098,0.376471,1),
    float3(0.25098,0.34902,1),
    float3(0.25098,0.317647,1),
    float3(0.25098,0.290196,1),
    float3(0.262745,0.258824,0.980392),
    float3(0.282353,0.243137,0.952941),
    float3(0.305882,0.239216,0.929412),
    float3(0.32549,0.231373,0.901961),
    float3(0.345098,0.223529,0.87451),
    float3(0.368627,0.215686,0.847059),
    float3(0.388235,0.207843,0.823529),
    float3(0.407843,0.203922,0.796078),
    float3(0.427451,0.196078,0.768627),
    float3(0.45098,0.188235,0.741176),
    float3(0.470588,0.180392,0.717647),
    float3(0.490196,0.176471,0.690196),
    float3(0.513726,0.168627,0.662745),
    float3(0.533333,0.160784,0.635294),
    float3(0.552941,0.152941,0.611765),
    float3(0.572549,0.145098,0.584314),
    float3(0.596078,0.141176,0.556863),
    float3(0.615686,0.133333,0.529412),
    float3(0.635294,0.12549,0.505882),
    float3(0.65098,0.117647,0.478431),
    float3(0.670588,0.113725,0.454902),
    float3(0.690196,0.105882,0.431373),
    float3(0.709804,0.101961,0.407843),
    float3(0.72549,0.0941176,0.380392),
    float3(0.745098,0.0901961,0.356863),
    float3(0.764706,0.0823529,0.333333),
    float3(0.784314,0.0745098,0.309804),
    float3(0.803922,0.0705882,0.282353),
    float3(0.819608,0.0627451,0.258824),
    float3(0.839216,0.0588235,0.235294),
    float3(0.858824,0.0509804,0.211765),
    float3(0.878431,0.0470588,0.184314),
    float3(0.894118,0.0392157,0.160784),
    float3(0.913725,0.0313726,0.137255),
    float3(0.933333,0.027451,0.113725),
    float3(0.952941,0.0196078,0.0862745),
    float3(0.968627,0.0156863,0.0627451),
    float3(0.988235,0.00784314,0.0392157),
    float3(0.984314,0.00392157,0.0156863),
    float3(0.937255,0,0.0156863),
    float3(0.894118,0,0.0705882),
    float3(0.85098,0,0.121569),
    float3(0.807843,0,0.176471),
    float3(0.760784,0,0.231373),
    float3(0.717647,0,0.282353),
    float3(0.67451,0,0.337255),
    float3(0.627451,0,0.392157),
    float3(0.584314,0,0.447059),
    float3(0.541176,0,0.498039),
    float3(0.498039,0,0.533333),
    float3(0.458824,0,0.572549),
    float3(0.419608,0,0.611765),
    float3(0.380392,0,0.65098),
    float3(0.341176,0,0.690196),
    float3(0.305882,0,0.729412),
    float3(0.266667,0,0.768627),
    float3(0.227451,0,0.807843),
    float3(0.188235,0,0.847059),
    float3(0.152941,0,0.886275),
    float3(0.113725,0,0.92549),
    float3(0.0745098,0,0.964706),
    float3(0.0352941,0,1)
};

struct VertexIn
{
	float3 PosL : POSITION;
    float2 TexC : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
    float2 TexC : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
    vout.TexC = vin.TexC;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    int iterations = 0;
    double2 Z = pin.TexC;
    const double threshold_squared = 16.0;
    while (iterations < gMaxIterations && dot(Z,Z) < threshold_squared) {
        double2 Z_squared;
        Z_squared.x = Z.x*Z.x - Z.y*Z.y;
        Z_squared.y = 2.0*Z.x*Z.y;
        Z = Z_squared + gC.xy;
        iterations++;
    }

    float4 color = float4(0.,0.,0.,1.);
    if (iterations == gMaxIterations)
    {
        color = float4(0.1,0.1,0.1, 1.);
    }
    else
    {
        //float ratio = float(iterations) / (float)(gMaxIterations);
        float ratio = smoothstep(0., (float)(gMaxIterations), float(iterations));
        float u = ratio * 255;
        float dis = frac(u);
        int u_floor = floor(u);
        int u_ceil = ceil(u);
        float3 color1 = palette[u_floor];
        float3 color2 = palette[u_ceil];
        color = float4(lerp(color1, color2, dis), 1.);
    }

    return color;
}


