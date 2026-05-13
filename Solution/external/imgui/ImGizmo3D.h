#pragma once
#include "imgui.h"

namespace ImGizmo3D
{
    enum Operation { TRANSLATE, ROTATE, SCALE };

    // Coordinate frame the gizmo axes use:
    //   World: gizmo axes aligned with the global X/Y/Z basis.
    //   Local: gizmo axes aligned with the entity's own basis, derived
    //          from the passed-in rotation Eulers (GTA rotation order 2:
    //          R = Rz(yaw) * Rx(pitch) * Ry(roll)).
    enum Mode { World, Local };

    // Call once per frame before any Manipulate calls
    void BeginFrame();

    // Set the screen-space rectangle for projection (typically full screen)
    void SetRect(float x, float y, float width, float height);

    // Manipulate a transform via 3D gizmo overlay.
    // view/projection: 4x4 column-major matrices
    // position: float[3], rotation: float[3] in degrees, scale: float[3]
    // snap: if non-null, snap increment value
    // mode: World (default) or Local axis orientation
    // Returns true if the transform was modified.
    bool Manipulate(const float* view, const float* projection, Operation operation,
                    float* position, float* rotation, float* scale,
                    const float* snap = nullptr, Mode mode = World);

    // Returns true if the gizmo is currently being dragged
    bool IsUsing();

    // Returns true if the gizmo is hovered or being dragged
    bool IsOver();

    // Call this to tell the gizmo that the mouse is over an ImGui window
    // so it should not start new interactions (continuing drags are still allowed)
    void SetMouseOverGui(bool over);
}
