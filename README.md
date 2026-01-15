<div align="center">

# 노 모어 히어로즈3 모작(팀 프로젝트(팀원))
<img src="https://github.com/SHim719/Image/blob/main/%EB%85%B8%EB%AA%A8%EC%96%B4%EC%8D%B8%EB%84%A4%EC%9D%BC.png" alt="이미지" width="500">

#### 역할: MainFramework, Shader, MiniGame 
#### 기술 스택: C++, DirectX 11, Visual Studio Graphics Debugger, ImGUI, FMOD

</div>

# 동영상
https://youtu.be/Cfby8ON7V2M


# 프로젝트 구현사항

## 쉐이더

### CSM(Cascaded Shadow Mapping)
<img src="https://github.com/SHim719/Image/blob/main/%EC%BA%90%EC%8A%A4%EC%BC%80%EC%9D%B4%EB%93%9C1.gif" alt="이미지" width="400"> <img src="https://github.com/SHim719/Image/blob/main/%EC%BA%90%EC%8A%A4%EC%BC%80%EC%9D%B4%EB%93%9C2.gif" alt="이미지" width="400">
<img src="https://github.com/SHim719/Image/blob/main/%EC%BA%90%EC%8A%A4%EC%BC%80%EC%9D%B4%EC%8A%A4%EC%A7%80%EC%98%A4%EB%A9%94%ED%8A%B8%EB%A6%AC.png" alt="이미지" width="500">

- 넓은 필드에서 근경은 선명하게, 원경은 효율적으로 그림자를 표현하기 위해 뷰 프러스텀을 분할하는 CSM 기법을 적용했습니다.
- **One-Pass Shadow Map**: Geometry Shader와 Texture2DArray를 활용하여, 여러 장의 섀도우 맵을 단 한 번의 드로우 콜로 렌더링하도록 최적화했습니다.
- **품질 개선**: 3x3 PCF 샘플링을 적용하여 그림자 경계의 계단 현상을 완화하고 부드러운 그림자를 구현했습니다.

### PBR
<img src="https://github.com/SHim719/Image/blob/main/PBR.png" alt="이미지" width="500"> 

- 노모어 히어로즈 3는 PBR 머티리얼을 사용하고 있었기 때문에, 조명 연산에서 퐁 쉐이딩 대신 PBR을 적용했습니다.
- 첫 번째 패스에서 Diffuse, Normal, ORM(Occlusion, Roughness, Metal)을 G-버퍼에 기록한 뒤, G-버퍼에 기록한 정보들을 토대로 디퍼드 렌더링으로 PBR 연산을 처리했습니다.

- ### SSR(Screen Space Reflection)
<img src="https://github.com/SHim719/Image/blob/main/SSR.gif" alt="이미지" width="500"><img src="https://github.com/SHim719/Image/blob/main/SSRRaymarch.png" alt="이미지" width="400">

- 바닥 재질의 리얼함을 살리기 위해 뷰 공간 정보를 활용한 SSR을 구현했습니다.
- Ray Marching으로 대략적인 반사 지점을 찾은 후, 이진 탐색을 통해 교차 지점을 정밀하게 보정함으로써 반사 품질을 높였습니다.

### Bloom + HDR
<img src="https://github.com/SHim719/Image/blob/main/Bloom1.gif" alt="이미지" width="400"> <img src="https://github.com/SHim719/Image/blob/main/Bloom2.gif" alt="이미지" width="400">
<img src="https://github.com/SHim719/Image/blob/main/Bloom%EC%BD%94%EB%93%9C.png" alt="이미지" width="400">

- PBR의 메탈 성분이 강렬한 빛 반사를 표현할 수 있도록 하고, 게임 이펙트의 다채로운 표현을 강화하기 위해 Bloom 효과를 구현했습니다.
- 스크린에서 밝은 픽셀만 추출한 뒤 블러 처리하고, 이를 원본 스크린 색상에 더해 Bloom 효과를 구현했습니다. 또한, Bloom 처리할 때 HDR 색상을 표현해야 했기 때문에, 톤 매핑을 사용해 HDR 색상을 LDR 색상으로 변환했습니다.
  
### DOF(Depth Of Field)
<img src="https://github.com/SHim719/Image/blob/main/DOF.gif" alt="이미지" width="500"><img src="https://github.com/SHim719/Image/blob/main/DOF%EC%BD%94%EB%93%9C.png" alt="이미지" width="500">

- 멀리 있는 물체를 흐릿하게 하는 효과를 구현하고자, Depth Of Field를 구현했습니다.
- 깊이 버퍼를 사용하여 뷰 공간의 깊이를 구한 후, DOF(Depth of Field)가 적용되는 최대 깊이와 최소 깊이에 대한 비율을 기준으로 블러된 스크린 텍스처와 원본 스크린 텍스처를 혼합했습니다.

### Stencil Effect
<img src="https://github.com/SHim719/Image/blob/main/%EC%8A%A4%ED%85%90%EC%8B%A41.png" alt="이미지" width="400"> <img src="https://github.com/SHim719/Image/blob/main/%EC%8A%A4%ED%85%90%EC%8B%A42.gif" alt="이미지" width="400">

- 모자이크 및 암전 효과가 필요한 특정 영역(플레이어, 사각형 범위 등)을 스텐실 버퍼에 표시하여, 해당 픽셀에만 선택적으로 후처리가 적용되도록 구현했습니다.

## 시네마틱 툴
<img src="https://github.com/SHim719/Image/blob/main/%EB%85%B8%EB%AA%A8%EC%96%B4%EC%8B%9C%EB%84%A4%EB%A7%88%ED%8B%B11.gif" alt="이미지" width="400"> <img src="https://github.com/SHim719/Image/blob/main/%EB%85%B8%EB%AA%A8%EC%96%B4%EC%8B%9C%EB%84%A4%EB%A7%88%ED%8B%B12.gif" alt="이미지" width="400">
<img src="https://github.com/SHim719/Image/blob/main/%EC%8B%9C%EB%84%A4%EB%A7%88%ED%88%B4.png" alt="이미지" width="800">

- 게임 내부의 시네마틱 제작의 편의를 위해 시네마틱 툴을 제작했습니다. 시간 순으로 정렬된 std::map을 이용해 키 프레임 데이터를 저장하고 시네마틱 실행시에 키 프레임 데이터를 보간하여 카메라, 시네마틱 액터의 움직임을 구현했습니다.
- 시네마틱을 관리하고 재생을 처리하는 CinematicManager를 설계하고, 시네마틱 시작과 종료 지점의 콜백 함수를 등록하여 시네마틱 시작이나 종료시에 자연스럽게 게임 흐름을 조정할 수 있도록 설계했습니다.


## 미니게임
<img src="https://github.com/SHim719/Image/blob/main/%EB%AF%B8%EB%8B%88%EA%B2%8C%EC%9E%841.gif" alt="이미지" width="400"> <img src="https://github.com/SHim719/Image/blob/main/%EB%AF%B8%EB%8B%88%EA%B2%8C%EC%9E%842.gif" alt="이미지" width="400">

- 탄막 슈팅 디펜스 장르의 미니게임을 구현했습니다. 좌측의 진행바가 꽉 차면 보스가 등장하고 보스를 잡으면 시네마틱 연출 이후, 다음 레벨로 넘어갑니다.

### 최적화 및 컨텐츠
- **오브젝트 풀링**: 탄막 슈팅 장르 특성상 빈번하게 발생하는 발사체 생성/파괴 비용을 줄이기 위해, 미리 할당된 객체를 재사용하는 풀링 시스템을 적용했습니다.
- **무한 순환 맵**: 여러 개의 도로 청크를 미리 생성하고 순환시키는 방식으로 무한히 이어지는 맵을 구현하여 메모리 사용량을 최소화했습니다.

### 3D 텍스트
<img src="https://github.com/SHim719/Image/blob/main/3D%ED%85%8D%EC%8A%A4%ED%8A%B8.png" alt="이미지" width="400">

- **RenderTarget-to-Texture**: 보물상자 위의 텍스트, 플레이어 체력 텍스트 3D 공간에 띄우기 위해 먼저 폰트를 렌더 타겟에 그린 후 이를 텍스처로 변환하여 사각형 메쉬에 입히는 방식을 고안했습니다.

### 슬롯머신 시스템
<img src="https://github.com/SHim719/Image/blob/main/%EB%A3%B0%EB%A0%9B.gif" alt="이미지" width="400"><img src="https://github.com/SHim719/Image/blob/main/%EC%8A%AC%EB%A1%AF%EB%A8%B8%EC%8B%A0.png" alt="이미지" width="400">

- **슬롯머신 연출**: UV 오프셋 애니메이션을 통해 텍스처가 밀려나는 연출을 구현하고, 확률에 기반한 랜덤 능력 부여 로직을 설계했습니다.
