<div align="center">
  
# 노 모어 히어로즈3 모작(C++, DirectX 11, 팀 프로젝트(팀원))
<img src="https://github.com/SHim719/Image/blob/main/%EB%85%B8%EB%AA%A8%EC%96%B4%EC%8D%B8%EB%84%A4%EC%9D%BC.webp" alt="이미지" width="500">

#### 역할: Shader, MiniGame 1

</div>

# 동영상
https://youtu.be/7j95oUnhB3o (플레이영상)

https://youtu.be/UGrMuIEuBsc (개인 파트 설명 영상)

# 프로젝트 구현사항

## 쉐이더

### PBR
<img src="https://github.com/SHim719/Image/blob/main/PBR.png" alt="이미지" width="500"> 

- No More Heroes 3"는 PBR 머티리얼을 사용하고 있었기 때문에, 조명 연산에서 퐁 쉐이딩 대신 PBR 방식을 적용했습니다.

- 첫 번째 패스에서 Diffuse, Normal, ORM(Occlusion, Roughness, Metal)을 G-버퍼에 기록한 뒤, 디퍼드 조명 연산을 수행했습니다.

### Bloom + HDR
<img src="https://github.com/SHim719/Image/blob/main/Bloom1.gif" alt="이미지" width="400"> <img src="https://github.com/SHim719/Image/blob/main/Bloom2.gif" alt="이미지" width="400">

- "PBR의 메탈 성분이 강렬한 빛 반사를 표현할 수 있도록 하고, 게임 이펙트의 다채로운 표현을 강화하기 위해 Bloom 효과를 구현했습니다.
  
- 스크린에서 밝은 픽셀만 추출한 뒤 블러 처리하고, 이를 원본 스크린 색상에 더해 Bloom 효과를 구현했습니다. 또한, Bloom 처리할 때 HDR 색상을 표현해야 했기 때문에, 톤 매핑을 사용해 HDR 색상을 LDR 색상으로 변환했습니다.


### SSAO(Screen Space Ambient Occulusion)
<img src="https://github.com/SHim719/Image/blob/main/SSAO1.png" alt="이미지" width="500"> <img src="https://github.com/SHim719/Image/blob/main/SSAO2.png" alt="이미지" width="500"> 

- 장면의 깊이감과 사실감을 높이기 위해 주변 음영 처리 기법인 SSAO를 구현했습니다.
  
- 픽셀의 뷰 공간 깊이와 임의 크기의 반구 위의 여러 점의 뷰 공간 깊이를 비교하여, 반 구위의 점의 깊이 값이 더 크면 차폐 처리를 적용했습니다.


### SSR(Screen Space Reflection)
<img src="https://github.com/SHim719/Image/blob/main/SSR.gif" alt="이미지" width="500">

- 바닥에 자연스럽고 예쁜 반사 효과를 구현하고 싶었지만, 성능까지 고려해야 했습니다. 이를 해결하기 위해 Screen Space Reflection(SSR)을 사용하여 효율적인 반사 효과를 구현했습니다.
  
- 뷰 공간에서 뷰 벡터와 노멀을 활용하여 반사 벡터를 계산한 뒤, Ray Marching 기법을 사용해 스크린 상에서 반사된 지점을 도출했습니다. 최종적으로 반사된 지점의 픽셀 색상과 원래 스크린의 색상을 혼합하여 자연스러운 반사 효과를 구현했습니다.

### DOF(Depth Of Field)
<img src="https://github.com/SHim719/Image/blob/main/DOF.gif" alt="이미지" width="500">

- 멀리 있는 물체를 흐릿하게 하는 효과를 구현하고자, Depth Of Field를 구현했습니다.
- 깊이 버퍼를 사용하여 뷰 공간의 깊이를 구한 후, DOF(Depth of Field)가 적용되는 최대 깊이와 최소 깊이에 대한 비율을 기준으로 블러된 스크린 텍스처와 원본 스크린 텍스처를 혼합했습니다.

### CSM(Cascade Shadow Mapping)
<img src="https://github.com/SHim719/Image/blob/main/%EC%BA%90%EC%8A%A4%EC%BC%80%EC%9D%B4%EB%93%9C1.gif" alt="이미지" width="400"> <img src="https://github.com/SHim719/Image/blob/main/%EC%BA%90%EC%8A%A4%EC%BC%80%EC%9D%B4%EB%93%9C2.gif" alt="이미지" width="400">

- 야외 같이 큰 맵은 기존 방식의 쉐도우 매핑으로는 한계가 있다고 생각했고, 이를 해결하기 위해 가까이에 있는 물체의 그림자를 더 선명하게 그리고 멀리있는 그림자의 해상도를 낮추는 Cascade Shadow Mapping을 구현했습니다. 
- 뷰 프러스텀을 3등분 하여, 각각 뷰, 투영 행렬을 생성하고, 이 뷰, 투영 행렬을 이용하여 3개의 깊이 버퍼에 깊이를 기록했습니다.
- 이로 인해 한 물체에 대해 3번의 드로우 콜이 발생하는 문제가 발생했습니다. 이를 해결하기 위해 지오메트리 쉐이더를 사용하여 한 번의 드로우 콜로 3개의 깊이 버퍼에 동시에 깊이를 기록하도록 최적화했습니다.

### Stencil
<img src="https://github.com/SHim719/Image/blob/main/%EC%8A%A4%ED%85%90%EC%8B%A41.png" alt="이미지" width="400"> <img src="https://github.com/SHim719/Image/blob/main/%EC%8A%A4%ED%85%90%EC%8B%A42.gif" alt="이미지" width="400">

- 모자이크 효과, 암전 효과는 스텐실 버퍼를 이용해서 구현했습니다.
- 모자이크는 모자이크 처리할 사각형 영역, 암전 효과는 플레이어와 이펙트에 대해서 스텐실 버퍼를 기록해줍니다.
- 스텐실이 기록된 픽셀만 후처리를 적용합니다.

## 미니게임

<img src="https://github.com/SHim719/Image/blob/main/%EB%AF%B8%EB%8B%88%EA%B2%8C%EC%9E%841.gif" alt="이미지" width="400"> <img src="https://github.com/SHim719/Image/blob/main/%EB%AF%B8%EB%8B%88%EA%B2%8C%EC%9E%842.gif" alt="이미지" width="400">

- 탄막 슈팅 장르의 미니게임을 구현했습니다. 좌측의 진행바가 꽉 차면 보스가 등장하고 보스를 잡으면 시네마틱 연출 이후, 다음 레벨로 넘어갑니다.

### 발사체
- 플레이어, 용, 적 등 모든 발사체는 오브젝트 풀링 기법을 사용하여 미리 생성한 객체들을 재사용함으로써 성능을 최적화했습니다.

### 무한 순환 맵
- 약 10개의 도로 메쉬로 이루어진 한 단위 청크를 3개 생성하고, 플레이어 방향으로 청크를 이동시켜 플레이어가 전진하는 것처럼 보이게 구현했습니다.
- 매 프레임 청크의 위치를 확인해 청크가 일정 좌표 미만에 위치에 있다면, 다시 맨 뒤 청크에 이어 붙입니다.

### 3D 텍스트
<img src="https://github.com/SHim719/Image/blob/main/3D%ED%85%8D%EC%8A%A4%ED%8A%B8.png" alt="이미지" width="400">

- 보물상자 위의 텍스트, 플레이어 체력 텍스트는 계속 변하는 텍스트를 3D 공간 상에 띄워야 하는 문제가 있었습니다.
- 먼저 비어있는 렌더 타겟에 폰트를 렌더링 한 후, 사각형 메쉬를 3D 공간 상에 띄우고 해당 렌더 타겟을 쉐이더 리소스로 사용하여 텍스쳐 샘플링 해주었습니다.

### 슬롯머신 시스템
<img src="https://github.com/SHim719/Image/blob/main/%EB%A3%B0%EB%A0%9B.gif" alt="이미지" width="400">

- 슬롯머신은 세 개의 능력 텍스처를 순차적으로 돌리며, 일정 시간이 지난 후 랜덤으로 능력이 결정됩니다.
- 슬롯머신 연출은 매 프레임 증가하는 Offset을 이용해 하나의 능력 텍스쳐를 다른 하나의 능력 텍스쳐를 밀어내는 연출을 표현했습니다.

