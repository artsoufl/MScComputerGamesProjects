////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012, 2013
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// Triangle example: The most basic OpenGL application
//
// Level: 0
//
// Demonstrates:
//   Basic framework app
//   Shaders
//   Basic Matrices
//
//


namespace octet {

	class camera {
		mat4t cameraInstance;
		float angle, angleX;
		int preMouseX, preMouseY;
		int mouseX, mouseY;
		enum { MAX_MOVE = 2, MAX_ROTATION = 5 };

	public:
		camera () { angle = 0.0f; angleX = 0.0f; cameraInstance.loadIdentity(); cameraInstance.translate(0.0f, 0.0f, 5.0f); }
		// if I don't return reerence then I can't see anything because it is not modified that way
		mat4t& getCamera () { return cameraInstance; }
		float& getCameraAngle () { return angle; }
		void setCameraAngle ( float _angle ) { angle = _angle; }
		void translateLeft () { cameraInstance.translate(-1.0f/MAX_MOVE, 0.0f, 0.0f); }
		void translateRight () { cameraInstance.translate( 1.0f/MAX_MOVE, 0.0f, 0.0f); }
		void translateForward () { cameraInstance.translate( 0.0f, 0.0f, -1.0f/MAX_MOVE); }
		void translateBack () { cameraInstance.translate( 0.0f, 0.0f, 1.0f/MAX_MOVE); }
		void translateUp ( float t ) { cameraInstance.translate( 0.0f, t, 0.0f); }
		void rotateLeft () { 
			cameraInstance.rotateX( -angleX );
			cameraInstance.rotateY( MAX_ROTATION );
			cameraInstance.rotateX( angleX );
		}
		void rotateRight () { 
			cameraInstance.rotateX( -angleX );
			cameraInstance.rotateY( -MAX_ROTATION );
			cameraInstance.rotateX( angleX );
		}
		void rotateUp () { 
			angleX += MAX_ROTATION;
			cameraInstance.rotateX( MAX_ROTATION ); 
		}
		void rotateDown () { 
			angleX -= MAX_ROTATION;
			cameraInstance.rotateX( -MAX_ROTATION );
		}
		int& getMouseX () { return  mouseX; }
		int& getMouseY () { return  mouseY; }
		void savePreviousMousePos () { preMouseX = mouseX; preMouseY = mouseY; }
		void detectMouse () {
			if ( preMouseX < mouseX )
				rotateRight();
			else if ( preMouseX > mouseX )
				rotateLeft();
			if ( preMouseY < mouseY )
				rotateDown();
			else if ( preMouseY > mouseY )
				rotateUp();
			savePreviousMousePos();
		}
		void reset () {
			cameraInstance.loadIdentity();
			cameraInstance.translate(0.0f, 0.0f, 5.0f);
		}
	};

	// READ FILE
  class L_Systems  {
    std::FILE *file;
    string axiom;
	string constants;
	int iterations;
	float angle;
	dynarray<char> var;
	dynarray<string> rule;
	dynarray<int> positions;
	dynarray<float> probabilities;

	enum { MAX_LINE = 30 };

  public:
    L_Systems () {	}

	void openFiles ( const char *filename) {

		file = fopen(filename, "r"); 
	    if ( file != NULL ) {
		  printf("file opened\n");
		  readData(file);
		  fclose(file);
	    }
	    else
		  printf("file error\n");
	}
	
	void readData ( FILE *in ) {
		
		static char stackBuf[MAX_LINE];
		char *buf = &(stackBuf[0]); 
		while ( fgets( buf, MAX_LINE, in) != NULL ) {
			if ( strncmp(buf, "axiom", 5) == 0) {
				axiom = fgets( buf, MAX_LINE, in); 
			}
			else if ( strncmp(buf, "constants", 9) == 0) {
				constants = fgets( buf, MAX_LINE, in);
			}
			else if ( strncmp(buf, "variables", 9) == 0) {
				fgets( buf, MAX_LINE, in);
				var.push_back(buf[0]);  
			}
			else if ( strncmp(buf, "rules", 5) == 0) {
				rule.push_back(fgets( buf, MAX_LINE, in));
			}
			else if ( strncmp(buf, "iterations", 10) == 0) {
				iterations = atoi(fgets( buf, MAX_LINE, in));
			}
			else if ( strncmp(buf, "angle", 5) == 0) {
				angle = (float)atof(fgets( buf, MAX_LINE, in));
			}
			else if (strncmp(buf, "positions", 8) == 0){
				positions.push_back( atoi(fgets( buf, MAX_LINE, in)) );
				positions.push_back( atoi(fgets( buf, MAX_LINE, in)) );
			}
			else if ( strncmp(buf, "probability", 5) == 0) {
				probabilities.push_back((float)atof(fgets( buf, MAX_LINE, in)));
			}
		}
	}


	void reset () {		
		var.reset();
		rule.reset();
		positions.reset();
		probabilities.reset();
	}

	string selectRule ( char c, int random ) {
		float temp = 0.0f;

		for ( unsigned int i = 0; i < var.size(); i++ ){
			if (c == var[i]){
				// check the probability value in that position ( i )
				if ( i < probabilities.size() ) {
					temp += probabilities[i]*100.0f;
					if ( random <= temp)
						return rule[i];
				}
			}
				
		}
		// if no rule was selected by probabilities then it means that this variable has only one rule 
		for ( unsigned int i = 0; i < var.size(); i++ ) {
			if (c == var[i])
				return rule[i];
		}
		return "";
	}

	bool isStohastic () {
		char temp = var[0];
		for ( unsigned int i = 0; i < var.size()-1; i++) {
			for( unsigned int j = i; j < var.size()-1; j++) {
				if ( var[i] == var[j+1] )
					return true;
			}
		}
		return false;
	}

	int getPosition ( int i ) { return positions[i]; }
	string getAxiom () { return axiom; }
	float getAngle () { return angle; }
	string getConstants () { return constants; }
	char getVariables ( int temp ) { return var[temp]; }
	string getRules ( int temp ) { return rule[temp]; }
	int sizeOfVariables () { return var.size(); }
	int getIterations () { return iterations; }
	int& setIterations ( ) { return iterations; }
	float& setAngle () { return angle; }
	bool existInVariables ( char temp ) {
		for ( int j = 0; j < sizeOfVariables(); j++){
			if ( temp == getVariables(j))
				return 1;
		}
		return 0;
	}
  };

	// mesh holds indices, vertices, uv coordinates
	class Mesh {
		dynarray<float> vertices;
		dynarray<GLuint> indices;
		dynarray<float> uvcoord;
		int index_number;
		GLuint vertexbuffer;
		GLuint indexbuffer;
		GLuint uvbuffer;
	public :
    Mesh ():index_number(0) { }

		void init () {
			index_number = 0;
		}

		void reset () {
			index_number = 0;
			vertices.reset();
			indices.reset();
			uvcoord.reset();
		}

		void pushVertex ( vec3 &vector) {
			vertices.push_back(vector.x());
			vertices.push_back(vector.y());
			vertices.push_back(vector.z());
		}

		void pushIndex ( ) {
			indices.push_back( index_number );
			index_number++;
		}

		void pushIndex ( int n ) {
			indices.push_back( n );
		}
		
		void pushUV ( float u_, float v_ ) { 
			uvcoord.push_back( u_ ); 
			uvcoord.push_back( v_ );
		}

		dynarray<float> &getVertices () { return vertices; }

		dynarray<GLuint> &getIndices () { return indices; }

		//void increaceIndexNumber () { index_number++; }

		int &getIndexNumber () { return index_number; }

		void setBuffers () {
			glEnableVertexAttribArray(attribute_pos);
		    glGenBuffers(1, &vertexbuffer);			
		    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_STATIC_DRAW);  // 3 korifes * sizeof(float)
			glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

			// http://stackoverflow.com/questions/15041120/crash-at-draw-call-in-nvoglv32-dll-on-new-video-card
			if(!uvcoord.is_empty()){
			glEnableVertexAttribArray(attribute_uv);
			glGenBuffers(1, &uvbuffer);	
			glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
			glBufferData(GL_ARRAY_BUFFER, uvcoord.size()*sizeof(float), &uvcoord[0], GL_STATIC_DRAW);  // 3 korifes * sizeof(float)
			glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
			}

		    //glEnableVertexAttribArray(attribute_pos);
			//glVertexPointer(3, GL_FLOAT, 0, 0); //sizeof(GLfloat)*3
			glGenBuffers(1, &indexbuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
		}

		void render() {		
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);		
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
		}
		
	};

	class HandleMesh {
		mat4t modelToWorld;
		float halfWidth;
	  float halfHeight;
		vec4 color;
		Mesh mesh_;
		dynarray<mat4t> modelToWorldMatrices;
    GLuint texture_handle_;         // this is an OpenGL texture handle which is used to draw the image.
		color_shader color_shader_;
		texture_shader texture_shader_;

    // push vertices and indices that form a cube into the mesh
		void pushIntoMesh ( vec4 &v1, vec4 &v2, vec4 &v3, vec4 &v4, vec4 &v5, vec4 &v6, vec4 &v7, vec4 &v8) {
			// push front face vertices into the mesh			
			mesh_.pushVertex(vec3 (v4.x(), v4.y(), v4.z()));
			mesh_.pushVertex(vec3 (v3.x(), v3.y(), v3.z()));
			mesh_.pushVertex(vec3 (v1.x(), v1.y(), v1.z()));
			mesh_.pushVertex(vec3 (v2.x(), v2.y(), v2.z()));
			// push indices for the front face
			pushIndexForQuad();

			// right face
			mesh_.pushVertex(vec3 (v3.x(), v3.y(), v3.z()));
			mesh_.pushVertex(vec3(v6.x(), v6.y(), v6.z()));
			mesh_.pushVertex(vec3(v2.x(), v2.y(), v2.z()));
			mesh_.pushVertex(vec3(v5.x(), v5.y(), v5.z()));

			pushIndexForQuad();	

			// back face
			// maybe not in the right order!!!!!		
			mesh_.pushVertex(vec3(v6.x(), v6.y(), v6.z()));
			mesh_.pushVertex(vec3(v8.x(), v8.y(), v8.z()));
			mesh_.pushVertex(vec3(v5.x(), v5.y(), v5.z()));
			mesh_.pushVertex(vec3(v7.x(), v7.y(), v7.z()));

			pushIndexForQuad();	

			// left face	
			mesh_.pushVertex(vec3(v8.x(), v8.y(), v8.z()));
			mesh_.pushVertex(vec3 (v4.x(), v4.y(), v4.z()));
			mesh_.pushVertex(vec3(v7.x(), v7.y(), v7.z()));
			mesh_.pushVertex(vec3 (v1.x(), v1.y(), v1.z()));

			pushIndexForQuad();

			// up face	
			mesh_.pushVertex(vec3(v8.x(), v8.y(), v8.z()));
			mesh_.pushVertex(vec3 (v6.x(), v6.y(), v6.z()));
			mesh_.pushVertex(vec3(v4.x(), v4.y(), v4.z()));
			mesh_.pushVertex(vec3 (v3.x(), v3.y(), v3.z()));

			pushIndexForQuad();
			
			// bottom face
			// maybe its clockwise instead of counterclockwise
			mesh_.pushVertex(vec3(v7.x(), v7.y(), v7.z()));
			mesh_.pushVertex(vec3 (v5.x(), v5.y(), v5.z()));
			mesh_.pushVertex(vec3(v1.x(), v1.y(), v1.z()));
			mesh_.pushVertex(vec3 (v2.x(), v2.y(), v2.z()));

			pushIndexForQuad();
		}

    void pushIndexForQuad() {
			mesh_.pushIndex();
			mesh_.pushIndex();
			mesh_.pushIndex();

			mesh_.pushIndex(mesh_.getIndexNumber()-2);
			mesh_.pushIndex();
			mesh_.pushIndex(mesh_.getIndexNumber()-2);
		}

	public:
		HandleMesh () { }

		void init() {
		  mesh_.init();
		}

		float getHeight() { return halfHeight*2; }
		void rotateY ( float angle ) { modelToWorld.rotateY(angle); }
		void rotateX ( float angle ) { modelToWorld.rotateX(angle); }
		void translate(float x, float y, float z) { modelToWorld.translate(x, y, z); }
		void rotateZ(float angle) { modelToWorld.rotateZ(angle); }

		// to load new tree we need the world to be the identity matrix and we translate it in the right position (x,y,z)
		// before we add the vertices into the mesh
		void loadNewObject (float x, float y, float z, float w, float h) {
			modelToWorld.loadIdentity();
		    modelToWorld.translate(x, y, z);
		    halfWidth = w*0.5f;
		    halfHeight = h*0.5f;
		    color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}

		mat4t &getLastModelToWorldmatrix () { return modelToWorldMatrices.back(); }
		mat4t &getModelToWorldMatrix () { return modelToWorld; };

		void pushVertex (vec3 &v) {
			mesh_.pushVertex(v);
		}

    void pushIndex () {
      mesh_.pushIndex();
    }

		void pushUV ( float u, float v ) { mesh_.pushUV( u , v ); }

		// create a cube for each branch. cube has 8 corners/vertices
		void puchCube () {
			float n = 1.0f/4.0f;  // 4 is the number of photos in .gif
      // 8 vertices that form a cube
			vec4 v1 =  vec4 ( -halfWidth, 0.0f, 0.0f, 1.0f) * modelToWorld;
			vec4 v2 = vec4 (  halfWidth, 0.0f, 0.0f, 1.0f) * modelToWorld;
			vec4 v3 =  vec4 (  halfWidth, 2*halfHeight, 0.0f, 1.0f ) * modelToWorld;
			vec4 v4 =  vec4 ( -halfWidth, 2*halfHeight, 0.0f, 1.0f) * modelToWorld;
			vec4 v5 = vec4 ( halfWidth, 0.0f, -2*halfWidth, 1.0f) * modelToWorld;
			vec4 v6 = vec4 ( halfWidth, 2*halfHeight, -2*halfWidth, 1.0f) * modelToWorld;
			vec4 v7 = vec4 ( -halfWidth, 0.0f, -2*halfWidth, 1.0f) * modelToWorld;
			vec4 v8 = vec4 ( -halfWidth, 2*halfHeight, -2*halfWidth, 1.0f) * modelToWorld;
			
			// push vertices and indices into the mesh according to the 8 vertices
			pushIntoMesh( v1, v2, v3, v4, v5, v6, v7, v8);

			// 6 calls, each for each face of cube
			for ( int i = 0; i < 6; i++) {
				mesh_.pushUV( 0.0f, 0.0f);
				mesh_.pushUV( n, 0.0f);
				mesh_.pushUV( n, 1.0f);
				mesh_.pushUV( 0.0f, 1.0f);
			}
			
		}
		

		// create leaf shapes
		void pushLeafs () {
			float n = 1.0f/4.0f;  // 4 is the number of photos in .gif
			//modelToWorld.rotateZ(90.0f);
			vec4 v1 =  vec4 ( 0.0f, 0.0f, 0.0f, 1.0f) * modelToWorld;
			vec4 v2 = vec4 (  halfHeight/2, halfHeight/2.0f, 0.0f, 1.0f) * modelToWorld;
			vec4 v3 =  vec4 (  0.0f, halfHeight, 0.0f, 1.0f ) * modelToWorld;
			vec4 v4 =  vec4 ( -halfHeight/2, halfHeight/2.0f, 0.0f, 1.0f) * modelToWorld;
			vec4 v5 = vec4 ( halfHeight/2, halfHeight/2.0f, -2*halfWidth, 1.0f) * modelToWorld;
			vec4 v6 = vec4 ( 0.0f, halfHeight, -2*halfWidth, 1.0f) * modelToWorld;
			vec4 v7 = vec4 ( 0.0f, 0.0f, -2*halfWidth, 1.0f) * modelToWorld;
			vec4 v8 = vec4 ( -halfHeight/2, halfHeight/2.0f, -2*halfWidth, 1.0f) * modelToWorld;
			//modelToWorld.rotateZ(-90.0f);
			pushIntoMesh( v1, v2, v3, v4, v5, v6, v7, v8);

			// 6 calls, each for each face of cube
			for ( int i = 0; i < 6; i++) {
				mesh_.pushUV( n, 0.0f);
				mesh_.pushUV( n + n, 0.0f);
				mesh_.pushUV( n + n, 1.0f);
				mesh_.pushUV( n, 1.0f);
			}
			/*
			// 6 calls, each for each face of cube
			mesh_.pushUVCoordinates2();
			mesh_.pushUVCoordinates2();
			mesh_.pushUVCoordinates2();
			mesh_.pushUVCoordinates2();
			mesh_.pushUVCoordinates2();
			mesh_.pushUVCoordinates2();
			*/

		}

		void meshReady () { mesh_.setBuffers(); }		

		void pushModelMatrix () { modelToWorldMatrices.push_back(modelToWorld); }
		void popModelMatrix () { modelToWorldMatrices.pop_back(); }
		//dynarray<mat4t> &getModelToWorldMatrices () { return modelToWorldMatrices; }
		void setIdentity(mat4t &matrix) { matrix.loadIdentity(); }

		// render mesh
		void render(color_shader &shader, mat4t &cameraToWorld) { 
		  modelToWorld.loadIdentity();
		 // modelToWorld.rotateX(45.0f);
		  mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

		  shader.render(modelToProjection, color);
		  mesh_.render();
		}

		// render mesh with texture shader
		void render(texture_shader &shader, mat4t &cameraToWorld) { 
		  modelToWorld.loadIdentity();
		 // modelToWorld.rotateX(45.0f);
		  mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);
		  texture_handle_ = resources::get_texture_handle(GL_RGB, "assets/terrain2.gif");  
		  glActiveTexture(GL_TEXTURE0);
		  glBindTexture(GL_TEXTURE_2D, texture_handle_);
		  // http://www.codeproject.com/Articles/236394/Bi-Cubic-and-Bi-Linear-Interpolation-with-GLSL
		  //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		  shader.render(modelToProjection, 0);  		
		  mesh_.render();
		}
		
		void reset () {
			
			modelToWorld.loadIdentity();
			modelToWorldMatrices.reset();
			mesh_.reset();
		}
	};
  
  class MeshEngine{
	  L_Systems file;
	  HandleMesh *mesh;

	  // this array consists the number of trees that should be generated of each species of tree
	  dynarray<int> numberOfSpecies;
	  dynarray<string> trees; // type of trees
	  string finalAxiom;
	  time_t timer;
	  enum Type { BUSH, LEAFTREE, BRANCHTREE, A } treeType;

    // create the sequence that will be rendered
	  void createSequence () {
		  string axiom = file.getAxiom();
		  string seq;
		  for ( int i=0; i<file.getIterations(); i++) {
			  for ( unsigned int j=0; j<strlen(axiom); j++) {
				  seq += ManageSequenceRules(axiom[j]); //tree.getRules();
			  }
			  axiom = seq;
			  seq = "";
		  }
		  if (file.getIterations() <= 0)
			  axiom = "";
		  finalAxiom = axiom;
		  printf("Final axiom is %s\n", finalAxiom);
	  }

    // each type of tree reads a different file
    void readFile() {
		  file.reset();
		  if (treeType == BUSH)
			  file.openFiles(app_utils::get_path("assets/source1.txt")); 
		  else if (treeType == BRANCHTREE)
			  file.openFiles(app_utils::get_path("assets/source2.txt")); 
		  else if (treeType == LEAFTREE)
			  file.openFiles(app_utils::get_path("assets/source3.txt")); 
		  else if (treeType == A)
			  file.openFiles(app_utils::get_path("assets/source4.txt")); 
    }

    // each input is translated to a type of tree
    void translateTrees ( int i ) {
		  if ( trees[i] == "mountain")
			  treeType = BUSH;
		  else if ( trees[i] == "dessert")
			  treeType = BRANCHTREE;
		  else if ( trees[i] == "river")
			  treeType = LEAFTREE;
		  else if ( trees[i] == "coast")
			  treeType = A;
	  }

    void createObjects () {
		  translateTrees(0);
		  readFile();
		  createSequence ();
		  for( unsigned int j = 0; j < numberOfSpecies.size(); j++) { // for every input in numberOfSpecies array which actuall says how many differenst species of trees we have
			  for ( unsigned int i = 0; i < numberOfSpecies[j]; i++){ // number of trees of that species
				  mesh->loadNewObject(file.getPosition(0) + i*20.0f, 0.0f, file.getPosition(0) + i*20.0f, 0.5f, 5.5f );
				  manageRendering(finalAxiom);

          if ( file.isStohastic() ) {
					  readFile();
					  createSequence ();
				  }
			  }
			  if ( j < numberOfSpecies.size() - 1 ) {
				  translateTrees(j+1);
				  readFile();
				  createSequence ();
			  }
		  }
		  //mesh.meshReady();
	  }
    // create the rendered tree
    void manageRendering ( string axiom ) {
			for ( int i = 0; i < axiom.size(); i++){
				if ( file.existInVariables(axiom[i]) ) {  
					if ( axiom[i] == 'F' ) {
						mesh->puchCube();
						mesh->translate( 0.0f, mesh->getHeight(), 0.0f );					
					}
					else if (axiom[i] == 'X' ) {
            mesh->pushLeafs();
						//mesh.translate( 0.0f, mesh.getHeight() / 2.0f, 0.0f );
					}	
				}
				else if ( axiom[i] == '+' ) {
					mesh->rotateZ( file.getAngle());
				}
				else if ( axiom[i] == '-' ) {
					mesh->rotateZ( -file.getAngle());
				}
				else if ( axiom[i] == '[' ) {
					mesh->pushModelMatrix();
				}
				else if ( axiom[i] == ']' ) {
					mesh->getModelToWorldMatrix() = mesh->getLastModelToWorldmatrix();
					mesh->popModelMatrix();
				}
				else if ( axiom[i] == '^' ) {
					mesh->rotateY( -file.getAngle() );
				}
				else if ( axiom[i] == '&' ) {
					mesh->rotateY( file.getAngle() );
				}
				else if ( axiom[i] == '<' ) {
					mesh->rotateX( file.getAngle() );
				}
				else if ( axiom[i] == '>' ) {
					mesh->rotateX( -file.getAngle() );
				}
				else if ( axiom[i] == '|' ) {
					mesh->rotateX( 180.0f );
				}
			}
	}

	  string chooseRule( int i, char c ) {
		  int temp = rand()%100;
		  if ( !file.isStohastic() ) // if not stochastic
			  return file.getRules(i);
		  else {                      // if it is stochastic
			  return file.selectRule(c, temp); // select rule depending on propabilities
		  }
		  
	  }

	  // replace its variable with the right rule
	  string ManageSequenceRules ( char temp ) {
		  string s;
		  s = &temp;
		  for ( int i = 0; i < file.sizeOfVariables(); i++) {
			
			  if ( temp == file.getVariables(i)) //'F'
				  return chooseRule( i, temp );//file.getRules(i);// edw parapioisi
		  }
	      if (temp == '\n')
			  return "";
		  else		
			  return s;		
	  }
  public:
	  MeshEngine () { }

	  void init ( dynarray<int> &num, dynarray<string> &types, HandleMesh &mesh_ ) {
		  mesh = &mesh_;
		  //mesh.init();
		  numberOfSpecies = num;
		  trees = types;
		  srand(time(NULL));
		  createObjects();		  
	  }

	  void render ( texture_shader &shader, mat4t &cameraToWorld) {
		  mesh->render(shader, cameraToWorld);
	  }

  };


 
  class Lsystems_app : public octet::app {
	camera cameraToWorld;
	MeshEngine tree;	
	HandleMesh mesh_;
	dynarray<int> treenums;
	dynarray<string> landtypes;
	color_shader color_shader_;
	texture_shader texture_shader_;
 
  public:

    Lsystems_app(int argc, char **argv) : app(argc, argv) {
    }

    void app_init() {
		  color_shader_.init();
		  texture_shader_.init();
		  cameraToWorld.translateUp(10.0f);

		  setValues(); // set number of trees for each species
		  tree.init(treenums, landtypes, mesh_);
		 
		  mesh_.meshReady(); // this SHOULD be calded after mesh is ready with all vertices in it
    }	

	// how many trees of each file
	void setValues () {
		treenums.push_back(3);
		landtypes.push_back("mountain");
		//treenums.push_back(3);
		//landtypes.push_back("dessert");
		//treenums.push_back(1);
		//landtypes.push_back("river");
		//treenums.push_back(1);
		//landtypes.push_back("coast");

	}
	
	void handleKeyStrokes () {
		
		// translate camera left
		if (is_key_down('A')) {
			cameraToWorld.translateLeft();
		}
		// translate camera right
		else if (is_key_down('D')) {
			cameraToWorld.translateRight();
		}
		// translate camera forward
		else if (is_key_down('W')) {
			cameraToWorld.translateForward();
		}
		// translate camra backwards
		else if (is_key_down('S')) {
			cameraToWorld.translateBack();
		}
		else if (is_key_down(key_up)) {
			cameraToWorld.rotateUp();
		}
		else if (is_key_down(key_down)) {
			cameraToWorld.rotateDown();
		}
		else if (is_key_down(key_left)) {
			cameraToWorld.rotateLeft();
		}
		else if (is_key_down(key_right)) {
			cameraToWorld.rotateRight();
		}
	}

    void draw_world(int x, int y, int w, int h) {
    
      glViewport(x, y, w, h);
	    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
      mesh_.render( texture_shader_, cameraToWorld.getCamera() );

	  handleKeyStrokes ();	  
	  //get_mouse_pos(cameraToWorld.getMouseX(), cameraToWorld.getMouseY());
	  //cameraToWorld.detectMouse();
    }
  };
}
