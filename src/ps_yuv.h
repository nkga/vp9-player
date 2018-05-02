#if 0
//
// Generated by Microsoft (R) D3D Shader Disassembler
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float       
// COLOR                    0   xyzw        1     NONE   float   xyzw
// TEXCOORD                 0   xy          2     NONE   float   xy  
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_TARGET                0   xyzw        0   TARGET   float   xyzw
//
//
// Sampler/Resource to DX9 shader sampler mappings:
//
// Target Sampler Source Sampler  Source Resource
// -------------- --------------- ----------------
// s0             s0              t0               
// s1             s0              t1               
// s2             s0              t2               
//
//
// Level9 shader bytecode:
//
    ps_2_0
    def c0, -0.0627451017, -0.501960814, -0.501960814, 1
    def c1, 1.16400003, 1.59599996, 0, 0
    def c2, 1.16400003, -0.391000003, -0.813000023, 0
    def c3, 1.16400003, 2.01799989, 0, 0
    dcl t0
    dcl t1.xy
    dcl_2d s0
    dcl_2d s1
    dcl_2d s2
    texld r0, t1, s0
    texld r1, t1, s1
    texld r2, t1, s2
    mov r0.y, r1.x
    mov r0.z, r2.x
    add r0.xyz, r0, c0
    mul r0.w, r0.x, c1.x
    mad r1.x, r0.z, c1.y, r0.w
    dp3 r1.y, r0, c2
    dp2add r1.z, r0, c3, c3.z
    mov r1.w, c0.w
    mul r0, r1, t0
    mov oC0, r0

// approximately 14 instruction slots used (3 texture, 11 arithmetic)
ps_4_0
dcl_sampler s0, mode_default
dcl_resource_texture2d (float,float,float,float) t0
dcl_resource_texture2d (float,float,float,float) t1
dcl_resource_texture2d (float,float,float,float) t2
dcl_input_ps linear v1.xyzw
dcl_input_ps linear v2.xy
dcl_output o0.xyzw
dcl_temps 2
sample r0.xyzw, v2.xyxx, t0.xyzw, s0
sample r1.xyzw, v2.xyxx, t1.xyzw, s0
mov r0.y, r1.x
sample r1.xyzw, v2.xyxx, t2.xyzw, s0
mov r0.z, r1.x
add r0.xyz, r0.xyzx, l(-0.062745, -0.501961, -0.501961, 0.000000)
dp2 r1.x, r0.xzxx, l(1.164000, 1.596000, 0.000000, 0.000000)
dp3 r1.y, r0.xyzx, l(1.164000, -0.391000, -0.813000, 0.000000)
dp2 r1.z, r0.xyxx, l(1.164000, 2.018000, 0.000000, 0.000000)
mov r1.w, l(1.000000)
mul o0.xyzw, r1.xyzw, v1.xyzw
ret 
// Approximately 0 instruction slots used
#endif

const BYTE ps_yuv_bc[] =
{
     68,  88,  66,  67,  90,   4, 
    209, 149, 241, 125,  93,  22, 
     64,  28, 201, 247, 143, 114, 
     26, 201,   1,   0,   0,   0, 
     92,   4,   0,   0,   4,   0, 
      0,   0,  48,   0,   0,   0, 
    212,   1,   0,   0, 180,   3, 
      0,   0,  40,   4,   0,   0, 
     65, 111, 110,  57, 156,   1, 
      0,   0, 156,   1,   0,   0, 
      0,   2, 255, 255, 108,   1, 
      0,   0,  48,   0,   0,   0, 
      0,   0,  48,   0,   0,   0, 
     48,   0,   0,   0,  48,   0, 
      3,   0,  36,   0,   0,   0, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   1,   0,   2,   0, 
      2,   0,   0,   2, 255, 255, 
     81,   0,   0,   5,   0,   0, 
     15, 160, 129, 128, 128, 189, 
    129, 128,   0, 191, 129, 128, 
      0, 191,   0,   0, 128,  63, 
     81,   0,   0,   5,   1,   0, 
     15, 160, 244, 253, 148,  63, 
    186,  73, 204,  63,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     81,   0,   0,   5,   2,   0, 
     15, 160, 244, 253, 148,  63, 
     39,  49, 200, 190, 197,  32, 
     80, 191,   0,   0,   0,   0, 
     81,   0,   0,   5,   3,   0, 
     15, 160, 244, 253, 148,  63, 
    233,  38,   1,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     31,   0,   0,   2,   0,   0, 
      0, 128,   0,   0,  15, 176, 
     31,   0,   0,   2,   0,   0, 
      0, 128,   1,   0,   3, 176, 
     31,   0,   0,   2,   0,   0, 
      0, 144,   0,   8,  15, 160, 
     31,   0,   0,   2,   0,   0, 
      0, 144,   1,   8,  15, 160, 
     31,   0,   0,   2,   0,   0, 
      0, 144,   2,   8,  15, 160, 
     66,   0,   0,   3,   0,   0, 
     15, 128,   1,   0, 228, 176, 
      0,   8, 228, 160,  66,   0, 
      0,   3,   1,   0,  15, 128, 
      1,   0, 228, 176,   1,   8, 
    228, 160,  66,   0,   0,   3, 
      2,   0,  15, 128,   1,   0, 
    228, 176,   2,   8, 228, 160, 
      1,   0,   0,   2,   0,   0, 
      2, 128,   1,   0,   0, 128, 
      1,   0,   0,   2,   0,   0, 
      4, 128,   2,   0,   0, 128, 
      2,   0,   0,   3,   0,   0, 
      7, 128,   0,   0, 228, 128, 
      0,   0, 228, 160,   5,   0, 
      0,   3,   0,   0,   8, 128, 
      0,   0,   0, 128,   1,   0, 
      0, 160,   4,   0,   0,   4, 
      1,   0,   1, 128,   0,   0, 
    170, 128,   1,   0,  85, 160, 
      0,   0, 255, 128,   8,   0, 
      0,   3,   1,   0,   2, 128, 
      0,   0, 228, 128,   2,   0, 
    228, 160,  90,   0,   0,   4, 
      1,   0,   4, 128,   0,   0, 
    228, 128,   3,   0, 228, 160, 
      3,   0, 170, 160,   1,   0, 
      0,   2,   1,   0,   8, 128, 
      0,   0, 255, 160,   5,   0, 
      0,   3,   0,   0,  15, 128, 
      1,   0, 228, 128,   0,   0, 
    228, 176,   1,   0,   0,   2, 
      0,   8,  15, 128,   0,   0, 
    228, 128, 255, 255,   0,   0, 
     83,  72,  68,  82, 216,   1, 
      0,   0,  64,   0,   0,   0, 
    118,   0,   0,   0,  90,   0, 
      0,   3,   0,  96,  16,   0, 
      0,   0,   0,   0,  88,  24, 
      0,   4,   0, 112,  16,   0, 
      0,   0,   0,   0,  85,  85, 
      0,   0,  88,  24,   0,   4, 
      0, 112,  16,   0,   1,   0, 
      0,   0,  85,  85,   0,   0, 
     88,  24,   0,   4,   0, 112, 
     16,   0,   2,   0,   0,   0, 
     85,  85,   0,   0,  98,  16, 
      0,   3, 242,  16,  16,   0, 
      1,   0,   0,   0,  98,  16, 
      0,   3,  50,  16,  16,   0, 
      2,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      0,   0,   0,   0, 104,   0, 
      0,   2,   2,   0,   0,   0, 
     69,   0,   0,   9, 242,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  16,  16,   0,   2,   0, 
      0,   0,  70, 126,  16,   0, 
      0,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
     69,   0,   0,   9, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     70,  16,  16,   0,   2,   0, 
      0,   0,  70, 126,  16,   0, 
      1,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
     54,   0,   0,   5,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   1,   0, 
      0,   0,  69,   0,   0,   9, 
    242,   0,  16,   0,   1,   0, 
      0,   0,  70,  16,  16,   0, 
      2,   0,   0,   0,  70, 126, 
     16,   0,   2,   0,   0,   0, 
      0,  96,  16,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
     66,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,   0,   0, 
      0,  10, 114,   0,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0, 129, 128, 
    128, 189, 129, 128,   0, 191, 
    129, 128,   0, 191,   0,   0, 
      0,   0,  15,   0,   0,  10, 
     18,   0,  16,   0,   1,   0, 
      0,   0, 134,   0,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0, 244, 253, 148,  63, 
    186,  73, 204,  63,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     16,   0,   0,  10,  34,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
    244, 253, 148,  63,  39,  49, 
    200, 190, 197,  32,  80, 191, 
      0,   0,   0,   0,  15,   0, 
      0,  10,  66,   0,  16,   0, 
      1,   0,   0,   0,  70,   0, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0, 244, 253, 
    148,  63, 233,  38,   1,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
    130,   0,  16,   0,   1,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  56,   0, 
      0,   7, 242,  32,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     70,  30,  16,   0,   1,   0, 
      0,   0,  62,   0,   0,   1, 
     73,  83,  71,  78, 108,   0, 
      0,   0,   3,   0,   0,   0, 
      8,   0,   0,   0,  80,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  92,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,  15,   0,   0,  98,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
      3,   3,   0,   0,  83,  86, 
     95,  80,  79,  83,  73,  84, 
     73,  79,  78,   0,  67,  79, 
     76,  79,  82,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0, 171,  79,  83,  71,  78, 
     44,   0,   0,   0,   1,   0, 
      0,   0,   8,   0,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     83,  86,  95,  84,  65,  82, 
     71,  69,  84,   0, 171, 171
};