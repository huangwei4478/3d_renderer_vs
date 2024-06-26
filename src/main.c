#include <SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"

// Array of triangles that should be rendered frame by frame
triangle_t* triangles_to_render = NULL;
vec3_t camera_position = { .x = 0, .y = 0, .z = 0 };

float fov_factor = 640;

Uint32 previous_frame_time = 0;

void setup(void) {
	color_buffer = (uint32_t*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
	color_buffer_texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH,
		SCREEN_HEIGHT);

	//load_cube_mesh_data();
	load_obj_file_data("assets/cube.obj");
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

	// Initialize the array of triangles to render
	triangles_to_render = NULL;

	mesh.rotation.x += 0.01;
	mesh.rotation.y += 0.01;
	mesh.rotation.z += 0.01;
	
	// Loop all triangle faces of our mesh
	int faces_count = array_length(mesh.faces);
	for (int i = 0; i < faces_count; i++) {
		face_t mesh_face = mesh.faces[i];

		vec3_t face_vertices[3];
		face_vertices[0] = mesh.vertices[mesh_face.a - 1];
		face_vertices[1] = mesh.vertices[mesh_face.b - 1];
		face_vertices[2] = mesh.vertices[mesh_face.c - 1];

		triangle_t projected_triangle;

		// Loop all three vertices of this current face, 
		// and apply transformation

		vec3_t transformed_vertices[3];

		for (int j = 0; j < 3; j++) {
			vec3_t transformed_vertex = face_vertices[j];
			transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
			transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
			transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

			// Translate the vertex away from the camera
			transformed_vertex.z += 5.0;

			transformed_vertices[j] = transformed_vertex;
		}


		// Perform back-face culling
		vec3_t vector_a = transformed_vertices[0];
		vec3_t vector_b = transformed_vertices[1];
		vec3_t vector_c = transformed_vertices[2];

		vec3_t vector_ab = vec3_sub(vector_b, vector_a);
		vec3_t vector_ac = vec3_sub(vector_c, vector_a);

		vec3_t normal = vec3_cross(vector_ab, vector_ac);
		vec3_t camera_ray = vec3_sub(camera_position, vector_a);

		float dot_normal_camera = vec3_dot(normal, camera_ray);

		if (dot_normal_camera < 0) { continue; }


		for (int j = 0; j < 3; j++) {

			// Project the current vertex
			vec2_t projected_point = project(transformed_vertices[j]);
			
			// Scale and translate the projected points to the middle of the screen
			projected_point.x += (SCREEN_WIDTH / 2);
			projected_point.y += (SCREEN_HEIGHT / 2);

			projected_triangle.points[j] = projected_point;
		}

		// save the projected triangle in the array of triangles to render
		array_push(triangles_to_render, projected_triangle);
	}
}

void render(void) {
	draw_grid();
	
	// Loop all projected triangles and render them
	int num_triangles = array_length(triangles_to_render);
	for (int i = 0; i < num_triangles; i++) {
		triangle_t triangle = triangles_to_render[i];
		
		// Draw vertex points
		draw_rect(triangle.points[0].x, triangle.points[0].y, 4, 4, 0xFF00FFFF);
		draw_rect(triangle.points[1].x, triangle.points[1].y, 4, 4, 0xFF00FFFF);
		draw_rect(triangle.points[2].x, triangle.points[2].y, 4, 4, 0xFF00FFFF);
	
		// Draw unfilled triangle
		draw_triangle(
			triangle.points[0].x,
			triangle.points[0].y,
			triangle.points[1].x,
			triangle.points[1].y,
			triangle.points[2].x,
			triangle.points[2].y,
			0xFF00FF00
		);
	}

	// Clear the array of triangles to render every frame loop
	array_free(triangles_to_render);

	render_color_buffer();

	clear_color_buffer(0xFF000000);
	SDL_RenderPresent(renderer);
}

void free_resources(void) {
	free(color_buffer);
	array_free(mesh.faces);
	array_free(mesh.vertices);
}

int main(void) {
	is_running = initialize_window();

	setup();

	while (is_running) {
		process_input();
		update();
		render();
	}

	free_resources();
	destroy_window();
	return 0;
}
