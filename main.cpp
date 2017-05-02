#include <iostream>
#include <glut.h>
#include <vector>
#include <string>
#include <stdlib.h>


//Constants
const int graphPlaceX = 400;
const int graphPlaceY = 400;
const int graphRadius = 200;
const int nodeRadius = 20;
const int w = 1280;
const int h = 720;
void *font = GLUT_BITMAP_TIMES_ROMAN_24;
//globalVariables
int turn = 0;
int startPoint = 0;
int V = 9;
int graph[9][9] =
{ { 0, 4, 12, 0, 0, 11, 0, 8, 0 },
{ 4, 0, 8, 0, 0, 0, 0, 11, 0 },
{ 0, 8, 0, 7, 0, 4, 0, 0, 2 },
{ 0, 0, 7, 0, 2, 8, 0, 0, 0 },
{ 0, 0, 0, 2, 0, 10, 0, 0, 0 },
{ 0, 0, 4, 8, 10, 0, 2, 0, 0 },
{ 0, 0, 0, 0, 0, 2, 0, 1, 6 },
{ 8, 11, 0, 0, 0, 0, 1, 0, 7 },
{ 0, 0, 2, 0, 0, 0, 6, 7, 0 }
};
struct node{
	int x;
	int y;
	int number;
	int dist;
	bool out;
	bool in;
	std::vector<int> prev;
	std::vector<int> shortestPaths;
	node() {

	}
	node(int x, int y,int number) {
		this->x = x;
		this->y = y;
		this->number = number;
	}
};
struct edge {
	node first;
	node second;
	int weight;
	bool color;
	edge() {

	}
	edge(node u, node v, int weight) {
		this->first = u;
		this->second = v;
		this->weight = weight;
		this->color = false;
	}
};
struct Graph {
	std::vector<node> nodes;
	std::vector<edge> edges;
	Graph() {

	}
	Graph(std::vector<node>&nodes, std::vector<edge>&edges) {
		this->nodes = nodes;
		this->edges = edges;
	}
};
std::vector<node> nodes;
std::vector<edge> edges;
std::vector<Graph> turns;

std::string convertToStr(int number) {
	std::string buffer;
	buffer = std::to_string(number);
	return buffer;
}
void drawText(
	float x,
	float y,
	std::string s) {
	glRasterPos2f(x, y);
	char c;
	for (int i = 0; i < s.size(); i++) {
		c = s.at(i);
		glutBitmapCharacter(font, c);
	}
}

void drawCircle(float x, float y, int amountSegments,int radius)
{
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < amountSegments; i++)
	{
		float angle = 2.0 * 3.1415926 * float(i) / float(amountSegments);
		float dx = radius * cosf(angle);
		float dy = radius * sinf(angle);
		glVertex2f(x + dx, y + dy);
	}
	glEnd();
}
//NodesDraw
void drawNodeIndex(node node) {
	drawText(node.x - (nodeRadius / 4), node.y - (nodeRadius / 3) , convertToStr(node.number+1));
}
void drawNodeWeight(node node) {
	std::string buffer;
	buffer = std::to_string(node.dist);
	if (node.dist == INT32_MAX) {
		drawText(node.x - (nodeRadius / 4), node.y + nodeRadius + 10, "?");
	}
	else {
		drawText(node.x - (nodeRadius / 4), node.y + nodeRadius + 10, convertToStr(node.dist));
	}
}
void drawNodeCircle(node node) {
	if (node.out == true) {
		glColor3f(1.0, 0.0, 0.0);
	}
	if (node.in==true){
		glColor3f(0.0, 1.0, 0.0);
	}
	drawCircle(node.x, node.y, 30,nodeRadius);
	glColor3f(0.0, 0.0, 0.0);
}

void drawNodes(std::vector<node>& nodesinTurn) {
	for (int i = 0; i < nodesinTurn.size(); i++) {
		drawNodeCircle(nodesinTurn[i]);
		drawNodeIndex(nodesinTurn[i]);
		drawNodeWeight(nodesinTurn[i]);
	}
}
//EDGESDraw
void drawArrow(float x, float y) {
	drawCircle(x, y, 30, 5);
}
void drawEdge(edge edg) {
	glLineWidth(1.5);
	float b = edg.second.x - edg.first.x;
	float a = edg.second.y - edg.first.y;
	float c = sqrt(a*a + b*b);
	float sina = a / c;
	float cosa = b / c;
	glBegin(GL_LINES);
	glVertex2f(edg.first.x + nodeRadius*cosa, edg.first.y + nodeRadius*sina);
	glVertex2f(edg.second.x - nodeRadius*cosa, edg.second.y - nodeRadius*sina);
	glEnd();
	drawArrow(edg.second.x - nodeRadius*cosa, edg.second.y - nodeRadius*sina);
}

void drawEdgeWeight(edge edg) {
	glColor3f(0.0, 0.0, 0.0);
	float x1 = edg.first.x;
	float y1 = edg.first.y;
	float x2 = edg.second.x;
	float y2 = edg.second.y;
	float srx = (x1 + x2) / 2;
	float sry = (y1 + y2) / 2;
	drawText(srx, sry, convertToStr(edg.weight));
}

void drawEdges(std::vector<edge>&edges) {
	int coloredNumber = -1;
	for (int i = 0; i < edges.size(); i++) {
		glColor3f(0.0, 0.0, 0.0);
		drawEdge(edges[i]);
		drawEdgeWeight(edges[i]);
		if (edges[i].color == true) {
			coloredNumber = i;
		}
	}
	if (coloredNumber != -1) {
		glColor3f(0.0, 1.0, 0.0);
		drawEdge(edges[coloredNumber]);
	}
}
//EDGES INITIALIZATION
void initializeEdges(std::vector<node>&nodes) {
	for (int i = 0; i < nodes.size(); i++) {
		for (int j = 0; j < nodes.size(); j++) {
			if (graph[i][j] != 0 && j != i) {
				edges.push_back(edge(nodes[i], nodes[j], graph[i][j]));
			}
		}
	}
}
//NODES INITIALIZATION
void getNodesXY(int nodeCount)
{
	for (int i = 0; i < nodeCount; i++)
	{
		float angle = 2.0 * 3.1415926 * float(i) / float(nodeCount);
		float dx = graphRadius * cosf(angle);
		float dy = graphRadius * sinf(angle);
		nodes.push_back(node(graphPlaceX + dx, graphPlaceY + dy, i));
	}
}
void initializeNodesForAlgortihm(std::vector<node>&nodes) {
	for (int i = 0; i < nodes.size(); i++) {
		nodes[i].out = false;
		nodes[i].in = false;
		if (nodes[i].number == startPoint) {
			nodes[i].dist = 0;
		}
		else {
			nodes[i].dist = INT32_MAX;
		}
	}
}
//DEJKSTRA ALGORITHM
int minDistance(std::vector<node>& nodes)
{
	int min = INT32_MAX;
	int index = 0;
	for (int i = 0; i < V; i++)
		if (!nodes[i].out && nodes[i].dist <= min) {
			min = nodes[i].dist;
			index = i;
		}
	return index;
}
void printPath(std::vector<int> path) {
	for (int i = 0; i < path.size(); i++) {
		std::cout << path[i] << "<-";
	}
}
void shortestPathToNode(std::vector<node>&nodes, node currentNode,std::vector<int> path) {
	if (currentNode.number == startPoint) {
		path.push_back(startPoint+1);
		printPath(path);
		std::cout << "\n";
		return;
	}
	path.push_back(currentNode.number + 1);
	for (int i = 0; i < currentNode.prev.size(); i++) {

		shortestPathToNode(nodes,nodes[currentNode.prev[i]],path);
	}

}
void getShortestPaths(std::vector<node>&nodes) {
	std::vector<int> path;
	for(int i = 0; i < nodes.size(); i++) {
		std::cout << "SHORTEST PATHS TO NODE NUMBER " << nodes[i].number + 1 << "\n";
		shortestPathToNode(nodes,  nodes[i],path);
	}
}
void DejkstraAlgorithm(std::vector<node>&nodes, std::vector<edge>&edges)
{
	initializeNodesForAlgortihm(nodes);
	nodes[startPoint].prev.push_back(startPoint);
	turns.push_back(Graph(nodes, edges));
	for (int count = 0; count < nodes.size() - 1; count++) {
		int i = minDistance(nodes);
		nodes[i].in = true;
		turns.push_back(Graph(nodes, edges));
		for (int j = 0; j < edges.size(); j++) {
			if (i == edges[j].first.number) {
				edges[j].color = true;
				if (!nodes[edges[j].second.number].out && nodes[i].dist != INT32_MAX) {
					if (nodes[i].dist + edges[j].weight == nodes[edges[j].second.number].dist) {
						nodes[edges[j].second.number].prev.push_back(i);
					}
					if (nodes[i].dist + edges[j].weight < nodes[edges[j].second.number].dist) {
						nodes[edges[j].second.number].dist = nodes[i].dist + edges[j].weight;
						nodes[edges[j].second.number].prev.clear();
						nodes[edges[j].second.number].prev.push_back(i);
					}
				}
				turns.push_back(Graph(nodes, edges));
				edges[j].color = false;
			}
		}
		nodes[i].in = false;
		nodes[i].out = true;
		turns.push_back(Graph(nodes, edges));
	}
}
//CONSOLELOGS
void printNodesDistances(std::vector<node>&nodes) {
	for (int i = 0; i < nodes.size(); i++) {
		std::cout << "Number " << nodes[i].number+1 << " Weight " << nodes[i].dist << "\n";
	}
}

//BUTTONS
void drawButtonBack() {
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex2f(100, 100);
	glVertex2f(100, 150);
	glVertex2f(100, 150);
	glVertex2f(200, 150);
	glVertex2f(200, 150);
	glVertex2f(200, 100);
	glVertex2f(200, 100);
	glVertex2f(100, 100);
	glEnd();
	drawText(120, 120, "BACK");
}
void drawButtonNext() {
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex2f(400, 100);
	glVertex2f(400, 150);
	glVertex2f(400, 150);
	glVertex2f(500, 150);
	glVertex2f(500, 150);
	glVertex2f(500, 100);
	glVertex2f(500, 100);
	glVertex2f(400, 100);
	glEnd();
	drawText(420, 120, "NEXT");
}
void drawButtonEnd() {
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex2f(250, 100);
	glVertex2f(250, 150);
	glVertex2f(250, 150);
	glVertex2f(350, 150);
	glVertex2f(350, 150);
	glVertex2f(350, 100);
	glVertex2f(350, 100);
	glVertex2f(250, 100);
	glEnd();
	drawText(270, 120, "END");
}
void drawButtons() {
	drawButtonBack();
	drawButtonEnd();
	drawButtonNext();
}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 0.0);
	drawNodes(turns[turn].nodes);
	drawEdges(turns[turn].edges);
	drawButtons();
	glutSwapBuffers();
}
void setup() {
	getNodesXY(V);
	initializeEdges(nodes);
	DejkstraAlgorithm(nodes, edges);
	printNodesDistances(nodes);
	getShortestPaths(turns[turns.size() - 1].nodes);
}
void OnMouseClick(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (x > 400 && x < 500 && h-y >100 && h-y < 150 && turn + 1 <= turns.size() - 1) {
				turn = turn + 1;
		}
		if (x > 100 && x < 200 && h - y >100 && h - y < 150  && turn - 1 >=0) {
				turn = turn - 1;
		}
		if (x > 250 && x < 350 && h - y >100 && h - y < 150) {
			turn = turns.size() - 1;
		}
		glClear(GL_COLOR_BUFFER_BIT);
		glutPostRedisplay();
	}
}
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
int main(int argc, char * argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); 

	glutInitWindowSize(w, h);
	glutInitWindowPosition(200, 200);

	glutCreateWindow("Dejkstra Visualization");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutMouseFunc(OnMouseClick);
	glClearColor(1.0, 1.0, 1.0, 0.0);
	setup();
	glutMainLoop();
	return 0;
}