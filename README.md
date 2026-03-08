# GAMES101-HOMEWORK

## Assignment 1
![image](https://github.com/nintreesssss/GAMES101-Homework/blob/main/Assignment1/out.png)


需要注意的是，代码框架中给出的变量**zNear zFar**为正值，而摄像机面对-z轴，若将这两个变量直接作为坐标传入函数会导致渲染三角形倒置

```cpp
    //......
    float rad = eye_fov * MY_PI / 180.0f;
    float t = zNear * std::tan(rad / 2.0f);
    float r = aspect_ratio * t;
    float n = -zNear;
    float f = -zFar;

    Eigen::Matrix4f persp2ortho = Eigen::Matrix4f::Zero();
    persp2ortho(0,0) = n;
    persp2ortho(1,1) = n;
    persp2ortho(2,2) = n + f;
    persp2ortho(2,3) = -n * f;
    persp2ortho(3,2) = 1.0f;
    
    Eigen::Matrix4f ortho_scale = Eigen::Matrix4f::Identity();
    ortho_scale << 1.0f / r, 0, 0, 0,
                   0, 1.0f / t, 0, 0,
                   0, 0, 2.0f / (n - f), 0,
                   0, 0, 0, 1;

    Eigen::Matrix4f ortho_trans = Eigen::Matrix4f::Identity();
    ortho_trans(2,3) = -(n + f) / 2;

    projection = ortho_scale * ortho_trans * persp2ortho;

    return projection;
```
---
## Assignment 2
*未使用SSAA:*  
![image](https://github.com/nintreesssss/GAMES101-Homework/blob/main/Assignment2/output.png)  

  
*使用SSAA:*  
![image](https://github.com/nintreesssss/GAMES101-Homework/blob/main/Assignment2/output_ssaa.png)  

*对比图:*
![image](https://github.com/nintreesssss/GAMES101-Homework/blob/main/Assignment2/compare.png)  

右边使用了SSAA，左边未使用，可以明显看出SSAA模糊了三角形的锯齿边缘  

## Assignment 3
需要注意的是：  
1. 计算重心坐标时注意透视矫正  
2. 半程向量，法线等向量需要进行归一化  
3. (u,v) 坐标在实际应用中可能存在负数或者 > 1（正常取值范围在 0 ~ 1），所以还需要做有效性检测  
  
![image](https://github.com/nintreesssss/GAMES101-Homework/blob/main/Assignment3/images/output_normal.png)  
![image](https://github.com/nintreesssss/GAMES101-Homework/blob/main/Assignment3/images/output_phong.png)  
![image](https://github.com/nintreesssss/GAMES101-Homework/blob/main/Assignment3/images/output_bump.png)  
![image](https://github.com/nintreesssss/GAMES101-Homework/blob/main/Assignment3/images/output_displacement.png)  
*使用了双线性纹理采样的方法，效果区别不大*
![image](https://github.com/nintreesssss/GAMES101-Homework/blob/main/Assignment3/images/output_displacement_2.png)  


