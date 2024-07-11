OpenGL을 이용한 간단한 3d모델 생성 및 렌더링 프로그램입니다.

## 실행환경
```
Visual Studio 2019
```
## 이용방법
* SOR의 소스코드를 먼저 실행하여 메쉬를 생성하여 SaveModel을 한 후,
* 이후 프로그램 정지 후, SOR을 주석 처리 한 뒤 showModel의 소스코드를 실행해주세요.

## Getting Strated
![image](https://github.com/Wisdom-Kim/OpenGL-CreateAndRendering3DModel/assets/52325199/d35ba8d9-d0b2-4f46-8b00-be5e96cb4d00)
1. 파일을 실행하기 전, **프로젝트>속성>일반** 에서 다음과 같이 설정해주세요.
```
추가라이브러리 디렉토리: (현재 레포에 올린 폴더)freeglut 내 include 경로
```
2. **프로젝트>속성>링커>모든옵션**에서 다음과 같이 설정해주세요.
```
추가라이브러리 디렉토리: freeglut 폴더 내 lib 경로
추가종속성
freeglut.lib
%(AdditionalDependencies)
```


# SOR.cpp
마우스를 통해 점을 찍고, 회전축을 통해 원하는 각도 주기로 점을 회전시켜 배열에 저장합니다.<br>
이 때 저장된 점을 이어 메쉬로 만들고, 3d모델 파일 객체(.dat)로 저장할 수 있습니다.
* ### 1. 마우스 왼쪽 버튼을 통해 점을 찍을 수 있습니다.
![image](https://github.com/Wisdom-Kim/WebGL-CreateAndRendering3DModel/assets/52325199/1ebb295a-29ce-4d07-b136-d2f0d45fe458)
* ### 2. 1번에서 찍은 점들을 회전축, 회전각을 정의하여 여러 점을 만들 수 있습니다.
![image](https://github.com/Wisdom-Kim/WebGL-CreateAndRendering3DModel/assets/52325199/8f4f3520-cf9e-4838-bab1-6d62028094d9)
![image](https://github.com/Wisdom-Kim/WebGL-CreateAndRendering3DModel/assets/52325199/7a331333-04d0-4130-9291-8232e79e4d94)
* ### 3. 마우스 오른쪽 - CreateMesh를 선택을 선택하면, 생성된 점을 바탕으로 메쉬를 생성합니다.
![image](https://github.com/Wisdom-Kim/WebGL-CreateAndRendering3DModel/assets/52325199/35ee1380-508f-4e70-b869-50e2ce5d3ccc)
* ### 4. SaveModel을 선택하면 3에서 생성된 Mesh를 기반으로 3D 모델을 생성합니다.

# showModel.cpp
### 1. SOR.cpp가 생성한 customModel.dat을 불러와 보여줍니다.
![image](https://github.com/Wisdom-Kim/WebGL-CreateAndRendering3DModel/assets/52325199/de672613-2627-4f2a-99aa-bdd0f8ab5129)
### 2. 왼쪽마우스 - 회전 (y축 기준으로 회전만 가능) , 마우스 중간 - 스케일
### 3. d/s 로 Flat Shade , Smooth Shade 적용이 가능합니다.
![image](https://github.com/Wisdom-Kim/WebGL-CreateAndRendering3DModel/assets/52325199/66942881-efdd-49b6-b224-fbd2db4d9d61)
![image](https://github.com/Wisdom-Kim/WebGL-CreateAndRendering3DModel/assets/52325199/fdb485be-4c1a-4279-a613-bed04a10387e)
