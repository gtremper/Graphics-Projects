
map<string,int> modelMap;

struct testVertex {
	GLfloat x, y, z;
	GLfloat r, g, b;
	GLfloat padding[2];
};

GLuint VertexVBOID1;
GLuint VertexIBOID1;

GLuint VertexVBOID2;
GLuint VertexIBOID2;

int indicies;
bool derp=true;

/* Parses a line of input and takes appropriate action */
void parseLine(string l, vector<command> &commands) {
	float arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8;
	stringstream line(l);
	string cmd;
	line >> cmd;
	if(cmd[0] == '#') { // comment
		return;
	} else if(cmd == "") { // blank line
		return;
	} else if(cmd == "size") {
		line >> width >> height;
	} else if(cmd == "camera") {
		line >> arg1 >> arg2 >> arg3 >> arg4 >> arg5 >> fovy;
		eyeinit = vec3(arg1,arg2,arg3);
		yawInit = arg4;
		pitchInit = arg5;
	} else if(cmd == "light") {
		if(numLights > 9) {
			return;
		} else {
			line >> arg1 >> arg2 >> arg3 >> arg4 >> arg5 >> arg6 >> arg7 >> arg8;
			light_position[numLights] = vec4(arg1,arg2,arg3,arg4);
			light_specular[numLights] = vec4(arg5,arg6,arg7,arg8);
			numLights++;
		}
	} else if(cmd == "ambient") {
		line >> arg1 >> arg2 >> arg3 >> arg4; //r g b a
		command com;
		com.op = amb;
		com.args = vec4(arg1,arg2,arg3,arg4);
		commands.push_back(com);
	} else if(cmd == "diffuse") {
		line >> arg1 >> arg2 >> arg3 >> arg4; //r g b a
		command com;
		com.op = diff;
		com.args = vec4(arg1,arg2,arg3,arg4);
		commands.push_back(com);
	} else if(cmd == "specular") {
		line >> arg1 >> arg2 >> arg3 >> arg4; //r g b a
		command com;
		com.op = spec;
		com.args = vec4(arg1,arg2,arg3,arg4);
		commands.push_back(com);
	} else if(cmd == "emission") {
		line >> arg1 >> arg2 >> arg3 >> arg4; //r g b a
		command com;
		com.op = emis;
		com.args = vec4(arg1,arg2,arg3,arg4);
		commands.push_back(com);
	} else if(cmd == "shininess") {
		line >> arg1; //s
		command com;
		com.op = shin;
		com.args = vec4(arg1,0.0,0.0,0.0);
		commands.push_back(com);
	} else if(cmd == "teapot") {
		line >> arg1; //size
		command com;
		com.op = teapot;
		com.args = vec4(arg1,0.0,0.0,0.0);
		commands.push_back(com);
	} else if(cmd == "sphere") {
		line >> arg1; //size
		command com;
		com.op = sphere;
		com.args = vec4(arg1,0.0,0.0,0.0);
		commands.push_back(com);
	} else if(cmd == "cube") {
		line >> arg1; //size
		command com;
		com.op = cube;
		com.args = vec4(arg1,0.0,0.0,0.0);
		commands.push_back(com);
	} else if(cmd == "translate") {
		line >> arg1 >> arg2 >> arg3; //x y z
		command com;
		com.op = trans;
		com.args = vec4(arg1,arg2,arg3,0.0);
		commands.push_back(com);
	} else if(cmd == "rotate") {
		line >> arg1 >> arg2 >> arg3 >> arg4; //x y z theta
		command com;
		com.op = rot;
		com.args = vec4(arg1,arg2,arg3,arg4);
		commands.push_back(com);
	} else if(cmd == "scale") {
		line >> arg1 >> arg2 >> arg3; //x y z
		command com;
		com.op = scal;
		com.args = vec4(arg1,arg2,arg3,0.0);
		commands.push_back(com);
	} else if(cmd == "pushTransform") {
		command com;
		com.op = push;
		commands.push_back(com);
	} else if(cmd == "popTransform") {
		command com;
		com.op = pop;
		commands.push_back(com);
	}
}

/* Parse the whole file */
vector<command> parseInput(char* filename) {
	ifstream myfile(filename,ifstream::in);
	string line;
	vector<command> commands;
	if(myfile.is_open()) {
		while(myfile.good()) {
			getline(myfile, line);
			parseLine(line,commands);
		}
	} else { 
		cerr << "Unable to open file " << filename << endl;
		exit(1);
	}
	myfile.close();
	return commands;
}


void parseOBJ(string filename, int modelNumber){
	vector<vec3> v; // vectors
	vector<vec4> f; // faces
	ifstream myfile(filename.c_str(), ifstream::in);
	int numVerts = 0;
	int numInds = 0;
	if(myfile.is_open()) {
		while(myfile.good()) {
			string line;
			string cmd;
			getline(myfile, line);
			stringstream ln(line);
			ln >> cmd;
			if(cmd == "v") {
				float x,y,z;
				ln >> x >> y >> z;
				vec3 p(x, y, z);
				v.push_back(p);
				numVerts++;
			}
			if(cmd == "f") {
				int v1, v2, v3, v4;
				ln >> v1 >> v2 >> v3 >> v4;
				vec4 t(v1-1, v2-1, v3-1, v4-1);
				f.push_back(t);
				numInds++;
			}
		}
	} else {
		cout << "Unable to open file " << filename << endl;
	}
	
	
	cout << "verts: " << numVerts << endl;
	cout << "inds: " << numInds << endl;
	
	testVertex verts[numVerts];
	unsigned short inds[numInds*4];
	indicies = numInds*4;
	for (int i=0; i<numVerts; i++) {
		verts[i].x = v[i][0];
		verts[i].y = v[i][1];
		verts[i].z = v[i][2];
		if (derp) {
		verts[i].r = 34.0f/255.0;
		verts[i].g = 139.0f/255.0;
		verts[i].b = 34.0f/255.0;
		} else {
		verts[i].r = 205.0f/255.0;
		verts[i].g = 201.0f/255.0;
		verts[i].b = 201.0f/255.0;	
		}
	}
	for (int i=0; i<numInds; i++){
		inds[4*i] = f[i][0];
		inds[4*i+1] = f[i][1];
		inds[4*i+2] = f[i][2];
		inds[4*i+3] = f[i][3];
	}
	if (derp){
	glGenBuffers(1, &VertexVBOID1);
	glGenBuffers(1, &VertexIBOID1);
	
	cout << "sizeofverts"<<sizeof(verts)<<endl;
	
	glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glVertexPointer(3, GL_FLOAT, 32, BUFFER_OFFSET(0));
	glEnableClientState(GL_VERTEX_ARRAY);
	glColorPointer(3, GL_FLOAT, 32, BUFFER_OFFSET(12));
	glEnableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexIBOID1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(inds), inds, GL_STATIC_DRAW);
	} else {
		glGenBuffers(1, &VertexVBOID2);
		glGenBuffers(1, &VertexIBOID2);

		cout << "sizeofverts"<<sizeof(verts)<<endl;

		glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
		glVertexPointer(3, GL_FLOAT, 32, BUFFER_OFFSET(0));
		glEnableClientState(GL_VERTEX_ARRAY);
		glColorPointer(3, GL_FLOAT, 32, BUFFER_OFFSET(12));
		glEnableClientState(GL_COLOR_ARRAY);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexIBOID2);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(inds), inds, GL_STATIC_DRAW);	
	}
	
	
	derp = false;
}



void loadObjects(char* filename) {
	ifstream myfile(filename,ifstream::in);
	string line;
	vector<string> modelFiles;
	int modelNum = 0;
	if(myfile.is_open()) {
		while(myfile.good()) {
			getline(myfile, line);
			stringstream path;
			path << "data/";
			path << line;
			modelMap[line] = modelNum;
			modelNum++;
			modelFiles.push_back(path.str());
		}
	} else { 
		cerr << "Unable to open file" << filename << endl;
		exit(1);
	}
	myfile.close();
	
	for (int i=0; i<modelNum; i++){
		string file = modelFiles[i];
		string extension = file.substr(file.find_last_of(".") + 1);
		if(extension == "obj") {
			parseOBJ(file,i);
		} else if (extension == "raw") {
			//run raw parser
		} else {
			cerr << "The extension \'" << extension <<"\' is not supported." << endl;
			exit(1);
		}
	}
}
