#include "stdafx.h"
#include "GeometryHelper.h"
using namespace DirectX;


void GeometryHelper::CreateBox(float width, float height, float depth,
    std::vector<StandardVertex>& outVertices,
    std::vector<uint32_t>& outIndices)
{
    outVertices.clear();
    outIndices.clear();

    // 중심(0,0,0)에서부터 각 모서리까지의 절반 거리 계산
    float w2 = width * 0.5f;
    float h2 = height * 0.5f;
    float d2 = depth * 0.5f;

    // 1. 24개의 정점 데이터 세팅 (6면 * 면당 4정점)
    outVertices.resize(24);

    // [면 1] 앞면 (Front: -Z 방향)
    outVertices[0] = { XMFLOAT3(-w2, -h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) }; // 좌하단
    outVertices[1] = { XMFLOAT3(-w2,  h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) }; // 좌상단
    outVertices[2] = { XMFLOAT3(w2,  h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) }; // 우상단
    outVertices[3] = { XMFLOAT3(w2, -h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) }; // 우하단

    // [면 2] 뒷면 (Back: +Z 방향)
    outVertices[4] = { XMFLOAT3(w2, -h2,  d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) };
    outVertices[5] = { XMFLOAT3(w2,  h2,  d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) };
    outVertices[6] = { XMFLOAT3(-w2,  h2,  d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) };
    outVertices[7] = { XMFLOAT3(-w2, -h2,  d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) };

    // [면 3] 윗면 (Top: +Y 방향)
    outVertices[8] = { XMFLOAT3(-w2,  h2, -d2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) };
    outVertices[9] = { XMFLOAT3(-w2,  h2,  d2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) };
    outVertices[10] = { XMFLOAT3(w2,  h2,  d2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) };
    outVertices[11] = { XMFLOAT3(w2,  h2, -d2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) };

    // [면 4] 아랫면 (Bottom: -Y 방향)
    outVertices[12] = { XMFLOAT3(-w2, -h2,  d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) };
    outVertices[13] = { XMFLOAT3(-w2, -h2, -d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) };
    outVertices[14] = { XMFLOAT3(w2, -h2, -d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) };
    outVertices[15] = { XMFLOAT3(w2, -h2,  d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) };

    // [면 5] 왼쪽면 (Left: -X 방향)
    outVertices[16] = { XMFLOAT3(-w2, -h2,  d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) };
    outVertices[17] = { XMFLOAT3(-w2,  h2,  d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) };
    outVertices[18] = { XMFLOAT3(-w2,  h2, -d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) };
    outVertices[19] = { XMFLOAT3(-w2, -h2, -d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) };

    // [면 6] 오른쪽면 (Right: +X 방향)
    outVertices[20] = { XMFLOAT3(w2, -h2, -d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) };
    outVertices[21] = { XMFLOAT3(w2,  h2, -d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) };
    outVertices[22] = { XMFLOAT3(w2,  h2,  d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) };
    outVertices[23] = { XMFLOAT3(w2, -h2,  d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) };

    // 2. 36개의 인덱스 데이터 세팅 (6면 * 면당 2개 삼각형 * 삼각형당 3정점)
    outIndices.resize(36);

    // 정점을 면(Face) 단위로 독립시켰기 때문에, 인덱스는 동일한 패턴의 반복으로 처리할 수 있습니다.
    int indexOffset = 0;
    for (int i = 0; i < 6; ++i) {
        // 첫 번째 삼각형 (0번, 1번, 2번 정점)
        outIndices[indexOffset++] = i * 4 + 0;
        outIndices[indexOffset++] = i * 4 + 1;
        outIndices[indexOffset++] = i * 4 + 2;

        // 두 번째 삼각형 (0번, 2번, 3번 정점)
        outIndices[indexOffset++] = i * 4 + 0;
        outIndices[indexOffset++] = i * 4 + 2;
        outIndices[indexOffset++] = i * 4 + 3;
    }
}