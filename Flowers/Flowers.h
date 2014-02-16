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
		vec4 color;
		Mesh mesh_;
    GLuint texture_handle_;         // this is an OpenGL texture handle which is used to draw the image.
		color_shader color_shader_;
		texture_shader texture_shader_;

	public:
		HandleMesh () { }

		void init() {
		  mesh_.init();
		}

		void pushVertex (vec3 &v) {
			mesh_.pushVertex(v);
		}

    void pushIndex () {
      mesh_.pushIndex();
    }

		void pushUV ( float u, float v ) { mesh_.pushUV( u , v ); }

		void meshReady () { mesh_.setBuffers(); }		

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

  class Catmull_Rom_Spline {
		dynarray<float> points;
		dynarray<vec3> control_points;
		dynarray<vec3> start_end_points;

    HandleMesh *mesh;

		float length, posZ;
		int knots;
    int num;
    float noiseX, noiseY, noiseZ, angle;
    string type;
    mat4t modelToWorld;

    // produce one point in a spline
    void formula (float p, const vec3 &p0, const vec3 &p1, const vec3 &p2, const vec3 &p3) {
			float t = p;
			float u3 = t * t * t; 
			float u2 = t * t;

			vec3 point;
			// polynomials
			float b0 = 0.5f*(-u3 + 2*u2 - t);
			float b1 = 0.5f*(3*u3 - 5*u2 + 2);
			float b2 = 0.5f*(-3*u3 + 4*u2 + t);
			float b3 = 0.5f*(u3 - u2);
			// desired point
			point.x() = b0*p0.x() + b1*p1.x() + b2*p2.x() + b3*p3.x();
			point.y() = b0*p0.y() + b1*p1.y() + b2*p2.y() + b3*p3.y();
			point.z() = b0*p0.z() + b1*p1.z() + b2*p2.z() + b3*p3.z();

			points.push_back(point.x());
			points.push_back(point.y());
			points.push_back(point.z());

      printf("%f %f %f\n", point.x(), point.y(), point.z() );

		}

    void calculatePoints (int num, dynarray<vec3> &vectors) {
			float t = 0.0f;
      vec3 center;
			for ( int i = 0; i <= num; i ++) {
				formula(t, vectors[0], vectors[1], vectors[2], vectors[3]);
				t += 1.0f/num;
			}
      //getCenter( center );
      //calculateRenderedTriangles(center);
		}

    void calculateRenderedTriangles ( vec3 &center, float distX, float distY ) {
      vec2 uv;
      mat4t rotate;
      rotate.loadIdentity();
      rotate = modelToWorld;
      for ( unsigned int j = 0; j < num; j++ ) { // create as many as the input says      
        rotate.rotateZ(angle);
        for ( unsigned int i = 0; i < points.size()/3-1; i++ ) {  // I am not sure if the condition is correct!!!!!!!!!!!!!!!
          mesh->pushVertex( center*rotate );
          //mesh->pushVertex( vec3( center.x(), center.y(), -3.0f ) );
          mesh->pushIndex();
          uv = findUVCoordinates( center, distX, distY);
          mesh->pushUV( uv.x(), uv.y() );

          // push 1st
          mesh->pushVertex( vec3( points[3*i], points[3*i+1], points[3*i+2]  ) *rotate );
          mesh->pushIndex();
          uv = findUVCoordinates( vec3( points[3*i], points[3*i+1], points[3*i+2]  ), distX, distY);
          mesh->pushUV( uv.x(), uv.y() );

          // push 2st
          mesh->pushVertex( vec3( points[3*i+3], points[3*i+4], points[3*i+5]  ) *rotate );
          mesh->pushIndex();

          uv = findUVCoordinates( vec3( points[3*i+3], points[3*i+4], points[3*i+5] ), distX, distY);
          mesh->pushUV( uv.x(), uv.y() );
        }
        //rotate.rotateZ(angle);
      }
     // reset();
    }

    void createObjects () {
      dynarray<vec3> four_points;
      vec3 center;
      float distX = 0.0f,distY = 0.0f;
			// for every 2 points in the array
      // this will cause trouble with more than 2 splines!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      // because I create the points for 2 splines and then calculate center and triangles
      // if I put 4 splines it will calculate center and triangels for all of them so I need an extra for loop
			for (unsigned int i = 0; i < control_points.size()/2; i++ ) {
        four_points.push_back(control_points[i*2]); // first control point
			  four_points.push_back(start_end_points[i*2]); // start point
			  four_points.push_back(start_end_points[i*2+1]); // end point 
			  four_points.push_back(control_points[i*2+1]); // second control point

			  calculatePoints(knots, four_points); // integer is the number of points that will be generated
        
        four_points.reset();
			}
      getCenter( center, distX, distY );
      calculateRenderedTriangles(center, distX, distY);
      reset();
		}

    void getCenter( vec3 &center, float &distX, float &distY) {
      float _minX = 0.0f, _maxX = 0.0f;
      float _minY = 0.0f, _maxY = 0.0f;
      for ( unsigned int i = 0; i < points.size(); i++ ) {
        if ( i%3 == 0 && points[i] < _minX )  // if it is x value
          _minX = points[i];
        else if ( i%3 == 0 && points[i] > _maxX )
          _maxX = points[i];
        if (i%3 == 1 && points[i] < _minY ) // if it is y value
          _minY = points[i];
        else if ( i%3 == 1 && points[i] > _maxY )
          _maxY = points[i];        
      }
      printf ("Center: %f %f\n", (_maxX-fabs(_minX))/2.0f, (_maxY-fabs(_minY))/2.0f);
      distX = abs(_maxX)-_minX;
      distY = abs(_maxY)-_minY;
      center[0]=(_maxX-abs(_minX))/2.0f;
      center[1]=(_maxY-abs(_minY))/2.0f;
      center[2]=0.0f;
    }

    vec2 findUVCoordinates ( const vec3 &v_, float distX, float distY) {
      float n = 1.0f/4.0f;
		  // if I divide each time x value with the distance then I have the correct coordinate on [0,1]
      if ( type == "leaf")
		    return vec2( n + n*2.0f*abs(v_.x() + distX/2.0f)/(distX*2.0f), abs(v_.y())/distY );
        //vec2( n + n*2.0f*abs(v_.x())/distX, abs(v_.y())/distY );
      else 
        return vec2( 2*n + n*2.0f*(abs(v_.x()+distX/2.0f)/(distX*2.0f)), abs(v_.y())/distY );
	  }

    void createPoints () {   
      start_end_points.push_back(vec3(0.0f, 0.0f, 0.0f));
      start_end_points.push_back(vec3(0.0f, length, posZ));

      start_end_points.push_back(vec3(0.0f, 0.0f, 0.0f));
      start_end_points.push_back(vec3(0.0f, length, posZ));
	  }

    void constructControlPoints() {
      mat4t rotate;
      rotate.loadIdentity();
			for (unsigned int i = 0; i < start_end_points.size()/4; i++ ) { // i < start_end_points.size()/2 !!!!!!!!!!!!!!!!
        control_points.push_back(vec3( -start_end_points[i*4+1].y() * noiseX, start_end_points[i*4].x(), 0.0f ));
				control_points.push_back(vec3( -start_end_points[i*4+1].y() * noiseX, start_end_points[i*4+1].y() * noiseY, noiseZ ));
        control_points.push_back(vec3( start_end_points[i*4+1].y() * noiseX, start_end_points[i*4+1].x(), 0.0f ));
				control_points.push_back(vec3( start_end_points[i*4+1].y() * noiseX, start_end_points[i*4+1].y() * noiseY, noiseZ ));
			}
		}

    void reset () {
		  points.reset();
      control_points.reset();
      start_end_points.reset();
      type = "";
		}

	public:

		Catmull_Rom_Spline() {}

    void init ( HandleMesh &mesh_ ) {
      mesh = &mesh_;     
      //set ( l, k, num_, type_, angle_, noiseX_, noiseY_, noiseZ_ );
    }

    void set ( int l, int k, float posZ_, int num_, string type_, float angle_, mat4t &modelToWorld_, float noiseX_= 0.0f, float noiseY_= 0.0f, float noiseZ_ = 0.0f ) {
      num = num_;
      length = static_cast<float>(l);
      posZ = posZ_;
			knots = k;
      type = type_;
      angle = angle_;
      noiseX = noiseX_;
      noiseY = noiseY_;
      noiseZ = noiseZ_;
      points.reset();
      modelToWorld = modelToWorld_;
      createPoints (); // create start and end points
			constructControlPoints(); // create control points		
			createObjects(); 
    }
  
	};


  class Flowers_app : public octet::app {
	camera cameraToWorld;
	HandleMesh mesh_;
	color_shader color_shader_;
	texture_shader texture_shader_;
  Catmull_Rom_Spline cut_splines;

  public:

    Flowers_app(int argc, char **argv) : app(argc, argv) {
    }

    void app_init() {
     // dynarray<vec3> points;
      //points.push_back(vec3(0.0f, 0.0f, 0.0f));
     // points.push_back(vec3(0.0f, 0.0f, 0.0f));
		  color_shader_.init();
		  texture_shader_.init();
		  cameraToWorld.translateUp(10.0f);

      mat4t mat;
      mat.loadIdentity();
      //mat.rotateY(90.0f);

      cut_splines.init( mesh_ );  // petal:4, 2   leaf:2, 1.5
      cut_splines.set( rand()%3+1, 10, static_cast<float>(rand()%2+1), 3, "rose", 120.0f, mat, static_cast<float>(rand()%4+1), static_cast<float>(rand()%2+1), 0.0f );  // noise: 4, 2, 0
      cut_splines.set( rand()%3+1, 10, static_cast<float>(-rand()%1+1), 1, "leaf", 45.0f, mat, static_cast<float>(rand()%2+1), static_cast<float>(rand()%1+1), 0.0f );  // noise: 2, 1.5, 0
					
      //cut_splines.set( 5, 10, 0.0f, 3, "rose", 120.0f, mat, 4.0f, 2.0f, .0f );
      //cut_splines.set( 4, 10, 1.5f, 1, "rose", 45.0f, mat, 4.0f, 2.0f, -1.0f );
      //cut_splines.set( 4, 10, 1.5f, 1, "rose", -45.0f, mat, 4.0f, 2.0f, -1.0f );
      //cut_splines.set( 4, 10, 1.5f, 1, "rose", 180.0f, mat, 4.0f, 2.0f, -1.0f );
      //cut_splines.set( 10, 10, 0.0f, 3, "leaf", 120.0f, mat, 2.0f, 5.5f,  .0f );
		  mesh_.meshReady(); // this SHOULD be calded after mesh is ready with all vertices in it
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
