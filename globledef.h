#pragma once
#ifndef GLOBLEDEF_H_
#define GLOBLEDEF_H_

#include <array>
#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <set>
#include <map>
#include <memory>

//for test
//正常流程用(strategy和range的dump)
#ifndef FOR_TEST_DUMP_
#define FOR_TEST_DUMP_
#endif 

#ifndef FOR_FLOW_
#define FOR_FLOW_
#endif 

//测试用
#ifndef FOR_TEST_DUMP_DETAIL_
#define FOR_TEST_DUMP_DETAIL_
#endif 

//#ifndef DEBUG_
//#define DEBUG_
//#endif 


const int COMBO_COUNT = 1326;
const int ISOFLOP_COUNT = 1755;
const double MIN_ALLIN_EV = 1;	//replace时，目标策略不存在则转为allin,条件是allin的EV>MIN_ALLIN_EV
const double MIN_ALLIN_RATIO = 0;	//转为allin，至少要存在的比例
const std::array<std::string, COMBO_COUNT> ComboMapping{ "2d2c","2h2c","2h2d","2s2c","2s2d","2s2h","3c2c","3c2d","3c2h","3c2s","3d2c","3d2d","3d2h","3d2s","3d3c","3h2c","3h2d","3h2h","3h2s","3h3c","3h3d","3s2c","3s2d","3s2h","3s2s","3s3c","3s3d","3s3h","4c2c","4c2d","4c2h","4c2s","4c3c","4c3d","4c3h","4c3s","4d2c","4d2d","4d2h","4d2s","4d3c","4d3d","4d3h","4d3s","4d4c","4h2c","4h2d","4h2h","4h2s","4h3c","4h3d","4h3h","4h3s","4h4c","4h4d","4s2c","4s2d","4s2h","4s2s","4s3c","4s3d","4s3h","4s3s","4s4c","4s4d","4s4h","5c2c","5c2d","5c2h","5c2s","5c3c","5c3d","5c3h","5c3s","5c4c","5c4d","5c4h","5c4s","5d2c","5d2d","5d2h","5d2s","5d3c","5d3d","5d3h","5d3s","5d4c","5d4d","5d4h","5d4s","5d5c","5h2c","5h2d","5h2h","5h2s","5h3c","5h3d","5h3h","5h3s","5h4c","5h4d","5h4h","5h4s","5h5c","5h5d","5s2c","5s2d","5s2h","5s2s","5s3c","5s3d","5s3h","5s3s","5s4c","5s4d","5s4h","5s4s","5s5c","5s5d","5s5h","6c2c","6c2d","6c2h","6c2s","6c3c","6c3d","6c3h","6c3s","6c4c","6c4d","6c4h","6c4s","6c5c","6c5d","6c5h","6c5s","6d2c","6d2d","6d2h","6d2s","6d3c","6d3d","6d3h","6d3s","6d4c","6d4d","6d4h","6d4s","6d5c","6d5d","6d5h","6d5s","6d6c","6h2c","6h2d","6h2h","6h2s","6h3c","6h3d","6h3h","6h3s","6h4c","6h4d","6h4h","6h4s","6h5c","6h5d","6h5h","6h5s","6h6c","6h6d","6s2c","6s2d","6s2h","6s2s","6s3c","6s3d","6s3h","6s3s","6s4c","6s4d","6s4h","6s4s","6s5c","6s5d","6s5h","6s5s","6s6c","6s6d","6s6h","7c2c","7c2d","7c2h","7c2s","7c3c","7c3d","7c3h","7c3s","7c4c","7c4d","7c4h","7c4s","7c5c","7c5d","7c5h","7c5s","7c6c","7c6d","7c6h","7c6s","7d2c","7d2d","7d2h","7d2s","7d3c","7d3d","7d3h","7d3s","7d4c","7d4d","7d4h","7d4s","7d5c","7d5d","7d5h","7d5s","7d6c","7d6d","7d6h","7d6s","7d7c","7h2c","7h2d","7h2h","7h2s","7h3c","7h3d","7h3h","7h3s","7h4c","7h4d","7h4h","7h4s","7h5c","7h5d","7h5h","7h5s","7h6c","7h6d","7h6h","7h6s","7h7c","7h7d","7s2c","7s2d","7s2h","7s2s","7s3c","7s3d","7s3h","7s3s","7s4c","7s4d","7s4h","7s4s","7s5c","7s5d","7s5h","7s5s","7s6c","7s6d","7s6h","7s6s","7s7c","7s7d","7s7h","8c2c","8c2d","8c2h","8c2s","8c3c","8c3d","8c3h","8c3s","8c4c","8c4d","8c4h","8c4s","8c5c","8c5d","8c5h","8c5s","8c6c","8c6d","8c6h","8c6s","8c7c","8c7d","8c7h","8c7s","8d2c","8d2d","8d2h","8d2s","8d3c","8d3d","8d3h","8d3s","8d4c","8d4d","8d4h","8d4s","8d5c","8d5d","8d5h","8d5s","8d6c","8d6d","8d6h","8d6s","8d7c","8d7d","8d7h","8d7s","8d8c","8h2c","8h2d","8h2h","8h2s","8h3c","8h3d","8h3h","8h3s","8h4c","8h4d","8h4h","8h4s","8h5c","8h5d","8h5h","8h5s","8h6c","8h6d","8h6h","8h6s","8h7c","8h7d","8h7h","8h7s","8h8c","8h8d","8s2c","8s2d","8s2h","8s2s","8s3c","8s3d","8s3h","8s3s","8s4c","8s4d","8s4h","8s4s","8s5c","8s5d","8s5h","8s5s","8s6c","8s6d","8s6h","8s6s","8s7c","8s7d","8s7h","8s7s","8s8c","8s8d","8s8h","9c2c","9c2d","9c2h","9c2s","9c3c","9c3d","9c3h","9c3s","9c4c","9c4d","9c4h","9c4s","9c5c","9c5d","9c5h","9c5s","9c6c","9c6d","9c6h","9c6s","9c7c","9c7d","9c7h","9c7s","9c8c","9c8d","9c8h","9c8s","9d2c","9d2d","9d2h","9d2s","9d3c","9d3d","9d3h","9d3s","9d4c","9d4d","9d4h","9d4s","9d5c","9d5d","9d5h","9d5s","9d6c","9d6d","9d6h","9d6s","9d7c","9d7d","9d7h","9d7s","9d8c","9d8d","9d8h","9d8s","9d9c","9h2c","9h2d","9h2h","9h2s","9h3c","9h3d","9h3h","9h3s","9h4c","9h4d","9h4h","9h4s","9h5c","9h5d","9h5h","9h5s","9h6c","9h6d","9h6h","9h6s","9h7c","9h7d","9h7h","9h7s","9h8c","9h8d","9h8h","9h8s","9h9c","9h9d","9s2c","9s2d","9s2h","9s2s","9s3c","9s3d","9s3h","9s3s","9s4c","9s4d","9s4h","9s4s","9s5c","9s5d","9s5h","9s5s","9s6c","9s6d","9s6h","9s6s","9s7c","9s7d","9s7h","9s7s","9s8c","9s8d","9s8h","9s8s","9s9c","9s9d","9s9h","Tc2c","Tc2d","Tc2h","Tc2s","Tc3c","Tc3d","Tc3h","Tc3s","Tc4c","Tc4d","Tc4h","Tc4s","Tc5c","Tc5d","Tc5h","Tc5s","Tc6c","Tc6d","Tc6h","Tc6s","Tc7c","Tc7d","Tc7h","Tc7s","Tc8c","Tc8d","Tc8h","Tc8s","Tc9c","Tc9d","Tc9h","Tc9s","Td2c","Td2d","Td2h","Td2s","Td3c","Td3d","Td3h","Td3s","Td4c","Td4d","Td4h","Td4s","Td5c","Td5d","Td5h","Td5s","Td6c","Td6d","Td6h","Td6s","Td7c","Td7d","Td7h","Td7s","Td8c","Td8d","Td8h","Td8s","Td9c","Td9d","Td9h","Td9s","TdTc","Th2c","Th2d","Th2h","Th2s","Th3c","Th3d","Th3h","Th3s","Th4c","Th4d","Th4h","Th4s","Th5c","Th5d","Th5h","Th5s","Th6c","Th6d","Th6h","Th6s","Th7c","Th7d","Th7h","Th7s","Th8c","Th8d","Th8h","Th8s","Th9c","Th9d","Th9h","Th9s","ThTc","ThTd","Ts2c","Ts2d","Ts2h","Ts2s","Ts3c","Ts3d","Ts3h","Ts3s","Ts4c","Ts4d","Ts4h","Ts4s","Ts5c","Ts5d","Ts5h","Ts5s","Ts6c","Ts6d","Ts6h","Ts6s","Ts7c","Ts7d","Ts7h","Ts7s","Ts8c","Ts8d","Ts8h","Ts8s","Ts9c","Ts9d","Ts9h","Ts9s","TsTc","TsTd","TsTh","Jc2c","Jc2d","Jc2h","Jc2s","Jc3c","Jc3d","Jc3h","Jc3s","Jc4c","Jc4d","Jc4h","Jc4s","Jc5c","Jc5d","Jc5h","Jc5s","Jc6c","Jc6d","Jc6h","Jc6s","Jc7c","Jc7d","Jc7h","Jc7s","Jc8c","Jc8d","Jc8h","Jc8s","Jc9c","Jc9d","Jc9h","Jc9s","JcTc","JcTd","JcTh","JcTs","Jd2c","Jd2d","Jd2h","Jd2s","Jd3c","Jd3d","Jd3h","Jd3s","Jd4c","Jd4d","Jd4h","Jd4s","Jd5c","Jd5d","Jd5h","Jd5s","Jd6c","Jd6d","Jd6h","Jd6s","Jd7c","Jd7d","Jd7h","Jd7s","Jd8c","Jd8d","Jd8h","Jd8s","Jd9c","Jd9d","Jd9h","Jd9s","JdTc","JdTd","JdTh","JdTs","JdJc","Jh2c","Jh2d","Jh2h","Jh2s","Jh3c","Jh3d","Jh3h","Jh3s","Jh4c","Jh4d","Jh4h","Jh4s","Jh5c","Jh5d","Jh5h","Jh5s","Jh6c","Jh6d","Jh6h","Jh6s","Jh7c","Jh7d","Jh7h","Jh7s","Jh8c","Jh8d","Jh8h","Jh8s","Jh9c","Jh9d","Jh9h","Jh9s","JhTc","JhTd","JhTh","JhTs","JhJc","JhJd","Js2c","Js2d","Js2h","Js2s","Js3c","Js3d","Js3h","Js3s","Js4c","Js4d","Js4h","Js4s","Js5c","Js5d","Js5h","Js5s","Js6c","Js6d","Js6h","Js6s","Js7c","Js7d","Js7h","Js7s","Js8c","Js8d","Js8h","Js8s","Js9c","Js9d","Js9h","Js9s","JsTc","JsTd","JsTh","JsTs","JsJc","JsJd","JsJh","Qc2c","Qc2d","Qc2h","Qc2s","Qc3c","Qc3d","Qc3h","Qc3s","Qc4c","Qc4d","Qc4h","Qc4s","Qc5c","Qc5d","Qc5h","Qc5s","Qc6c","Qc6d","Qc6h","Qc6s","Qc7c","Qc7d","Qc7h","Qc7s","Qc8c","Qc8d","Qc8h","Qc8s","Qc9c","Qc9d","Qc9h","Qc9s","QcTc","QcTd","QcTh","QcTs","QcJc","QcJd","QcJh","QcJs","Qd2c","Qd2d","Qd2h","Qd2s","Qd3c","Qd3d","Qd3h","Qd3s","Qd4c","Qd4d","Qd4h","Qd4s","Qd5c","Qd5d","Qd5h","Qd5s","Qd6c","Qd6d","Qd6h","Qd6s","Qd7c","Qd7d","Qd7h","Qd7s","Qd8c","Qd8d","Qd8h","Qd8s","Qd9c","Qd9d","Qd9h","Qd9s","QdTc","QdTd","QdTh","QdTs","QdJc","QdJd","QdJh","QdJs","QdQc","Qh2c","Qh2d","Qh2h","Qh2s","Qh3c","Qh3d","Qh3h","Qh3s","Qh4c","Qh4d","Qh4h","Qh4s","Qh5c","Qh5d","Qh5h","Qh5s","Qh6c","Qh6d","Qh6h","Qh6s","Qh7c","Qh7d","Qh7h","Qh7s","Qh8c","Qh8d","Qh8h","Qh8s","Qh9c","Qh9d","Qh9h","Qh9s","QhTc","QhTd","QhTh","QhTs","QhJc","QhJd","QhJh","QhJs","QhQc","QhQd","Qs2c","Qs2d","Qs2h","Qs2s","Qs3c","Qs3d","Qs3h","Qs3s","Qs4c","Qs4d","Qs4h","Qs4s","Qs5c","Qs5d","Qs5h","Qs5s","Qs6c","Qs6d","Qs6h","Qs6s","Qs7c","Qs7d","Qs7h","Qs7s","Qs8c","Qs8d","Qs8h","Qs8s","Qs9c","Qs9d","Qs9h","Qs9s","QsTc","QsTd","QsTh","QsTs","QsJc","QsJd","QsJh","QsJs","QsQc","QsQd","QsQh","Kc2c","Kc2d","Kc2h","Kc2s","Kc3c","Kc3d","Kc3h","Kc3s","Kc4c","Kc4d","Kc4h","Kc4s","Kc5c","Kc5d","Kc5h","Kc5s","Kc6c","Kc6d","Kc6h","Kc6s","Kc7c","Kc7d","Kc7h","Kc7s","Kc8c","Kc8d","Kc8h","Kc8s","Kc9c","Kc9d","Kc9h","Kc9s","KcTc","KcTd","KcTh","KcTs","KcJc","KcJd","KcJh","KcJs","KcQc","KcQd","KcQh","KcQs","Kd2c","Kd2d","Kd2h","Kd2s","Kd3c","Kd3d","Kd3h","Kd3s","Kd4c","Kd4d","Kd4h","Kd4s","Kd5c","Kd5d","Kd5h","Kd5s","Kd6c","Kd6d","Kd6h","Kd6s","Kd7c","Kd7d","Kd7h","Kd7s","Kd8c","Kd8d","Kd8h","Kd8s","Kd9c","Kd9d","Kd9h","Kd9s","KdTc","KdTd","KdTh","KdTs","KdJc","KdJd","KdJh","KdJs","KdQc","KdQd","KdQh","KdQs","KdKc","Kh2c","Kh2d","Kh2h","Kh2s","Kh3c","Kh3d","Kh3h","Kh3s","Kh4c","Kh4d","Kh4h","Kh4s","Kh5c","Kh5d","Kh5h","Kh5s","Kh6c","Kh6d","Kh6h","Kh6s","Kh7c","Kh7d","Kh7h","Kh7s","Kh8c","Kh8d","Kh8h","Kh8s","Kh9c","Kh9d","Kh9h","Kh9s","KhTc","KhTd","KhTh","KhTs","KhJc","KhJd","KhJh","KhJs","KhQc","KhQd","KhQh","KhQs","KhKc","KhKd","Ks2c","Ks2d","Ks2h","Ks2s","Ks3c","Ks3d","Ks3h","Ks3s","Ks4c","Ks4d","Ks4h","Ks4s","Ks5c","Ks5d","Ks5h","Ks5s","Ks6c","Ks6d","Ks6h","Ks6s","Ks7c","Ks7d","Ks7h","Ks7s","Ks8c","Ks8d","Ks8h","Ks8s","Ks9c","Ks9d","Ks9h","Ks9s","KsTc","KsTd","KsTh","KsTs","KsJc","KsJd","KsJh","KsJs","KsQc","KsQd","KsQh","KsQs","KsKc","KsKd","KsKh","Ac2c","Ac2d","Ac2h","Ac2s","Ac3c","Ac3d","Ac3h","Ac3s","Ac4c","Ac4d","Ac4h","Ac4s","Ac5c","Ac5d","Ac5h","Ac5s","Ac6c","Ac6d","Ac6h","Ac6s","Ac7c","Ac7d","Ac7h","Ac7s","Ac8c","Ac8d","Ac8h","Ac8s","Ac9c","Ac9d","Ac9h","Ac9s","AcTc","AcTd","AcTh","AcTs","AcJc","AcJd","AcJh","AcJs","AcQc","AcQd","AcQh","AcQs","AcKc","AcKd","AcKh","AcKs","Ad2c","Ad2d","Ad2h","Ad2s","Ad3c","Ad3d","Ad3h","Ad3s","Ad4c","Ad4d","Ad4h","Ad4s","Ad5c","Ad5d","Ad5h","Ad5s","Ad6c","Ad6d","Ad6h","Ad6s","Ad7c","Ad7d","Ad7h","Ad7s","Ad8c","Ad8d","Ad8h","Ad8s","Ad9c","Ad9d","Ad9h","Ad9s","AdTc","AdTd","AdTh","AdTs","AdJc","AdJd","AdJh","AdJs","AdQc","AdQd","AdQh","AdQs","AdKc","AdKd","AdKh","AdKs","AdAc","Ah2c","Ah2d","Ah2h","Ah2s","Ah3c","Ah3d","Ah3h","Ah3s","Ah4c","Ah4d","Ah4h","Ah4s","Ah5c","Ah5d","Ah5h","Ah5s","Ah6c","Ah6d","Ah6h","Ah6s","Ah7c","Ah7d","Ah7h","Ah7s","Ah8c","Ah8d","Ah8h","Ah8s","Ah9c","Ah9d","Ah9h","Ah9s","AhTc","AhTd","AhTh","AhTs","AhJc","AhJd","AhJh","AhJs","AhQc","AhQd","AhQh","AhQs","AhKc","AhKd","AhKh","AhKs","AhAc","AhAd","As2c","As2d","As2h","As2s","As3c","As3d","As3h","As3s","As4c","As4d","As4h","As4s","As5c","As5d","As5h","As5s","As6c","As6d","As6h","As6s","As7c","As7d","As7h","As7s","As8c","As8d","As8h","As8s","As9c","As9d","As9h","As9s","AsTc","AsTd","AsTh","AsTs","AsJc","AsJd","AsJh","AsJs","AsQc","AsQd","AsQh","AsQs","AsKc","AsKd","AsKh","AsKs","AsAc","AsAd","AsAh" };
const std::unordered_set<std::string> IsoFlops{ "2h2d2c","2h2d3h","2h2d3c","2h2d4h","2h2d4c","2h2d5h","2h2d5c","2h2d6h","2h2d6c","2h2d7h","2h2d7c","2h2d8h","2h2d8c","2h2d9h","2h2d9c","2h2dTh","2h2dTc","2h2dJh","2h2dJc","2h2dQh","2h2dQc","2h2dKh","2h2dKc","2h2dAh","2h2dAc","2h3h3d","2h3d3c","2h3d4c","2h3d4h","2h3h4d","2d3h4h","2h3h4h","2h3d5c","2h3d5h","2h3h5d","2d3h5h","2h3h5h","2h3d6c","2h3d6h","2h3h6d","2d3h6h","2h3h6h","2h3d7c","2h3d7h","2h3h7d","2d3h7h","2h3h7h","2h3d8c","2h3d8h","2h3h8d","2d3h8h","2h3h8h","2h3d9c","2h3d9h","2h3h9d","2d3h9h","2h3h9h","2h3dTc","2h3dTh","2h3hTd","2d3hTh","2h3hTh","2h3dJc","2h3dJh","2h3hJd","2d3hJh","2h3hJh","2h3dQc","2h3dQh","2h3hQd","2d3hQh","2h3hQh","2h3dKc","2h3dKh","2h3hKd","2d3hKh","2h3hKh","2h3dAc","2h3dAh","2h3hAd","2d3hAh","2h3hAh","2h4h4d","2h4d4c","2h4d5c","2h4d5h","2h4h5d","2d4h5h","2h4h5h","2h4d6c","2h4d6h","2h4h6d","2d4h6h","2h4h6h","2h4d7c","2h4d7h","2h4h7d","2d4h7h","2h4h7h","2h4d8c","2h4d8h","2h4h8d","2d4h8h","2h4h8h","2h4d9c","2h4d9h","2h4h9d","2d4h9h","2h4h9h","2h4dTc","2h4dTh","2h4hTd","2d4hTh","2h4hTh","2h4dJc","2h4dJh","2h4hJd","2d4hJh","2h4hJh","2h4dQc","2h4dQh","2h4hQd","2d4hQh","2h4hQh","2h4dKc","2h4dKh","2h4hKd","2d4hKh","2h4hKh","2h4dAc","2h4dAh","2h4hAd","2d4hAh","2h4hAh","2h5h5d","2h5d5c","2h5d6c","2h5d6h","2h5h6d","2d5h6h","2h5h6h","2h5d7c","2h5d7h","2h5h7d","2d5h7h","2h5h7h","2h5d8c","2h5d8h","2h5h8d","2d5h8h","2h5h8h","2h5d9c","2h5d9h","2h5h9d","2d5h9h","2h5h9h","2h5dTc","2h5dTh","2h5hTd","2d5hTh","2h5hTh","2h5dJc","2h5dJh","2h5hJd","2d5hJh","2h5hJh","2h5dQc","2h5dQh","2h5hQd","2d5hQh","2h5hQh","2h5dKc","2h5dKh","2h5hKd","2d5hKh","2h5hKh","2h5dAc","2h5dAh","2h5hAd","2d5hAh","2h5hAh","2h6h6d","2h6d6c","2h6d7c","2h6d7h","2h6h7d","2d6h7h","2h6h7h","2h6d8c","2h6d8h","2h6h8d","2d6h8h","2h6h8h","2h6d9c","2h6d9h","2h6h9d","2d6h9h","2h6h9h","2h6dTc","2h6dTh","2h6hTd","2d6hTh","2h6hTh","2h6dJc","2h6dJh","2h6hJd","2d6hJh","2h6hJh","2h6dQc","2h6dQh","2h6hQd","2d6hQh","2h6hQh","2h6dKc","2h6dKh","2h6hKd","2d6hKh","2h6hKh","2h6dAc","2h6dAh","2h6hAd","2d6hAh","2h6hAh","2h7h7d","2h7d7c","2h7d8c","2h7d8h","2h7h8d","2d7h8h","2h7h8h","2h7d9c","2h7d9h","2h7h9d","2d7h9h","2h7h9h","2h7dTc","2h7dTh","2h7hTd","2d7hTh","2h7hTh","2h7dJc","2h7dJh","2h7hJd","2d7hJh","2h7hJh","2h7dQc","2h7dQh","2h7hQd","2d7hQh","2h7hQh","2h7dKc","2h7dKh","2h7hKd","2d7hKh","2h7hKh","2h7dAc","2h7dAh","2h7hAd","2d7hAh","2h7hAh","2h8h8d","2h8d8c","2h8d9c","2h8d9h","2h8h9d","2d8h9h","2h8h9h","2h8dTc","2h8dTh","2h8hTd","2d8hTh","2h8hTh","2h8dJc","2h8dJh","2h8hJd","2d8hJh","2h8hJh","2h8dQc","2h8dQh","2h8hQd","2d8hQh","2h8hQh","2h8dKc","2h8dKh","2h8hKd","2d8hKh","2h8hKh","2h8dAc","2h8dAh","2h8hAd","2d8hAh","2h8hAh","2h9h9d","2h9d9c","2h9dTc","2h9dTh","2h9hTd","2d9hTh","2h9hTh","2h9dJc","2h9dJh","2h9hJd","2d9hJh","2h9hJh","2h9dQc","2h9dQh","2h9hQd","2d9hQh","2h9hQh","2h9dKc","2h9dKh","2h9hKd","2d9hKh","2h9hKh","2h9dAc","2h9dAh","2h9hAd","2d9hAh","2h9hAh","2hThTd","2hTdTc","2hTdJc","2hTdJh","2hThJd","2dThJh","2hThJh","2hTdQc","2hTdQh","2hThQd","2dThQh","2hThQh","2hTdKc","2hTdKh","2hThKd","2dThKh","2hThKh","2hTdAc","2hTdAh","2hThAd","2dThAh","2hThAh","2hJhJd","2hJdJc","2hJdQc","2hJdQh","2hJhQd","2dJhQh","2hJhQh","2hJdKc","2hJdKh","2hJhKd","2dJhKh","2hJhKh","2hJdAc","2hJdAh","2hJhAd","2dJhAh","2hJhAh","2hQhQd","2hQdQc","2hQdKc","2hQdKh","2hQhKd","2dQhKh","2hQhKh","2hQdAc","2hQdAh","2hQhAd","2dQhAh","2hQhAh","2hKhKd","2hKdKc","2hKdAc","2hKdAh","2hKhAd","2dKhAh","2hKhAh","2hAhAd","2hAdAc","3h3d3c","3h3d4h","3h3d4c","3h3d5h","3h3d5c","3h3d6h","3h3d6c","3h3d7h","3h3d7c","3h3d8h","3h3d8c","3h3d9h","3h3d9c","3h3dTh","3h3dTc","3h3dJh","3h3dJc","3h3dQh","3h3dQc","3h3dKh","3h3dKc","3h3dAh","3h3dAc","3h4h4d","3h4d4c","3h4d5c","3h4d5h","3h4h5d","3d4h5h","3h4h5h","3h4d6c","3h4d6h","3h4h6d","3d4h6h","3h4h6h","3h4d7c","3h4d7h","3h4h7d","3d4h7h","3h4h7h","3h4d8c","3h4d8h","3h4h8d","3d4h8h","3h4h8h","3h4d9c","3h4d9h","3h4h9d","3d4h9h","3h4h9h","3h4dTc","3h4dTh","3h4hTd","3d4hTh","3h4hTh","3h4dJc","3h4dJh","3h4hJd","3d4hJh","3h4hJh","3h4dQc","3h4dQh","3h4hQd","3d4hQh","3h4hQh","3h4dKc","3h4dKh","3h4hKd","3d4hKh","3h4hKh","3h4dAc","3h4dAh","3h4hAd","3d4hAh","3h4hAh","3h5h5d","3h5d5c","3h5d6c","3h5d6h","3h5h6d","3d5h6h","3h5h6h","3h5d7c","3h5d7h","3h5h7d","3d5h7h","3h5h7h","3h5d8c","3h5d8h","3h5h8d","3d5h8h","3h5h8h","3h5d9c","3h5d9h","3h5h9d","3d5h9h","3h5h9h","3h5dTc","3h5dTh","3h5hTd","3d5hTh","3h5hTh","3h5dJc","3h5dJh","3h5hJd","3d5hJh","3h5hJh","3h5dQc","3h5dQh","3h5hQd","3d5hQh","3h5hQh","3h5dKc","3h5dKh","3h5hKd","3d5hKh","3h5hKh","3h5dAc","3h5dAh","3h5hAd","3d5hAh","3h5hAh","3h6h6d","3h6d6c","3h6d7c","3h6d7h","3h6h7d","3d6h7h","3h6h7h","3h6d8c","3h6d8h","3h6h8d","3d6h8h","3h6h8h","3h6d9c","3h6d9h","3h6h9d","3d6h9h","3h6h9h","3h6dTc","3h6dTh","3h6hTd","3d6hTh","3h6hTh","3h6dJc","3h6dJh","3h6hJd","3d6hJh","3h6hJh","3h6dQc","3h6dQh","3h6hQd","3d6hQh","3h6hQh","3h6dKc","3h6dKh","3h6hKd","3d6hKh","3h6hKh","3h6dAc","3h6dAh","3h6hAd","3d6hAh","3h6hAh","3h7h7d","3h7d7c","3h7d8c","3h7d8h","3h7h8d","3d7h8h","3h7h8h","3h7d9c","3h7d9h","3h7h9d","3d7h9h","3h7h9h","3h7dTc","3h7dTh","3h7hTd","3d7hTh","3h7hTh","3h7dJc","3h7dJh","3h7hJd","3d7hJh","3h7hJh","3h7dQc","3h7dQh","3h7hQd","3d7hQh","3h7hQh","3h7dKc","3h7dKh","3h7hKd","3d7hKh","3h7hKh","3h7dAc","3h7dAh","3h7hAd","3d7hAh","3h7hAh","3h8h8d","3h8d8c","3h8d9c","3h8d9h","3h8h9d","3d8h9h","3h8h9h","3h8dTc","3h8dTh","3h8hTd","3d8hTh","3h8hTh","3h8dJc","3h8dJh","3h8hJd","3d8hJh","3h8hJh","3h8dQc","3h8dQh","3h8hQd","3d8hQh","3h8hQh","3h8dKc","3h8dKh","3h8hKd","3d8hKh","3h8hKh","3h8dAc","3h8dAh","3h8hAd","3d8hAh","3h8hAh","3h9h9d","3h9d9c","3h9dTc","3h9dTh","3h9hTd","3d9hTh","3h9hTh","3h9dJc","3h9dJh","3h9hJd","3d9hJh","3h9hJh","3h9dQc","3h9dQh","3h9hQd","3d9hQh","3h9hQh","3h9dKc","3h9dKh","3h9hKd","3d9hKh","3h9hKh","3h9dAc","3h9dAh","3h9hAd","3d9hAh","3h9hAh","3hThTd","3hTdTc","3hTdJc","3hTdJh","3hThJd","3dThJh","3hThJh","3hTdQc","3hTdQh","3hThQd","3dThQh","3hThQh","3hTdKc","3hTdKh","3hThKd","3dThKh","3hThKh","3hTdAc","3hTdAh","3hThAd","3dThAh","3hThAh","3hJhJd","3hJdJc","3hJdQc","3hJdQh","3hJhQd","3dJhQh","3hJhQh","3hJdKc","3hJdKh","3hJhKd","3dJhKh","3hJhKh","3hJdAc","3hJdAh","3hJhAd","3dJhAh","3hJhAh","3hQhQd","3hQdQc","3hQdKc","3hQdKh","3hQhKd","3dQhKh","3hQhKh","3hQdAc","3hQdAh","3hQhAd","3dQhAh","3hQhAh","3hKhKd","3hKdKc","3hKdAc","3hKdAh","3hKhAd","3dKhAh","3hKhAh","3hAhAd","3hAdAc","4h4d4c","4h4d5h","4h4d5c","4h4d6h","4h4d6c","4h4d7h","4h4d7c","4h4d8h","4h4d8c","4h4d9h","4h4d9c","4h4dTh","4h4dTc","4h4dJh","4h4dJc","4h4dQh","4h4dQc","4h4dKh","4h4dKc","4h4dAh","4h4dAc","4h5h5d","4h5d5c","4h5d6c","4h5d6h","4h5h6d","4d5h6h","4h5h6h","4h5d7c","4h5d7h","4h5h7d","4d5h7h","4h5h7h","4h5d8c","4h5d8h","4h5h8d","4d5h8h","4h5h8h","4h5d9c","4h5d9h","4h5h9d","4d5h9h","4h5h9h","4h5dTc","4h5dTh","4h5hTd","4d5hTh","4h5hTh","4h5dJc","4h5dJh","4h5hJd","4d5hJh","4h5hJh","4h5dQc","4h5dQh","4h5hQd","4d5hQh","4h5hQh","4h5dKc","4h5dKh","4h5hKd","4d5hKh","4h5hKh","4h5dAc","4h5dAh","4h5hAd","4d5hAh","4h5hAh","4h6h6d","4h6d6c","4h6d7c","4h6d7h","4h6h7d","4d6h7h","4h6h7h","4h6d8c","4h6d8h","4h6h8d","4d6h8h","4h6h8h","4h6d9c","4h6d9h","4h6h9d","4d6h9h","4h6h9h","4h6dTc","4h6dTh","4h6hTd","4d6hTh","4h6hTh","4h6dJc","4h6dJh","4h6hJd","4d6hJh","4h6hJh","4h6dQc","4h6dQh","4h6hQd","4d6hQh","4h6hQh","4h6dKc","4h6dKh","4h6hKd","4d6hKh","4h6hKh","4h6dAc","4h6dAh","4h6hAd","4d6hAh","4h6hAh","4h7h7d","4h7d7c","4h7d8c","4h7d8h","4h7h8d","4d7h8h","4h7h8h","4h7d9c","4h7d9h","4h7h9d","4d7h9h","4h7h9h","4h7dTc","4h7dTh","4h7hTd","4d7hTh","4h7hTh","4h7dJc","4h7dJh","4h7hJd","4d7hJh","4h7hJh","4h7dQc","4h7dQh","4h7hQd","4d7hQh","4h7hQh","4h7dKc","4h7dKh","4h7hKd","4d7hKh","4h7hKh","4h7dAc","4h7dAh","4h7hAd","4d7hAh","4h7hAh","4h8h8d","4h8d8c","4h8d9c","4h8d9h","4h8h9d","4d8h9h","4h8h9h","4h8dTc","4h8dTh","4h8hTd","4d8hTh","4h8hTh","4h8dJc","4h8dJh","4h8hJd","4d8hJh","4h8hJh","4h8dQc","4h8dQh","4h8hQd","4d8hQh","4h8hQh","4h8dKc","4h8dKh","4h8hKd","4d8hKh","4h8hKh","4h8dAc","4h8dAh","4h8hAd","4d8hAh","4h8hAh","4h9h9d","4h9d9c","4h9dTc","4h9dTh","4h9hTd","4d9hTh","4h9hTh","4h9dJc","4h9dJh","4h9hJd","4d9hJh","4h9hJh","4h9dQc","4h9dQh","4h9hQd","4d9hQh","4h9hQh","4h9dKc","4h9dKh","4h9hKd","4d9hKh","4h9hKh","4h9dAc","4h9dAh","4h9hAd","4d9hAh","4h9hAh","4hThTd","4hTdTc","4hTdJc","4hTdJh","4hThJd","4dThJh","4hThJh","4hTdQc","4hTdQh","4hThQd","4dThQh","4hThQh","4hTdKc","4hTdKh","4hThKd","4dThKh","4hThKh","4hTdAc","4hTdAh","4hThAd","4dThAh","4hThAh","4hJhJd","4hJdJc","4hJdQc","4hJdQh","4hJhQd","4dJhQh","4hJhQh","4hJdKc","4hJdKh","4hJhKd","4dJhKh","4hJhKh","4hJdAc","4hJdAh","4hJhAd","4dJhAh","4hJhAh","4hQhQd","4hQdQc","4hQdKc","4hQdKh","4hQhKd","4dQhKh","4hQhKh","4hQdAc","4hQdAh","4hQhAd","4dQhAh","4hQhAh","4hKhKd","4hKdKc","4hKdAc","4hKdAh","4hKhAd","4dKhAh","4hKhAh","4hAhAd","4hAdAc","5h5d5c","5h5d6h","5h5d6c","5h5d7h","5h5d7c","5h5d8h","5h5d8c","5h5d9h","5h5d9c","5h5dTh","5h5dTc","5h5dJh","5h5dJc","5h5dQh","5h5dQc","5h5dKh","5h5dKc","5h5dAh","5h5dAc","5h6h6d","5h6d6c","5h6d7c","5h6d7h","5h6h7d","5d6h7h","5h6h7h","5h6d8c","5h6d8h","5h6h8d","5d6h8h","5h6h8h","5h6d9c","5h6d9h","5h6h9d","5d6h9h","5h6h9h","5h6dTc","5h6dTh","5h6hTd","5d6hTh","5h6hTh","5h6dJc","5h6dJh","5h6hJd","5d6hJh","5h6hJh","5h6dQc","5h6dQh","5h6hQd","5d6hQh","5h6hQh","5h6dKc","5h6dKh","5h6hKd","5d6hKh","5h6hKh","5h6dAc","5h6dAh","5h6hAd","5d6hAh","5h6hAh","5h7h7d","5h7d7c","5h7d8c","5h7d8h","5h7h8d","5d7h8h","5h7h8h","5h7d9c","5h7d9h","5h7h9d","5d7h9h","5h7h9h","5h7dTc","5h7dTh","5h7hTd","5d7hTh","5h7hTh","5h7dJc","5h7dJh","5h7hJd","5d7hJh","5h7hJh","5h7dQc","5h7dQh","5h7hQd","5d7hQh","5h7hQh","5h7dKc","5h7dKh","5h7hKd","5d7hKh","5h7hKh","5h7dAc","5h7dAh","5h7hAd","5d7hAh","5h7hAh","5h8h8d","5h8d8c","5h8d9c","5h8d9h","5h8h9d","5d8h9h","5h8h9h","5h8dTc","5h8dTh","5h8hTd","5d8hTh","5h8hTh","5h8dJc","5h8dJh","5h8hJd","5d8hJh","5h8hJh","5h8dQc","5h8dQh","5h8hQd","5d8hQh","5h8hQh","5h8dKc","5h8dKh","5h8hKd","5d8hKh","5h8hKh","5h8dAc","5h8dAh","5h8hAd","5d8hAh","5h8hAh","5h9h9d","5h9d9c","5h9dTc","5h9dTh","5h9hTd","5d9hTh","5h9hTh","5h9dJc","5h9dJh","5h9hJd","5d9hJh","5h9hJh","5h9dQc","5h9dQh","5h9hQd","5d9hQh","5h9hQh","5h9dKc","5h9dKh","5h9hKd","5d9hKh","5h9hKh","5h9dAc","5h9dAh","5h9hAd","5d9hAh","5h9hAh","5hThTd","5hTdTc","5hTdJc","5hTdJh","5hThJd","5dThJh","5hThJh","5hTdQc","5hTdQh","5hThQd","5dThQh","5hThQh","5hTdKc","5hTdKh","5hThKd","5dThKh","5hThKh","5hTdAc","5hTdAh","5hThAd","5dThAh","5hThAh","5hJhJd","5hJdJc","5hJdQc","5hJdQh","5hJhQd","5dJhQh","5hJhQh","5hJdKc","5hJdKh","5hJhKd","5dJhKh","5hJhKh","5hJdAc","5hJdAh","5hJhAd","5dJhAh","5hJhAh","5hQhQd","5hQdQc","5hQdKc","5hQdKh","5hQhKd","5dQhKh","5hQhKh","5hQdAc","5hQdAh","5hQhAd","5dQhAh","5hQhAh","5hKhKd","5hKdKc","5hKdAc","5hKdAh","5hKhAd","5dKhAh","5hKhAh","5hAhAd","5hAdAc","6h6d6c","6h6d7h","6h6d7c","6h6d8h","6h6d8c","6h6d9h","6h6d9c","6h6dTh","6h6dTc","6h6dJh","6h6dJc","6h6dQh","6h6dQc","6h6dKh","6h6dKc","6h6dAh","6h6dAc","6h7h7d","6h7d7c","6h7d8c","6h7d8h","6h7h8d","6d7h8h","6h7h8h","6h7d9c","6h7d9h","6h7h9d","6d7h9h","6h7h9h","6h7dTc","6h7dTh","6h7hTd","6d7hTh","6h7hTh","6h7dJc","6h7dJh","6h7hJd","6d7hJh","6h7hJh","6h7dQc","6h7dQh","6h7hQd","6d7hQh","6h7hQh","6h7dKc","6h7dKh","6h7hKd","6d7hKh","6h7hKh","6h7dAc","6h7dAh","6h7hAd","6d7hAh","6h7hAh","6h8h8d","6h8d8c","6h8d9c","6h8d9h","6h8h9d","6d8h9h","6h8h9h","6h8dTc","6h8dTh","6h8hTd","6d8hTh","6h8hTh","6h8dJc","6h8dJh","6h8hJd","6d8hJh","6h8hJh","6h8dQc","6h8dQh","6h8hQd","6d8hQh","6h8hQh","6h8dKc","6h8dKh","6h8hKd","6d8hKh","6h8hKh","6h8dAc","6h8dAh","6h8hAd","6d8hAh","6h8hAh","6h9h9d","6h9d9c","6h9dTc","6h9dTh","6h9hTd","6d9hTh","6h9hTh","6h9dJc","6h9dJh","6h9hJd","6d9hJh","6h9hJh","6h9dQc","6h9dQh","6h9hQd","6d9hQh","6h9hQh","6h9dKc","6h9dKh","6h9hKd","6d9hKh","6h9hKh","6h9dAc","6h9dAh","6h9hAd","6d9hAh","6h9hAh","6hThTd","6hTdTc","6hTdJc","6hTdJh","6hThJd","6dThJh","6hThJh","6hTdQc","6hTdQh","6hThQd","6dThQh","6hThQh","6hTdKc","6hTdKh","6hThKd","6dThKh","6hThKh","6hTdAc","6hTdAh","6hThAd","6dThAh","6hThAh","6hJhJd","6hJdJc","6hJdQc","6hJdQh","6hJhQd","6dJhQh","6hJhQh","6hJdKc","6hJdKh","6hJhKd","6dJhKh","6hJhKh","6hJdAc","6hJdAh","6hJhAd","6dJhAh","6hJhAh","6hQhQd","6hQdQc","6hQdKc","6hQdKh","6hQhKd","6dQhKh","6hQhKh","6hQdAc","6hQdAh","6hQhAd","6dQhAh","6hQhAh","6hKhKd","6hKdKc","6hKdAc","6hKdAh","6hKhAd","6dKhAh","6hKhAh","6hAhAd","6hAdAc","7h7d7c","7h7d8h","7h7d8c","7h7d9h","7h7d9c","7h7dTh","7h7dTc","7h7dJh","7h7dJc","7h7dQh","7h7dQc","7h7dKh","7h7dKc","7h7dAh","7h7dAc","7h8h8d","7h8d8c","7h8d9c","7h8d9h","7h8h9d","7d8h9h","7h8h9h","7h8dTc","7h8dTh","7h8hTd","7d8hTh","7h8hTh","7h8dJc","7h8dJh","7h8hJd","7d8hJh","7h8hJh","7h8dQc","7h8dQh","7h8hQd","7d8hQh","7h8hQh","7h8dKc","7h8dKh","7h8hKd","7d8hKh","7h8hKh","7h8dAc","7h8dAh","7h8hAd","7d8hAh","7h8hAh","7h9h9d","7h9d9c","7h9dTc","7h9dTh","7h9hTd","7d9hTh","7h9hTh","7h9dJc","7h9dJh","7h9hJd","7d9hJh","7h9hJh","7h9dQc","7h9dQh","7h9hQd","7d9hQh","7h9hQh","7h9dKc","7h9dKh","7h9hKd","7d9hKh","7h9hKh","7h9dAc","7h9dAh","7h9hAd","7d9hAh","7h9hAh","7hThTd","7hTdTc","7hTdJc","7hTdJh","7hThJd","7dThJh","7hThJh","7hTdQc","7hTdQh","7hThQd","7dThQh","7hThQh","7hTdKc","7hTdKh","7hThKd","7dThKh","7hThKh","7hTdAc","7hTdAh","7hThAd","7dThAh","7hThAh","7hJhJd","7hJdJc","7hJdQc","7hJdQh","7hJhQd","7dJhQh","7hJhQh","7hJdKc","7hJdKh","7hJhKd","7dJhKh","7hJhKh","7hJdAc","7hJdAh","7hJhAd","7dJhAh","7hJhAh","7hQhQd","7hQdQc","7hQdKc","7hQdKh","7hQhKd","7dQhKh","7hQhKh","7hQdAc","7hQdAh","7hQhAd","7dQhAh","7hQhAh","7hKhKd","7hKdKc","7hKdAc","7hKdAh","7hKhAd","7dKhAh","7hKhAh","7hAhAd","7hAdAc","8h8d8c","8h8d9h","8h8d9c","8h8dTh","8h8dTc","8h8dJh","8h8dJc","8h8dQh","8h8dQc","8h8dKh","8h8dKc","8h8dAh","8h8dAc","8h9h9d","8h9d9c","8h9dTc","8h9dTh","8h9hTd","8d9hTh","8h9hTh","8h9dJc","8h9dJh","8h9hJd","8d9hJh","8h9hJh","8h9dQc","8h9dQh","8h9hQd","8d9hQh","8h9hQh","8h9dKc","8h9dKh","8h9hKd","8d9hKh","8h9hKh","8h9dAc","8h9dAh","8h9hAd","8d9hAh","8h9hAh","8hThTd","8hTdTc","8hTdJc","8hTdJh","8hThJd","8dThJh","8hThJh","8hTdQc","8hTdQh","8hThQd","8dThQh","8hThQh","8hTdKc","8hTdKh","8hThKd","8dThKh","8hThKh","8hTdAc","8hTdAh","8hThAd","8dThAh","8hThAh","8hJhJd","8hJdJc","8hJdQc","8hJdQh","8hJhQd","8dJhQh","8hJhQh","8hJdKc","8hJdKh","8hJhKd","8dJhKh","8hJhKh","8hJdAc","8hJdAh","8hJhAd","8dJhAh","8hJhAh","8hQhQd","8hQdQc","8hQdKc","8hQdKh","8hQhKd","8dQhKh","8hQhKh","8hQdAc","8hQdAh","8hQhAd","8dQhAh","8hQhAh","8hKhKd","8hKdKc","8hKdAc","8hKdAh","8hKhAd","8dKhAh","8hKhAh","8hAhAd","8hAdAc","9h9d9c","9h9dTh","9h9dTc","9h9dJh","9h9dJc","9h9dQh","9h9dQc","9h9dKh","9h9dKc","9h9dAh","9h9dAc","9hThTd","9hTdTc","9hTdJc","9hTdJh","9hThJd","9dThJh","9hThJh","9hTdQc","9hTdQh","9hThQd","9dThQh","9hThQh","9hTdKc","9hTdKh","9hThKd","9dThKh","9hThKh","9hTdAc","9hTdAh","9hThAd","9dThAh","9hThAh","9hJhJd","9hJdJc","9hJdQc","9hJdQh","9hJhQd","9dJhQh","9hJhQh","9hJdKc","9hJdKh","9hJhKd","9dJhKh","9hJhKh","9hJdAc","9hJdAh","9hJhAd","9dJhAh","9hJhAh","9hQhQd","9hQdQc","9hQdKc","9hQdKh","9hQhKd","9dQhKh","9hQhKh","9hQdAc","9hQdAh","9hQhAd","9dQhAh","9hQhAh","9hKhKd","9hKdKc","9hKdAc","9hKdAh","9hKhAd","9dKhAh","9hKhAh","9hAhAd","9hAdAc","ThTdTc","ThTdJh","ThTdJc","ThTdQh","ThTdQc","ThTdKh","ThTdKc","ThTdAh","ThTdAc","ThJhJd","ThJdJc","ThJdQc","ThJdQh","ThJhQd","TdJhQh","ThJhQh","ThJdKc","ThJdKh","ThJhKd","TdJhKh","ThJhKh","ThJdAc","ThJdAh","ThJhAd","TdJhAh","ThJhAh","ThQhQd","ThQdQc","ThQdKc","ThQdKh","ThQhKd","TdQhKh","ThQhKh","ThQdAc","ThQdAh","ThQhAd","TdQhAh","ThQhAh","ThKhKd","ThKdKc","ThKdAc","ThKdAh","ThKhAd","TdKhAh","ThKhAh","ThAhAd","ThAdAc","JhJdJc","JhJdQh","JhJdQc","JhJdKh","JhJdKc","JhJdAh","JhJdAc","JhQhQd","JhQdQc","JhQdKc","JhQdKh","JhQhKd","JdQhKh","JhQhKh","JhQdAc","JhQdAh","JhQhAd","JdQhAh","JhQhAh","JhKhKd","JhKdKc","JhKdAc","JhKdAh","JhKhAd","JdKhAh","JhKhAh","JhAhAd","JhAdAc","QhQdQc","QhQdKh","QhQdKc","QhQdAh","QhQdAc","QhKhKd","QhKdKc","QhKdAc","QhKdAh","QhKhAd","QdKhAh","QhKhAh","QhAhAd","QhAdAc","KhKdKc","KhKdAh","KhKdAc","KhAhAd","KhAdAc","AhAdAc" };
const std::vector<std::string>GameTypeName{"Max6_NL50_SD200","Max6_NL50_SD150","Max6_NL50_SD100","Max6_NL50_SD75","Max6_NL50_SD50","Max6_NL50_SD20",""};
const std::array<std::string, 6> PositionSymble{"UTG","HJ","CO","BTN","SB","BB"};
const std::array<int, 5> CandicateStackDepth{ 20,50,75,100,150 };
const std::string sDoubleNLimpRAbbr{ "#_vs_nlimp2raise" };
const std::array<std::string, 169> AbbrComboMapping{ "22","33","44","55","66","77","88","99","AA","KK","QQ","JJ","TT","AKs","AKo","AQs","AJs","AQo","KQs","ATs","AJo","KJs","KQo","QJs","KTs","A9s","ATo","QTs","A8s","JTs","A7s","A5s","KJo","K9s","A4s","A6s","Q9s","A3s","T9s","QJo","J9s","A2s","KTo","A9o","K8s","QTo","K7s","JTo","Q8s","K6s","T8s","A8o","J8s","K5s","98s","A5o","A7o","K4s","K9o","Q7s","K3s","Q6s","A4o","T7s","A6o","K2s","J7s","Q9o","T9o","Q5s","97s","A3o","J9o","87s","Q4s","Q3s","A2o","K8o","76s","Q2s","J6s","T6s","J5s","65s","96s","86s","54s","K7o","J4s","Q8o","T8o","J3s","J8o","K6o","75s","98o","J2s","T5s","K5o","T4s","64s","53s","85s","95s","T3s","K4o","T2s","43s","Q7o","74s","K3o","Q6o","T7o","J7o","97o","63s","87o","K2o","52s","Q5o","93s","84s","94s","42s","92s","Q4o","32s","73s","76o","Q3o","65o","54o","86o","T6o","Q2o","J6o","96o","62s","J5o","83s","82s","J4o","75o","J3o","72s","64o","53o","J2o","85o","T5o","95o","T4o","43o","T3o","T2o","74o","63o","52o","42o","84o","93o","94o","92o","32o","73o","62o","82o","83o","72o" };

const double ALLIN_THRESHOLD_RAISE = 0.4;   //(暂时不用)非river时，当hero动作为raise,raise后剩余筹码占底池比例<ALLIN_THRESHOLD_RAISE,则raise变成allin
const double ALLIN_THRESHOLD_CALL = 0.2;    //当hero剩余筹码占当时底池<ALLIN_THRESHOLD_CALL时，最差临界牌型也要跟注

const double BOUND_EXCEED_THREADHOLD = 0.05; //下注size匹配，当超过阈值时认为为夹层
const double UP_BOUND_EXCEED_THREADHOLD = 0.2; //超过最大下注（不包括allin）该比例时认为越界

const double SELECT_PRECISION_SRC = 0.001; //匹配exploi actionToMatch %0.1
const double SELECT_PRECISION_DES = 0.005; //匹配exploi actionToReplace %0.5
const double EXPLOI_MATCH_DIS = 0.03;       //匹配size的误差 3%



typedef enum { Max6_NL50_SD200, Max6_NL50_SD150, Max6_NL50_SD100, Max6_NL50_SD75, Max6_NL50_SD50, Max6_NL50_SD20, GAMETYPE_none}GameType; //SD为筹码深度，每个筹码深度相关配置和数据都独立
const std::array<GameType, 6>GameTypes{ Max6_NL50_SD200, Max6_NL50_SD150, Max6_NL50_SD100, Max6_NL50_SD75, Max6_NL50_SD50, Max6_NL50_SD20 };
typedef enum { preflop, flop, turn, river }Round;
typedef enum { UTG, HJ, CO, BTN, SB, BB, nonepos }Position; //nonepos作为算法标识
typedef enum { OOP, IP }RelativePosition;
typedef enum { OOPA, OOPD }OOPX;
typedef enum { none,call, fold, raise, allin, check}ActionType; //none作为算法标识
typedef enum { h, d, c, s,n }Suit; //h：红桃，d：方块，c：草花，s：黑桃 //n标记
typedef enum { match_preflop, match_toflop }ActionMatchMode;	//用正则匹配NodeName的模式
typedef enum { from_wizard, from_solver_presave, from_solver_realtime, from_solver_calc, multi_players, from_strategy_file }StrategyFrom;
typedef enum { calc_sync, calc_async }SolverCalcMode; //计算solver解用同步还是异步模式
typedef enum { space_bet, space_raise,space_donk }SpaceMode; //获取策略树的部分

typedef enum { op_value, op_bluff, op_all }OP_obj; //作用的对象
typedef enum { adjust_average, adjust_ascend, adjust_descend }adjust_methord; //调整方法,平均、升序、降序
typedef enum { by_whole, by_action, by_valid }ratio_type; //按总范围，按调整的动作总范围，按符合条件的范围和


typedef std::unordered_map<std::string, double> StrategyData;
typedef std::unordered_map<std::string, double> EVData;

typedef std::array<double, COMBO_COUNT> WizardStrategyData;
typedef std::array<double, COMBO_COUNT> WizardEVData;
typedef std::unordered_map<std::string, double> RangeData;
typedef std::vector<std::pair<std::string, std::string>> RegexTB; //preflop节点类型匹配表

const std::unordered_map<Position, int> PositionRank{ {SB,0},{BB,1},{UTG,2},{HJ,3},{CO,4},{BTN,5} };

const std::string sDefaultPreflopStrategy{ "no_match_default" };

typedef struct tagAction {
	ActionType actionType = none;
	float fBetSize = 0; //下注bb数，不为0则启用，否则使用fBetSizeByPot，
	float fBetSizeByPot = 0; //下注占底池比例
	float fBetSizeByCash = 0; //下注现金数，只用于solution返回时，当 = -1时代表通配

	bool operator==(const tagAction& p)//raise 
	{
		if (actionType == p.actionType) {
			if (actionType == raise) {
				bool blEqual = false;
				if(fabs(fBetSizeByCash + 1) < 0.00001 || fabs(p.fBetSizeByCash + 1) < 0.00001) //?
					blEqual = true;
				else if (fBetSize != 0 || p.fBetSize != 0) {
					if (fabs(fBetSize - p.fBetSize) < 0.00001)
						blEqual = true;
				}
				else if (fBetSizeByPot != 0 || p.fBetSizeByPot != 0) {
					if (fabs(fBetSizeByPot - p.fBetSizeByPot) < 0.00001)
						blEqual = true;
				}
                else if (p.fBetSizeByPot == 0 && p.fBetSize == 0 && p.fBetSizeByPot == 0) //3个都为0代表不指定
                    blEqual = true;

				return blEqual;
			}
			else
				return true;
		}
		else
			return false;
	}

    bool operator>(const tagAction& p)
    {
        std::map<ActionType, int> actionRanks{ {fold, 0}, { check,1 }, { call,2 }, { raise,3 }, { allin,4 }};
        if (actionRanks[actionType] > actionRanks[p.actionType])
            return true;
        else {
            if (actionType == raise) {
                if (fBetSizeByPot > p.fBetSizeByPot)
                    return true;
            }  
        }
        return false;
    }
}Action;

typedef struct tagSuitReplace {
	bool blIsNeeded = false; //preflop必须为false,其他各街不在同构集合中则为true
	Suit h2; //h转为
	Suit d2; //d转为
	Suit c2; //c转为
	Suit s2; //s转为
}SuitReplace;

//策略对应的筹码，为每条街开始时的数据
typedef struct tagStacks {
	double dEStack;	//实际有效筹码
	double dPot;	//实际底池
}Stacks;



//以下是牌型分析相关定义

typedef enum { MAX_HIGH_CARD, MAX_PAIR, MAX_TWO_PAIR, MAX_THREE_OF_A_KIND, MAX_STRAIGHT, MAX_FLUSH, MAX_FULL_HOUSE, MAX_FOUR_OF_A_KIND, MAX_STRAIGHT_FLUSH, DRAW_STRAIGHT, DRAW_FLUSH, DRAW_OTHERS, DRAW_NONE }PokerClass;
const std::map<char, int> c2p{ {'2',2},{'3',3},{'4',4},{'5',5},{'6',6},{'7',7},{'8',8},{'9',9},{'T',10},{'J',11},{'Q',12},{'K',13},{'A',14} };
const std::map<char, Suit> c2s{ {'h',h},{'c',c}, {'s',s}, {'d',d} };
const std::vector<std::string> PokerClassString{ "HIGH_CARD", "PAIR", "TWO_PAIR", "THREE_OF_A_KIND", "STRAIGHT", "FLUSH", "FULL_HOUSE", " FOUR_OF_A_KIND", "STRAIGHT_FLUSH", "DRAW_STRAIGHT", "DRAW_FLUSH", "DRAW_OTHERS", "DRAW_NONE" };

const int ev_nuts = 0;
const int ev_sec_nuts = 1;
const int ev_large = 2;
const int ev_sec_large = 3;
const int ev_middle = 4;
const int ev_small = 5;
const int ev_catch_bulff = 6;
const int ev_nothing = 7;

const int evsub_large = 0;
const int evsub_middle = 1;
const int evsub_small = 2;

const int high_card_point = 11;

class myCard {
public:
    Suit m_suit;
    int m_point;

    bool operator == (const myCard& p) const {
        return m_point == p.m_point && m_suit == p.m_suit;
    }

    friend bool operator > (myCard& p1, myCard& p2) {
        return p1.m_point > p2.m_point;
    }

    bool operator <  (const myCard& p2) const {
        return m_point < p2.m_point;
    }


    myCard(const std::string& c) {
        m_point = c2p.at(c[0]);
        m_suit = c2s.at(c[1]);
    }

    myCard(const Suit suit, const int point) {
        m_suit = suit;
        m_point = point;
    }

    myCard() {};

};
const std::vector<std::string> Deck{ "2h","2c","2d","2s","3h","3c","3d","3s", "4h","4c","4d","4s", "5h","5c","5d","5s", "6h","6c","6d","6s", "7h","7c","7d","7s", "8h","8c","8d","8s", "9h","9c","9d","9s", "Th","Tc","Td","Ts", "Jh","Jc","Jd","Js", "Qh","Qc","Qd","Qs", "Kh","Kc","Kd","Ks", "Ah","Ac","Ad","As" };
typedef std::vector<myCard> MyCards;

//draw牌结构
typedef struct tag_DrawStruct {
    PokerClass pokerClass; //牌型,DRAW_
    int nNeedHandsNum = 0;
    int nOutsNum = 0;           //顺花相关outs
    int nRank;
    int nAppendOutsNum = 0;    //其他outs
}DrawStruct;

//成手牌结构
typedef struct tag_MadehandStruct {
    int nNeedHandsNum = 0; //0则代表不成立
    PokerClass pokerClass; //牌型
    int nRank; //按不同牌型解释
    int nAppend = 0;//根据牌型设定的附加说明
}MadehandStruct;

//公牌结构
typedef struct tag_PublicStruct {
    PokerClass pokerClass; //最大牌型
    int nNeedtoFlush = 0; //1,2,3,4
    int nNeedtoStraight = 0; //1,2
    int nPair = 0;
    int nAppend = 0; //当river为同花顺、顺、葫芦时，1代表是nuts,2代表转为抓咋牌（river时公牌为nuts则不可能flod,足够大时对手可能在咋呼）
    int nMaxRank = 0; //最大张
    //ex for exploi;
    int nHighCardCount = 0; //高牌数量
    bool blOESDpossible = false; //两张点数差<=3(不包括A，只用于flop)
}PublicStruct;

typedef struct tag_OnecardToStraight {
    std::vector<int> cardPoints;
    std::vector<int> outsPoints;
}OnecardToStraight;

typedef struct tag_RankGroup {
    //int nMadeOrDraw = 0; //0成牌，1draw牌
    int nMainGroup = -1; //7大类
    int nSubGroup = -1;  //3小类
    double dlScore = 0;


    bool operator > (const tag_RankGroup& p) const//raise 
    {
        if (nMainGroup == -1)
            return false;
        if (nMainGroup != -1 && p.nMainGroup == -1)
            return true;

        if (nMainGroup < p.nMainGroup)
            return true;
        else if (nMainGroup == p.nMainGroup) {
            if (nSubGroup < p.nSubGroup)
                return true;
            else
                return false;
        }
        else
            return false;

        return true;

    }

    bool operator == (const tag_RankGroup& p) const//raise 
    {
        if (nMainGroup == p.nMainGroup && nSubGroup == p.nSubGroup)
            return true;
        else
            return false;
    }

    bool operator < (const tag_RankGroup& p) const//raise 
    {
        if (nMainGroup == -1)
            return false;
        if (nMainGroup != -1 && p.nMainGroup == -1)
            return false;

        if (nMainGroup > p.nMainGroup)
            return true;
        else if (nMainGroup == p.nMainGroup) {
            if (nSubGroup > p.nSubGroup)
                return true;
            else
                return false;
        }
        else
            return false;

        return true;

    }

    bool operator >= (const tag_RankGroup& p) const//raise 
    {
        if (nMainGroup == -1)
            return false;
        if (nMainGroup != -1 && p.nMainGroup == -1)
            return true;

        if (nMainGroup < p.nMainGroup)
            return true;
        else if (nMainGroup == p.nMainGroup) {
            if (nSubGroup <= p.nSubGroup)
                return true;
            else
                return false;
        }
        else
            return false;

        return true;

    }

}RankGroup;

//牌型的最终评价
typedef struct tag_PokerEvaluate {
    RankGroup rankGroup;    //一般只用这个
    MadehandStruct madehandStruct;
    DrawStruct drawStruct;
    PublicStruct publicStruct;
}PokerEvaluate;


//多人策略相关定义
typedef enum { multi_front, multi_between, multi_back, multi_none }Multi_Position; //none代表不做判断
typedef enum { hero_initiative, hero_passive ,hero_none}Multi_HeroActive; //none代表不做判断

typedef enum { cs_less, cs_less_equal,cs_equal,cs_large,cs_large_equal }CompareSymbol;

typedef struct tag_MultiCondition {
    Round Round;
    std::string sActionLine; 
    int nPlayerCount;
    double dlBetSize;   //为底池比例
    double dlSpr;       //为新一轮开始时的筹码底池比，有效筹码为hero和其他玩家最大筹码取较小值
    Multi_Position MultiPosition; //对人中的位置，前中后
    Multi_HeroActive HeroActive;
    double dlSprCurrent; //hero动作前,当前hero的有效筹码和底池的比例，用于计算raise allin的阈值和最低标准的call

    std::string sOriActionInfo; //用于计算raise的size
    Position heroPosition;
    double dPot;        //换街初始的pot
    double dHeroEStack;    //换街初始的hero剩余筹码
}MultiCondition;    //hero的位置

//公牌条件相关定义
typedef enum { pc_flush, pc_straight, pc_pair, pc_high, pc_none }PublicConditionToCheck;
typedef enum { lc_and, lc_or, lc_none }LogicCompareSymbol;
typedef struct tag_PublicCondition {
    PublicConditionToCheck pcToCheck;
    CompareSymbol rankComparer;
    int nCount;
    LogicCompareSymbol lcComparer;
}PublicCondition;


#endif // !GLOBLEDEF_H_
