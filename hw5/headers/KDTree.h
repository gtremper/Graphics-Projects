/* Class definitions for KDTree acceleration structure */

class TreeNode {
public:
	TreeNode(std::vector<Shape*>&,int,AABB);
	~TreeNode();
	Intersection intersect(Ray&);
	TreeNode* left;
	TreeNode* right;
	std::vector<Shape*> primatives;
	AABB aabb;
	int axis;
	double split;
};

