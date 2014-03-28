
namespace octet {

  class Camera {
		mat4t cameraInstance;
		float angle, angleX;
		int preMouseX, preMouseY;
		int mouseX, mouseY;
		enum { MAX_MOVE = 2, MAX_ROTATION = 5 };

	public:
		Camera () { angle = 0.0f; angleX = 0.0f; cameraInstance.loadIdentity(); cameraInstance.translate(0.0f, 25.0f, 50.0f); }
	
		mat4t& getCamera () { return cameraInstance; }
		float& getCameraAngle () { return angle; }
		void setCameraAngle ( float _angle ) { angle = _angle; }
		void translateLeft () { cameraInstance.translate(-1.0f/MAX_MOVE, 0.0f, 0.0f); }
		void translateRight () { cameraInstance.translate( 1.0f/MAX_MOVE, 0.0f, 0.0f); }
		void translateForward () { cameraInstance.translate( 0.0f, 0.0f, -1.0f/MAX_MOVE); }
		void translateBack () { cameraInstance.translate( 0.0f, 0.0f, 1.0f/MAX_MOVE); }
		void translateUp ( float t ) { cameraInstance.translate( 0.0f, t, 0.0f); }
    void translateDown ( float t ) { cameraInstance.translate( 0.0f, -t, 0.0f); }
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
			cameraInstance.translate(0.0f, 25.0f, 50.0f);
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

		void pushVertex ( const vec3 &vector) {
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

    void pushUV ( ) { 
			uvcoord.push_back( 0.0f ); 
			uvcoord.push_back( 0.0f );

      uvcoord.push_back( 1.0f ); 
			uvcoord.push_back( 0.0f );

      uvcoord.push_back( 1.0f ); 
			uvcoord.push_back( 1.0f );

      uvcoord.push_back( 0.0f ); 
			uvcoord.push_back( 1.0f );
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

  // creates rooms and corridors
  class Polygons {
    Mesh mesh;
  
  public:
    Polygons () {  }

    void add ( const vec3 &vector ) {
      mesh.pushVertex( vector );
      mesh.pushIndex();
    }

    // v1 is in front face left bottom vertex and v2 is front face right bottom vertex
    void createHorrizontalCorridor ( const vec3 &vector1, const vec3 &vector2 ) {
      vec3 v1 = vector1;
      vec3 v2 = vector2;
      vec3 v3 = vec3(vector2.x(), vector2.y()+1.0f, vector2.z());
      vec3 v4 = vec3(vector1.x(), vector1.y()+1.0f, vector1.z());
      vec3 v5 = vec3(vector2.x(), vector2.y(), vector2.z()+1.0f); // +b or -b???
      vec3 v6 = vec3(vector2.x(), vector2.y()+1.0f, vector2.z()+1.0f);
      vec3 v7 = vec3(vector1.x(), vector1.y(), vector1.z()+1.0f);
      vec3 v8 = vec3(vector1.x(), vector1.y()+1.0f, vector1.z()+1.0f);

      cube ( v1, v2, v3, v4, v5, v6, v7, v8 );
    }
    // v1 is front face bottom left vertex and v2 is front face right up vertex
    void createVerticalCorridor ( const vec3 &vector1, const vec3 &vector2 ) {
      vec3 v1 = vector1;
      vec3 v2 = vec3(vector1.x()+1.0f, vector1.y(), vector1.z());
      vec3 v3 = vector2;
      vec3 v4 = vec3(vector2.x()-1.0f, vector2.y(), vector2.z());
      vec3 v5 = vec3(vector1.x()+1.0f, vector1.y(), vector1.z()+1.0f);
      vec3 v6 = vec3(vector2.x(), vector2.y(), vector2.z()+1.0f);
      vec3 v7 = vec3(vector1.x(), vector1.y(), vector1.z()+1.0f);
      vec3 v8 = vec3(vector2.x()-1.0f, vector2.y(), vector2.z()+1.0f);

      cube ( v1, v2, v3, v4, v5, v6, v7, v8 );
    }

    void cube ( vec3 &v1, vec3 &v2, vec3 &v3, vec3 &v4, vec3 &v5, vec3 &v6, vec3 &v7, vec3 &v8 ) {
      // front face
      add ( v1 );
      add ( v2 );
      add ( v3 );
      add ( v1 );
      add ( v3 );
      add ( v4 );
      // right face
      add ( v2 );
      add ( v5 );
      add ( v6 );
      add ( v2 );
      add ( v6 );
      add ( v3 );
      // back face
      add ( v5 );
      add ( v7 );
      add ( v8 );
      add ( v5 );
      add ( v8 );
      add ( v6 );
      // left face
      add ( v7 );
      add ( v1 );
      add ( v4 );
      add ( v7 );
      add ( v4 );
      add ( v8 );
      // top face
      add ( v4 );
      add ( v3 );
      add ( v6 );
      add ( v4 );
      add ( v6 );
      add ( v8 );
      // bottom face
      add ( v1 );
      add ( v2 );
      add ( v5 );
      add ( v1 );
      add ( v5 );
      add ( v7 );
    }

    // create a cube
    void createRoom ( const vec3 &vector1, const vec3 &vector2 ) {
      vec3 v1 = vector1;
      vec3 v2 = vec3( vector2.x(), vector1.y(), vector1.z() );
      vec3 v3 = vec3( vector2.x(), vector2.y(), vector1.z() );
      vec3 v4 = vec3( vector1.x(), vector2.y(), vector1.z() );
      vec3 v5 = vec3( vector2.x(), vector1.y(), vector2.z() );
      vec3 v6 = vector2;
      vec3 v7 = vec3( vector1.x(), vector1.y(), vector2.z() );
      vec3 v8 = vec3( vector1.x(), vector2.y(), vector2.z() );
      
      cube ( v1, v2, v3, v4, v5, v6, v7, v8 );
    }

    void meshReady () { mesh.setBuffers(); }

    void reset () { mesh.reset(); }

    // render mesh
		void render(color_shader &shader, mat4t &cameraToWorld) { 
      mat4t modelToWorld;
		  modelToWorld.loadIdentity();
		  mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

		  shader.render(modelToProjection, vec4(1.0f, 0.0f, 0.0f, 1.0f));
		  mesh.render();
    }

    void render(stripes_shader &shader, mat4t &cameraToWorld) { 
      mat4t modelToWorld;
		  modelToWorld.loadIdentity();
		  mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

		  shader.render(modelToProjection, vec4(1.0f, 0.0f, 0.0f, 1.0f));
		  mesh.render();
    }
  };

  struct Node {
      vec3 corner1;
      vec3 corner2;
      Node *left;
      Node *right;
      Node *parent;
      Node ():left(NULL),right(NULL),parent(NULL) {};
      Node ( const vec3 &_corner1, const vec3 &_corner2 ): corner1(_corner1), corner2(_corner2), left(NULL), right(NULL),parent(NULL) { };
      void setLeft ( Node *next ) { left = next; };
      void setRight ( Node *next ) { right = next; };
      void setParent ( Node *parent_ ) { parent = parent_; };
      // returns the min distance between two nodes
      float minDistance ( Node * node ) {
        float min = 0.0f;
        if ( abs( this->corner1.x() -  node->corner1.x()) < min ) 
            min = abs( this->corner1.x() -  node->corner1.x());
        else if ( abs( this->corner1.x() -  node->corner2.x()) < min ) 
          min = abs( this->corner1.x() -  node->corner2.x());
        else if ( abs( this->corner2.x() -  node->corner1.x()) < min )
          min = abs( this->corner2.x() -  node->corner1.x());
        else if ( abs( this->corner2.x() -  node->corner2.x()) < min ) 
          min = abs( this->corner2.x() -  node->corner2.x());
        return min;
      };
    };

  // hold all rooms in a binary tree
  class Tree {
    Node *head; 
    dynarray<Node*> nodes;

    bool isLeaf ( Node *node ) {
      if ( node->left == NULL && node->right == NULL )
        return true;
      else
        return false;
    }
    // keep nodes of the same level to an array of pointers
    void addNodeToPointer ( Node *node ) {
      nodes.push_back(node);
    }

    // returns the height of tree
    int maxHeight ( Node *root ) {
      if ( root == NULL )
        return 0;
      else {
        int ldepth = maxHeight( root->left );
        int rdepth = maxHeight( root->right );

        // use the larger one
        if ( ldepth > rdepth )
          return (ldepth+1);
        else
          return (rdepth+1);
      }
    }

    // find nodes in a specific height
    void findLeaves ( int height, int currHeight, Node *root ) {
      if ( currHeight == height ) {
        addNodeToPointer ( root );
        //printf ("leaf %d \n", root->x);
      }    
      if ( root->left != NULL && currHeight != height )
        findLeaves ( height, currHeight+1, root->left );
      if ( root->right != NULL && currHeight != height )
        findLeaves ( height, currHeight+1, root->right );
      if ( isLeaf(root) && height != currHeight )
        return;
    }

    void deleteNode ( Node *node ) {
      if ( node-> left != NULL )
        deleteNode( node->left);
      if ( node-> right != NULL )
        deleteNode( node->right );
      if ( node->left == NULL && node->right == NULL ) {
        if ( node->parent != NULL ) {
          if ( node->parent->left == node ) 
            node->parent->left = NULL;
          else
            node->parent->right = NULL;
        }
        delete ( node );
        node = 0;       
      }
  }
    // return the number of leafs of the hole tree
    int numberOfleafs () {
      findLeaves( height(), 1, head );
      return nodes.size();
    }

  bool isParent ( Node *node, Node *parent ) {
    Node *temp;
    for ( temp = node; temp->parent != NULL; temp = temp->parent ) {
      if ( temp->parent == parent )
        return true;
    }
    return false;
  }

  public:
    Tree ():head(NULL){  }

    void setSpace ( vec3 &corner1, vec3 &corner2 ) {
      head = new Node ( corner1, corner2 );
    }

    void insert ( Node *parent_node, Node *child_node ) {
      if ( parent_node->left == NULL )   
        parent_node->setLeft ( child_node );        
      else 
        parent_node->setRight ( child_node );
      child_node->parent = parent_node;
    }

    int height () { return maxHeight(head); }

    dynarray<Node*> nodesAtLevel ( int level ) {
      nodes.reset();
      findLeaves ( level, 1, head);
      return nodes;
    }

    Node* getleafOfNode ( Node *node ) {
      Node* temp;
      for ( temp = node; isLeaf(temp) == false; temp=temp->left ) { }
      return temp;
    }

    Node* getHead() { return head; }

    void reset () { deleteNode(head); }
  };

  class BSR {
    Tree tree;
    float threshold;
    Polygons polygon;
    int counter;
  public:
    BSR (): threshold( 20.0f ), counter(0) {}

    void setSpace ( vec3 &corner1, vec3 &corner2 ) { 
      tree.setSpace( corner1, corner2 ); 
    }

    void BSRAlgorithm () {
      divide( tree.getHead() );
      findLeaves ( tree.getHead() );
      printf("Total %d\n", counter );
      connectNodes ();
      polygon.meshReady();
      
    }
    // choose a corridor at the right side depending on the relative position between nodes
    void corridor ( Node *node1, Node *node2 ) {
      vec3 v1, v2;      
      vec2 temp, posX;
      // if node 1 at left side of node2
      if ( (node1->corner2.x() <= node2->corner1.x()) && (node1->corner1.x() <= node2->corner1.x()) ) {
        // find a corner in each node with the min distance in z axis
        // v1 is front face left bottom vertex
        v1 = vec3 ( node1->corner2.x(), node1->corner1.y(), (node1->corner1.z()+node1->corner2.z())/2.0f ); 
        //v2 is front face right bottom vertex
        v2 = vec3 ( node2->corner1.x(), node2->corner1.y(), (node2->corner1.z()+ node2->corner2.z())/2.0f );
        polygon.createHorrizontalCorridor( v1, v2 );
      }
      // if node1 is at the right side of node2
      else if ( (node1->corner1.x() >= node2->corner2.x()) && (node1->corner1.x() >= node2->corner1.x()) ) {
        v1 = vec3 ( node2->corner2.x(),node2->corner1.y(), (node2->corner1.z()+ node2->corner2.z())/2.0f );
        v2 = vec3 ( node1->corner1.x(), node1->corner1.y(), (node1->corner1.z()+node1->corner2.z())/2.0f );
        polygon.createHorrizontalCorridor( v1, v2 );
      }
      // if node1 is on top of node2
      else if ( (node2->corner2.y() <= node1->corner1.y() &&  node2->corner2.y() <= node1->corner2.y()) ) {
        v1 = vec3( (node2->corner1.x()+node2->corner2.x())/2.0f, node2->corner2.y(), (node2->corner1.z()+ node2->corner2.z())/2.0f );
        v2 = vec3( (node1->corner1.x()+node1->corner2.x())/2.0f, node1->corner1.y(), (node1->corner1.z()+node1->corner2.z())/2.0f );
        polygon.createVerticalCorridor(v1,v2);
      }
      // if node1 is down node2
      else if ( node2->corner1.y() >= node1->corner1.y() &&  node2->corner1.y() >= node1->corner2.y() ) {
        v1 = vec3 ( (node1->corner1.x()+node1->corner2.x())/2.0f, node1->corner2.y(), (node1->corner1.z()+node1->corner2.z())/2.0f );
        v2 = vec3 ( (node2->corner1.x()+node2->corner2.x())/2.0f, node2->corner1.y(), (node2->corner1.z()+ node2->corner2.z())/2.0f );
        polygon.createVerticalCorridor(v1,v2);
      }
      // if node1 is in front of node2
      else if ( node1->corner1.z() >= node2->corner1.z() && node1->corner1.z() >= node2->corner2.z() ) {
        v1 = vec3( (node1->corner1.x()+node1->corner2.x())/2.0f, node1->corner1.y(), (node1->corner1.z()+node1->corner2.z())/2.0f );
        v2 = vec3 ( (node2->corner1.x()+node2->corner2.x())/2.0f, node2->corner1.y(), (node2->corner1.z()+ node2->corner2.z())/2.0f );
        polygon.createVerticalCorridor(v1,v2);
      }
      // if node1 is back of node2
      else if ( node2->corner1.z() >= node1->corner2.z() && node2->corner1.z() >= node1->corner1.z() ) {
        v1 = vec3 ( (node2->corner1.x()+node2->corner2.x())/2.0f, node2->corner1.y(), (node2->corner1.z()+ node2->corner2.z())/2.0f );
        v2 = vec3 ( (node1->corner1.x()+node1->corner2.x())/2.0f, node1->corner1.y(), (node1->corner1.z()+node1->corner2.z())/2.0f );
        polygon.createVerticalCorridor(v1,v2);
      }
      else {
        printf("NO CORRIDOR CONTITION MATCHES\n");
      }
    }

    // create corridors by connecting children of the same parent
    // when not leaves then I have to pass a children/leaf of each to connect 
    void connectNodes () {
      int h = tree.height();
      // get nodes in the same level of the tree
      dynarray<Node*> nodes = tree.nodesAtLevel( h );      
      for ( int j = h; j > 1; j-- ) {
        nodes = tree.nodesAtLevel( j );
        // check which of these have the same parent
        for ( int i = 0; i < nodes.size()-1; i++ ) {
          if ( nodes[i]->parent == nodes[i+1]->parent ) { // this might be wrong as it doesnt check all nodes between then!!!!!!!!!!!!!!!!!!           
            // then create corridor
            corridor ( tree.getleafOfNode(nodes[i]), tree.getleafOfNode(nodes[i+1]) );
          }
        }
        nodes.reset();
      }
    }

    void divide ( Node *node ) {    
      if ( node != NULL) {
        // create two children
        partition( node );
        // if children are big enough then divide them as well
        if ( abs(node->left->corner1.x() - node->left->corner2.x()) > threshold && abs(node->left->corner1.y() - node->left->corner2.y()) > threshold && abs(node->left->corner1.z() - node->left->corner2.z()) > threshold ) 
          divide( node->left );
        if ( abs(node->right->corner1.x() - node->right->corner2.x()) > threshold && abs(node->right->corner1.y() - node->right->corner2.y()) > threshold && abs(node->right->corner1.z() - node->right->corner2.z()) > threshold )     
          divide( node->right );
      }
    }

    // find 2 points in the area of node to create a room
    void createRandomRooms ( Node *node ) {
      float x1 = static_cast<float> ( rand()% static_cast<int>( node->corner2.x() - node->corner1.x() )/3 + 1) + node->corner1.x();
      float y1 = static_cast<float> ( rand()% static_cast<int>( node->corner2.y() - node->corner1.y() )/3 + 1) + node->corner1.y();
      float z1 = static_cast<float> ( rand()% static_cast<int>( node->corner2.z() - node->corner1.z() )/3 + 1) + node->corner1.z();

      float x2 = node->corner2.x() - static_cast<float> ( rand()% static_cast<int>( node->corner2.x() - node->corner1.x() )/3 + 1);
      float y2 = node->corner2.y() - static_cast<float> ( rand()% static_cast<int>( node->corner2.y() - node->corner1.y() )/3 + 1);
      float z2 = node->corner2.z() - static_cast<float> ( rand()% static_cast<int>( node->corner2.z() - node->corner1.z() )/3 + 1);

      float dx = (x2-x1);
      float dy = (y2-y1);
      float dz = (z2-z1);
      // adjust the x,y,z depending on the smallest one so there are no rooms with weird shapes
      if ( (dx < dy) &&  (dx < dz)  ) { // dx smaller that others
          y2 = y1 + dx;
          if ( dz > 2.0f*dx )
            z2 = z1 + 2.0f*dx;
      }
      else if ( (dy < dx) && ( dy < dz ) ) { // dy smaller
        x2 = x1 + dy;
        if ( dz > 2.0f*(x2-x1) )
          z2 = z1 + 2.0f*(x2-x1);
      }
      else  { // ( (dz < dx) && ( dz < dy ) ) { // dz smaller
        x2 = x1 + dz;
        if ( dy > (x2 - x1) ) 
          y2 = y1 + ( x2 - x1 );
      }

      node->corner1 = vec3(x1, y1, z1);
      node->corner2 = vec3(x2, y2, z2);

      polygon.createRoom( node->corner1, node->corner2 );
    }

    // find all leaves of tree to create the rooms
    void findLeaves ( Node *node ) {
      if ( node->left != NULL )
        findLeaves( node->left );
      else if ( node->left == NULL ) {
        createRandomRooms ( node );
        counter++;
        return;
      }
      if ( node->right != NULL )
        findLeaves( node->right );
      else if ( node->right == NULL ) {
        createRandomRooms ( node );
        counter++;
        return;
      }

    }

    void partition ( Node *_node ) {     
      // partition its space randomly horizontal or vertical
      int random = rand()%3; 
      if ( random == 0 )
        splitHorizontal(_node);
      else if ( random == 1 )
        SplitVertical(_node);
      else if ( random == 2 )
        SplitAtZAxis(_node);
    }

    void splitHorizontal ( Node *parent ) {
      vec3 point;
      if ( parent != NULL) {
        point.y() = abs(parent->corner2.y()-parent->corner1.y())/2.0f; 
        // point holds the coordinates at which we will split the node 
        tree.insert ( parent, new Node ( parent->corner1, vec3( parent->corner2.x(), parent->corner1.y()+point.y(), parent->corner2.z() ) ) );
        tree.insert ( parent, new Node ( vec3( parent->corner1.x(), parent->corner1.y()+point.y(), parent->corner1.z() ), parent->corner2 ) );
      }      
    }

    void SplitVertical ( Node *parent ) {
      vec3 point;
      if ( parent != NULL) {
        // find random point in x axis
        point.x() = abs(parent->corner2.x() - parent->corner1.x())/2.0f;
        tree.insert ( parent, new Node ( parent->corner1, vec3( parent->corner1.x() + point.x(), parent->corner2.y(), parent->corner2.z() ) ) );
        tree.insert ( parent, new Node ( vec3( parent->corner1.x() + point.x(), parent->corner1.y(), parent->corner1.z() ), parent->corner2 ) );
      }
    }

    void SplitAtZAxis ( Node *parent ) {
      vec3 point;
      if ( parent != NULL) {
        // find random point in z axis
        point.z() = abs(parent->corner2.z() - parent->corner1.z())/2.0f;
        tree.insert ( parent, new Node ( parent->corner1, vec3( parent->corner2.x(), parent->corner2.y(), parent->corner1.z()+point.z() ) ) );
        tree.insert ( parent, new Node ( vec3( parent->corner1.x(), parent->corner1.y(), parent->corner1.z()+point.z() ), parent->corner2 ) );
      }
    }

    void reset () { tree.reset(); polygon.reset(); }

    void render ( color_shader &shader, mat4t &cameraToWorld ) { polygon.render( shader, cameraToWorld ); }
    void render ( stripes_shader &shader, mat4t &cameraToWorld ) { polygon.render( shader, cameraToWorld ); }
  };

  
  class Dungeons_app : public octet::app {  

    Camera cameraToWorld;
    stripes_shader stripes_shader_;
    BSR dungeon;

  public:

    Dungeons_app(int argc, char **argv) : app(argc, argv) {}

    void app_init() {
      stripes_shader_.init();
      srand( time(NULL) );
      // give the initial space that I want to be partitioned
      dungeon.setSpace( vec3( 0.0f, 0.0f, 0.0f), vec3( 50.0f, 50.0f, 50.0f ) );
      dungeon.BSRAlgorithm();
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
      else if (is_key_down('Q')) {
        cameraToWorld.translateUp(1.0f);
		  }
      else if (is_key_down('E')) {
        cameraToWorld.translateDown(1.0f);
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
      else if (is_key_down(key_space)) {
        dungeon.reset();
			  dungeon.setSpace( vec3( 0.0f, 0.0f, 0.0f), vec3( 50.0f, 50.0f, 50.0f ) );
        dungeon.BSRAlgorithm();
        cameraToWorld.reset();
		  }
	}

    void draw_world(int x, int y, int w, int h) {
     
      glViewport(x, y, w, h);
      glClearColor(0.2f, 0, 0.7f, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);

      handleKeyStrokes ();
      // render dungeons as one mesh
      dungeon.render( stripes_shader_, cameraToWorld.getCamera() );
    }
  };
}
