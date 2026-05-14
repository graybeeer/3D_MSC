#pragma once
#include <vector>
struct StandardVertex {
    XMFLOAT3 Position;  // 정점의 위치
    XMFLOAT3 Normal;    // 정점이 바라보는 방향 (조명 계산용)
    XMFLOAT2 TexCoord;  // 텍스처 맵핑 좌표 (UV)
};
class GeometryHelper {
public:
    static void CreateBox(float width, float height, float depth,
        std::vector<StandardVertex>& outVertices,
        std::vector<uint32_t>& outIndices);
};