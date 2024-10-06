#include <game/world.hpp>

void orbital_camera(Camera& camera, const float camera_orbit_speed);
void pause_display(bool sim_paused, size_t screen_height);
int main(int argc, char** args)
{
    const int screen_width = 1280;
    const int screen_height = 768;

    InitWindow(screen_width, screen_height, "Game");
    SetTargetFPS(60);
//
    Camera camera = { 0 };
    camera.position = Vector3{ 10.1f, 20.0f, 0.f }; // Camera position
    camera.target = Vector3{ 0.f, 0.f, 0.f }; // Camera looking at point
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f }; // Camera up vector (rotation towards target)
    camera.fovy = 90.0f; // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE; // Camera projection type
    Game::GameWorld world(Game::default_cell_colors);
    //for (size_t ii = 0; ii < 20; ++ii)
    //    world.mutable_at(world.dimensions().x / 2 + ii, world.dimensions().y / 2, 0) = 1;
    //for (size_t ii = 0; ii < 2000; ++ii)
        //world.mutable_at(GetRandomValue(0, world.dimensions().x - 1), GetRandomValue(0, world.dimensions().y - 1), 0) = 1;
    for (size_t ii = 0; ii < 1000; ++ii)
    {
        size_t x = GetRandomValue(0, world.dimensions().x - 1);
        size_t y = GetRandomValue(0, world.dimensions().y - 1);
        world.mutable_at(x, y, 0) = 3;
        //world.commit();
        //world.mutable_at(x, y, 0) = 3;
        //world.commit();
        world.langton_position = Game::Index3{ x, y, 0};
    }
    world.langton_direction = Game::Direction::Forward;
    size_t grid_update_period = 6;
    size_t frame = 0;
    const float camera_orbit_speed = .1f;
    DisableCursor();
    bool pause_sim = true;
    while (!WindowShouldClose())
    {
        orbital_camera(camera, camera_orbit_speed);
        Vector2 mouse_position = GetMousePosition();
//        std::cout << camera.up.x << ", "
//            << camera.up.y << ", "
//            << camera.up.z << "\n";
//            //// Check if the left mouse button is clicked
//        //if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//        //    // Calculate the column and row of the rectangle clicked
//        //    int clickedColumn = mousePosition.x / rectSize;
//        //    int clickedRow = mousePosition.y / rectSize;
//        //    grid[clickedRow][clickedColumn] = 255;
//        //}

        ++frame;
        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode3D(camera);
                    world.draw_3d(::Vector3{0.f, 0.f, 0.f});
                    DrawGrid(100, 1.0f);
            EndMode3D();
            DrawFPS(10, 10);
            pause_display(pause_sim, screen_height);
        EndDrawing();
        if (GetKeyPressed() == KEY_P)
            pause_sim = !pause_sim;
        if (pause_sim == false)
        {
            if (frame % grid_update_period == 0)
            {
                frame = 0;
                //world.conway();
                world.langton(1);
                world.copy_mutable_buffer(std::array<uint8_t, 2>{0, 3});

                //std::cout << world.langton_position << "\n";
                //world.commit();
            }
        }
    }
    // Cleanup //
    CloseWindow();
    return 0;
}
void orbital_camera(Camera& camera, const float camera_orbit_speed)
{
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) == true)
    {
        ::Vector2 orbit_vector = Vector2Normalize(GetMouseDelta());
        orbit_vector.x *= camera_orbit_speed;
        orbit_vector.y *= camera_orbit_speed;
        Matrix yaw = MatrixRotate(GetCameraUp(&camera), orbit_vector.x);
        Matrix pitch = MatrixRotate(GetCameraRight(&camera), orbit_vector.y);
        Vector3 view = Vector3Subtract(camera.position, camera.target);
        view = Vector3Transform(view, yaw);
        camera.position = Vector3Add(camera.target, view);
        view = Vector3Transform(view, pitch);
        camera.position = Vector3Add(camera.target, view);
    }
    float zoom = GetMouseWheelMove();
    ::Vector3 camera_forward = GetCameraForward(&camera);
    camera_forward.x *= zoom;
    camera_forward.y *= zoom;
    camera_forward.z *= zoom;
    camera.position = Vector3Add(camera.position, camera_forward);
}
void pause_display(bool pause_sim, size_t screen_height)
{
    DrawText("Simulation: ", 10, screen_height - 20, 10, BLACK);
    if (pause_sim == false)
        DrawText("[RUNNING]", 70, screen_height - 20, 10, GREEN);
    else
        DrawText("[PAUSED]", 70, screen_height - 20, 10, RED);
}
