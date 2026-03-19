//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    Vector3f L_dir{0.f}, L_indir{0.f};
    Intersection isect = intersect(ray);
    if (!isect.happened) {
        return backgroundColor;
    }

    if (isect.m->hasEmission()) {
        // 只有第一级射线看到光源才返回亮度
        // 之后的递归（间接光）如果撞到光源，返回0，避免与直接光采样重复
        return (depth == 0) ? isect.m->getEmission() : Vector3f(0.f);
    }

    float pdf_light;
    Intersection light_isect;
    sampleLight(light_isect, pdf_light);
    Vector3f x = light_isect.coords; //光源采样点
    Vector3f p = isect.coords; //当前着色点
    Vector3f N = isect.normal; //当前着色点法线
    Vector3f NN = light_isect.normal; //光源采样点的法线
    Vector3f ws = (x - p).normalized(); //从当前着色点指向光源采样点的方向向量
    Vector3f wo = -ray.direction; //从当前着色点指向相机的方向向量 注意负号
    Vector3f emit = light_isect.emit; //光源采样点的发光强度
    Material* m = isect.m; //当前着色点的材质信息

    //判断当前着色点与光源采样点之间是否有物体遮挡
    Ray ray_p_to_light(p + N * EPSILON, ws); //处理自遮挡问题，偏移一个很小的距离EPSILON
    Intersection isect_p_to_light = intersect(ray_p_to_light);
    if (!isect_p_to_light.happened || (isect_p_to_light.coords - p).norm() - (x - p).norm() > -EPSILON) {
        float cos_theta = std::max(0.f, dotProduct(N, ws));
        float cos_theta_light = std::max(0.f, dotProduct(NN, -ws)); //注意负号
        L_dir = emit * m->eval(ws,wo,N) * cos_theta * cos_theta_light / (pow((x - p).norm(), 2)) / pdf_light;
    }

    
}
