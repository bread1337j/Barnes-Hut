#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "raylib.h"
#include "raymath.h"
#include "quadtree.hpp"
#include "object.hpp"
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
	//printf("Initializing starter vars\n");
	quadTree* qtree = new quadTree(0, 0);
	double leftCorner = 0;
	double rightCorner = 0;
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
		objarr.push_back(object(dist(e2)*800, dist(e2)*800, 6e10));
		objarr[i].vx = (dist(e2)-0.5)*0.1f;
		objarr[i].vy = (dist(e2)-0.5)*0.1f;
	}
	
	char* title = (char*) (malloc(sizeof(char) * 100));
	double t = 0;
	double dtDef = 0.1;
	
	double dt = dtDef;

	Camera2D camera = { 0 };
	camera.zoom = 1.0f;
	int drawTree = 0;
	int infinitisemalParticles = 1;
	while(!WindowShouldClose()){
		
		//qtree->root->divide(500, 400);
		if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){
			Vector2 pos = GetMousePosition();
			pos = GetScreenToWorld2D(pos, camera);
			for(int i=0; i<4; i++){
				objarr.push_back(object((dist(e2)-0.5)*64 + pos.x, (dist(e2)-0.5)*64 + pos.y, 2e9));
				objarr.back().vx = (dist(e2)-0.5)*0.1f;
				objarr.back().vy = (dist(e2)-0.5)*0.1f;
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
			if(dt == 0){
				dt = dtDef;
			}else{
				dt = 0;
			}
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

		leftCorner = rightCorner = objarr[0].x;
		for(int i=0; i<objarr.size(); i++){
			objarr[i].physTick(dt);
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
		}
		//printf("Trying to clear the qtree\n");
		qtree->clear(leftCorner, rightCorner-leftCorner);
		//printf("Cleared the qtree, new size should be 1, is: %lu\n", qtree->tree.size());
		
		//printf("Building the new qtree\n");
		qtree->tree.reserve(1 + objarr.size() * 4);
		for(int i=0; i<objarr.size(); i++){
			objarr[i].grabNode(*qtree);
		}
		//printf("Refining the new qtree\n");
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
			objarr[i].gravTick(*qtree, qtree->root, dt); //cache be damned
		}

		//printf("Drawing\n");
		BeginDrawing();
			BeginMode2D(camera);
				ClearBackground(BLACK);
				for(int i=0; i<objarr.size(); i++){
					if(infinitisemalParticles){
						DrawCircleV({(float)objarr[i].x, (float)objarr[i].y}, 8 / camera.zoom, DARKBLUE);
					}else{
						DrawCircleV({(float)objarr[i].x, (float)objarr[i].y}, 8, DARKBLUE);
					}
				}
				if(drawTree){
					traverseQuadNode(qtree, qtree->root, camera.zoom);
				}

			EndMode2D();
		EndDrawing();
		sprintf(title, "%f | %lu", 1/GetFrameTime(), objarr.size());
		SetWindowTitle(title);
		t+=dt;
	}

	CloseWindow();

	return 0;
}
