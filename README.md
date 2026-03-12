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

---
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

---
## Assignment 4
![image](https://github.com/nintreesssss/GAMES101-Homework/blob/main/Assignment4/image/my_bezier_curve.png)  

*加入了抗锯齿，此处使用的抗锯齿算法是根据采样点距离像素中心点的距离进行上色，同时加粗了曲线*
![image](https://github.com/nintreesssss/GAMES101-Homework/blob/main/Assignment4/image/antialiasing.png)  

---
## Assignment 5
计算主光线方向，因为相机已经位于原点，所以也就是要计算每一个像素在世界空间中的坐标  
此处给出线性映射公式：**输出 = 输入-输入下限 / 输入范围 * 输出范围 + 输出下限**  
需要注意的是，因为像素坐标是从左上方的像素为起点开始遍历的，所以计算y坐标时需要乘以负一来使原点回到左下角  
```cpp
//......
for (int j = 0; j < scene.height; ++j)
    {
        for (int i = 0; i < scene.width; ++i)
        {     
            auto x_ndc = (i + 0.5f) * 2 / (float)scene.width - 1;
            auto y_ndc = (j + 0.5f) * 2 / (float)scene.height - 1;
            float x = x_ndc * imageAspectRatio * scale;
            float y = y_ndc * scale * -1;
            Vector3f dir = Vector3f(x, y, -1);
            dir = normalize(dir); // 注意记得把方向向量归一化
            framebuffer[m++] = castRay(eye_pos, dir, scene, 0);
        }
        UpdateProgress(j / (float)scene.height);
    }
```
![image](https://github.com/nintreesssss/GAMES101-Homework/blob/main/Assignment5/image/test.png)  



