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

---
## Assignment 6
根据课件算法公式实现即可，注意框架函数的输入和输出
![image](https://github.com/nintreesssss/GAMES101-Homework/blob/main/Assignment6/images/image.png)  

---
## Assignment 7
注意点：  
1. 根据课程内容所有方向向量需要从当前着色点指向外部，所以光线出射方向（也就是从着色点射向相机的光线）以及后面计算余弦时需要注意添加负号
2. 为了避免光源光线的自遮挡问题，需要将光源起点向法线方向偏移一个很小的距离
3. 计算L_dir时需要判断光源是否会被遮挡，采样时可能会撞击到光源本身，需要进行检测判断，否则可能会产生黑色横线
4. 计算L_indir时用到的pdr极端情况可能会很小趋近于0，如果直接做除法可能会溢出，需要做判断否则会产生噪点
   
*路径追踪实现细节较多，此处图像生成使用的SPP为512，使用了16线程渲染*  
![image](https://github.com/nintreesssss/GAMES101-Homework/blob/main/Assignment7/images/image.png)  

总结一下路径追踪算法的流程：  
1. 从像素点随机打出光线，判断是否命中物体，若未命中则结束（如果第一级光线直接命中光源返回亮度，否则返回0，避免和后序间接光重复计算
2. 直接对光源进行采样，得到采样点和着色点的法线，方向向量等信息
3. 判断当前着色点与光源之间是否有遮挡
4. 若无遮挡则根据渲染方程计算L_dir
5. 判断俄罗斯轮盘赌
6. 根据出射光线和法线采样一根漫反射的入射光线
7. 对新入射光线进行碰撞检测（向外）
8. 如果命中非光源，递归使用函数计算入射光线击中新的命中点为原着色点带来的间接光
9. 叠加间接光和直接光，得到原着色点的颜色  
