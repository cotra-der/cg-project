#pragma once
#include <vector>
namespace FaceLandmarks {
inline const std::vector<int> FACE_OVAL={10,338,284,389,454,397,365,379,152,149,136,172,234,162,54,109};
inline const std::vector<int> LEFT_EYE={33,160,158,133,153,144};
inline const std::vector<int> RIGHT_EYE={362,385,387,263,373,380};
inline const std::vector<int> LEFT_EYEBROW={70,63,105,107};
inline const std::vector<int> RIGHT_EYEBROW={336,334,296,300};
inline const std::vector<int> OUTER_LIPS={61,40,0,270,291,321,17,91};
inline const std::vector<int> INNER_LIPS={78,81,13,311,308,402,14,178};
inline const std::vector<int> NOSE_BRIDGE={168,6,1};
inline const std::vector<int> NOSE_BOTTOM={98,2,327};
inline const std::vector<std::vector<int>> GROUPS={FACE_OVAL,LEFT_EYE,RIGHT_EYE,LEFT_EYEBROW,RIGHT_EYEBROW,OUTER_LIPS,INNER_LIPS,NOSE_BRIDGE,NOSE_BOTTOM};
}
