/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <sstream>
#include <string>

namespace vglx::math {

using Pair = struct { float x, y; };

constexpr float pi = 3.1415926535897932384626433832795f;
constexpr float two_pi = 6.2831853071795864769252867665590f;
constexpr float pi_over_2 = 1.5707963267948966192313216916398f;
constexpr float pi_over_4 = 0.78539816339744830961566084581988f;
constexpr float pi_over_6 = 0.52359877559829887307710723054658f;
constexpr float tau = 6.2831853071795864769252867665590f;
constexpr float tau_over_2 = 3.1415926535897932384626433832795f;
constexpr float tau_over_4 = 1.5707963267948966192313216916398f;
constexpr float tau_over_6 = 1.0471975511965977461542144610932f;
constexpr float tau_over_256 = 0.0245436926f;
constexpr float inv_tau = 40.74366543f;
constexpr float ln_2 = 0.69314718055994530941723212145818F;
constexpr float eps = 1e-6f;

alignas(64) inline constexpr auto trig_table = std::array<std::array<uint32_t, 2>, 256> {{
	{0x3F800000, 0x00000000}, {0x3F7FEC43, 0x3CC90AB0}, {0x3F7FB10F, 0x3D48FB30}, {0x3F7F4E6D, 0x3D96A905}, {0x3F7EC46D, 0x3DC8BD36}, {0x3F7E1324, 0x3DFAB273}, {0x3F7D3AAC, 0x3E164083}, {0x3F7C3B28, 0x3E2F10A3},
	{0x3F7B14BE, 0x3E47C5C2}, {0x3F79C79D, 0x3E605C13}, {0x3F7853F8, 0x3E78CFCD}, {0x3F76BA07, 0x3E888E94}, {0x3F74FA0B, 0x3E94A031}, {0x3F731447, 0x3EA09AE5}, {0x3F710908, 0x3EAC7CD4}, {0x3F6ED89E, 0x3EB8442A},
	{0x3F6C835E, 0x3EC3EF16}, {0x3F6A09A6, 0x3ECF7BCB}, {0x3F676BD8, 0x3EDAE880}, {0x3F64AA59, 0x3EE63375}, {0x3F61C597, 0x3EF15AEA}, {0x3F5EBE05, 0x3EFC5D28}, {0x3F5B941A, 0x3F039C3D}, {0x3F584853, 0x3F08F59B},
	{0x3F54DB31, 0x3F0E39DA}, {0x3F514D3D, 0x3F13682B}, {0x3F4D9F02, 0x3F187FC0}, {0x3F49D112, 0x3F1D7FD2}, {0x3F45E403, 0x3F22679A}, {0x3F41D870, 0x3F273656}, {0x3F3DAEF9, 0x3F2BEB4A}, {0x3F396842, 0x3F3085BB},
	{0x3F3504F3, 0x3F3504F3}, {0x3F3085BA, 0x3F396842}, {0x3F2BEB49, 0x3F3DAEFA}, {0x3F273655, 0x3F41D871}, {0x3F226799, 0x3F45E403}, {0x3F1D7FD1, 0x3F49D112}, {0x3F187FC0, 0x3F4D9F02}, {0x3F13682A, 0x3F514D3D},
	{0x3F0E39D9, 0x3F54DB32}, {0x3F08F59B, 0x3F584853}, {0x3F039C3C, 0x3F5B941B}, {0x3EFC5D27, 0x3F5EBE05}, {0x3EF15AE7, 0x3F61C598}, {0x3EE63374, 0x3F64AA59}, {0x3EDAE881, 0x3F676BD8}, {0x3ECF7BC9, 0x3F6A09A7},
	{0x3EC3EF15, 0x3F6C835E}, {0x3EB84427, 0x3F6ED89E}, {0x3EAC7CD3, 0x3F710908}, {0x3EA09AE2, 0x3F731448}, {0x3E94A030, 0x3F74FA0B}, {0x3E888E93, 0x3F76BA07}, {0x3E78CFC8, 0x3F7853F8}, {0x3E605C12, 0x3F79C79D},
	{0x3E47C5BC, 0x3F7B14BF}, {0x3E2F10A0, 0x3F7C3B28}, {0x3E164085, 0x3F7D3AAC}, {0x3DFAB26C, 0x3F7E1324}, {0x3DC8BD35, 0x3F7EC46D}, {0x3D96A8FB, 0x3F7F4E6D}, {0x3D48FB29, 0x3F7FB10F}, {0x3CC90A7E, 0x3F7FEC43},
	{0x00000000, 0x3F800000}, {0xBCC90A7E, 0x3F7FEC43}, {0xBD48FB29, 0x3F7FB10F}, {0xBD96A8FB, 0x3F7F4E6D}, {0xBDC8BD35, 0x3F7EC46D}, {0xBDFAB26C, 0x3F7E1324}, {0xBE164085, 0x3F7D3AAC}, {0xBE2F10A0, 0x3F7C3B28},
	{0xBE47C5BC, 0x3F7B14BF}, {0xBE605C12, 0x3F79C79D}, {0xBE78CFC8, 0x3F7853F8}, {0xBE888E93, 0x3F76BA07}, {0xBE94A030, 0x3F74FA0B}, {0xBEA09AE2, 0x3F731448}, {0xBEAC7CD3, 0x3F710908}, {0xBEB84427, 0x3F6ED89E},
	{0xBEC3EF15, 0x3F6C835E}, {0xBECF7BC9, 0x3F6A09A7}, {0xBEDAE881, 0x3F676BD8}, {0xBEE63374, 0x3F64AA59}, {0xBEF15AE7, 0x3F61C598}, {0xBEFC5D27, 0x3F5EBE05}, {0xBF039C3C, 0x3F5B941B}, {0xBF08F59B, 0x3F584853},
	{0xBF0E39D9, 0x3F54DB32}, {0xBF13682A, 0x3F514D3D}, {0xBF187FC0, 0x3F4D9F02}, {0xBF1D7FD1, 0x3F49D112}, {0xBF226799, 0x3F45E403}, {0xBF273655, 0x3F41D871}, {0xBF2BEB49, 0x3F3DAEFA}, {0xBF3085BA, 0x3F396842},
	{0xBF3504F3, 0x3F3504F3}, {0xBF396842, 0x3F3085BB}, {0xBF3DAEF9, 0x3F2BEB4A}, {0xBF41D870, 0x3F273656}, {0xBF45E403, 0x3F22679A}, {0xBF49D112, 0x3F1D7FD2}, {0xBF4D9F02, 0x3F187FC0}, {0xBF514D3D, 0x3F13682B},
	{0xBF54DB31, 0x3F0E39DA}, {0xBF584853, 0x3F08F59B}, {0xBF5B941A, 0x3F039C3D}, {0xBF5EBE05, 0x3EFC5D28}, {0xBF61C597, 0x3EF15AEA}, {0xBF64AA59, 0x3EE63375}, {0xBF676BD8, 0x3EDAE880}, {0xBF6A09A6, 0x3ECF7BCB},
	{0xBF6C835E, 0x3EC3EF16}, {0xBF6ED89E, 0x3EB8442A}, {0xBF710908, 0x3EAC7CD4}, {0xBF731447, 0x3EA09AE5}, {0xBF74FA0B, 0x3E94A031}, {0xBF76BA07, 0x3E888E94}, {0xBF7853F8, 0x3E78CFCD}, {0xBF79C79D, 0x3E605C13},
	{0xBF7B14BE, 0x3E47C5C2}, {0xBF7C3B28, 0x3E2F10A3}, {0xBF7D3AAC, 0x3E164083}, {0xBF7E1324, 0x3DFAB273}, {0xBF7EC46D, 0x3DC8BD36}, {0xBF7F4E6D, 0x3D96A905}, {0xBF7FB10F, 0x3D48FB30}, {0xBF7FEC43, 0x3CC90AB0},
	{0xBF800000, 0x00000000}, {0xBF7FEC43, 0xBCC90AB0}, {0xBF7FB10F, 0xBD48FB30}, {0xBF7F4E6D, 0xBD96A905}, {0xBF7EC46D, 0xBDC8BD36}, {0xBF7E1324, 0xBDFAB273}, {0xBF7D3AAC, 0xBE164083}, {0xBF7C3B28, 0xBE2F10A3},
	{0xBF7B14BE, 0xBE47C5C2}, {0xBF79C79D, 0xBE605C13}, {0xBF7853F8, 0xBE78CFCD}, {0xBF76BA07, 0xBE888E94}, {0xBF74FA0B, 0xBE94A031}, {0xBF731447, 0xBEA09AE5}, {0xBF710908, 0xBEAC7CD4}, {0xBF6ED89E, 0xBEB8442A},
	{0xBF6C835E, 0xBEC3EF16}, {0xBF6A09A6, 0xBECF7BCB}, {0xBF676BD8, 0xBEDAE880}, {0xBF64AA59, 0xBEE63375}, {0xBF61C597, 0xBEF15AEA}, {0xBF5EBE05, 0xBEFC5D28}, {0xBF5B941A, 0xBF039C3D}, {0xBF584853, 0xBF08F59B},
	{0xBF54DB31, 0xBF0E39DA}, {0xBF514D3D, 0xBF13682B}, {0xBF4D9F02, 0xBF187FC0}, {0xBF49D112, 0xBF1D7FD2}, {0xBF45E403, 0xBF22679A}, {0xBF41D870, 0xBF273656}, {0xBF3DAEF9, 0xBF2BEB4A}, {0xBF396842, 0xBF3085BB},
	{0xBF3504F3, 0xBF3504F3}, {0xBF3085BA, 0xBF396842}, {0xBF2BEB49, 0xBF3DAEFA}, {0xBF273655, 0xBF41D871}, {0xBF226799, 0xBF45E403}, {0xBF1D7FD1, 0xBF49D112}, {0xBF187FC0, 0xBF4D9F02}, {0xBF13682A, 0xBF514D3D},
	{0xBF0E39D9, 0xBF54DB32}, {0xBF08F59B, 0xBF584853}, {0xBF039C3C, 0xBF5B941B}, {0xBEFC5D27, 0xBF5EBE05}, {0xBEF15AE7, 0xBF61C598}, {0xBEE63374, 0xBF64AA59}, {0xBEDAE881, 0xBF676BD8}, {0xBECF7BC9, 0xBF6A09A7},
	{0xBEC3EF15, 0xBF6C835E}, {0xBEB84427, 0xBF6ED89E}, {0xBEAC7CD3, 0xBF710908}, {0xBEA09AE2, 0xBF731448}, {0xBE94A030, 0xBF74FA0B}, {0xBE888E93, 0xBF76BA07}, {0xBE78CFC8, 0xBF7853F8}, {0xBE605C12, 0xBF79C79D},
	{0xBE47C5BC, 0xBF7B14BF}, {0xBE2F10A0, 0xBF7C3B28}, {0xBE164085, 0xBF7D3AAC}, {0xBDFAB26C, 0xBF7E1324}, {0xBDC8BD35, 0xBF7EC46D}, {0xBD96A8FB, 0xBF7F4E6D}, {0xBD48FB29, 0xBF7FB10F}, {0xBCC90A7E, 0xBF7FEC43},
	{0x00000000, 0xBF800000}, {0x3CC90A7E, 0xBF7FEC43}, {0x3D48FB29, 0xBF7FB10F}, {0x3D96A8FB, 0xBF7F4E6D}, {0x3DC8BD35, 0xBF7EC46D}, {0x3DFAB26C, 0xBF7E1324}, {0x3E164085, 0xBF7D3AAC}, {0x3E2F10A0, 0xBF7C3B28},
	{0x3E47C5BC, 0xBF7B14BF}, {0x3E605C12, 0xBF79C79D}, {0x3E78CFC8, 0xBF7853F8}, {0x3E888E93, 0xBF76BA07}, {0x3E94A030, 0xBF74FA0B}, {0x3EA09AE2, 0xBF731448}, {0x3EAC7CD3, 0xBF710908}, {0x3EB84427, 0xBF6ED89E},
	{0x3EC3EF15, 0xBF6C835E}, {0x3ECF7BC9, 0xBF6A09A7}, {0x3EDAE881, 0xBF676BD8}, {0x3EE63374, 0xBF64AA59}, {0x3EF15AE7, 0xBF61C598}, {0x3EFC5D27, 0xBF5EBE05}, {0x3F039C3C, 0xBF5B941B}, {0x3F08F59B, 0xBF584853},
	{0x3F0E39D9, 0xBF54DB32}, {0x3F13682A, 0xBF514D3D}, {0x3F187FC0, 0xBF4D9F02}, {0x3F1D7FD1, 0xBF49D112}, {0x3F226799, 0xBF45E403}, {0x3F273655, 0xBF41D871}, {0x3F2BEB49, 0xBF3DAEFA}, {0x3F3085BA, 0xBF396842},
	{0x3F3504F3, 0xBF3504F3}, {0x3F396842, 0xBF3085BB}, {0x3F3DAEF9, 0xBF2BEB4A}, {0x3F41D870, 0xBF273656}, {0x3F45E403, 0xBF22679A}, {0x3F49D112, 0xBF1D7FD2}, {0x3F4D9F02, 0xBF187FC0}, {0x3F514D3D, 0xBF13682B},
	{0x3F54DB31, 0xBF0E39DA}, {0x3F584853, 0xBF08F59B}, {0x3F5B941A, 0xBF039C3D}, {0x3F5EBE05, 0xBEFC5D28}, {0x3F61C597, 0xBEF15AEA}, {0x3F64AA59, 0xBEE63375}, {0x3F676BD8, 0xBEDAE880}, {0x3F6A09A6, 0xBECF7BCB},
	{0x3F6C835E, 0xBEC3EF16}, {0x3F6ED89E, 0xBEB8442A}, {0x3F710908, 0xBEAC7CD4}, {0x3F731447, 0xBEA09AE5}, {0x3F74FA0B, 0xBE94A031}, {0x3F76BA07, 0xBE888E94}, {0x3F7853F8, 0xBE78CFCD}, {0x3F79C79D, 0xBE605C13},
	{0x3F7B14BE, 0xBE47C5C2}, {0x3F7C3B28, 0xBE2F10A3}, {0x3F7D3AAC, 0xBE164083}, {0x3F7E1324, 0xBDFAB273}, {0x3F7EC46D, 0xBDC8BD36}, {0x3F7F4E6D, 0xBD96A905}, {0x3F7FB10F, 0xBD48FB30}, {0x3F7FEC43, 0xBCC90AB0}
}};

[[nodiscard]] constexpr auto GetTrigPair(int32_t index) {
    return std::bit_cast<Pair>(trig_table[index & 255]);
}

alignas(64) inline constexpr auto arctan_table = std::array<uint32_t, 65> {
    0x00000000, 0x3C7FFAAB, 0x3CFFEAAE, 0x3D3FDC0C, 0x3D7FAADE, 0x3D9FACF8, 0x3DBF70C1, 0x3DDF1CF6,
    0x3DFEADD5, 0x3E0F0FD8, 0x3E1EB777, 0x3E2E4C09, 0x3E3DCBDA, 0x3E4D3547, 0x3E5C86BB, 0x3E6BBEAF,
    0x3E7ADBB0, 0x3E84EE2D, 0x3E8C5FAD, 0x3E93C1B9, 0x3E9B13BA, 0x3EA25522, 0x3EA9856D, 0x3EB0A420,
    0x3EB7B0CA, 0x3EBEAB02, 0x3EC5926A, 0x3ECC66AA, 0x3ED32776, 0x3ED9D489, 0x3EE06DA6, 0x3EE6F29A,
    0x3EED6338, 0x3EF3BF5C, 0x3EFA06E8, 0x3F001CE4, 0x3F032BF5, 0x3F0630A3, 0x3F092AED, 0x3F0C1AD4,
    0x3F0F005D, 0x3F11DB8F, 0x3F14AC73, 0x3F177314, 0x3F1A2F81, 0x3F1CE1C9, 0x3F1F89FE, 0x3F222833,
    0x3F24BC7D, 0x3F2746F3, 0x3F29C7AC, 0x3F2C3EC1, 0x3F2EAC4C, 0x3F311069, 0x3F336B32, 0x3F35BCC5,
    0x3F38053E, 0x3F3A44BC, 0x3F3C7B5E, 0x3F3EA941, 0x3F40CE86, 0x3F42EB4B, 0x3F44FFB0, 0x3F470BD5, 0x3F490FDB
};

alignas(64) inline constexpr auto exp_val_table = std::array<uint32_t, 180> {
    0x00000000, 0x00B33687, 0x01739362, 0x022586E0, 0x02E0F96D, 0x0398E2CB, 0x044FCB22, 0x050D35D7,
    0x05BFECBA, 0x06826D27, 0x07314490, 0x07F0EE94, 0x08A3BAF0, 0x095E884F, 0x0A1739FB, 0x0ACD89C1,
    0x0B8BAD78, 0x0C3DD771, 0x0D0102BF, 0x0DAF5800, 0x0E6E511E, 0x0F21F3FE, 0x0FDC1DF9, 0x109595C7,
    0x114B4EA4, 0x120A295C, 0x12BBC7F1, 0x137F388B, 0x142D70C9, 0x14EBBAEC, 0x15A031FC, 0x1659BA5A,
    0x1713F623, 0x17C919B9, 0x1888A975, 0x1939BE2B, 0x19FC7361, 0x1AAB8EDC, 0x1B692BEB, 0x1C1E74DD,
    0x1CD75D5D, 0x1D925B02, 0x1E46EAF1, 0x1F072DBA, 0x1FB7BA0F, 0x2079B5EA, 0x2129B229, 0x21E6A405,
    0x229CBC92, 0x235506F2, 0x2410C457, 0x24C4C239, 0x2585B61D, 0x2635BB8D, 0x26F7000F, 0x27A7DAA4,
    0x28642328, 0x291B090F, 0x29D2B706, 0x2A8F3216, 0x2B429F81, 0x2C044295, 0x2CB3C295, 0x2D7451BD,
    0x2E26083C, 0x2EE1A93F, 0x2F995A46, 0x30506D87, 0x310DA433, 0x31C082B8, 0x3282D314, 0x3331CF19,
    0x33F1AADE, 0x34A43AE5, 0x355F3638, 0x3617B02A, 0x36CE2A62, 0x378C1AA1, 0x383E6BCE, 0x39016791,
    0x39AFE108, 0x3A6F0B5D, 0x3B227290, 0x3BDCC9FF, 0x3C960AAE, 0x3D4BED86, 0x3E0A9555, 0x3EBC5AB2,
    0x3F800000, 0x402DF854, 0x40EC7326, 0x41A0AF2E, 0x425A6481, 0x431469C5, 0x43C9B6E3, 0x44891443,
    0x453A4F54, 0x45FD38AC, 0x46AC14EE, 0x4769E224, 0x481EF0B3, 0x48D805AD, 0x4992CD62, 0x4A478665,
    0x4B07975F, 0x4BB849A4, 0x4C7A7910, 0x4D2A36C8, 0x4DE75844, 0x4E9D3710, 0x4F55AD6E, 0x5011357A,
    0x50C55BFE, 0x51861E9D, 0x52364993, 0x52F7C118, 0x53A85DD2, 0x5464D572, 0x551B8238, 0x55D35BB3,
    0x568FA1FE, 0x5743379A, 0x5804A9F1, 0x58B44F11, 0x597510AD, 0x5A2689FE, 0x5AE2599A, 0x5B99D21F,
    0x5C51106A, 0x5D0E12E4, 0x5DC1192B, 0x5E833952, 0x5F325A0E, 0x5FF267BB, 0x60A4BB3E, 0x615FE4A9,
    0x621826B5, 0x62CECB81, 0x638C881F, 0x643F009E, 0x6501CCB3, 0x65B06A7B, 0x666FC62D, 0x6722F184,
    0x67DD768B, 0x68967FF0, 0x694C8CE5, 0x6A0B01A3, 0x6ABCEDE5, 0x6B806408, 0x6C2E804A, 0x6CED2BEF,
    0x6DA12CC1, 0x6E5B0F2E, 0x6F14DDC1, 0x6FCA5487, 0x70897F64, 0x713AE0EE, 0x71FDFE91, 0x72AC9B6A,
    0x736A98EC, 0x741F6CE9, 0x74D8AE7F, 0x7593401C, 0x76482254, 0x77080156, 0x77B8D9AA, 0x787B3CCF,
    0x792ABBCE, 0x79E80D11, 0x7A9DB1ED, 0x7B56546B, 0x7C11A6F5, 0x7CC5F63B, 0x7D86876D, 0x7E36D809,
    0x7EF882B7, 0x7F800000
};

[[nodiscard]] constexpr auto GetExpValue(int32_t index) {
    return std::bit_cast<float>(exp_val_table[index]);
}

/**
 * @brief Clamps a value to the inclusive range [lo, hi].
 * @ingroup MathGroup
 *
 * @param v Input value.
 * @param lo Lower bound (inclusive).
 * @param hi Upper bound (inclusive).
 * @return Value clamped to [lo, hi].
 */
[[nodiscard]] constexpr auto Clamp(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

/**
 * @brief Converts degrees to radians.
 * @ingroup MathGroup
 *
 * @param degrees Angle in degrees.
 * @return Angle in radians.
 */
[[nodiscard]] constexpr auto DegToRad(const float degrees) {
    return degrees * pi / 180.0f;
}

/**
 * @brief Converts radians to degrees.
 * @ingroup MathGroup
 *
 * @param radians Angle in radians.
 * @return Angle in degrees.
 */
[[nodiscard]] constexpr auto RadToDeg(const float radians) {
    return radians * 180.0f / pi;
}

/**
 * @brief Linearly interpolates between two values.
 * @ingroup MathGroup
 *
 * @param a Start value.
 * @param b End value.
 * @param f Interpolation factor in [0, 1].
 * @return Interpolated value.
 */
[[nodiscard]] constexpr auto Lerp(const float a, const float b, const float f) {
    return std::lerp(a, b, f);
}

/**
 * @brief Computes the Cantor pairing of two values.
 * @ingroup MathGroup
 *
 * @tparam T Integer-like type.
 * @param x First value.
 * @param y Second value.
 * @return Unique paired value.
 */
[[nodiscard]] constexpr auto CantorPairing(const auto x, const auto y) {
    return ((x + y) * (x + y + 1)) / 2 + y;
}

/**
 * @brief Computes unordered Cantor pairing (order-invariant).
 * @ingroup MathGroup
 *
 * @tparam T Integer-like type.
 * @param x First value.
 * @param y Second value.
 * @return Unique paired value, independent of input order.
 */
[[nodiscard]] constexpr auto CantorPairingUnordered(const auto x, const auto y) {
    return x > y ? CantorPairing(y, x) : CantorPairing(x, y);
}

/**
 * @brief Returns the absolute value.
 * @ingroup MathGroup
 *
 * @param x Input float.
 * @return Absolute value.
 */
[[nodiscard]] constexpr auto Fabs(float x) {
    return (x < 0.0F) ? -x : x;
}
/**
 * @brief Approximates the exponential function e^x.
 *
 * Uses a combination of a precomputed integer-range lookup table
 * and a Taylor series polynomial correction for the fractional part.
 *
 * @param x Input exponent.
 */
[[nodiscard]] constexpr auto Exp(float x) -> float {
    if (x > 91.0f) return std::numeric_limits<float>::infinity();
    if (x < -88.0f) return 0.0f;

    const auto ix = static_cast<int32_t>(x);
    const auto f = static_cast<float>(ix - (x < static_cast<float>(ix)));
    const auto i = static_cast<int32_t>(f) + 88;
    if (i > 0) {
        x -= f;
        float r = 1.0f / 362880.0f;
        r = r * x + 1.0f / 40320.0f;
        r = r * x + 1.0f / 5040.0f;
        r = r * x + 1.0f / 720.0f;
        r = r * x + 1.0f / 120.0f;
        r = r * x + 1.0f / 24.0f;
        r = r * x + 1.0f / 6.0f;
        r = r * x + 0.5f;

        float poly = (r * (x * x) + (x + 1.0f));
        return poly * GetExpValue(i < 179 ? i : 179);
    }
    return 0.0f;
}

/**
 * @brief Approximates the natural logarithm ln(x).
 *
 * Uses bit-manipulation to decompose the input into mantissa and exponent,
 * applying the identity ln(m * 2^e) = ln(m) + e * ln(2). The mantissa
 * is approximated using a Taylor series polynomial.
 *
 * @param x Input value (must be greater than 0).
 */
[[nodiscard]] constexpr auto Log(float x) -> float {
    if (x > 0) {
        auto i = std::bit_cast<int32_t>(x);
        auto e = (i >> 23) - 127;

        i = (i & 0x007FFFFF) | 0x3F800000;
        auto f = std::bit_cast<float>(i);

        if (f > 1.41421356f) {
            f *= 0.5f;
            e++;
        }

        auto m = f - 1.0f;
        auto r = 1.0f / 7.0f;
        r = r * m - 1.0f / 6.0f;
        r = r * m + 1.0f / 5.0f;
        r = r * m - 1.0f / 4.0f;
        r = r * m + 1.0f / 3.0f;
        r = r * m - 0.5f;

        return (r * (m * m) + m) + (static_cast<float>(e) * ln_2);
    }

    if (x == 0.0f) return -std::numeric_limits<float>::infinity();

    return std::numeric_limits<float>::quiet_NaN();
}

/**
 * @brief Computes base raised to the power of exponent (base^exponent).
 *
 * @param exponent The exponent value.
 */
[[nodiscard]] constexpr auto Pow(float base, float exponent) -> float {
    if (exponent == 0.0f) return 1.0f;
    if (base == 0.0f) return (exponent > 0.0f) ? 0.0f : std::numeric_limits<float>::infinity();
    if (base < 0.0f) return std::numeric_limits<float>::quiet_NaN();

    return Exp(Log(base) * exponent);
}

/**
 * @brief Computes square root using fast inverse sqrt refinement.
 * @ingroup MathGroup
 *
 * @param x Input value.
 * @return Approximated square root.
 */
[[nodiscard]] constexpr auto Sqrt(float x) {
    if (x <= 0.0f) {
        return 0.0f;
    }

    auto i = std::bit_cast<uint32_t>(x);
    i = 0x5F375A86 - (i >> 1);
    auto r = std::bit_cast<float>(i);
    r = (0.5f * r) * (3.0f - x * r * r);
    r = (0.5f * r) * (3.0f - x * r * r);

    return r * x;
}

/**
 * @brief Computes inverse square root using fast inverse sqrt refinement.
 * @ingroup MathGroup
 *
 * @param x Input value.
 * @return Approximated 1 / sqrt(x).
 */
[[nodiscard]] constexpr auto InverseSqrt(float x) {
    if (x <= 0.0f) {
        return std::numeric_limits<float>::infinity();
    }

    auto i = std::bit_cast<uint32_t>(x);
    i = 0x5F375A86 - (i >> 1);
    auto r = std::bit_cast<float>(i);
    r = (0.5f * r) * (3.0f - x * r * r);
    r = (0.5f * r) * (3.0f - x * r * r);

    return r;
}

/**
 * @brief Approximates cosine using lookup + polynomial correction.
 * @ingroup MathGroup
 *
 * @param x Angle in radians.
 * @return Approximate cosine.
 */
[[nodiscard]] constexpr auto Cos(float x) {
    auto b = Fabs(x) * inv_tau;
    auto i = static_cast<int32_t>(b);
    b = (b - float(i)) * tau_over_256;

    auto cossin_alpha = GetTrigPair(i & 255);

    auto b2 = b * b;
    auto sine_beta = b - b * b2 * (0.1666666667f - b2 * 0.0083333333f);
    auto cosine_beta = 1.0f - b2 * (0.5f - b2 * 0.0416666667f);

    return cossin_alpha.x * cosine_beta - cossin_alpha.y * sine_beta;
}

/**
 * @brief Approximates sine using lookup + polynomial correction.
 * @ingroup MathGroup
 *
 * @param x Angle in radians.
 * @return Approximate sine.
 */
[[nodiscard]] constexpr auto Sin(float x) {
    auto b = Fabs(x) * inv_tau;
    auto i = static_cast<int32_t>(b);
    b = (b - float(i)) * tau_over_256;

    auto cossin_alpha = GetTrigPair(i & 255);

    auto b2 = b * b;
    auto sine_beta = b - b * b2 * (0.1666666667f - b2 * 0.0083333333f);
    auto cosine_beta = 1.0f - b2 * (0.5f - b2 * 0.0416666667f);

    auto sine = cossin_alpha.y * cosine_beta + cossin_alpha.x * sine_beta;
    return x < 0.0f ? -sine : sine;
}

/**
 * @brief Approximates arctangent of a single value.
 * @ingroup MathGroup
 *
 * Uses piecewise approximation with a small lookup table.
 *
 * @param x Input value.
 * @return atan(x) in radians.
 */
[[nodiscard]] constexpr auto Atan(float x) {
    auto a = Fabs(x);

    if (a <= 1.0f) {
        auto b = a * 64.0f;
        auto i = static_cast<int32_t>(b);
        b = float(i) * 0.015625f; // inv_table_size: 1.0f / 64.0f

        auto arctan_b = std::bit_cast<float>(arctan_table[i]);
        auto c = (a - b) / (a * b + 1.0F);
        auto c2 = c * c;

        auto arctan_c = c * (1.0F - c2 * (0.3333333333f + c2 * (0.2f - c2 * 0.1428571429f)));
        a = arctan_b + arctan_c;
    } else {
        a = 1.0f / a;
        auto b = a * 64.0f;
        auto i = static_cast<int32_t>(b);
        b = float(i) * 0.015625f;

        auto arctan_b = std::bit_cast<float>(arctan_table[i]);
        auto c = (a - b) / (a * b + 1.0F);
        auto c2 = c * c;

        auto arctan_c = c * (1.0F - c2 * (0.3333333333f + c2 * (0.2f - c2 * 0.1428571429f)));
        a = tau_over_4 - (arctan_b + arctan_c);
    }

    return x < 0.0f ? -a : a;
}

/**
 * @brief Approximates atan2 with quadrant correction.
 * @ingroup MathGroup
 *
 * @param y Y-coordinate.
 * @param x X-coordinate.
 * @return Angle in radians from positive X-axis.
 */
[[nodiscard]] constexpr auto Atan2(float y, float x) {
    if (Fabs(x) > eps) {
        auto r = Atan(y / x);
        if (x < 0.0f) {
            return y >= 0.0f ? r + math::pi : r - math::pi;
        }
        return r;
    }

    if (Fabs(y) > eps) {
        return y > 0.0f ? math::pi_over_2 : -math::pi_over_2;
    }

    return 0.0f;
}

/**
 * @brief Approximates arcsine using arctangent and inverse sqrt.
 * @ingroup MathGroup
 *
 * @param y Sine value in [-1, 1].
 * @return Angle in radians.
 */
[[nodiscard]] constexpr auto Asin(float y) {
    return (Atan(y * InverseSqrt(1.0f - y * y)));
}

/**
 * @brief Converts an sRGB channel value to linear.
 *
 * @param c Input and output are in the range [0, 1].
 */
[[nodiscard]] constexpr auto SRGBToLinear(float c) -> float {
    c = Clamp(c, 0.0f, 1.0f);
    if (c <= 0.04045f) {
        return c / 12.92f;
    }
    return Pow((c + 0.055f) / 1.055f, 2.4f);
}

/**
 * @brief Converts a linear channel value to sRGB.
 *
 * @param c Input and output are in the range [0, 1].
 */
[[nodiscard]] constexpr auto LinearToSRGB(float c) -> float {
    c = Clamp(c, 0.0f, 1.0f);
    if (c <= 0.0031308f) {
        return c * 12.92f;
    }
    return 1.055f * Pow(c, 1.0f / 2.4f) - 0.055f;
}

/**
 * @brief Combines a seed with a new value to produce a composite hash.
 * @ingroup MathGroup
 *
 * @param seed The existing hash seed to be updated.
 * @param value The value to hash and combine into the seed.
 */
template <typename T>
inline auto HashCombine(size_t& seed, const T& value) -> void {
    seed ^= std::hash<T>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

/**
 * @brief Generates a UUID string (version 4-like).
 * @ingroup MathGroup
 *
 * @return Random UUID as a string.
 */
[[nodiscard]] inline auto GenerateUUID() -> std::string {
    static std::vector<std::string> lut{
        "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0a",
        "0b", "0c", "0d", "0e", "0f", "10", "11", "12", "13", "14", "15",
        "16", "17", "18", "19", "1a", "1b", "1c", "1d", "1e", "1f", "20",
        "21", "22", "23", "24", "25", "26", "27", "28", "29", "2a", "2b",
        "2c", "2d", "2e", "2f", "30", "31", "32", "33", "34", "35", "36",
        "37", "38", "39", "3a", "3b", "3c", "3d", "3e", "3f", "40", "41",
        "42", "43", "44", "45", "46", "47", "48", "49", "4a", "4b", "4c",
        "4d", "4e", "4f", "50", "51", "52", "53", "54", "55", "56", "57",
        "58", "59", "5a", "5b", "5c", "5d", "5e", "5f", "60", "61", "62",
        "63", "64", "65", "66", "67", "68", "69", "6a", "6b", "6c", "6d",
        "6e", "6f", "70", "71", "72", "73", "74", "75", "76", "77", "78",
        "79", "7a", "7b", "7c", "7d", "7e", "7f", "80", "81", "82", "83",
        "84", "85", "86", "87", "88", "89", "8a", "8b", "8c", "8d", "8e",
        "8f", "90", "91", "92", "93", "94", "95", "96", "97", "98", "99",
        "9a", "9b", "9c", "9d", "9e", "9f", "a0", "a1", "a2", "a3", "a4",
        "a5", "a6", "a7", "a8", "a9", "aa", "ab", "ac", "ad", "ae", "af",
        "b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8", "b9", "ba",
        "bb", "bc", "bd", "be", "bf", "c0", "c1", "c2", "c3", "c4", "c5",
        "c6", "c7", "c8", "c9", "ca", "cb", "cc", "cd", "ce", "cf", "d0",
        "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "da", "db",
        "dc", "dd", "de", "df", "e0", "e1", "e2", "e3", "e4", "e5", "e6",
        "e7", "e8", "e9", "ea", "eb", "ec", "ed", "ee", "ef", "f0", "f1",
        "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "fa", "fb", "fc",
        "fd", "fe", "ff"
    };

    static auto rd = std::random_device {};
    static auto e2 = std::mt19937 {rd()};
    static auto dist = std::uniform_int_distribution<uint32_t> {};

    const auto d0 = dist(e2);
    const auto d1 = dist(e2);
    const auto d2 = dist(e2);
    const auto d3 = dist(e2);

    auto uuid = lut[d0 & 0xff] + lut[d0 >> 8 & 0xff] + lut[d0 >> 16 & 0xff] +
                lut[d0 >> 24 & 0xff] + '-' + lut[d1 & 0xff] +
                lut[d1 >> 8 & 0xff]  + '-' + lut[d1 >> 16 & 0x0f | 0x40] +
                lut[d1 >> 24 & 0xff] + '-' + lut[d2 & 0x3f | 0x80] +
                lut[d2 >> 8 & 0xff]  + '-' + lut[d2 >> 16 & 0xff] +
                lut[d2 >> 24 & 0xff] + lut[d3 & 0xff] + lut[d3 >> 8 & 0xff] +
                lut[d3 >> 16 & 0xff] + lut[d3 >> 24 & 0xff];

    return uuid;
}

}
