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

#define IX(i,j) ((i)+(N+2)*(j))
#define SWAP(x0,x) {float * tmp=x0;x0=x;x=tmp;}
#define FOR_EACH_CELL for ( i=1 ; i<=N ; i++ ) { for ( j=1 ; j<=N ; j++ ) {
#define END_FOR }}
/* macros */
// array elements
#define IX(i,j) ((i)+(N+2)*(j))

  /* global variables */

static int N;
static float dt, diff, visc;
static float force, source;
static int dvel;

// density and velocity
static float * u, * v, * u_prev, * v_prev;
static float * dens, * dens_prev;

static int win_id;
static int win_x, win_y;
static int mouse_down[3];
static int omx, omy, mx, my;


  class OctetMesh {
		mat4t modelToWorld;
		mesh_builder mb;
		mesh m;

	public:
		OctetMesh ( ) { }
		 void init () {
			 mb.init ();
       modelToWorld.loadIdentity();
		 }

		 void bindMesh () { mb.get_mesh(m); }

     void bindColor () { m.enable_attribute_Color(); }

		 void addCube ( float x, float y, float z, float size, vec4 &color= vec4(1.0f,1.0,1.0f,1.0f) ) {
       mb.add_cube_coordinates( x, y, z, size/2.0f, color);
		 }

     void changeColor ( vec4 &color= vec4(1.0f,1.0,1.0f,1.0f) ) {
       mb.change_color( color );
     }

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

     void render ( fluid_shader3D &shader, mat4t &cameraToWorld ) {
			 mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);
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
	    int i;
      // if - is at the 2nd one then everything moves towards left side of window
	    for ( i=1 ; i<=N ; i++ ) {
		    x[IX(0  ,i)] = b==1 ? -x[IX(1,i)] : x[IX(1,i)]; // vertical boundaries
		    x[IX(N+1,i)] = b==1 ? -x[IX(N,i)] : x[IX(N,i)];
		    x[IX(i,0  )] = b==2 ? -x[IX(i,1)] : x[IX(i,1)]; // horizontal boundaries
		    x[IX(i,N+1)] = b==2 ? -x[IX(i,N)] : x[IX(i,N)];
        //x[IX(0  ,i)] = b==1 ? -x[IX(N,i)] : x[IX(N,i)];
		    //x[IX(N+1,i)] = b==1 ? -x[IX(1,i)] : x[IX(1,i)];
		    //x[IX(i,0  )] =  x[IX(i,N+1)] ;
		    //x[IX(i,N+1)] =  x[IX(i,0)] ;

        //x[IX(i,7)] = -x[IX(i,8)];
        //x[IX(7,i)] = -x[IX(8,i)];
      /*
          for (int j=0 ; j<=N ; j++ ) {
            if ( j <= 7 ) {
              x[IX(j,7)] = -x[IX(j,8)];
              x[IX(7,j)] = -x[IX(8,j)];
            }
              //x[IX(i,j)] -= x[IX(i,j)];
              //x[IX(i+1,j+1)] = b==1 ? -x[IX(i+2,j+1)] : x[IX(i+2,j+1)];
          
          }
          
        */
	    }
	    x[IX(0  ,0  )] = 0.5f*(x[IX(1,0  )]+x[IX(0  ,1)]);
	    x[IX(0  ,N+1)] = 0.5f*(x[IX(1,N+1)]+x[IX(0  ,N)]);
	    x[IX(N+1,0  )] = 0.5f*(x[IX(N,0  )]+x[IX(N+1,1)]);
	    x[IX(N+1,N+1)] = 0.5f*(x[IX(N,N+1)]+x[IX(N+1,N)]);
      //x[IX(0  ,0  )] = 0.5f*(x[IX(1,N+1)]+x[IX(0  ,N)]);
	    //x[IX(0  ,N+1)] = 0.5f*(x[IX(1,0  )]+x[IX(0  ,1)]);
	    //x[IX(N+1,0  )] = 0.5f*(x[IX(N,N+1)]+x[IX(N+1,N)]);
	    //x[IX(N+1,N+1)] = 0.5f*(x[IX(N,0  )]+x[IX(N+1,1)]);
    }
    // difussion solver 
    // computes exchange in every grid cell
    void lin_solve ( int N, int b, float * x, float * x0, float a, float c )
    {
	    int i, j, k;

	    for ( k=0 ; k<20 ; k++ ) {
		    FOR_EACH_CELL
			    x[IX(i,j)] = (x0[IX(i,j)] + a*(x[IX(i-1,j)]+x[IX(i+1,j)]+x[IX(i,j-1)]+x[IX(i,j+1)]))/c;
		    END_FOR
		    set_bnd ( N, b, x );
	    }
    }
    // gauss-seidel relaxation
    void diffuse ( int N, int b, float * x, float * x0, float diff, float dt )
    {
	    float a=dt*diff*N*N;
	    lin_solve ( N, b, x, x0, a, 1+4*a );
    }
    // look for particles that end up exactly at the cell centers
    void advect ( int N, int b, float * d, float * d0, float * u, float * v, float dt )
    {
	    int i, j, i0, j0, i1, j1;
	    float x, y, s0, t0, s1, t1, dt0;

	    dt0 = dt*N;
	    FOR_EACH_CELL
		    x = i-dt0*u[IX(i,j)]; y = j-dt0*v[IX(i,j)];
		    if (x<0.5f) x=0.5f; if (x>N+0.5f) x=N+0.5f; i0=(int)x; i1=i0+1;
		    if (y<0.5f) y=0.5f; if (y>N+0.5f) y=N+0.5f; j0=(int)y; j1=j0+1;
        //if (x<0.5f) x=N+0.5f; if (x>N+0.5f) x=0.5f; i0=(int)x; i1=i0+1;
		    //if (y<0.5f) y=N+0.5f; if (y>N+0.5f) y=0.5f; j0=(int)y; j1=j0+1;
		    s1 = x-i0; s0 = 1-s1; t1 = y-j0; t0 = 1-t1;
		    d[IX(i,j)] = s0*(t0*d0[IX(i0,j0)]+t1*d0[IX(i0,j1)])+
					     s1*(t0*d0[IX(i1,j0)]+t1*d0[IX(i1,j1)]);
	    END_FOR
	    set_bnd ( N, b, d );
    }

    void project ( int N, float * u, float * v, float * p, float * div )
    {
	    int i, j;

	    FOR_EACH_CELL
		    div[IX(i,j)] = -0.5f*(u[IX(i+1,j)]-u[IX(i-1,j)]+v[IX(i,j+1)]-v[IX(i,j-1)])/N;
		    p[IX(i,j)] = 0;
	    END_FOR	
	    set_bnd ( N, 0, div ); set_bnd ( N, 0, p );

	    lin_solve ( N, 0, p, div, 1, 4 );

	    FOR_EACH_CELL
		    u[IX(i,j)] -= 0.5f*N*(p[IX(i+1,j)]-p[IX(i-1,j)]);
		    v[IX(i,j)] -= 0.5f*N*(p[IX(i,j+1)]-p[IX(i,j-1)]);
	    END_FOR
	    set_bnd ( N, 1, u ); set_bnd ( N, 2, v );
    }
    // all steps grouped together in a single routine
    // sourve densities are initial in x0
    void dens_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt )
    {
	    add_source ( N, x, x0, dt );
      // swap swaps the two array pointers
	    SWAP ( x0, x ); diffuse ( N, 0, x, x0, diff, dt );
	    SWAP ( x0, x ); advect ( N, 0, x, x0, u, v, dt );
    }
    // force fields u0, v0
    void vel_step ( int N, float * u, float * v, float * u0, float * v0, float visc, float dt )
    {
	    add_source ( N, u, u0, dt ); add_source ( N, v, v0, dt );
	    SWAP ( u0, u ); diffuse ( N, 1, u, u0, visc, dt );
	    SWAP ( v0, v ); diffuse ( N, 2, v, v0, visc, dt );
      // project forces the velocity to be mass conserving
      // visually it forces the flow to have many vortices which produce realistic swirly-like flows 
	    project ( N, u, v, u0, v0 );
	    SWAP ( u0, u ); SWAP ( v0, v );
	    advect ( N, 1, u, u0, u0, v0, dt ); advect ( N, 2, v, v0, u0, v0, dt );
      // recall project as advect behaves more accurate when the velocity field is mass conserving
	    project ( N, u, v, u0, v0 );
    }
  };

  // this is an app to draw a triangle, it takes some of its workings from the class "app"
  class fluids_app : public app {
    
    mat4t modelToWorld;
    mat4t cameraToWorld;

    // shader to draw a solid color
    color_shader color_shader_;
    fluid_shader fluid_shader_;
    fluid_shader3D fluid_shader_3D_;

    Solver solver;

    dynarray<float> vertices;
    dynarray<float> color;
	  float angle;

    dynarray <OctetMesh> octet_meshes_;

  public:

    // this is called when we construct the class
    fluids_app(int argc, char **argv) : app(argc, argv) {
    }

    // this is called once OpenGL is initialized
    void app_init() {
      // initialize the shader
      color_shader_.init();
      fluid_shader_.init();
      fluid_shader_3D_.init();

      srand(time(NULL));
      //mesh.init();
      // put the triangle at the center of the world
      modelToWorld.loadIdentity();

      // put the camera a short distance from the center, looking towards the triangle
      cameraToWorld.loadIdentity();
      cameraToWorld.translate(0.5f, 0.5f, 0.5f);
     
      for (int i=0 ; i<3 ; i++ ) {
        OctetMesh mesh;
        mesh.init();
        octet_meshes_.push_back( mesh);
      }

	    angle = 0.0f;

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


      //mouse_func();
      //motion_func();
      //idle_func();
      //display_func();
    }

void free_data ( void )
{
	if ( u ) free ( u );
	if ( v ) free ( v );
	if ( u_prev ) free ( u_prev );
	if ( v_prev ) free ( v_prev );
	if ( dens ) free ( dens );
	if ( dens_prev ) free ( dens_prev );
}

void clear_data ( void )
{
	int i, size=(N+2)*(N+2);

	for ( i=0 ; i<size ; i++ ) {
		u[i] = v[i] = u_prev[i] = v_prev[i] = dens[i] = dens_prev[i] = 0.0f;
	}
}

int allocate_data ( void )
{
	int size = (N+2)*(N+2);

	u			= (float *) malloc ( size*sizeof(float) );
	v			= (float *) malloc ( size*sizeof(float) );
	u_prev		= (float *) malloc ( size*sizeof(float) );
	v_prev		= (float *) malloc ( size*sizeof(float) );
	dens		= (float *) malloc ( size*sizeof(float) );	
	dens_prev	= (float *) malloc ( size*sizeof(float) );

	if ( !u || !v || !u_prev || !v_prev || !dens || !dens_prev ) {
		fprintf ( stderr, "cannot allocate data\n" );
		return ( 0 );
	}

	return ( 1 );
}

void draw_velocity ( void )
{
	int i, j;
	float x, y, h;

	h = 1.0f/N;

	glColor3f ( 1.0f, 1.0f, 1.0f );
	glLineWidth ( 1.0f );

	glBegin ( GL_LINES );

		for ( i=1 ; i<=N ; i++ ) {
			x = (i-0.5f)*h;
			for ( j=1 ; j<=N ; j++ ) {
				y = (j-0.5f)*h;

				glVertex2f ( x, y );
				glVertex2f ( x+u[IX(i,j)], y+v[IX(i,j)] );
			}
		}

	glEnd ();
}


void draw_density ( void )
{
	int i, j;
	float x, y, z, h, d00, d01, d10, d11;
  vec3 v1, v2, v3, v4, v5, v6, v7, v8;
  int currentMesh = 0;
  int numberIndices = 0;
  static int temp = 0;
	h = 1.0f/N;

		for ( i=0 ; i<=N ; i++ ) {  
			x = (i-0.5f)*h;
			for ( j=0 ; j<=N ; j++ ) {
				y = (j-0.5f)*h;
        for (int k = 0; k <= N; k++ ) {
          z = (k-0.5f)*h;

				  d00 = dens[IX(i,j)];
				  d01 = dens[IX(i,j+1)];
				  d10 = dens[IX(i+1,j)];
				  d11 = dens[IX(i+1,j+1)];

          d00 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
          d01 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
          d10 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
          d11 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

          if(numberIndices < 65000){
            octet_meshes_[currentMesh].addCube( x, y, z, h, vec4( d00, d01, d10, d11 ) );
            numberIndices+=36;
          }else{
            currentMesh++;
            octet_meshes_[currentMesh].addCube( x, y, z, h, vec4( d00, d01, d10, d11 ) );
            numberIndices=36;
          }
         // else 
         //   octet_mesh_.changeColor( vec4 ( d00, d01, d10, d11 ) );
        }
      
			}    
		}

    //mesh_->setBuffers();
   // if ( temp == 0 )
    for(int i=0; i<octet_meshes_.size(); i++) {
      octet_meshes_[i].bindMesh();
    }

    for ( int i=0; i<octet_meshes_.size(); i++ ) {
      printf("current mesh %d current indices %d\n", i, octet_meshes_[i].getIndices() );
    };

   // else
    //  octet_mesh_.bindColor();

    temp++;
}

void get_from_UI ( float * d, float * u, float * v )
{
	int i, j, size = (N+2)*(N+2);

	for ( i=0 ; i<size ; i++ ) {
		u[i] = v[i] = d[i] = 0.0f;
	}
  /*
  //d[IX(N,N-1)] = source;
  u[IX(N,N-1)] = force * (mx-omx);
	v[IX(N,N-1)] = force * (omy-my);

  printf ("omx %d mx %d omy %d my %d\n", omx, mx, omy, my);
  */
	if ( !is_key_down(key_lmb) && !is_key_down(key_rmb) ) return;

	i = (int)((       mx /(float)win_x)*N+1);
	j = (int)(((win_y-my)/(float)win_y)*N+1);

	if ( i<1 || i>N || j<1 || j>N ) return;

	if ( is_key_down(key_lmb) ) {
		u[IX(i,j)] = force * (mx-omx);
		v[IX(i,j)] = force * (omy-my);
	}

	if ( is_key_down(key_rmb) ) {
		d[IX(i,j)] = source;
	}

	omx = mx;
	omy = my;

	return;
}

void mouse_func (  ) // int button, int state, int x, int y
{
  if ( is_key_down(key_lmb) || is_key_down(key_rmb)) {
    get_mouse_pos(mx, my);
    omx = mx ;
	  omx = my ;
	}

	//mouse_down[button] = state == GLUT_DOWN;
}

void motion_func (  ) //int &x, int &y
{
	//mx = x;
	//my = y;
  get_mouse_pos( mx, my);
}

void idle_func ( void )
{
	get_from_UI ( dens_prev, u_prev, v_prev );
	solver.vel_step ( N, u, v, u_prev, v_prev, visc, dt );
	solver.dens_step ( N, dens, dens_prev, u, v, diff, dt );
}

void display_func ( void )
{
		if ( dvel ) draw_velocity ();
		else		draw_density ();
}

    void draw_world(int x, int y, int w, int h) {

      glViewport(x, y, w, h);
      
      glClearColor(0, 0, 0, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);
      
      //mouse_func();
      //motion_func();
      //idle_func();
      //display_func();
    
      //fluid_shader_3D_.render(modelToProjection, color);
    
      for(int i=0; i<octet_meshes_.size(); i++) {
        octet_meshes_[i].render( fluid_shader_3D_, cameraToWorld);
        //octet_meshes_[i].setRender(GL_LINES);
      }
      for ( int i=0; i<octet_meshes_.size(); i++ ) {
        //octet_meshes_[i].reset();
      }

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
    else if (is_key_down(key_space)){
		  cameraToWorld.loadIdentity();
      cameraToWorld.translate(0.5f, 0.5f, 0.5f);
    }
    }
  };
}
