/**
 * @file
 * 
 * Many useful mathematical constants.
 */
#pragma once

template<typename T>
inline constexpr T T_PI         = 3.14159265358979323846264338327950; // pi

template<typename T>
inline constexpr T T_2PI        = 6.28318530717958647692528676655900; // 2pi | tau

template<typename T>
inline constexpr T T_PI_2       = 1.57079632679489661923132169163975; // pi/2

template<typename T>
inline constexpr T T_PI_3       = 1.04719755119659774615421446109316; // pi/3

template<typename T>
inline constexpr T T_PI_4       = 0.78539816339744830961566084581987; // pi/4

template<typename T>
inline constexpr T T_PI_6       = 0.52359877559829887307710723054658; // pi/6

template<typename T>
inline constexpr T T_PI_8       = 0.39269908169872415480783042290993; // pi/8

template<typename T>
inline constexpr T T_PI_9       = 0.34906585039886591538473815369772; // pi/9

template<typename T>
inline constexpr T T_PI_12      = 0.26179938779914943653855361527329; // pi/12

template<typename T>
inline constexpr T T_2_PI_3     = 2.09439510239319549230842892218633; // 2pi/3

template<typename T>
inline constexpr T T_3_PI_4     = 2.35619449019234492884698253745962; // 3pi/4

template<typename T>
inline constexpr T T_4_PI_3     = 4.18879020478639098461685784437267; // 4pi/3

template<typename T>
inline constexpr T T_3_PI_2     = 4.71238898038468985769396507491925; // 3pi/2

template<typename T>
inline constexpr T T_5_PI_3     = 5.23598775598298873077107230546583; // 5pi/3

template<typename T>
inline constexpr T T_11_PI_6    = 5.75958653158128760384817953601242; // 11pi/6

template<typename T>
inline constexpr T T_1_PI       = 0.31830988618379067153776752674502; // 1/pi

template<typename T>
inline constexpr T T_2_PI       = 0.63661977236758134307553505349005; // 2/pi

template<typename T>
inline constexpr T T_PI_P2      = 9.86960440108935861883449099987615; // pi**2

template<typename T>
inline constexpr T T_PI_P3      = 31.0062766802998201754763150671013; // pi**3

template<typename T>
inline constexpr T T_SQRT_PI    = 1.77245385090551602729816748334114; // sqrt(pi)

template<typename T>
inline constexpr T T_CBRT_PI    = 1.46459188756152326302014252726379; // cbrt(pi)

template<typename T>
inline constexpr T T_1_SQRT_PI  = 0.56418958354775628694807945156077; // 1/sqrt(pi)

template<typename T>
inline constexpr T T_2_SQRT_PI  = 1.12837916709551257389615890312154; // 2/sqrt(pi)

template<typename T>
inline constexpr T T_1_SQRT_2PI = 0.39894228040143267793994605993438; // 1/sqrt(2pi)

template<typename T>
inline constexpr T T_2_SQRT_2PI = 0.79788456080286535587989211986876; // 2/sqrt(2pi)

template<typename T>
inline constexpr T T_PI_E       = 8.53973422267356706546355086954657; // pi*e

template<typename T>
inline constexpr T T_PI_PE      = 22.4591577183610454734271522045437; // pi**e

template<typename T>
inline constexpr T T_E_PPI      = 23.1406926327792690057290863679485; // e**pi


template<typename T>
inline constexpr T T_LN_PI      = 1.14472988584940017414342735135305; // ln(pi)

template<typename T>
inline constexpr T T_LN_2PI     = 1.83787706640934548356065947281123; // ln(2pi) | ln(tau)

template<typename T>
inline constexpr T T_2_LN_PI    = 2.28945977169880034828685470270611; // 2ln(pi) | ln(pi**2)

template<typename T>
inline constexpr T T_2_LN_2PI   = 3.67575413281869096712131894562247; // 2ln(2pi) | ln(2pi**2)


template<typename T>
inline constexpr T T_E          = 2.71828182845904523536028747135266; // e

template<typename T>
inline constexpr T T_2E         = 5.43656365691809047072057494270532; // 2e

template<typename T>
inline constexpr T T_1_E        = 0.36787944117144232159552377016146; // 1/e

template<typename T>
inline constexpr T T_2_E        = 0.73575888234288464319104754032292; // 2/e

template<typename T>
inline constexpr T T_E_2        = 1.35914091422952261768014373567633; // e/2

template<typename T>
inline constexpr T T_E_P2       = 7.38905609893065022723042746057500; // e**2

template<typename T>
inline constexpr T T_SQRT_E     = 1.64872127070012814684865078781416; // sqrt(e)

template<typename T>
inline constexpr T T_SQRT_E_2   = 0.82436063535006407342432539390708; // sqrt(e)/2 | 1/2sqrt(e)

template<typename T>
inline constexpr T T_2SQRT_E    = 3.29744254140025629369730157562832; // 2sqrt(e)

template<typename T>
inline constexpr T T_1_SQRT_E   = 0.60653065971263342360379953499118; // 1/sqrt(e)

template<typename T>
inline constexpr T T_2_SQRT_E   = 1.21306131942526684720759906998236; // 2/sqrt(e)



template<typename T>
inline constexpr T T_LN_2       = 0.69314718055994530941723212145817; // ln(2)

template<typename T>
inline constexpr T T_1_LN_2     = 1.44269504088896340735992468100189; // 1/ln(2) | log2(e)

template<typename T>
inline constexpr T T_2LN_2      = 1.38629436111989061883446424291635; // 2ln(2) | ln(2**2)

template<typename T>
inline constexpr T T_2_LN_2     = 2.88539008177792681471984936200378; // 2/ln(2) | 2log2(e) | log2(e**2)

template<typename T>
inline constexpr T T_LN_10      = 2.30258509299404568401799145468436; // ln(10)

template<typename T>
inline constexpr T T_1_LN_10    = 0.43429448190325182765112891891660; // 1/ln(10) | log10(e)

template<typename T>
inline constexpr T T_2_LN_10    = 0.86858896380650365530225783783321; // 2/ln(10) | 2log10(e) | log10(e**2)

template<typename T>
inline constexpr T T_LN_100     = 4.60517018598809136803598290936872; // ln(100)

template<typename T>
inline constexpr T T_LOG2_10    = 3.32192809488736234787031942948939; // log2(10)

template<typename T>
inline constexpr T T_LOG2_100   = 6.64385618977472469574063885897878; // log2(100)


template<typename T>
inline constexpr T T_SQRT_2     = 1.41421356237309504880168872420969; // sqrt(2) | 2/sqrt(2)

template<typename T>
inline constexpr T T_1_SQRT_2   = 0.70710678118654752440084436210484; // sqrt(2)/2 | 1/sqrt(2) | 1/2 * sqrt(2)

template<typename T>
inline constexpr T T_2SQRT_2    = 2.82842712474619009760337744841939; // 2sqrt(2)

template<typename T>
inline constexpr T T_SQRT_3     = 1.73205080756887729352744634150587; // sqrt(3) | 3/sqrt(3)

template<typename T>
inline constexpr T T_1_SQRT_3   = 0.57735026918962576450914878050195; // sqrt(3)/3 | 1/sqrt(3) | 1/3 * sqrt(3)

/**
 * Equal to pi/180.
 */
template<typename T>
inline constexpr T RADIANS_TO_DEGREES_CONVERTER_VAL = 57.29577951308232087679815481410517;
/**
* Equal to 180/pi
*/
template<typename T>
inline constexpr T DEGREES_TO_RADIANS_CONVERTER_VAL = 0.017453292519943295769236907684886;
