#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "iostream"
#include "iomanip"
#include <vector>
#include <algorithm>

using namespace std;

void line(int, int, int, int, TGAImage&, const TGAColor&); //make sure you get those & in the prototype variables if you have it in function declaration
void sweepTriangle(Vec2i, Vec2i, Vec2i, TGAImage&, const TGAColor&);
void baryTriangle(Vec2i*, TGAImage&, const TGAColor&);
void ZTriangle(Vec3f*, int*, TGAImage&, const TGAColor&);
bool barycentric(Vec2i*, Vec2i);
void lineT(Vec2i, Vec2i, TGAImage&, const TGAColor&);
void rasterize(Vec2i, Vec2i, TGAImage&, const TGAColor&, int[]);
Vec3f barycentric2(Vec2i*, Vec2i);

void line1(int, int, int, int, TGAImage&, TGAColor);
void line2(int, int, int, int, TGAImage&, TGAColor);
void line3(int, int, int, int, TGAImage&, TGAColor);
void line4(int, int, int, int, TGAImage&, TGAColor);

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor gray  = TGAColor(127, 127, 127, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0  , 255, 0  , 255);
const TGAColor blue  = TGAColor(0  , 0  , 255, 255);

bool prevprev = false;
bool previous = false;
bool current = false;

int main(int argc, char** argv) {
	const int width = 800;
	const int height = 800;
	TGAImage image(width, height, TGAImage::RGB);
	//image.set(52, 41, red);
	/*line(9, 9, 99, 99, image, green);
	line(80, 30, 13, 10, image, white);*/
	//Porsche_911_GT2.obj  //z coord float goes beyond 2.0, does not stay under 1.0
	//african_head.obj
	Model model1("resources\\african_head.obj"); //turns out a cstring is a char pointer 
	Model* model = &model1; //works after providing correct arg to constructor
	int renderType = 0;
	cout << "Enter 1 to render wireframe, \n 2 to render colored triangle view, \n or 3 to render Illuminated model. \n";
	while (renderType < 1 || renderType > 3) {
		cin >> renderType;
		cout << "Render type: " << renderType << endl;
		if (renderType < 1 || renderType > 3) cout << "Invalid number. ";
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	if (renderType == 1) {  //wireframe
		for (int i = 0; i < model->nfaces(); i++) {  //nfaces is number of faces, returns size of vector array of int vectors of faces, the -> (arrow operator) is used to access members of a class or struct through a pointer, it is equivalent to (*model).nfaces()
			vector<int> face = model->getFace(i); //face is a int vector of which vertices make up a face eg. 956,27,1250, creates and sets int vector names face = to face number i, face number i is an int vector from meta vector faces_ , 
			for (int j = 0; j < 3; j++) {
				Vec3f v0 = model->vert(face[j]); //gets vector of float coordinates for vertex j, j = 0,1,2  //the two different Vec3f means it draws lines from vertex 0 to 1, 1 to 2, 2 to 0 to make a triangle face
				Vec3f v1 = model->vert(face[(j + 1) % 3]); //gets vector of float coordinates for vertex j, j = 1,2,0 , this one re-arranges order of vertices
				int x0 = (v0.x + 1.0) * width / 2.0;  //it does the plus 1 divide by 2 because the floats can be negative, translates -1.0 to 1.0 range to 0 to 1.0
				int y0 = (v0.y + 1.0) * height / 2.0;  //these for loops will always draw the model touching the top and bottom of image
				int x1 = (v1.x + 1.0) * width / 2.0;   //if you replace the .x's with z's you get a side view
				int y1 = (v1.y + 1.0) * height / 2.0;
				line(x0, y0, x1, y1, image, white);
			}
		}
	}
	if (renderType == 2) {  //random color triangles
		for (int i = 0; i < model->nfaces(); i++) {
			vector<int> face = model->getFace(i);
			Vec2i triangle[3];
			for (int j = 0; j < 3; j++) {
				Vec3f v0 = model->vert(face[j]); //gets vector of float coordinates for vertex j, j = 0,1,2  //the two different Vec3f means it draws lines from vertex 0 to 1, 1 to 2, 2 to 0 to make a triangle face
				int x = (v0.x + 1.0) * width / 2.0;  //it does the plus 1 divide by 2 because the floats can be negative, translates -1.0 to 1.0 range to 0 to 1.0
				int y = (v0.y + 1.0) * height / 2.0;  //these for loops will always draw the model touching the top and bottom of image
				triangle[j] = Vec2i(x, y);
			}
			baryTriangle(triangle, image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
		}
	}
	if (renderType == 3) {  //
		Vec3f lightDirection(0, 0, -1);
		for (int i = 0; i < model->nfaces(); i++) {
			vector<int> face = model->getFace(i);
			Vec2i screenCoords[3];
			Vec3f worldCoords[3];
			for (int j = 0; j < 3; j++) {       //loads worldCoords and screenCoords vectors
				Vec3f vert = model->vert(face[j]); //because its a vert of face we know for this face
				worldCoords[j] = vert;
				screenCoords[j] = Vec2i((vert.x + 1.0) * width * 0.5, (vert.y + 1.0) * height * 0.5);
			}
			Vec3f normal = (worldCoords[2] - worldCoords[0]) ^ (worldCoords[1] - worldCoords[0]);
			normal.normalize();  //now its actually the normal
			float lightIntensity = normal * lightDirection;  //scalar product of normal and light direction
			if (lightIntensity > 0) {   //if light intensity is less that zero, the normal points the other direction
				baryTriangle(screenCoords, image, TGAColor(lightIntensity * 255, lightIntensity * 255, lightIntensity * 255, 255));
			}
		}
	}
	//if (renderType == 4) {  //
	//	cerr << "1 ";
	//	Vec3f lightDirection(0, 0, -1);
	//	int zBuffer1[width * height];
	//	int* zBuffer = zBuffer1;
	//	for (int i = 0; i < model->nfaces(); i++) {
	//		vector<int> face = model->getFace(i);
	//		//Vec2i screenCoords[3];
	//		Vec3f worldCoords[3];
	//		for (int j = 0; j < 3; j++) {       //loads worldCoords and screenCoords vectors
	//			Vec3f vert = model->vert(face[j]); //because its a vert of face we know for this face
	//			worldCoords[j] = vert;
	//		}
	//		Vec3f normal = (worldCoords[2] - worldCoords[0]) ^ (worldCoords[1] - worldCoords[0]);
	//		normal.normalize();  //now its actually the normal
	//		float lightIntensity = normal * lightDirection;  //scalar product of normal and light direction
	//		if (lightIntensity > 0) {   //if light intensity is less that zero, the normal points the other direction
	//			ZTriangle(worldCoords, zBuffer, image, TGAColor(lightIntensity * 255, lightIntensity * 255, lightIntensity * 255, 255));
	//		}
	//	}
	//	cerr << "render type 4 end \n";
	}
	


	//Vec2i pts[3] = { Vec2i(10,10),Vec2i(100,30),Vec2i(190,160) };
	//triangle(pts, image, red);
	Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };  //provided triangles to make triangle/polygon filling algorithm
	Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };  //each one one of these is an arry of type Vec2i
	Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
	//sweepTriangle(t0[0], t0[1], t0[2], image, red);
	//sweepTriangle(t1[0], t1[1], t1[2], image, white);
	//sweepTriangle(t2[0], t2[1], t2[2], image, green);
	/*baryTriangle(t0, image, red);
	baryTriangle(t1, image, white);
	baryTriangle(t2, image, green);*/
	//TGAImage scene(width, height, TGAImage::RGB);
	//lineT(Vec2i(20, 34), Vec2i(744, 400), scene, red);
	//lineT(Vec2i(120, 434), Vec2i(444, 400), scene, green);
	//lineT(Vec2i(330, 463), Vec2i(594, 200), scene, blue);
	//TGAImage render(width, 16, TGAImage::RGB);
	//int ybuffer[width];
	//for (int i = 0; i < width; i++) {
	//	ybuffer[i] = numeric_limits<int>::min(); //sets ybuffer[i] to minimum value of int data type
	//}
	//rasterize(Vec2i(20, 34), Vec2i(744, 400), render, red, ybuffer);
	//rasterize(Vec2i(120, 434), Vec2i(444, 400), render, green, ybuffer);
	//rasterize(Vec2i(330, 463), Vec2i(594, 200), render, blue, ybuffer);
	//render.flip_vertically(); 
	//render.write_tga_file("outputTestRender.tga");


	//scene.flip_vertically();
	//scene.write_tga_file("outputTest.tga");

	image.flip_vertically(); //to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	cout << "Program finished " << endl;
	return 0;
}

bool barycentric(Vec2i* pts, Vec2i P) {  //to compute barycenter, returns barycentric coordinates of P
	//found this algorithm while looking for one online
	Vec2i vB = pts[1] - pts[0], vC = pts[2] - pts[0], vP = P - pts[0]; //made a custom operator function for this
	float denom = (vB.x * vC.y - vC.x * vB.y);
	//if (denom == 0.0f) denom = 1.0f;
	float denomMult = 1.0f / denom;          //dividing it here instead of later means less divisions, a little more efficient
	float u = (vP.x * vC.y - vC.x * vP.y) * denomMult;  //uses vector to P and vector to C to get point C barycentric coordinate/weight
	float v = (vB.x * vP.y - vP.x * vB.y) * denomMult;  //uses vector to P and vector to B to get point B barycentric coordinate/weight
	float w = 1.0f - u - v;
	if (u < 0 || v < 0 || w < 0) {
		return false;
	}
	else
		return true; //somehow it was filling most pixels but not all even without a return true??
}

void baryTriangle(Vec2i* pts, TGAImage& image, const TGAColor& color) {
	if (pts[0] == pts[1] || pts[0] == pts[2] || pts[1] == pts[2]) return; //if two points are the same triangle is degenerate so return, would also cause a /0 error. made an operator overload for this
	Vec2i bboxMin(image.get_width() - 1, image.get_height() - 1); //get coordinates of image edges
	Vec2i bboxMax(0, 0);										//they start out opposite of what they will be
	Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
	for (int i = 0; i < 3; i++) {                         //the initial loop will make the bounding box only one pixel, points 2 and 3 mae the bounding box
		bboxMin.x = max(0, min(bboxMin.x, pts[i].x));
		bboxMin.y = max(0, min(bboxMin.y, pts[i].y));
		bboxMax.x = min(clamp.x, max(bboxMax.x, pts[i].x));
		bboxMax.y = min(clamp.y, max(bboxMax.y, pts[i].y));
	}
	Vec2i P;
	//cout << "before filling tri pixels " << endl;
	for (P.x = bboxMin.x; P.x <= bboxMax.x; P.x++) {
		for (P.y = bboxMin.y; P.y <= bboxMax.y; P.y++) {
			if (barycentric(pts, P) == false) {
				continue;
			} //breaks current iteration of loop if point is outside triangle
			image.set(P.x, P.y, color);
		}
	}
}

void rasterize(Vec2i p0, Vec2i p1, TGAImage& image, const TGAColor& color, int ybuffer[]) {
	if (p0.x > p1.x) {
		std::swap(p0, p1);
	}
	for (int x = p0.x; x <= p1.x; x++) {
		float t = (x - p0.x) / (float)(p1.x - p0.x);  //distance travelled so far divided by total distance, becomes float between 0 and 1
		int y = (float)(p0.y * (1. - t) + p1.y * t);    //finds correct y coord from distance left * starting y coord + distance remaining * end y coord
		if (ybuffer[x] < y) {
			ybuffer[x] = y;
			image.set(x, 0, color);
		}
	}
}

void ZTriangle(Vec3f* pts, int* zBuffer, TGAImage& image, const TGAColor& color) {
	if (pts[0] == pts[1] || pts[0] == pts[2] || pts[1] == pts[2]) return; //if two points are the same triangle is degenerate so return, would also cause a /0 error. made an operator overload for this
	Vec2f bboxMin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());    //get coordinates of image edges but float
	Vec2f bboxMax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	Vec2f clamp(image.get_width() - 1, image.get_height() - 1);   //so it wont draw anyhting beyond this
	Vec2i screenCoords[3];
	for (int i = 0; i < 3; i++) {                         //the initial loop will make the bounding box only one pixel, points 2 and 3 make the bounding box
		bboxMin.x = std::max(0.f, std::min(bboxMin.x, pts[i].x));
		bboxMin.y = std::max(0.f, std::min(bboxMin.y, pts[i].y));
		bboxMax.x = std::min(clamp.x, std::max(bboxMax.x, pts[i].x));
		bboxMax.y = std::min(clamp.y, std::max(bboxMax.y, pts[i].y));
		screenCoords[i] = Vec2i((pts[i].x + 1.0) * image.get_width() * 0.5, (pts[i].y + 1.0) * image.get_height() * 0.5);
	}
	Vec3f Pf;
	cout << endl << "TC " << Pf.x << " " << bboxMax.x << " ";
	for (Pf.x = bboxMin.x; Pf.x <= bboxMax.x; Pf.x += (2.0 / image.get_width())) {
		cout << " TCx: Pf: " << Pf.x;
		for (Pf.y = bboxMin.y; Pf.y <= bboxMax.y; Pf.y += (2.0 / image.get_height())) {
			//cout << "TCy ";
			Vec2i P((Pf.x + 1.0) * image.get_width() * 0.5, (Pf.y + 1.0) * image.get_height() * 0.5);
			Vec3f baryScreen = barycentric2(screenCoords, P);
			if (baryScreen.u < 0 || baryScreen.v < 0 || baryScreen.w < 0) {
				continue;
			} //breaks current iteration of loop if point is outside triangle
			Pf.z = 0.0f;
			for (int i = 0; i < 3; i++) 
				Pf.z += pts[i].z * baryScreen.raw[i];
			if (zBuffer[int(Pf.x + Pf.y * image.get_width())] < Pf.z) {
				zBuffer[int(Pf.x + Pf.y * image.get_width())] = Pf.z;
				image.set(P.x, P.y, color);
				//image.set(P.x, P.y, color);
			}
		}
	}
}

Vec3f barycentric2(Vec2i* pts, Vec2i P) {  //to compute barycenter, returns barycentric coordinates of P
	//found this algorithm while looking for one online
	Vec2i vB = pts[1] - pts[0], vC = pts[2] - pts[0], vP = P - pts[0]; //made a custom operator function for this
	float denom = (vB.x * vC.y - vC.x * vB.y);
	//if (denom == 0.0f) denom = 1.0f;
	float denomMult = 1.0f / denom;          //dividing it here instead of later means less divisions, a little more efficient
	float u = (vP.x * vC.y - vC.x * vP.y) * denomMult;  //uses vector to P and vector to C to get point C barycentric coordinate/weight
	float v = (vB.x * vP.y - vP.x * vB.y) * denomMult;  //uses vector to P and vector to B to get point B barycentric coordinate/weight
	float w = 1.0f - u - v;
	return Vec3f(u, v, w); //in this order because pts[] starts at a then b then c
}


void sweepTriangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, const TGAColor& color) {   //line sweeping method, old
	if (t0.y == t1.y && t0.y == t2.y) return;
	if (t0.y > t1.y) swap(t0, t1);
	if (t0.y > t2.y) swap(t0, t2);
	if (t1.y > t2.y) swap(t1, t2);   //vertices now ordered by y coord t2 > t1 > t0, x coords still vary
	int totalHeight = t2.y - t0.y;
	for (int i = 0; i <= totalHeight; i++) {
		bool secondHalf = i > t1.y - t0.y || t1.y == t0.y;
		int segmentHeight = secondHalf ? t2.y - t1.y : t1.y - t0.y;
		if (segmentHeight == 0) segmentHeight++;                      // the + 1 is added so you dont divide by zero
		float alpha = (float)i / totalHeight;                          //percent 0 - 1 total height travelled, from y distance line has traveled so far / total height
		float beta = (float)(i - (secondHalf ? t1.y - t0.y : 0)) / segmentHeight;   //percent 0 - 1 segment height travelled, from y distance line has traveled so far / segment height
		Vec2i A = t0 + (t2 - t0) * alpha;                             //custom operator that adds the coordinates
		Vec2i B = secondHalf ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;      //these lines now work right after combining top and bottom triangle halves
		int xa = (t0.x + (t2.x - t0.x) * alpha);                                      //I wrote these two lines to do the same as the two previous
		int xb = secondHalf ? t1.x + (t2.x - t1.x) * beta : (t0.x + (t1.x - t0.x) * beta);  //but its not exacty the same? //now they might be
		if (A.x > B.x) swap(A.x, B.x);
		for (int j = A.x; j <= B.x; j++) {
			image.set(j, t0.y + i, color);                                 //due to int casts, t0.y+i does not equal A.y
		}

	}
}

void line(int x0, int y0, int x1, int y1, TGAImage& image, const TGAColor& color) {     //the all integer line function
	bool steep = false;
	if (abs(x0 - x1) < abs(y0 - y1)) {    //if the line is steep, transpose the image, x distance will always be longer now
		swap(x0, y0);
		swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {   // make it left to right
		swap(x0, x1);
		swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror = abs(dy) * 2; //distance error is twice y distance, y distance is always shorter than or equal to x distance
	int error = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		if (steep) {                  //to place first pixel then calculate next after
			image.set(y, x, color);  //if transposed, de transpose
		}
		else
			image.set(x, y, color);
		error += derror;      //, related to how many pixels it needs to place before moving to next y coord
		if (error > 0.5) {  //y wont change if error is within rounding values of where it should be.
			y += (y1 > y0 ? 1 : -1);  //positive and negative for going both directions
			error -= dx * 2;  //will always go to negative, the ratio of how how many y coord moves per x coord move (less than 1.0) is based on how often error is above 0

		}
	}
}

void lineT(Vec2i t0,Vec2i t1, TGAImage& image, const TGAColor& color) {     //the all integer line function, Vec2i coordinate version
	cout << " lineT called " << endl;
	bool steep = false;
	if (abs(t0.x - t1.x) < abs(t0.y - t1.y)) {    //if the line is steep, transpose the image, x distance will always be longer now
		swap(t0.x,t0.y);
		swap(t1.x,t1.y);
		steep = true;
	}
	if (t0.x > t1.x) {   // make it left to right
		swap(t0.x, t1.x);
		swap(t0.y, t1.y);
	}
	int dx = t1.x - t0.x;
	int dy = t1.y - t0.y;
	int derror = abs(dy) * 2; //distance error is twice y distance, y distance is always shorter than or equal to x distance
	int error = 0;
	int y = t0.y;
	for (int x = t0.x; x <= t1.x; x++) {
		if (steep) {                  //to place first pixel then calculate next after
			image.set(y, x, color);  //if transposed, de transpose
		}
		else
			image.set(x, y, color);
		error += derror;      //, related to how many pixels it needs to place before moving to next y coord
		if (error > 0.5) {  //y wont change if error is within rounding values of where it should be.
			y += (t1.y > t0.y ? 1 : -1);  //positive and negative for going both directions
			error -= dx * 2;  //will always go to negative, the ratio of how how many y coord moves per x coord move (less than 1.0) is based on how often error is above 0

		}
	}
}

void lineHorizontal(int y, int start, int end, TGAImage& image, const TGAColor& color) {
	int distance = end - start;
	for (int i = start + 1; i < end; i++) {
		image.set(start, y, color);
	}
}

void line1(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
	for (float t = 0; t < 1.00001; t += 0.01) {
		int x = x0 + (x1 - x0) * t;
		int y = y0 + (y1 - y0) * t;
		image.set(x, y, color);
	}
}

void line2(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
	for (int x = x0; x <= x1; x++) {
		float t = (x - x0) / (float)(x1 - x0);  //percentage of how far along you currently are expressed as a float between 1 and 0, 1 = 100%, 0.33 = 33%
		int y = (float)(y0 * (1.0 - t) + (y1 * t));     // start coord * (% remaining) + end coord * % distance travelled = next y coord inline???
		image.set(x, y, color);
	}
}

void line3(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {   //line 2 with info printed out
	cout << setprecision(8);
	for (int x = x0; x <= x1; x++) {
		cout << " x: " << x;
		float t = (x - x0) / (float)(x1 - x0);
		cout << " t: " << t;
		int y = (float)((y0 * (1.0 - t)) + (y1 * t)); //YOU NEED THE (float) FOR IT TO WORK RIGHT //it just works better, not entirely right
		float z = ((y0 * (1.0 - t)) + (y1 * t)); //could try either making it a double or use rounding from cmath or math.h later
		cout << " z: " << z;
		int u = (float)z;
		cout << " u: " << u;
		//cout << " 1-t: " << (1. - t) << " y0*1-t: " << (y0 * (1. - t)) << " y1 * t: " << (y1 * t) << " y coord float: " << (y0 * (1.0 - t)) + (y1 * t);
		cout << " y: " << y << endl;
		image.set(x, y, color);
	}
}

void line4(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
	bool steep = false;
	if (abs(x0 - x1) < abs(y0 - y1)) {    //if the line is steep, transpose the image
		swap(x0, y0);
		swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {   // make it left to right
		swap(x0, x1);
		swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	float derror = abs(dy / float(dx)); //gets ratio of y distance compared to x distance, 4 means y distance is 4 times longer
	float error = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		if (steep) {                  //to place first pixel then calculate next after
			image.set(y, x, color);  //if transposed, de transpose
		}else
			image.set(x, y, color);
		error += derror;      //y distance is always shorter than x, this is related to how many pixels it needs to place before moving to next y coord
		if (error > 0.5) {  //y wont change if error is within rounding values of where it should be.
			y += (y1 > y0 ? 1 : -1);  //positive and negative for going both directions
			error -= 1.0;

		}
	}
}






