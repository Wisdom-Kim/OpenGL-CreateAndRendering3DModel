## 이용방법
초기세팅방법 적어야함...(freeGLUT세팅 등..)

# SOR.cpp
### 1.
![image](https://github.com/Wisdom-Kim/WebGL-CreateAndRendering3DModel/assets/52325199/1ebb295a-29ce-4d07-b136-d2f0d45fe458)
### 2. 1번에서 찍은 점들을 회전축, 회전각을 정의하여 여러 점을 만들 수 있습니다.
![image](https://github.com/Wisdom-Kim/WebGL-CreateAndRendering3DModel/assets/52325199/8f4f3520-cf9e-4838-bab1-6d62028094d9)
![image](https://github.com/Wisdom-Kim/WebGL-CreateAndRendering3DModel/assets/52325199/7a331333-04d0-4130-9291-8232e79e4d94)
### 3. 마우스 오른쪽 - CreateMesh를 선택을 선택하면, 생성된 점을 바탕으로 메쉬를 생성합니다.
![image](https://github.com/Wisdom-Kim/WebGL-CreateAndRendering3DModel/assets/52325199/35ee1380-508f-4e70-b869-50e2ce5d3ccc)
### 4. SaveModel을 선택하면 3에서 생성된 Mesh를 기반으로 3D 모델을 생성합니다.

# showModel.cpp
### 1. SOR.cpp가 생성한 customModel.dat을 불러와 보여줍니다.
![image](https://github.com/Wisdom-Kim/WebGL-CreateAndRendering3DModel/assets/52325199/de672613-2627-4f2a-99aa-bdd0f8ab5129)
### 2. 왼쪽마우스 - 회전 (y축 기준으로 회전만 가능) , 마우스 중간 - 스케일
### 3. d/s 로 Flat Shade , Smooth Shade 적용이 가능합니다.
![image](https://github.com/Wisdom-Kim/WebGL-CreateAndRendering3DModel/assets/52325199/66942881-efdd-49b6-b224-fbd2db4d9d61)
![image](https://github.com/Wisdom-Kim/WebGL-CreateAndRendering3DModel/assets/52325199/fdb485be-4c1a-4279-a613-bed04a10387e)
