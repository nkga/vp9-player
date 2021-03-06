struct vs_output_t {
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

Texture2D y_tex : register(t0);
Texture2D u_tex : register(t1);
Texture2D v_tex : register(t2);
SamplerState r_samp : register(s0);

float4 main(vs_output_t input) : SV_TARGET {
	float3 yuv;
	yuv.x = y_tex.Sample(r_samp, input.tex).r;
	yuv.y = u_tex.Sample(r_samp, input.tex).r;
	yuv.z = v_tex.Sample(r_samp, input.tex).r;
	yuv += float3(-0.0627451017, -0.501960814, -0.501960814);

	float4 output;
	output.r = dot(yuv, float3(1.164,  0.000,  1.596));
	output.g = dot(yuv, float3(1.164, -0.391, -0.813));
	output.b = dot(yuv, float3(1.164,  2.018,  0.000));
	output.a = 1.0f;

	return output;
}
