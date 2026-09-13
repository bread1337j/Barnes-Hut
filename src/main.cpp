#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include "raylib.h"
#include "raymath.h"
#include "quadtree.hpp"
#include "object.hpp"
#include "multiprocessing.h"
#include <random>


void traverseQuadNode(quadTree* tree, int node, float zoom){
	Color c = GREEN;
	DrawRectangleLinesEx({ (float)tree->access(node).x, (float)tree->access(node).y, (float)tree->access(node).size_x, (float)tree->access(node).size_x }, 1.0f / zoom, c);
	/*if(tree->access(node).count > 0){
	  DrawLineV({(float)tree->access(node).sx/tree->access(node).count, (float)tree->access(node).sy/tree->access(node).count}, { (float)tree->access(node).x, (float)tree->access(node).y }, YELLOW);
	  DrawLineV({(float)tree->access(node).sx/tree->access(node).count, (float)tree->access(node).sy/tree->access(node).count}, { (float)(tree->access(node).x+tree->access(node).size_x), (float)(tree->access(node).y) }, YELLOW);
	  DrawLineV({(float)tree->access(node).sx/tree->access(node).count, (float)tree->access(node).sy/tree->access(node).count}, { (float)(tree->access(node).x+tree->access(node).size_x), (float)(tree->access(node).y+tree->access(node).size_y) }, YELLOW);
	  DrawLineV({(float)tree->access(node).sx/tree->access(node).count, (float)tree->access(node).sy/tree->access(node).count}, { (float)(tree->access(node).x), (float)(tree->access(node).y+tree->access(node).size_y) }, YELLOW);
	  DrawCircleV({ (float)tree->access(node).sx/tree->access(node).count, (float) tree->access(node).sy/tree->access(node).count }, (float) tree->access(node).size_x/100, RED);
	  }*/
	if(tree->access(node).children == -1){
		return;
	}
	for(int i=0; i<4; i++){
		traverseQuadNode(tree, tree->access(node).children+i, zoom);
	}
}


int main(){
	cudaState* state = initCudaState();
	//printf("Initializing starter vars\n");
	quadTree* qtree = new quadTree(0, 0);
	double leftCorner = 0;
	double rightCorner = 0;
	int debounce = 1;
	SetConfigFlags(FLAG_VSYNC_HINT);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 600, "quad tree test");
	//SetTargetFPS(60);
	/*qtree->root->divide();
	  for(int i=0; i<4; i++){
	  (qtree->root->children+i)->divide();
	  }*/

	std::random_device rd;
	std::mt19937 e2(rd());
	std::uniform_real_distribution<> dist(0.0f, 1.0f);
	//printf("%f\n", dist(e2));


	int objcount = 10;
	//object** objarr = (object**) malloc(sizeof(object*)* objcount);
	//printf("Creating obj arr\n");
	std::vector<object> objarr = {};
	for(int i=0; i<objcount; i++){
		objarr.push_back(object(dist(e2)*800, dist(e2)*800, 2e12));
		objarr[i].vx = (dist(e2)-0.5)*0.1f;
		objarr[i].vy = (dist(e2)-0.5)*0.1f;
	}

	char* title = (char*) (malloc(sizeof(char) * 100));
	double t = 0;
	int iters = 0;
	double dtDef = 0.0001;

	double dt = dtDef;

	Camera2D camera = { 0 };
	camera.zoom = 1.0f;
	int drawTree = 0;
	int infinitisemalParticles = 1;
	int render = 1;
	int running = 1;

	double spawnedMass = 8e13;

	doObjectDrift(state, objarr, qtree);
	while(!WindowShouldClose()){

		//qtree->root->divide(500, 400);
		if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){
			Vector2 pos = GetMousePosition();
			pos = GetScreenToWorld2D(pos, camera);
			for(int i=0; i<16; i++){
				objarr.push_back(object((dist(e2)-0.5)*64 + pos.x, (dist(e2)-0.5)*64 + pos.y, spawnedMass));
				objarr.back().vx = (dist(e2)-0.5)*0.1f;
				objarr.back().vy = (dist(e2)-0.5)*0.1f;
			}
		}
		if(IsKeyPressed(KEY_O)){
			Vector2 pos = GetMousePosition();
			pos = GetScreenToWorld2D(pos, camera);
			int count = 128;
			double dtheta = 360.0f / count;
			for(int i=0; i<count; i++){
#define TORADIAN M_PI / 180.0f 
				objarr.push_back(object(sin(i*TORADIAN*dtheta) * 64 + pos.x, cos(i*TORADIAN*dtheta) * 64 + pos.y, spawnedMass)); 
				objarr.back().vx = -cos(i*TORADIAN*dtheta) * 800;
				objarr.back().vy = sin(i*TORADIAN*dtheta) * 800;
			}
		}

		//printf("Clearing\n");
		float wheel = GetMouseWheelMove();
		if (wheel != 0) {
			Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
			camera.offset = GetMousePosition();
			camera.target = mouseWorldPos;
			camera.zoom *= 1.0f + wheel * 0.1f;
		}
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			Vector2 delta = GetMouseDelta();
			delta = Vector2Scale(delta, -1.0f/camera.zoom);
			camera.target = Vector2Add(camera.target, delta);
		}
		if(IsKeyPressed(KEY_SPACE)){
			running = !running;
		}
		if(IsKeyPressed(KEY_LEFT)){
			dt *= 0.9;
			if(dt <= 0){
				dt = 0;
			}
		}
		if(IsKeyPressed(KEY_RIGHT)){
			dt *= 1.1;
		}
		if(IsKeyPressed(KEY_P)){
			drawTree = !drawTree;
		}
		if(IsKeyPressed(KEY_Z)){
			infinitisemalParticles = !infinitisemalParticles;
		}
		if(IsKeyPressed(KEY_R)){
			render = !render;
		}
		if(running){
			leftCorner = rightCorner = objarr[0].x;
			for(int i=0; i<objarr.size(); i++){
				objarr[i].kick(dt);
			}
			//printf("Trying to clear the qtree\n");
			//printf("Cleared the qtree, new size should be 1, is: %lu\n", qtree->tree.size());

			for(int i=0; i<objarr.size(); i++){
				objarr[i].refine(*qtree);
			}
			//printf("Propogating the new qtree\n");
			for(int i=0; i<objarr.size(); i++){
				objarr[i].propogate(*qtree);
			}
			qtree->propogate();



			//printf("Doing gravity\n");
			for(int i=0; i<objarr.size(); i++){
				objarr[i].drift(*qtree, dt); 
			}
			for(int i=0; i<objarr.size(); i++){
				objarr[i].move(dt);
				if(objarr[i].x > rightCorner){
					rightCorner = objarr[i].x;
				}
				if(objarr[i].y > rightCorner){
					rightCorner = objarr[i].y;
				}
				if(objarr[i].x < leftCorner){
					leftCorner = objarr[i].x;
				}
				if(objarr[i].y < leftCorner){
					leftCorner = objarr[i].y;
				}
				objarr[i].calcKey();
			}
			qtree->tree.reserve(1 + objarr.size() * 4);
			qtree->clear(leftCorner, rightCorner-leftCorner);
			for(int i=0; i<objarr.size(); i++){
				objarr[i].kick(dt); 
			}
			std::sort(objarr.begin(), objarr.end());
			for(int i=0; i<objarr.size(); i++){

				objarr[i].grabNode(*qtree);
			}
		}

		//printf("Drawing\n");
		BeginDrawing();
		BeginMode2D(camera);
		if(render){
			ClearBackground(BLACK);
			for(int i=0; i<objarr.size(); i++){
				if(infinitisemalParticles){
					DrawCircleV({(float)objarr[i].x, (float)objarr[i].y}, 8 / camera.zoom, DARKBLUE);
				}else{
					DrawPixelV({(float) objarr[i].x, (float)objarr[i].y}, DARKBLUE);
					//DrawCircleV({(float)objarr[i].x, (float)objarr[i].y}, 8, DARKBLUE);
				}
			}
			if(drawTree){
				traverseQuadNode(qtree, 0, camera.zoom);
			}
		}

		EndMode2D();
		EndDrawing();
		sprintf(title, "%f | %lu", 1/GetFrameTime(), objarr.size());
		SetWindowTitle(title);
		t+=dt;
		iters++;
	}
	debounce = 0;

	CloseWindow();

	return 0;
}
