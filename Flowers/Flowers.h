namespace octet {

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

      for ( int j = 0; j < num; j++ ) { // create as many as the input says      

        rotate.rotateZ(angle);

        for ( unsigned int i = 0; i < points.size()/3-1; i++ ) {  // I am not sure if the condition is correct!!!!!!!!!!!!!!!

          //if ( center.x() < points[3*i] || i == 0 ) {
          //if ( i <= ( points.size()/3-1)/2 ) { // like it

          // push vertices always underclockwise
          if ( i <= (points.size()/3-1)/2 ) {

            mesh->pushVertex( center*rotate );

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
          else {
            mesh->pushVertex( center*rotate );

            mesh->pushIndex();

            uv = findUVCoordinates( center, distX, distY);

            mesh->pushUV( uv.x(), uv.y() );

            // push 2st

            mesh->pushVertex( vec3( points[3*i+3], points[3*i+4], points[3*i+5]  ) *rotate );

            mesh->pushIndex();


            uv = findUVCoordinates( vec3( points[3*i+3], points[3*i+4], points[3*i+5] ), distX, distY);

            mesh->pushUV( uv.x(), uv.y() );

            // push 1st

            mesh->pushVertex( vec3( points[3*i], points[3*i+1], points[3*i+2]  ) *rotate );

            mesh->pushIndex();

            uv = findUVCoordinates( vec3( points[3*i], points[3*i+1], points[3*i+2]  ), distX, distY);

            mesh->pushUV( uv.x(), uv.y() );
         
          }

        }

      }

    }


    void createObjects () {

      dynarray<vec3> four_points;

      vec3 center;

      float distX = 0.0f,distY = 0.0f;

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

      float _minZ = 0.0f, _maxZ = 0.0f;

      for ( unsigned int i = 0; i < points.size(); i++ ) {

        if ( i%3 == 0 && points[i] < _minX )  // if it is x value

          _minX = points[i];

        else if ( i%3 == 0 && points[i] > _maxX )

          _maxX = points[i];

        if (i%3 == 1 && points[i] < _minY ) // if it is y value

          _minY = points[i];

        else if ( i%3 == 1 && points[i] > _maxY )

          _maxY = points[i];    

        if (i%3 == 2 && points[i] < _minZ ) // if it is y value
          _minZ = points[i];
        else if ( i%3 == 2 && points[i] > _maxZ )
          _maxZ = points[i];  

      }

      distX = abs(_maxX)-_minX;

      distY = abs(_maxY)-_minY;

      center[0]=(_maxX-abs(_minX))/2.0f;

      center[1]=(_maxY-abs(_minY))/2.0f;

      center[2]=(_maxZ-abs(_minZ))/2.0f;

    }


    vec2 findUVCoordinates ( const vec3 &v_, float distX, float distY) {

      float n = 1.0f/4.0f;

		  // if I divide each time x value with the distance then I have the correct coordinate on [0,1]

      // v should be between 0.0f - 0.16f

      if ( type == "leaf")

		    return vec2( n + n*2.0f*abs(v_.x() + distX/2.0f)/(distX*2.0f), (abs(v_.y())/distY * 0.16f) );

        //vec2( n + n*2.0f*abs(v_.x())/distX, abs(v_.y())/distY );

      else 

        return vec2( 3*n + n*2.0f*(abs(v_.x()+distX/2.0f)/(distX*2.0f)), (abs(v_.y())/distY * 0.16f) );

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


    void set ( float l, int k, float posZ_, int num_, string type_, float angle_, mat4t &modelToWorld_, float noiseX_= 0.0f, float noiseY_= 0.0f, float noiseZ_ = 0.0f ) {

      num = num_;

      length = l;

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

  




} // namespace octet
