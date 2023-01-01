#include <SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "display.h"
#include "vector.h"

#define N_POINTS (9 * 9 * 9)
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];
vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };
vec3_t cube_rotation = { .x = 0, .y = 0, .z = 0 };

float fov_factor = 640;

Uint32 previous_frame_time = 0;

void setup(void) {
	color_buffer = (uint32_t*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
	color_buffer_texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH,
		SCREEN_HEIGHT);
	
	int point_count = 0;

	// start loading my array of vectors
	// from -1 to 1 (in this 9 x 9 x 9 cube)
	for (float x = -1; x <= 1; x += 0.25) {
		for (float y = -1; y <= 1; y += 0.25) {
			for (float z = -1; z <= 1; z += 0.25) {
				vec3_t new_point = { .x = x, .y = y, .z = z };
				cube_points[point_count++] = new_point;
			}
		}
	}
}

void process_input(void) {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT: {				// click "x" button of the window
			is_running = false;
			break;
		}
		case SDL_KEYDOWN: {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				is_running = false;
			}
			break;
		}
		default: {
			break;
		}
		}
	}
}

// 3d vector -> projected 2d point
vec2_t project(vec3_t point) {
	vec2_t projected_point = {
		.x = (fov_factor * point.x) / point.z,
		.y = (fov_factor * point.y) / point.z
	};
	return projected_point;
}

void update(void) {
	// wait some time until the reach the target frame time in millisecond
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

	// only delay execution if we are running too fast
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
		SDL_Delay(time_to_wait);
	}
	
	previous_frame_time = SDL_GetTicks();

	cube_rotation.x += 0.01;
	cube_rotation.y += 0.01;
	cube_rotation.z += 0.01;

	for (int i = 0; i < N_POINTS; i++) {
		vec3_t point = cube_points[i];
		
		vec3_t rotated_point = vec3_rotate_x(point, cube_rotation.x);
		rotated_point = vec3_rotate_y(rotated_point, cube_rotation.y);
		rotated_point = vec3_rotate_z(rotated_point, cube_rotation.z);

		// Move the points away from the camera
		rotated_point.z -= camera_position.z;

		// project the current point
		vec2_t projected_point = project(rotated_point);

		// save the projected 2D vector in the array of projected points
		projected_points[i] = projected_point;
	}
}

void render(void) {
	draw_grid();

	// Loop all projected points and render them
	for (int i = 0; i < N_POINTS; i++) {
		vec2_t projected_point = projected_points[i];
		draw_rect(
			projected_point.x + (SCREEN_WIDTH / 2), 
			projected_point.y + (SCREEN_HEIGHT / 2),
			4,
			4,
			0xFFFFFF00
		);
	}

	render_color_buffer();

	clear_color_buffer(0xFF000000);
	SDL_RenderPresent(renderer);
}

int main(void) {
	is_running = initialize_window();

	setup();

	while (is_running) {
		process_input();
		update();
		render();
	}

	destroy_window();
	return 0;
}
