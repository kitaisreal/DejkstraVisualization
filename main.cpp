#include <iostream>
#include <glut.h>
#include <vector>
#include <string>
#include <stdlib.h>

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
//Constants
const int graphPlaceX = 400;
const int graphPlaceY = 400;
const int graphRadius = 200;
const int nodeRadius = 20;
const int w = 1280;
const int h = 720;
void *font = GLUT_BITMAP_TIMES_ROMAN_24;
int turn = 0;
int startPoint = 0;
int V = 9;
int graph[9][9] = 
{ { 0, 4, 0, 0, 0, 0, 0, 8, 0 },
{ 4, 0, 8, 0, 0, 0, 0, 11, 0 },
{ 0, 8, 0, 7, 0, 4, 0, 0, 2 },
{ 0, 0, 7, 0, 9, 14, 0, 0, 0 },
{ 0, 0, 0, 9, 0, 10, 0, 0, 0 },
{ 0, 0, 4, 14, 10, 0, 2, 0, 0 },
{ 0, 0, 0, 0, 0, 2, 0, 1, 6 },
{ 8, 11, 0, 0, 0, 0, 1, 0, 7 },
{ 0, 0, 2, 0, 0, 0, 6, 7, 0 }
};

struct node{
	int x;
	int y;
	int number;
	int weight;
	bool visited;
	bool out;
	bool in;
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

void printEdges(std::vector<edge>&edges) {
	for (int i = 0; i < edges.size(); i++) {
		std::cout << " Edge " << i << " FIRST " << edges[i].first.number << "  SECOND " << edges[i].second.number << " WEIGHT " << edges[i].weight << "\n";
	}
}
void printNodes(std::vector<node>&nodes) {
	for (int i = 0; i < nodes.size(); i++) {
		std::cout << "Number " << nodes[i].number << " Weight " << nodes[i].weight << "\n";
	}
}
void OnMouseClick(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		std::cout << "TURNS " << turns.size() << "\n";
		//std::cout << turns.size();
		//printNodes(turns[turns.size()-1].nodes);
	}
}
void renderBitmapString(
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
std::string convertToStr(int number) {
	std::string buffer;
	buffer = std::to_string(number);
	return buffer;
}
void drawNodesIndexes(std::vector<node>& nodes) {
	for (int i = 0; i < nodes.size(); i++) {
		renderBitmapString(nodes[i].x - (nodeRadius / 4), nodes[i].y - (nodeRadius / 3) , convertToStr(nodes[i].number+1));
	}
}
void drawNodesWeight(std::vector<node>&nodes) {
	std::string buffer;
	for (int i= 0; i < nodes.size(); i++) {
		buffer = std::to_string(nodes[i].weight);
		if (nodes[i].weight == INT32_MAX) {
			renderBitmapString(nodes[i].x - (nodeRadius / 4), nodes[i].y + nodeRadius + 10, "?");
		}
		else {
			renderBitmapString(nodes[i].x - (nodeRadius / 4), nodes[i].y + nodeRadius + 10, convertToStr(nodes[i].weight));
		}
	}
}
void drawNodesCircles(std::vector<node>&nodes) {
	for (int i = 0; i < nodes.size(); i++) {
		drawCircle(nodes[i].x, nodes[i].y, 30,nodeRadius);
	}
}

void drawArrow(float x, float y) {
	glColor3f(0.0, 1.0, 0.0);
	drawCircle(x, y, 30, 5);
	glColor3f(0.0, 0.0, 0.0);

}
void drawEdge(edge edg) {
	glLineWidth(2);
	glBegin(GL_LINES);
	int b = edg.second.x - edg.first.x;
	int a = edg.second.y - edg.first.y;
	float c = sqrt(a*a + b*b);
	float sina = a / c;
	float cosa = b / c;
	glVertex2f(edg.first.x+nodeRadius*cosa, edg.first.y+nodeRadius*sina);
	glVertex2f(edg.second.x-nodeRadius*cosa, edg.second.y- nodeRadius*sina);
	glEnd();
	drawArrow(edg.second.x - nodeRadius*cosa, edg.second.y - nodeRadius*sina);
}
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
void initializeNodesWeight(std::vector<node>&nodes) {
	for (int i = 0; i < nodes.size(); i++) {
		if (nodes[i].number == startPoint) {
			nodes[i].weight = 0;
		}
		else {
			nodes[i].weight = INT32_MAX;
		}
	}
}
void initializeNodesVisited(std::vector<node>&nodes) {
	for (int i = 0; i < nodes.size(); i++) {
		nodes[i].visited = false;
	}
}
void initializeEdges(std::vector<node>&nodes) {
	for (int i = 0; i < nodes.size(); i++) {
		for (int j = 0; j < nodes.size(); j++) {
			if (graph[i][j] != 0 && j != i) {
				edges.push_back(edge(nodes[i], nodes[j], graph[i][j]));
			}
		}
	}
}
void drawEdgeWeight(edge edg) {
	float x1 = edg.first.x;
	float y1 = edg.first.y;
	float x2 = edg.second.x;
	float y2 = edg.second.y;
	float srx = (x1 + x2) / 2;
	float sry = (y1 + y2) / 2;
	renderBitmapString(srx, sry, convertToStr(edg.weight));
}
void drawEdges(std::vector<edge>&edges) {
	for (int i = 0; i < edges.size(); i++) {
		drawEdge(edges[i]);
		drawEdgeWeight(edges[i]);
	}
}
void initializeButtons() {
	glBegin(GL_LINES);
	glVertex2f(150, 250);
	glVertex2f(150, 120);
	glVertex2f(300, 120);
	glVertex2f(300, 250);
	glEnd();
}
void drawNodes() {
	drawNodesCircles(nodes);
	drawNodesIndexes(nodes);
	drawNodesWeight(nodes);
}
int minDistance(std::vector<node>& nodes)
{
	int min = INT32_MAX;
	int index = 0;

	for (int i = 0; i < V; i++)
		if (!nodes[i].out && nodes[i].weight <= min) {
			min = nodes[i].weight;
			index = i;
		}
	return index;
}
void initializeNodesInOut(std::vector<node>&nodes) {
	for (int i = 0; i < nodes.size(); i++) {
		nodes[i].out = false;
		nodes[i].in = false;
	}
}
void printNodesWeights(std::vector<node>&nodes) {
	for (int i = 0; i < nodes.size(); i++) {
		std::cout << " i " << nodes[i].weight << "\n";
	}
}
std::vector<edge> getMinDistanceEdges(std::vector<edge>&edges, int minnumber) {
	std::vector<edge> minDistEdges;
	for (int i = 0; i < edges.size(); i++) {
		if (edges[i].first.number == minnumber) {
			minDistEdges.push_back(edges[i]);
		}
	}
	return minDistEdges;
}
void DejkstraAlgorithm(std::vector<node>&nodes, std::vector<edge>&edges)
{
	initializeNodesInOut(nodes);
	initializeNodesWeight(nodes);
	turns.push_back(Graph(nodes, edges));
	for (int count = 0; count < nodes.size() - 1; count++) {
		int i = minDistance(nodes);
		std::cout << "i " << i << "\n";
		nodes[i].in = true;
		turns.push_back(Graph(nodes, edges));
		std::vector<edge> minDistEdges = getMinDistanceEdges(edges, i);
		for (int j = 0; j < minDistEdges.size(); j++) {
			edges[i].color = true;
			if (!nodes[minDistEdges[j].second.number].out && nodes[i].weight != INT32_MAX && nodes[i].weight + minDistEdges[j].weight < nodes[minDistEdges[j].second.number].weight) {
				nodes[minDistEdges[j].second.number].weight = nodes[i].weight + minDistEdges[j].weight;
			}
			turns.push_back(Graph(nodes, edges));
			edges[j].color = false;
		}
		nodes[i].in = false;
		nodes[i].out = true;
		turns.push_back(Graph(nodes, edges));
	}

	/*for (int count = 0; count < V - 1; count++)
	{
	int u = minDistance(nodes);
	nodes[u].visited = true;
	nodes[u].in = true;
	for (int v = 0; v < V; v++) {
	if (!nodes[v].visited && graph[u][v] && nodes[u].weight != INT_MAX && nodes[u].weight + graph[u][v] < nodes[v].weight)
	{
	nodes[v].weight = nodes[u].weight + graph[u][v];
	}
	}
	nodes[u].in = false;
	nodes[u].out = true;
	}

	//printSolution(dist, V);*/
}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 0.0);
	getNodesXY(9);
	initializeNodesWeight(nodes);
	initializeEdges(nodes);
	drawNodes();
	drawEdges(edges);
	DejkstraAlgorithm(nodes, edges);
	glutSwapBuffers();
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
	glutMainLoop();
	return 0;
}