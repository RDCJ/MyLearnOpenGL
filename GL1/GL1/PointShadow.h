#pragma once
#include "Shadow.h"
class PointShadow : public Shadow
{
    public:
        static const int FOV;
        static const std::vector<glm::vec3> six_dir;
        static const std::vector<glm::vec3> six_up;
        PointShadow(int width, int height, Light* light, TexParams& depth_tex_params, float z_near, float z_far);
        

    private:

        // ͨ�� Shadow �̳�
        void InitTexture(TexParams& depth_tex_params) override;
};

