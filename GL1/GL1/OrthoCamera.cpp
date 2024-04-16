#include "OrthoCamera.h"

glm::mat4 OrthoCamera::GetProjection()
{
    return glm::ortho(ortho_left, ortho_right, ortho_bottom, ortho_top, Z_Near, Z_Far);
}
