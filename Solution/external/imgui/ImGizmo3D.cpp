#include "ImGizmo3D.h"
#include <cmath>
#include <algorithm>
#include <cfloat>
#include <cstring>

namespace ImGizmo3D
{
    static constexpr float DEG2RAD = 3.14159265358979323846f / 180.0f;
    static constexpr float RAD2DEG = 180.0f / 3.14159265358979323846f;
    static constexpr float PI = 3.14159265358979323846f;
    static constexpr float AXIS_HIT_THRESHOLD = 12.0f;
    static constexpr float CIRCLE_HIT_THRESHOLD = 14.0f;
    static constexpr float GIZMO_SCREEN_FACTOR = 0.15f;
    static constexpr int   CIRCLE_SEGMENTS = 64;

    static const ImU32 AXIS_COLORS[3] = {
        IM_COL32(220, 50, 50, 255),   // X = Red
        IM_COL32(50, 180, 50, 255),   // Y = Green
        IM_COL32(50, 100, 220, 255),  // Z = Blue
    };
    static const ImU32 AXIS_COLORS_HIGHLIGHT[3] = {
        IM_COL32(255, 120, 120, 255),
        IM_COL32(120, 255, 120, 255),
        IM_COL32(120, 180, 255, 255),
    };
    static const ImU32 AXIS_COLORS_DIM[3] = {
        IM_COL32(150, 40, 40, 180),
        IM_COL32(40, 120, 40, 180),
        IM_COL32(40, 70, 150, 180),
    };

    struct GizmoState
    {
        float rectX, rectY, rectW, rectH;
        bool using_;
        bool over;
        bool mouseOverGui;
        int hoveredAxis;  // -1 = none, 0/1/2 = X/Y/Z
        int activeAxis;
        float dragStartMouse[2];
        float dragStartValue[3];
        float dragScreenAxis[2];   // normalized projected axis direction
        float dragScaleFactor;     // world units per screen pixel along the axis
        Mode  dragMode;            // Mode captured at click time (keeps drag math consistent)
        float dragAxisWorld[3];    // active axis direction in world space at click time
    };

    static GizmoState g_State = {};

    static float Vec3Dot(const float* a, const float* b)
    {
        return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
    }

    static float Vec3Length(const float* v)
    {
        return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    }

    static void Vec3Normalize(float* v)
    {
        float len = Vec3Length(v);
        if (len > 1e-6f)
        {
            v[0] /= len;
            v[1] /= len;
            v[2] /= len;
        }
    }

    static void EulerToBasis(const float* eulerDeg, float xCol[3], float yCol[3], float zCol[3])
    {
        float pitch = eulerDeg[0] * DEG2RAD;
        float roll  = eulerDeg[1] * DEG2RAD;
        float yaw   = eulerDeg[2] * DEG2RAD;
        float cp = cosf(pitch), sp = sinf(pitch);
        float cr = cosf(roll),  sr = sinf(roll);
        float cy = cosf(yaw),   sy = sinf(yaw);

        // Columns derived from R = Rz(yaw) * Rx(pitch) * Ry(roll)
        xCol[0] = cy*cr - sy*sp*sr;
        xCol[1] = sy*cr + cy*sp*sr;
        xCol[2] = -cp*sr;

        yCol[0] = -sy*cp;
        yCol[1] =  cy*cp;
        yCol[2] =  sp;

        zCol[0] = cy*sr + sy*sp*cr;
        zCol[1] = sy*sr - cy*sp*cr;
        zCol[2] = cp*cr;
    }

    static void BasisToEuler(const float xCol[3], const float yCol[3], const float zCol[3], float* outEulerDeg)
    {
        // Indices into the conceptual R matrix where R[i][j] = column j, component i.
        float r21 = yCol[2]; // sp
        float r01 = yCol[0]; // -sy*cp
        float r11 = yCol[1]; //  cy*cp
        float r20 = xCol[2]; // -cp*sr
        float r22 = zCol[2]; //  cp*cr

        float sp = r21;
        if (sp >  1.0f) sp =  1.0f;
        if (sp < -1.0f) sp = -1.0f;
        float pitch = asinf(sp);
        float cp = cosf(pitch);

        float yaw, roll;
        if (fabsf(cp) > 1e-5f)
        {
            yaw  = atan2f(-r01, r11);
            roll = atan2f(-r20, r22);
        }
        else
        {
            // Gimbal lock at pitch = +/- 90: keep roll, fold into yaw.
            yaw  = atan2f(xCol[1], xCol[0]);
            roll = 0.0f;
        }

        outEulerDeg[0] = pitch * RAD2DEG;
        outEulerDeg[1] = roll  * RAD2DEG;
        outEulerDeg[2] = yaw   * RAD2DEG;
    }

    // Build a rotation basis from an axis-angle (Rodrigues).
    static void AxisAngleToBasis(const float axis[3], float angleRad,
                                 float xCol[3], float yCol[3], float zCol[3])
    {
        float kx = axis[0], ky = axis[1], kz = axis[2];
        float c = cosf(angleRad), s = sinf(angleRad), v = 1.0f - c;

        xCol[0] = kx*kx*v + c;
        xCol[1] = ky*kx*v + kz*s;
        xCol[2] = kz*kx*v - ky*s;

        yCol[0] = kx*ky*v - kz*s;
        yCol[1] = ky*ky*v + c;
        yCol[2] = kz*ky*v + kx*s;

        zCol[0] = kx*kz*v + ky*s;
        zCol[1] = ky*kz*v - kx*s;
        zCol[2] = kz*kz*v + c;
    }

    // 3x3 multiply on column-vec layout: out = a * b
    static void Basis3Mul(const float aX[3], const float aY[3], const float aZ[3],
                          const float bX[3], const float bY[3], const float bZ[3],
                          float outX[3], float outY[3], float outZ[3])
    {
        float tmpX[3], tmpY[3], tmpZ[3];
        for (int i = 0; i < 3; i++)
        {
            tmpX[i] = aX[i]*bX[0] + aY[i]*bX[1] + aZ[i]*bX[2];
            tmpY[i] = aX[i]*bY[0] + aY[i]*bY[1] + aZ[i]*bY[2];
            tmpZ[i] = aX[i]*bZ[0] + aY[i]*bZ[1] + aZ[i]*bZ[2];
        }
        for (int i = 0; i < 3; i++) { outX[i] = tmpX[i]; outY[i] = tmpY[i]; outZ[i] = tmpZ[i]; }
    }

    // Transform a 3D point by a 4x4 column-major matrix, returns (x, y, z, w)
    static void TransformPoint(const float* m, const float* p, float* out4)
    {
        out4[0] = m[0] * p[0] + m[4] * p[1] + m[8]  * p[2] + m[12];
        out4[1] = m[1] * p[0] + m[5] * p[1] + m[9]  * p[2] + m[13];
        out4[2] = m[2] * p[0] + m[6] * p[1] + m[10] * p[2] + m[14];
        out4[3] = m[3] * p[0] + m[7] * p[1] + m[11] * p[2] + m[15];
    }

    // Multiply two 4x4 column-major matrices: result = a * b
    static void MatMul(const float* a, const float* b, float* result)
    {
        float tmp[16];
        for (int col = 0; col < 4; col++)
        {
            for (int row = 0; row < 4; row++)
            {
                float sum = 0;
                for (int k = 0; k < 4; k++)
                {
                    sum += a[k * 4 + row] * b[col * 4 + k];
                }
                tmp[col * 4 + row] = sum;
            }
        }
        memcpy(result, tmp, sizeof(float) * 16);
    }

    // Project a world-space point to screen coordinates. Returns false if behind camera.
    static bool WorldToScreen(const float* worldPos, const float* viewProj, float screenW, float screenH, float* sx, float* sy)
    {
        float clip[4];
        TransformPoint(viewProj, worldPos, clip);
        if (clip[3] <= 0.001f) return false;
        float ndcX = clip[0] / clip[3];
        float ndcY = clip[1] / clip[3];
        *sx = (ndcX * 0.5f + 0.5f) * screenW;
        *sy = (1.0f - (ndcY * 0.5f + 0.5f)) * screenH;
        return true;
    }

    // Distance from point P to line segment AB in 2D, with parameter t in [0,1]
    static float PointSegmentDist2D(float px, float py,
                                    float ax, float ay, float bx, float by,
                                    float* t)
    {
        float dx = bx - ax, dy = by - ay;
        float len2 = dx * dx + dy * dy;
        if (len2 < 1e-6f)
        {
            *t = 0;
            return sqrtf((px - ax) * (px - ax) + (py - ay) * (py - ay));
        }
        *t = ((px - ax) * dx + (py - ay) * dy) / len2;
        *t = std::max(0.0f, std::min(1.0f, *t));
        float cx = ax + (*t) * dx;
        float cy = ay + (*t) * dy;
        return sqrtf((px - cx) * (px - cx) + (py - cy) * (py - cy));
    }

    static void DrawArrowHead(ImDrawList* dl, float tipX, float tipY,
                              float originX, float originY, ImU32 color)
    {
        float dx = tipX - originX;
        float dy = tipY - originY;
        float len = sqrtf(dx * dx + dy * dy);
        if (len < 5.0f) return;

        float ndx = dx / len, ndy = dy / len;
        float perpX = -ndy, perpY = ndx;
        float arrowSize = 10.0f;

        ImVec2 tip(tipX, tipY);
        ImVec2 a1(tip.x - ndx * arrowSize + perpX * arrowSize * 0.4f,
                  tip.y - ndy * arrowSize + perpY * arrowSize * 0.4f);
        ImVec2 a2(tip.x - ndx * arrowSize - perpX * arrowSize * 0.4f,
                  tip.y - ndy * arrowSize - perpY * arrowSize * 0.4f);
        dl->AddTriangleFilled(tip, a1, a2, color);
    }

    void BeginFrame()
    {
        if (!g_State.using_)
        {
            g_State.hoveredAxis = -1;
            g_State.over = false;
        }
    }

    void SetRect(float x, float y, float w, float h)
    {
        g_State.rectX = x;
        g_State.rectY = y;
        g_State.rectW = w;
        g_State.rectH = h;
    }

    bool IsUsing() { return g_State.using_; }
    bool IsOver()  { return g_State.over || g_State.using_; }

    void SetMouseOverGui(bool over)
    {
        g_State.mouseOverGui = over;
    }

    bool Manipulate(const float* view, const float* projection, Operation operation,
                    float* position, float* rotation, float* scale,
                    const float* snap, Mode mode)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImDrawList* drawList = ImGui::GetForegroundDrawList();

        float screenW = g_State.rectW;
        float screenH = g_State.rectH;

        // Combined view-projection
        float viewProj[16];
        MatMul(projection, view, viewProj);

        // Extract camera position from view matrix (column-major)
        float camPos[3];
        camPos[0] = -(view[0] * view[12] + view[1] * view[13] + view[2]  * view[14]);
        camPos[1] = -(view[4] * view[12] + view[5] * view[13] + view[6]  * view[14]);
        camPos[2] = -(view[8] * view[12] + view[9] * view[13] + view[10] * view[14]);

        // Gizmo size scales with distance so it's consistent on screen
        float diff[3] = { position[0] - camPos[0], position[1] - camPos[1], position[2] - camPos[2] };
        float dist = Vec3Length(diff);
        float gizmoScale = dist * GIZMO_SCREEN_FACTOR;
        if (gizmoScale < 0.1f) gizmoScale = 0.1f;

        // Project gizmo origin
        float screenOrigin[2];
        if (!WorldToScreen(position, viewProj, screenW, screenH, &screenOrigin[0], &screenOrigin[1]))
        {
            g_State.using_ = false;
            return false;
        }

        float mx = io.MousePos.x;
        float my = io.MousePos.y;
        bool changed = false;

        // Can we start new interactions? (don't steal clicks from ImGui windows)
        bool canStartInteraction = !g_State.mouseOverGui;

        // Axis basis for this frame. World: identity. Local: columns of the
        // entity's rotation matrix, so the gizmo aligns to the object.
        float axes[3][3];
        if (mode == Local)
        {
            EulerToBasis(rotation, axes[0], axes[1], axes[2]);
        }
        else
        {
            axes[0][0] = 1; axes[0][1] = 0; axes[0][2] = 0;
            axes[1][0] = 0; axes[1][1] = 1; axes[1][2] = 0;
            axes[2][0] = 0; axes[2][1] = 0; axes[2][2] = 1;
        }

        if (operation == TRANSLATE || operation == SCALE)
        {
            // Project axis tips
            float screenTips[3][2];
            bool axisBehind[3] = {};

            for (int i = 0; i < 3; i++)
            {
                float tip[3] = {
                    position[0] + axes[i][0] * gizmoScale,
                    position[1] + axes[i][1] * gizmoScale,
                    position[2] + axes[i][2] * gizmoScale,
                };
                axisBehind[i] = !WorldToScreen(tip, viewProj, screenW, screenH,
                                               &screenTips[i][0], &screenTips[i][1]);
            }

            // Hit test
            if (!g_State.using_)
            {
                g_State.hoveredAxis = -1;

                if (canStartInteraction)
                {
                    float minDist = AXIS_HIT_THRESHOLD;
                    for (int i = 0; i < 3; i++)
                    {
                        if (axisBehind[i]) continue;
                        float t;
                        float d = PointSegmentDist2D(mx, my,
                                                     screenOrigin[0], screenOrigin[1],
                                                     screenTips[i][0], screenTips[i][1], &t);
                        if (d < minDist && t > 0.05f)
                        {
                            minDist = d;
                            g_State.hoveredAxis = i;
                        }
                    }
                }

                g_State.over = (g_State.hoveredAxis >= 0);

                // Start drag
                if (g_State.hoveredAxis >= 0 && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    g_State.using_ = true;
                    g_State.activeAxis = g_State.hoveredAxis;
                    g_State.dragStartMouse[0] = mx;
                    g_State.dragStartMouse[1] = my;
                    g_State.dragMode = mode;

                    float* src = (operation == TRANSLATE) ? position : scale;
                    g_State.dragStartValue[0] = src[0];
                    g_State.dragStartValue[1] = src[1];
                    g_State.dragStartValue[2] = src[2];

                    // Capture the active axis in world space so the drag stays
                    // anchored even if rotation changes mid-drag.
                    int a = g_State.activeAxis;
                    g_State.dragAxisWorld[0] = axes[a][0];
                    g_State.dragAxisWorld[1] = axes[a][1];
                    g_State.dragAxisWorld[2] = axes[a][2];

                    // Projected axis direction & scale factor
                    float sdx = screenTips[a][0] - screenOrigin[0];
                    float sdy = screenTips[a][1] - screenOrigin[1];
                    float slen = sqrtf(sdx * sdx + sdy * sdy);
                    if (slen > 1e-3f)
                    {
                        g_State.dragScreenAxis[0] = sdx / slen;
                        g_State.dragScreenAxis[1] = sdy / slen;
                        g_State.dragScaleFactor = gizmoScale / slen;
                    }
                }
            }

            // Dragging
            if (g_State.using_ && (operation == TRANSLATE || operation == SCALE))
            {
                if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
                {
                    float dmx = mx - g_State.dragStartMouse[0];
                    float dmy = my - g_State.dragStartMouse[1];
                    float projected = dmx * g_State.dragScreenAxis[0] + dmy * g_State.dragScreenAxis[1];

                    int a = g_State.activeAxis;

                    if (operation == TRANSLATE)
                    {
                        float worldDelta = projected * g_State.dragScaleFactor;
                        if (snap)
                        {
                            worldDelta = roundf(worldDelta / snap[0]) * snap[0];
                        }

                        if (g_State.dragMode == Local)
                        {
                            // Move along the captured world-space local axis,
                            // spreading the delta across all three components.
                            for (int k = 0; k < 3; k++)
                            {
                                float newK = g_State.dragStartValue[k]
                                           + worldDelta * g_State.dragAxisWorld[k];
                                if (position[k] != newK)
                                {
                                    position[k] = newK;
                                    changed = true;
                                }
                            }
                        }
                        else
                        {
                            float newVal = g_State.dragStartValue[a] + worldDelta;
                            if (position[a] != newVal)
                            {
                                position[a] = newVal;
                                changed = true;
                            }
                        }
                    }
                    else // SCALE — kept as world axis component only (GTA has no runtime scale anyway).
                    {
                        float scaleDelta = projected * 0.01f;
                        float newVal = g_State.dragStartValue[a] + scaleDelta;
                        if (newVal < 0.01f) newVal = 0.01f;
                        if (snap)
                        {
                            newVal = roundf(newVal / snap[0]) * snap[0];
                        }
                        if (scale[a] != newVal)
                        {
                            scale[a] = newVal;
                            changed = true;
                        }
                    }
                }
                else
                {
                    g_State.using_ = false;
                    g_State.activeAxis = -1;
                }
            }

            // Draw axes
            for (int i = 0; i < 3; i++)
            {
                if (axisBehind[i]) continue;

                bool isActive  = (g_State.using_ && g_State.activeAxis == i);
                bool isHovered = (!g_State.using_ && g_State.hoveredAxis == i);
                ImU32 color    = (isActive || isHovered) ? AXIS_COLORS_HIGHLIGHT[i] : AXIS_COLORS[i];
                float thickness = (isActive || isHovered) ? 4.0f : 3.0f;

                if (g_State.using_ && g_State.activeAxis != i)
                {
                    color = AXIS_COLORS_DIM[i];
                    thickness = 2.0f;
                }

                drawList->AddLine(ImVec2(screenOrigin[0], screenOrigin[1]), ImVec2(screenTips[i][0], screenTips[i][1]), color, thickness);

                if (operation == TRANSLATE)
                {
                    DrawArrowHead(drawList, screenTips[i][0], screenTips[i][1], screenOrigin[0], screenOrigin[1], color);
                }
                else // SCALE - box at tip
                {
                    float bs = (isActive || isHovered) ? 6.0f : 5.0f;
                    drawList->AddRectFilled(
                        ImVec2(screenTips[i][0] - bs, screenTips[i][1] - bs),
                        ImVec2(screenTips[i][0] + bs, screenTips[i][1] + bs),
                        color);
                }

                // Axis label
                const char* labels[] = {"X", "Y", "Z"};
                drawList->AddText(ImVec2(screenTips[i][0] + 6, screenTips[i][1] - 8), color, labels[i]);
            }

            // Center dot
            drawList->AddCircleFilled(ImVec2(screenOrigin[0], screenOrigin[1]), 4.0f, IM_COL32(255, 255, 255, 200));
        }

        else if (operation == ROTATE)
        {
            float circleRadius = gizmoScale * 0.8f;

            const int tIdx1[3] = { 1, 0, 0 };
            const int tIdx2[3] = { 2, 2, 1 };
            float tangents1[3][3];
            float tangents2[3][3];
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    tangents1[i][j] = axes[tIdx1[i]][j];
                    tangents2[i][j] = axes[tIdx2[i]][j];
                }
            }

            // Hit test
            if (!g_State.using_)
            {
                g_State.hoveredAxis = -1;

                if (canStartInteraction)
                {
                    float minDist = CIRCLE_HIT_THRESHOLD;

                    for (int axisIdx = 0; axisIdx < 3; axisIdx++)
                    {
                        float closestDist = FLT_MAX;
                        for (int s = 0; s < CIRCLE_SEGMENTS; s++)
                        {
                            float angle = (float)s / CIRCLE_SEGMENTS * 2.0f * PI;
                            float c = cosf(angle), sn = sinf(angle);
                            float wp[3] = {
                                position[0] + (tangents1[axisIdx][0] * c + tangents2[axisIdx][0] * sn) * circleRadius,
                                position[1] + (tangents1[axisIdx][1] * c + tangents2[axisIdx][1] * sn) * circleRadius,
                                position[2] + (tangents1[axisIdx][2] * c + tangents2[axisIdx][2] * sn) * circleRadius,
                            };
                            float sp[2];
                            if (WorldToScreen(wp, viewProj, screenW, screenH, &sp[0], &sp[1]))
                            {
                                float d = sqrtf((mx - sp[0]) * (mx - sp[0]) + (my - sp[1]) * (my - sp[1]));
                                if (d < closestDist) closestDist = d;
                            }
                        }

                        if (closestDist < minDist)
                        {
                            minDist = closestDist;
                            g_State.hoveredAxis = axisIdx;
                        }
                    }
                }

                g_State.over = (g_State.hoveredAxis >= 0);

                // Start drag
                if (g_State.hoveredAxis >= 0 && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    g_State.using_ = true;
                    g_State.activeAxis = g_State.hoveredAxis;
                    g_State.dragStartMouse[0] = mx;
                    g_State.dragStartMouse[1] = my;
                    g_State.dragStartValue[0] = rotation[0];
                    g_State.dragStartValue[1] = rotation[1];
                    g_State.dragStartValue[2] = rotation[2];
                    g_State.dragMode = mode;

                    int a = g_State.activeAxis;
                    g_State.dragAxisWorld[0] = axes[a][0];
                    g_State.dragAxisWorld[1] = axes[a][1];
                    g_State.dragAxisWorld[2] = axes[a][2];
                }
            }

            // Rotation dragging
            if (g_State.using_ && operation == ROTATE)
            {
                if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
                {
                    float startDx = g_State.dragStartMouse[0] - screenOrigin[0];
                    float startDy = g_State.dragStartMouse[1] - screenOrigin[1];
                    float curDx = mx - screenOrigin[0];
                    float curDy = my - screenOrigin[1];

                    float startAngle = atan2f(startDy, startDx);
                    float curAngle   = atan2f(curDy, curDx);
                    float deltaAngleRad = (curAngle - startAngle);

                    // Flip sign based on whether the rotation axis points toward camera.
                    float camDir[3] = {
                        position[0] - camPos[0],
                        position[1] - camPos[1],
                        position[2] - camPos[2],
                    };
                    Vec3Normalize(camDir);
                    float dotAxis = Vec3Dot(camDir, g_State.dragAxisWorld);
                    if (dotAxis < 0) deltaAngleRad = -deltaAngleRad;

                    if (g_State.dragMode == Local)
                    {
                        // Rotate around the captured local axis (world-space dir).
                        // newR = AxisAngle(axis, delta) * R_start, then re-extract Euler.
                        float Rx[3], Ry[3], Rz[3];
                        EulerToBasis(g_State.dragStartValue, Rx, Ry, Rz);

                        float Dx[3], Dy[3], Dz[3];
                        AxisAngleToBasis(g_State.dragAxisWorld, deltaAngleRad, Dx, Dy, Dz);

                        float Nx[3], Ny[3], Nz[3];
                        Basis3Mul(Dx, Dy, Dz, Rx, Ry, Rz, Nx, Ny, Nz);

                        float newEuler[3];
                        BasisToEuler(Nx, Ny, Nz, newEuler);

                        if (snap)
                        {
                            for (int i = 0; i < 3; i++)
                                newEuler[i] = roundf(newEuler[i] / snap[0]) * snap[0];
                        }

                        for (int i = 0; i < 3; i++)
                        {
                            if (rotation[i] != newEuler[i])
                            {
                                rotation[i] = newEuler[i];
                                changed = true;
                            }
                        }
                    }
                    else
                    {
                        // World mode: just nudge the matching Euler component.
                        int a = g_State.activeAxis;
                        float deltaAngle = deltaAngleRad * RAD2DEG;
                        float newVal = g_State.dragStartValue[a] + deltaAngle;

                        while (newVal > 360.0f)  newVal -= 360.0f;
                        while (newVal < -360.0f) newVal += 360.0f;

                        if (snap)
                        {
                            newVal = roundf(newVal / snap[0]) * snap[0];
                        }
                        if (rotation[a] != newVal)
                        {
                            rotation[a] = newVal;
                            changed = true;
                        }
                    }
                }
                else
                {
                    g_State.using_ = false;
                    g_State.activeAxis = -1;
                }
            }

            // Draw circles
            for (int axisIdx = 0; axisIdx < 3; axisIdx++)
            {
                bool isActive  = (g_State.using_ && g_State.activeAxis == axisIdx);
                bool isHovered = (!g_State.using_ && g_State.hoveredAxis == axisIdx);
                ImU32 color    = (isActive || isHovered) ? AXIS_COLORS_HIGHLIGHT[axisIdx] : AXIS_COLORS[axisIdx];
                float thickness = (isActive || isHovered) ? 3.5f : 2.5f;

                if (g_State.using_ && g_State.activeAxis != axisIdx)
                {
                    color = AXIS_COLORS_DIM[axisIdx];
                    thickness = 1.5f;
                }

                float prevSp[2] = {};
                bool prevValid = false;

                for (int s = 0; s <= CIRCLE_SEGMENTS; s++)
                {
                    float angle = (float)(s % CIRCLE_SEGMENTS) / CIRCLE_SEGMENTS * 2.0f * PI;
                    float c = cosf(angle), sn = sinf(angle);
                    float wp[3] = {
                        position[0] + (tangents1[axisIdx][0] * c + tangents2[axisIdx][0] * sn) * circleRadius,
                        position[1] + (tangents1[axisIdx][1] * c + tangents2[axisIdx][1] * sn) * circleRadius,
                        position[2] + (tangents1[axisIdx][2] * c + tangents2[axisIdx][2] * sn) * circleRadius,
                    };
                    float sp[2];
                    bool valid = WorldToScreen(wp, viewProj, screenW, screenH, &sp[0], &sp[1]);

                    if (valid && prevValid)
                    {
                        drawList->AddLine(ImVec2(prevSp[0], prevSp[1]), ImVec2(sp[0], sp[1]), color, thickness);
                    }
                    prevSp[0] = sp[0];
                    prevSp[1] = sp[1];
                    prevValid = valid;
                }

                // Axis label near the top of each circle
                float labelWorldPos[3] = {
                    position[0] + tangents2[axisIdx][0] * circleRadius * 1.1f,
                    position[1] + tangents2[axisIdx][1] * circleRadius * 1.1f,
                    position[2] + tangents2[axisIdx][2] * circleRadius * 1.1f,
                };
                float labelSp[2];
                if (WorldToScreen(labelWorldPos, viewProj, screenW, screenH, &labelSp[0], &labelSp[1]))
                {
                    const char* labels[] = {"X", "Y", "Z"};
                    drawList->AddText(ImVec2(labelSp[0] + 4, labelSp[1] - 8), color, labels[axisIdx]);
                }
            }

            // Center dot
            drawList->AddCircleFilled(ImVec2(screenOrigin[0], screenOrigin[1]), 4.0f, IM_COL32(255, 255, 255, 200));
        }

        return changed;
    }
}
