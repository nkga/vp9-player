// Vertex shader entry point. Well known trick adapted from AMD recommendations for drawing a fullscreen buffer.
// Avoids rasterization inaccuracies (and performance cost) of a typical two triangle setup, and doesn't
// require the use of any vertex, index or instance buffers. Generates a single triangle that covers the screen.
void main(in uint vid : SV_VERTEXID, out float4 pos : SV_Position, out float2 tex : TEXCOORD0) {
	tex = float2((vid << 1) & 2, vid & 2);
	pos = float4(tex * float2(2, -2) + float2(-1, 1), 0, 1);
};
