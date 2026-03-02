// clang-format off
//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>


rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}


static bool insideTriangle(int x, int y, const Vector3f* _v)
{   
    float px = x + 0.5;
    float py = y + 0.5;
    Eigen::Vector2f va(_v[0].x(),_v[0].y());
    Eigen::Vector2f vb(_v[1].x(),_v[1].y());
    Eigen::Vector2f vc(_v[2].x(),_v[2].y());
    Eigen::Vector2f p(px, py);

    auto EgdeCross = [](const Eigen::Vector2f& v1, const Eigen::Vector2f& v2, const Eigen::Vector2f& p) {
        Eigen::Vector2f ab = v2 - v1;
        Eigen::Vector2f ap = p - v1;
        return ab.x() * ap.y() - ab.y() * ap.x();
    };

    float cross1 = EgdeCross(va, vb, p);
    float cross2 = EgdeCross(vb, vc, p);
    float cross3 = EgdeCross(vc, va, p);

    bool b1 = cross1 >= 0 && cross2 >= 0 && cross3 >= 0;
    bool b2 = cross1 <= 0 && cross2 <= 0 && cross3 <= 0;

    return b1 || b2;
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f* v)
{
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto& i : ind)
    {
        Triangle t;
        Eigen::Vector4f v[] = {
                mvp * to_vec4(buf[i[0]], 1.0f),
                mvp * to_vec4(buf[i[1]], 1.0f),
                mvp * to_vec4(buf[i[2]], 1.0f)
        };
        //Homogeneous division
        for (auto& vec : v) {
            vec /= vec.w();
        }
        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);

        rasterize_triangle(t);
    }
}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t) {
    auto v = t.toVector4();
    // Find bounding box in screen space
    float min_x = std::min(std::min(t.v[0].x(), t.v[1].x()), t.v[2].x());
    float max_x = std::max(std::max(t.v[0].x(), t.v[1].x()), t.v[2].x());
    float min_y = std::min(std::min(t.v[0].y(), t.v[1].y()), t.v[2].y());
    float max_y = std::max(std::max(t.v[0].y(), t.v[1].y()), t.v[2].y());

    // Clamp the bounding box to the screen dimensions
    int x0 = std::max(0, (int)std::floor(min_x));
    int x1 = std::min(width - 1, (int)std::ceil(max_x));
    int y0 = std::max(0, (int)std::floor(min_y));
    int y1 = std::min(height - 1, (int)std::ceil(max_y));

    // iterate through the bounding box
    for (int x = x0; x <= x1; ++x) {
        for (int y = y0; y <= y1; ++y) {
            if (!insideTriangle(x, y, t.v)) continue;

            // sample at pixel center
            float px = x + 0.5f;
            float py = y + 0.5f;

            auto bary = computeBarycentric2D(px, py, t.v);
            float alpha = std::get<0>(bary);
            float beta  = std::get<1>(bary);
            float gamma = std::get<2>(bary);

            float w_reciprocal = 1.0f / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
            float z_interpolated = (alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w());
            z_interpolated *= w_reciprocal;

            int idx = get_index(x, y);
            if (z_interpolated < depth_buf[idx]) {
                depth_buf[idx] = z_interpolated;
                Eigen::Vector3f color = t.getColor();
                set_pixel(Eigen::Vector3f((float)x, (float)y, z_interpolated), color);
            }
        }
    }
}

void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height-1-y)*width + x;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    //old index: auto ind = point.y() + point.x() * width;
    auto ind = (height-1-point.y())*width + point.x();
    frame_buf[ind] = color;

}

// clang-format on