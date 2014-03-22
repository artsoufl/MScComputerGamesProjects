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

// namespace help us to group classes and avoid clashes
namespace octet {

#define IX(i,j,k) ((i)+(N+2)*(j)+(N+2)*(N+2)*(k))
#define SWAP(x0,x) {float * tmp=x0;x0=x;x=tmp;}
  // maybe start from 0???????????????/ to 
#define FOR_EACH_CELL for ( i=1 ; i<=N ; i++ ) { for ( j=1 ; j<=N ; j++ ) { for ( k=1 ; k<=N ; k++ ) { 
#define END_FOR }}}

  /* global variables */

static int N;
static float dt, diff, visc;
static float force, source;
static int dvel;

// density and velocity
static float * u, * v, *w, * u_prev, * v_prev, * w_prev;
static float * dens, * dens_prev, * dens2, * dens_prev2;

static int win_id;
static int win_x, win_y;
static int mouse_down[3];
static int omx, omy, mx, my;

		class Background {

    mat4t modelToWorld;
    mat4t camera;
    int texture;
  public:

    void init(int _texture, mat4t &m) {
      modelToWorld = m;
      modelToWorld.translate(0.5f, -4.0f, 0.0f);
      camera.loadIdentity();
      camera.translate(0.5f, 1.5f, 2.5f);
      texture = _texture;
    }

    void render(texture_shader &shader, mat4t &cameraToWorld, float d ) {

      // build a projection matrix: model -> world -> camera -> projection
      // the projection space is the cube -1 <= x/w, y/w, z/w <= 1
      
      
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, camera);

      glActiveTexture(GL_TEXTURE10);
      glBindTexture(GL_TEXTURE_2D, texture);

      // set up the uniforms for the shader
      shader.render(modelToProjection, 0);

      // this is an array of the positions of the corners of the box in 3D
      // a straight "float" here means this array is being generated here at runtime.
      float vertices[] = {
        -(d*0.5f), 0, 0,
        (d*0.5f), 0, 0,
         (d*0.5f),  d, 0,
        -(d*0.5f),  d, 0,
      };

      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices );
      glEnableVertexAttribArray(attribute_pos);

      // this is an array of the positions of the corners of the texture in 2D
      static const float uvs[] = {
        0,  0,
        1,  0,
        1,  1,
        0,  1,
      };

      // attribute_uv is position in the texture of each corner
      // each corner (vertex) has 2 floats (x, y)
      // there is no gap between the 2 floats and hence the stride is 2*sizeof(float)
      glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)uvs );
      glEnableVertexAttribArray(attribute_uv);

      // finally, draw the sprite (4 vertices)
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    }
  };

  class OctetMesh {
		mat4t modelToWorld;
		mesh_builder mb;
		mesh m;
    bool lockflag;

	public:
		OctetMesh ( ):lockflag(false) { }
		 void init () {
			 mb.init ();
       modelToWorld.loadIdentity();
		 }

		 void bindMesh () { mb.get_mesh(m); }

     void lock () { m.lock(); }
     void unlock () { m.unlock(); }
     bool isLocked () { return lockflag; }
     void setLocked (bool value) { lockflag = value; }


     void allocate () { m.allocate( m.get_num_vertices(), m.get_num_indices() ); }

		 void addCube ( float x, float y, float z, float size, vec4 &color= vec4(1.0f,1.0,1.0f,1.0f) ) {
       mb.add_cube_coordinates( x, y, z, size/2.0f, color);
		 }

     void changeColor (/*const mesh &m , */vec4 &color= vec4(1.0f,1.0,1.0f,1.0f)) {
       mb.change_color(color);
     }

     void changemeshColor ( vec4 &color, int clr ) { m.cube_Color( color, clr ); }
     void changemeshColor ( vec4 &color, vec4 &color2, int clr ) { m.cube_Color( color, color2, clr ); }

     void setRender(unsigned int value){
       m.set_mode(value);
      }

     int getIndices () {
      return m.get_num_indices();
     }

		 void reset () {
			 modelToWorld.loadIdentity();
			 m.get_indices()->reset();
			m.get_vertices()->reset();
			m.set_indices(0);
			m.set_vertices(0);
			m.set_num_indices(0);
			m.set_num_vertices(0);
			m = NULL;
			mb.init();
			//m.~mesh();
		 }

		 void render ( color_shader &shader, mat4t &cameraToWorld ) {
			 mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);
		     shader.render(modelToProjection, vec4(0.0f, 1.0f, 0.0f, 1.0f));
			 m.render();
		 }

     void render ( fluid_shader3D &shader, mat4t &cameraToWorld , float degree) {
        mat4t rotationMatrix(1.0f);
        rotationMatrix.rotateY(degree);

        mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld * rotationMatrix, cameraToWorld);
        shader.render(modelToProjection, vec4(0.0f, 1.0f, 0.0f, 1.0f));
        m.render();
		 }
	};

  class Solver {
  public:
    Solver () {}

    // sources for a given frame is given are provided in s
    // mouse movement
    // routine that adds the source to the density
    // through diffusion each cell  exchange density with its neightbors
    void add_source ( int N, float * x, float * s, float dt )
    {
	    int i, size=(N+2)*(N+2)*(N+2);
	    for ( i=0 ; i<size ; i++ ) x[i] += dt*s[i];
    }

    // sets the boundary cells
    // we assume that the fluid is inside a box with solid boundaries, no flow exits the walls
    // horizontal component of velocity should be 0 on the vertical walls
    // vertical component of velocity should be 0 on the horizontal walls
    void set_bnd ( int N, int b, float * x )
    {
	    int i, k;
      // if - is at the 2nd one then everything moves towards left side of window
	    for ( i=1 ; i<=N ; i++ ) {
        for ( k = 1; k<=N ; k++ ) {
		      x[IX(0  ,i, k )] = b==1 ? -x[IX(1,i, k)] : x[IX(1,i, k)];//-x[IX(1,i, k+1)] : x[IX(1,i, k+1)]; // vertical boundaries // left side
		      x[IX(N+1,i, k)] = b==1 ? -x[IX(N,i, k)] : x[IX(N,i, k)];// -x[IX(N,i, k-1)] : x[IX(N,i, k-1)];
		      x[IX(i,0,k )] = b==2 ? -x[IX(i,1, k)] : x[IX(i,1, k)];//-x[IX(i,1, k+1)] : x[IX(i,1, k+1)]; // horizontal boundaries
		      x[IX(i,N+1,k)] = b==2 ? -x[IX(i,N, k)] : x[IX(i,N, k)];//-x[IX(i,N, k-1)] : x[IX(i,N, k-1)];
          x[IX(i ,k, 0 )] = b==3 ? -x[IX(i ,k, 1)] : x[IX(i,k, 1)];//-x[IX(i+1 ,k, 1)] : x[IX(i+1 ,k, 1)]; // extra dimensions front
		      x[IX(i,k, N+1)] = b==3 ? -x[IX(i,k, N)] : x[IX(i, k, N)];//-x[IX(i+1,k, N)] : x[IX(i+1, k, N)];  // back
        }
	    }

	      x[IX(0  ,0 , 0 )] = 0.33f*(x[IX(1,0 , 0 )]+x[IX(0  ,1, 0)]+x[IX(0  ,0, 1)]); // left bottom front corner
	      x[IX(0  ,N+1, 0)] = 0.33f*(x[IX(1,N+1, 0)]+x[IX(0  ,N+1, 1)]+x[IX(0  ,N+1, 1)]); // left up front corner   2nd as +x[IX(0  ,N, 1)] !!!!!!!!!!!
	      x[IX(N+1,0 , 0 )] = 0.33f*(x[IX(N,0 , 0 )]+x[IX(N,0, 1)]+x[IX(N+1,0, 1)]); // front right bottom corner
	      x[IX(N+1,N+1, 0)] = 0.33f*(x[IX(N,N+1, 0)]+x[IX(N,N+1, 1)]+x[IX(N+1,N+1, 1)]); // front right up
        x[IX(0  ,0, N+1)] = 0.33f*(x[IX(1,0 , N+1 )]+x[IX(1  ,0, N)]+x[IX(0  ,0, N)]); // left bottom back
	      x[IX(0 ,N+1,N+1)] = 0.33f*(x[IX(1,N+1, N+1)]+x[IX(1,N+1, N)]+x[IX(0,N+1, N)]); // left up back
	      x[IX(N+1,0 ,N+1 )] = 0.33f*(x[IX(N,0 , N+1)]+x[IX(N,0, N)]+x[IX(N+1,0, N)]); // right bottom back
	      x[IX(N+1,N+1,N+1)] = 0.33f*(x[IX(N,N+1, N+1)]+x[IX(N,N+1,N)]+x[IX(N+1,N+1,N)]); // right up back

    }
    // difussion solver 
    // computes exchange in every grid cell
    // extend it using http://www.mat.ucsb.edu/~wakefield/594cm/assignment.htm
    void lin_solve ( int N, int b, float * x, float * x0, float a, float c )
    {
	    int i, j, k, l;

	    for ( l=0 ; l<20 ; l++ ) {
		    FOR_EACH_CELL
			    x[IX(i,j,k)] = (x0[IX(i,j,k)] + a*(x[IX(i-1,j,k)]+x[IX(i+1,j,k)]+x[IX(i,j-1,k)]+x[IX(i,j+1,k)]+x[IX(i,j,k-1)]+x[IX(i,j,k+1)]))/c;
		    END_FOR
		    set_bnd ( N, b, x );
	    }
    }
    // gauss-seidel relaxation
    void diffuse ( int N, int b, float * x, float * x0, float diff, float dt )
    {
	    float a=dt*diff*N*N*N;
	    lin_solve ( N, b, x, x0, a, 1+6*a ); // 6 is because of 6 surrounding cubes
    }
    // look for particles that end up exactly at the cell centers
    void advect ( int N, int b, float * d, float * d0, float * u, float * v, float * w, float dt )
    {
	    int i, j, k, i0, j0, k0, i1, j1, k1;
	    float x, y, z, s0, t0, u0, s1, t1, u1, dt0;

	    dt0 = dt*N;
	    FOR_EACH_CELL
		    x = i-dt0*u[IX(i,j,k)]; 
        y = j-dt0*v[IX(i,j,k)];
        z = k-dt0*w[IX(i,j,k)];
		    if (x<0.5f) x=0.5f; if (x>N+0.5f) x=N+0.5f; i0=(int)x; i1=i0+1;
		    if (y<0.5f) y=0.5f; if (y>N+0.5f) y=N+0.5f; j0=(int)y; j1=j0+1;
        if (z<0.5f) z=0.5f; if (z>N+0.5f) z=N+0.5f; k0=(int)z; k1=k0+1;
		    s1 = x-i0; s0 = 1-s1; 
        t1 = y-j0; t0 = 1-t1; 
        u1 = z-k0; u0 = 1-u1;
		    d[IX(i,j,k)] = u0 * ( s0 * ( t0 * d0[IX(i0, j0, k0)] + t1 * d0[IX(i0, j1, k0)] )  + 
							                s1 * ( t0 * d0[IX(i1, j0, k0)] + t1 * d0[IX(i1, j1, k0)]) ) +
					             u1 * ( s0 * ( t0 * d0[IX(i0, j0, k1)] + t1 * d0[IX(i0,j1,k1)]) + 
							                s1 * ( t0 * d0[IX(i1, j0, k1)] + t1 * d0[IX(i1, j1, k1)]) );
	    END_FOR
	    set_bnd ( N, b, d );
    }

    void project ( int N, float * u, float * v, float * w, float * p, float * div )
    {
	    int i, j, k;

	    FOR_EACH_CELL
        // this might be wrong!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  0.33 or 0.5 ????
		    div[IX(i,j,k)] = -0.33f*(u[IX(i+1,j,k)]-u[IX(i-1,j,k)]+v[IX(i,j+1,k)]-v[IX(i,j-1,k)]+w[IX(i,j,k+1)]-w[IX(i,j,k-1)])/N;
		    p[IX(i,j,k)] = 0.0f;
	    END_FOR	
	    set_bnd ( N, 0, div ); 
      set_bnd ( N, 0, p );

	    lin_solve ( N, 0, p, div, 1, 6 );

	    FOR_EACH_CELL
		    u[IX(i,j,k)] -= 0.5f*N*(p[IX(i+1,j,k)]-p[IX(i-1,j,k)]);
		    v[IX(i,j,k)] -= 0.5f*N*(p[IX(i,j+1,k)]-p[IX(i,j-1,k)]);
        w[IX(i,j,k)] -= 0.5f*N*(p[IX(i,j,k+1)]-p[IX(i,j,k-1)]);
	    END_FOR
	    set_bnd ( N, 1, u ); set_bnd ( N, 2, v ); set_bnd ( N, 3, w );
    }
    // all steps grouped together in a single routine
    // sourve densities are initial in x0
    void dens_step ( int N, float * x, float * x0, float * u, float * v, float * w, float diff, float dt )
    {
	    add_source ( N, x, x0, dt );
      // swap swaps the two array pointers
	    SWAP ( x0, x ); diffuse ( N, 0, x, x0, diff, dt );
	    SWAP ( x0, x ); advect ( N, 0, x, x0, u, v, w, dt );
    }
    // force fields u0, v0
    void vel_step ( int N, float * u, float * v, float * w, float * u0, float * v0, float * w0, float visc, float dt )
    {
	    add_source ( N, u, u0, dt ); add_source ( N, v, v0, dt ); add_source ( N, w, w0, dt );
	    SWAP ( u0, u ); diffuse ( N, 1, u, u0, visc, dt );
	    SWAP ( v0, v ); diffuse ( N, 2, v, v0, visc, dt );
      SWAP ( w0, w ); diffuse ( N, 3, w, w0, visc, dt ); 
      // project forces the velocity to be mass conserving
      // visually it forces the flow to have many vortices which produce realistic swirly-like flows 
	    project ( N, u, v, w, u0, v0 ); // w0??????????????????????????????
	    SWAP ( u0, u ); SWAP ( v0, v ); SWAP ( w0, w );
	    advect ( N, 1, u, u0, u0, v0, w0, dt ); 
      advect ( N, 2, v, v0, u0, v0, w0, dt );
      advect ( N, 3, w, w0, u0, v0, w0, dt ); // ????????????????
      // recall project as advect behaves more accurate when the velocity field is mass conserving
	    project ( N, u, v, w, u0, v0 );
    }
  };

  

  class fluids_app : public app {
    
    mat4t modelToWorld;
    mat4t cameraToWorld;

    fluid_shader3D fluid_shader_3D_;
    texture_shader texture_shader_;

    Solver solver;
    dynarray<float> vertices;
    dynarray<float> color;
    dynarray <OctetMesh> octet_meshes_;

    float angle; // for camera rotation
    Background backgrd;
    GLuint background;

    bool withBackground;
    enum State { EXPLOSION, BIG, TWO } state;
    static int temp;
    float degree;

  public:

    // this is called when we construct the class
    fluids_app(int argc, char **argv) : app(argc, argv) {}

    // this is called once OpenGL is initialized
    void app_init() {
     
      texture_shader_.init();
      fluid_shader_3D_.init();

      background = resources::get_texture_handle(GL_RGBA, "assets/land.gif");

      srand(time(NULL));
      modelToWorld.loadIdentity();

      backgrd.init(background, modelToWorld );

      cameraToWorld.loadIdentity();
      cameraToWorld.translate(0.5f, 1.5f, 2.5f);
     
      for (int i=0 ; i<3 ; i++ ) {
        OctetMesh mesh;
        mesh.init();
        octet_meshes_.push_back( mesh);
      }

      N = 16; // it was 64
		  dt = 0.1f;
		  diff = 0.0f;
		  visc = 0.0f;
      //visc = 10.0f; // wow!
		  force = 5.0f;
		  source = 100.0f;
      dvel = 0;        
      get_viewport_size( win_x, win_y);
      allocate_data();
      clear_data ();
      withBackground = true;
      angle = 0.0f;
      degree = 0.0f;

      state = EXPLOSION;
      //temp = 0;
    }

void free_data ( void )
{
	if ( u ) free ( u );
	if ( v ) free ( v );
  if ( w ) free ( w );
	if ( u_prev ) free ( u_prev );
	if ( v_prev ) free ( v_prev );
  if ( w_prev ) free ( w_prev );
	if ( dens ) free ( dens );
	if ( dens_prev ) free ( dens_prev );
  if ( dens2 ) free ( dens2 );
	if ( dens_prev2 ) free ( dens_prev2 );
}

void clear_data ( void )
{
	int i, size=(N+2)*(N+2)*(N+2);

	for ( i=0 ; i<size ; i++ ) {
		u[i] = v[i] = w[i] = u_prev[i] = v_prev[i] = w_prev[i] = dens[i] = dens_prev[i] = dens2[i] = dens_prev2[i] = 0.0f;
	}
}

int allocate_data ( void )
{
	int size = (N+2)*(N+2)*(N+2);

	u			= (float *) malloc ( size*sizeof(float) );
	v			= (float *) malloc ( size*sizeof(float) );
  w			= (float *) malloc ( size*sizeof(float) );
	u_prev		= (float *) malloc ( size*sizeof(float) );
	v_prev		= (float *) malloc ( size*sizeof(float) );
  w_prev		= (float *) malloc ( size*sizeof(float) );
	dens		= (float *) malloc ( size*sizeof(float) );	
	dens_prev	= (float *) malloc ( size*sizeof(float) );
  dens2		= (float *) malloc ( size*sizeof(float) );	
	dens_prev2	= (float *) malloc ( size*sizeof(float) );

	if ( !u || !v || !w || !u_prev || !v_prev || !w_prev || !dens || !dens_prev || !dens2 || !dens_prev2 ) {
		fprintf ( stderr, "cannot allocate data\n" );
		return ( 0 );
	}

	return ( 1 );
}

//void draw_velocity ( void )
//{
//	int i, j;
//	float x, y, h;
//
//	h = 1.0f/N;
//
//	glColor3f ( 1.0f, 1.0f, 1.0f );
//	glLineWidth ( 1.0f );
//
//	glBegin ( GL_LINES );
//
//		for ( i=1 ; i<=N ; i++ ) {
//			x = (i-0.5f)*h;
//			for ( j=1 ; j<=N ; j++ ) {
//				y = (j-0.5f)*h;
//
//				glVertex2f ( x, y );
//				glVertex2f ( x+u[IX(i,j)], y+v[IX(i,j)] );
//			}
//		}
//
//	glEnd ();
//}


void draw_density ( void )
{
	int i, j;
	float x, y, z, h, d000, d010, d100, d110, d001, d011, d101, d111;
  vec3 v1, v2, v3, v4, v5, v6, v7, v8;
  int currentMesh = 0;
  int numberIndices = 0;
  static int temp = 0;
  int counter = 0, lock = 0;
	h = 1.0f/N;
  int color = 0;

  z = 0.0f;

		for ( i=0 ; i<=N; i++ ) {  
			x = (i-0.5f)*h;
			for ( j=0 ; j<=N; j++ ) {
				y = (j-0.5f)*h;
        for (int k = 0; k <= N; k++ ) {
          z = (k-0.5f)*h;

          if ( state == EXPLOSION ) {
            // N/2,1,N/2 source pos
            if ( sqrt((i-N/2)*(i-N/2) + (j-1)*(j-1) + (k-N/2)*(k-N/2)) < 15.0f) {
              color = 1 + (rand()) /( (RAND_MAX/(3-1)));
            }
          }

				  d000 = dens[IX(i,j,k)];
				  d010 = dens[IX(i,j+1,k)];
				  d100 = dens[IX(i+1,j,k)];
				  d110 = dens[IX(i+1,j+1,k)];

          if ( state == TWO ) {
            d001 =  dens2[IX(i,j,k)];
				    d011 =  dens2[IX(i,j+1,k)];
				    d101 =  dens2[IX(i+1,j,k)];
				    d111 =  dens2[IX(i+1,j+1,k)];
          }

          //d001 = dens[IX(i,j,k+1)];
				  //d011 = dens[IX(i,j+1,k+1)];
          //d101 = dens[IX(i+1,j,k+1)];
				  //d111 = dens[IX(i+1,j+1,k+1)];
          
          // only the first time we create the vertices of the cubes
          if ( temp == 0 ) {
            if(numberIndices < 65000){
              octet_meshes_[currentMesh].addCube( x, y, z, h, vec4( d000, d010, d100, d110 ) );
              numberIndices+=36;
            }else{
              currentMesh++;
              octet_meshes_[currentMesh].addCube( x, y, z, h, vec4( d000, d010, d100, d110 ) );
              numberIndices=36;
            }
          }else {
            // lock the next mesh
            if ( !octet_meshes_[mesh::getCurrentMesh()].isLocked() ){
              octet_meshes_[mesh::getCurrentMesh()].setLocked(true); // flag
              octet_meshes_[mesh::getCurrentMesh()].lock(); // vertices
              numberIndices = 0;
            }
            if ( state == TWO ) {
              octet_meshes_[mesh::getCurrentMesh()].changemeshColor( vec4( d000, d010, d100, d110 ), vec4( d001, d011, d101, d111 ),  color);
            }
            else {
              octet_meshes_[mesh::getCurrentMesh()].changemeshColor( vec4( d000, d010, d100, d110 ),  color);
            }
              color = 0;
              numberIndices += 36;
              // if one mesh is full then go to the next one
              if ( numberIndices > 65000 ) {
                mesh::IncreasecurrentMesh();
                numberIndices = 0;
              }
          }

        }      
			}   
		}

    if ( temp != 0) {// if not the first time
      for(int i=0; i<octet_meshes_.size(); i++) {
        octet_meshes_[i].unlock();
        octet_meshes_[i].setLocked(false);
      }
      mesh::setCurrentMeshZero();
    }
    // if its the first time then bind the mesh
    if ( temp == 0) {
      for(int i=0; i<octet_meshes_.size(); i++) {
        octet_meshes_[i].bindMesh();
      }
    }

    temp++;

}

void reset () {
  clear_data();
  mesh::setCurrentMeshZero();
  temp = 0;
}

void get_from_UI ( float * d, float * u, float * v, float *w )
{
	int i, j, size = (N+2)*(N+2)*(N+2);
  //static int temp = 0;

	for ( i=0 ; i<size ; i++ ) {
		u[i] = v[i] = w[i] = d[i] = 0.0f;
	}

  if ( state == EXPLOSION ) {
    if ( temp <= 80) {
     d[IX(N/2,1,N/2)] = 5.0f;
     //u[IX(N/2,2,N/2)] = force*2.5f;
	   v[IX(N/2,2,N/2)] = force*5.5f;
    // w[IX(N/2,2,N/2)] = force*2.5f;
     temp++;
    }
  }
  else if ( state == BIG ) {
    d[IX(N/2,1,N/2)] = 15.0f*source;
	  v[IX(N/2,2,N/2)] = force*5.5f;

  }
  else if ( state == TWO ) {
    if ( d == dens_prev) {
      d[IX(1,1,N-1)] = 55.0f*source;
	    v[IX(1,1,N-1)] = force*15.5f;
    }
    else {
      d[IX(N-1,1,N-1)] = 55.0f*source;
	    v[IX(N/2,1,N-3)] = force*55.5f;
    }
  }

	//if ( !is_key_down(key_lmb) && !is_key_down(key_rmb) ) return;
  //
	//i = (int)((       mx /(float)win_x)*N+1);
	//j = (int)(((win_y-my)/(float)win_y)*N+1);
  //
	//if ( i<1 || i>N || j<1 || j>N ) return;
  //
	//if ( is_key_down(key_lmb) ) {
	//	u[IX(i,j)] = force * (mx-omx);
	//	v[IX(i,j)] = force * (omy-my);
	//}
  //
	//if ( is_key_down(key_rmb) ) {
	//	d[IX(i,j)] = source;
	//}
  //
	//omx = mx;
	//omy = my;

	return;
}

void mouse_func (  ) // int button, int state, int x, int y
{
  if ( is_key_down(key_lmb) || is_key_down(key_rmb)) {
    get_mouse_pos(mx, my);
    omx = mx ;
	  omx = my ;
	}
}

void motion_func (  ) //int &x, int &y
{
  get_mouse_pos( mx, my);
}

void idle_func (  )
{
	get_from_UI ( dens_prev, u_prev, v_prev, w_prev );
	solver.vel_step ( N, u, v, w, u_prev, v_prev, w_prev, visc, dt );
	solver.dens_step ( N, dens, dens_prev, u, v, w, diff, dt );

  if ( state == TWO ) {
    get_from_UI ( dens_prev2, u_prev, v_prev, w_prev );
	  solver.vel_step ( N, u, v, w, u_prev, v_prev, w_prev, visc, dt );
	  solver.dens_step ( N, dens2, dens_prev2, u, v, w, diff, dt );
  }
}

void display_func (  )
{
      draw_density ();
}

void keyboard () {
  if (is_key_down(key_left)) {
		  cameraToWorld.rotateX(-angle);
		  cameraToWorld.rotateY(10.0f);
		  cameraToWorld.rotateX(angle);
      }
	  else if (is_key_down(key_right)) {
		  cameraToWorld.rotateX(-angle);
		  cameraToWorld.rotateY(-10.0f);
		  cameraToWorld.rotateX(angle);
      }
	  else if (is_key_down(key_up)) {
		  angle += 10.0f;
		  cameraToWorld.rotateX(10.0f);
      }
	  else if (is_key_down(key_down)) {
		  angle -= 10.0f;
		  cameraToWorld.rotateX(-10.0f);
      }
	  else if (is_key_down('W'))
		  cameraToWorld.translate(0.0f, -0.1f, 0.0f);
	  else if (is_key_down('S'))
		  cameraToWorld.translate(0.0f, 0.1f, 0.0f);
    else if (is_key_down('Q'))
		  cameraToWorld.translate(0.0f, 0.0f, -0.1f);
	  else if (is_key_down('E'))
		  cameraToWorld.translate(0.0f, 0.0f, 0.1f);
	  else if (is_key_down('A'))
		  cameraToWorld.translate(-0.1f, 0.0f, 0.0f);
	  else if (is_key_down('D'))
		  cameraToWorld.translate(0.1f, 0.0f, 0.0f);
    else if (is_key_down('B')){
		 if(withBackground){
       withBackground =false;
     }else{
       withBackground =true;
     }
    }
    else if (is_key_down(key_space)){
		  cameraToWorld.loadIdentity();
      cameraToWorld.translate(0.5f, 1.5f, 2.5f);
      degree=0;
    }
    else if (is_key_down('H')){
      state = EXPLOSION;
      reset();
    }
    else if (is_key_down('J')){
      state = BIG;
      reset();
    }
    else if (is_key_down('K')){
      state = TWO;
      reset();
    }
    else if (is_key_down('Z')){
      degree += 1.5f;
    }
    else if (is_key_down('X')){
      degree -= 1.5f;
    }
}

void draw_world(int x, int y, int w, int h) {

  glViewport(x, y, w, h);
  
  glClearColor(0.61, 0.94f, 1.0f, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND); 
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  
  if(withBackground)
    backgrd.render( texture_shader_, cameraToWorld, 10.0f );

 // mouse_func();
  //motion_func();
  idle_func();
  display_func();

  for(int i=0; i<octet_meshes_.size(); i++) {
    octet_meshes_[i].setRender(GL_TRIANGLES);
    octet_meshes_[i].render( fluid_shader_3D_, cameraToWorld, degree);       
  }

  keyboard();

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
  };

  int fluids_app::temp = 0;
}


