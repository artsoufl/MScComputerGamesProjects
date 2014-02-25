
// Reference: http://stackoverflow.com/questions/1644868/c-define-macro-for-debug-printing
#ifdef _DEBUG
#define DEBUG_PRINT(fmt, ...) do{ printf(fmt, __VA_ARGS__); } while(0)
#endif
#ifndef _DEBUG
#define DEBUG_PRINT(fmt, ...)
#endif

namespace octet {

  // --
  
  template <typename T>
  T clamp(const T& a, const T& min, const T& max) {
    return std::max(std::min(a, max), min);
  };

  template <typename T>
  T lerp(const T& begin, const T& end, float t) {
    return ((1.f - t) * begin) + (t * end);
  };

  // Reference: http://en.wikipedia.org/wiki/Smoothstep
  // Reference: http://sol.gfxile.net/interpolation/
  template <typename T>
  T smoothstep(const T& begin, const T& end, float t) {
    return lerp(begin, end, (t * t * (3.0f - 2.0f * t)));
  };
  
  // Reference: http://en.wikipedia.org/wiki/Smoothstep
  template <typename T>
  T smootherstep(const T& begin, const T& end, float t) {
    return lerp(begin, end, (t * t * t * (t * ((t * 6) - 15) + 10)));
  };

  template <typename T>
  inline T square(const T& t) {
    return t * t;
  };
  
  template <typename T>
  inline T cube(const T& t) {
    return t * t * t;
  };

  float pi() {
    return 3.14159265f;
  };
  
  /**
   *  @return a random number between 0.0f and 1.0f
   **/
  float randf() {
    return ((float) rand()) / ((float) RAND_MAX);
  };

  // --

  struct Colour {
    // Reference: app_utils.h
    static vec4 parse(const char* hex) {
      unsigned val = 0;
      unsigned ndigits = 0;
      for (int i = 0; hex[i]; ++i) {
        char c = toupper(hex[i]);
        val = val * 16 + ( ( c <= '9' ? c - '0' : c - 'A' + 10 ) & 0x0f );
        ndigits++;
      }

      if (ndigits == 8) {
        val >>= 8;
      }
      
      return vec4(
        ((val >> 16) & 0xFF) / 255.f,
        ((val >> 8) & 0xFF) / 255.f,
        ((val >> 0) & 0xFF) / 255.f,
        1.f
      );
    };
  };

  // --
  
  /**
    *  Utility class which manages input.
    *  Used to delay key-press.
    *
    *  @author Brian Gatt
    */
  class InputManager {
  private:
    app* _app;

    // The key delay amount per poll request
    unsigned int _keyDelay;

    // Delay accumulator for processing key presses
    unsigned int _delay;

    // Non-copyable entity
    InputManager(const InputManager&);
    InputManager& operator=(const InputManager&);

  public:
    typedef unsigned int keycode;

    explicit InputManager(app* app = NULL, unsigned int keyDelay = 5) :
      _app(app),
      _keyDelay(keyDelay),
      _delay(0) {
    };

    ~InputManager() {
    };

    /**
      *  Attaches this manager instance with the
      *  specified app instance.
      *  @param app The app which requires input handling
      */
    void attach(app* app) {
      _app = app;
      _delay = 0;
    };
      
    /**
      *  Specify the key delay
      *  @param keyDelay The key delay
      */
    void setKeyDelay(unsigned int keyDelay) {
      _keyDelay = keyDelay;
      _delay = 0;
    };
      
    /**
      *  @param key The keycode to check for
      *  @return true if the specified key is pressed; false otherwise
      */
    bool isKeyDown(keycode key) const {
      return _delay == 0 && _app != NULL && _app->is_key_down(key);
    };
      
    /**
      *  @param key The keycode to check for
      *  @return true if the specified key is pressed and handled internally; false otherwise
      */
    bool processKey(keycode key) {
      if (isKeyDown(key)) {
        _delay = _keyDelay;
        return true;
      };

      return false;
    };
      
    /**
      *  Check for new key presses
      */
    void poll() {
      _delay = (_delay == 0 ? 0 : _delay - 1);
    };
  };

  
  // --

  /**
   *  Camera manipulation
   *
   *  @author Artemis Tsouflidou
   **/
	class Camera {
		mat4t cameraInstance;
		float angle, angleX;
		int preMouseX, preMouseY;
		int mouseX, mouseY;
		enum { MAX_MOVE = 2, MAX_ROTATION = 5 };

	public:
		Camera () { angle = 0.0f; angleX = 0.0f; cameraInstance.loadIdentity(); cameraInstance.translate(0.0f, 0.0f, 7.0f); }
		// if I don't return reerence then I can't see anything because it is not modified that way
		mat4t& getCamera () { return cameraInstance; }
		float& getCameraAngle () { return angle; }
		void setCameraAngle ( float _angle ) { angle = _angle; }
		void translateLeft () { cameraInstance.translate(-1.0f/MAX_MOVE, 0.0f, 0.0f); }
		void translateRight () { cameraInstance.translate( 1.0f/MAX_MOVE, 0.0f, 0.0f); }
		void translateForward () { cameraInstance.translate( 0.0f, 0.0f, -1.0f/MAX_MOVE); }
		void translateBack () { cameraInstance.translate( 0.0f, 0.0f, 1.0f/MAX_MOVE); }
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
      angle = 0.0f;
      angleX = 0.0f;
		}
	};
  
  // --

  /**
   *  Spline definition
   *
   *  @author Brian Gatt
   */ 
  class SplineDefinition {
  private:
    float _length;
    float _radius;
    unsigned int _knots;

    vec4 _initialColour;
    vec4 _endColour;

  public:
    SplineDefinition() :
      _length(0.f),
      _radius(0.f),
      _knots(0),
      _initialColour(getDefaultColour()),
      _endColour(getDefaultColour()) {
    };
      
    /**
     *  @return the length of the bounding cylinder of the spline
     **/
    float getLength() const {
      return _length;
    };
    
    void setLength(float length) {
      _length = length;
    };
    
    /**
     *  @return the bounding cylinder radius of the spline
     **/
    float getRadius() const {
      return _radius;
    };
    
    void setRadius(float radius) {
      _radius = radius;
    };
    
    /**
     *  @return the number of intermediary knots the spline will consist of
     **/
    unsigned int getKnotCount() const {
      return _knots;
    };
    
    void setKnotCount(unsigned int knots) {
      _knots = knots;
    };
    
    /**
     *  @return the initial colour value
     **/
    const vec4& getInitialColour() const {
      return _initialColour;
    };
    
    void setInitialColour(const vec4& initialColour) {
      _initialColour = initialColour;
    };
    
    /**
     *  @return the end colour value
     **/
    const vec4& getEndColour() const {
      return _endColour;
    };
    
    void setEndColour(const vec4& endColour) {
      _endColour = endColour;
    };

    static const vec4& getDefaultColour() {
      static vec4 white(1.f);
      return white;
    };
  };

  // --

  /** 
   *  Splines configuration parameters
   *
   *  @author Brian Gatt
   */
  class SplineConfigParams {
  private:
    unsigned int _count;
    dynarray<SplineDefinition> _definitions;

  public:
    SplineConfigParams() :
      _count(1) {
    };

    /**
     *  @return the number of splines to be generated
     **/
    unsigned int getCount() const {
      return _count;
    };
    
    void setCount(unsigned int count) {
      _count = count;
    };
    
    const dynarray<SplineDefinition>& getDefinitions() const {
      return _definitions;
    };
    
    dynarray<SplineDefinition>& getDefinitions() {
      return _definitions;
    };
  };
  
  // --
  
  /** 
   *  Spline XML Configuration parser
   *
   *  @author Brian Gatt
   */
  class SplineXMLConfig {
  private:
    const char* _path;
    
    void parseSplineDefinition(const TiXmlElement& element, SplineDefinition& definition) {
      if (strcmp(element.Value(), "length") == 0) {
        definition.setLength((float) ::atof(element.GetText()));
      } else if (strcmp(element.Value(), "radius") == 0) {
        definition.setRadius((float) ::atof(element.GetText()));
      } else if (strcmp(element.Value(), "knots") == 0) {
        definition.setKnotCount(::atoi(element.GetText()));
      } else if (strcmp(element.Value(), "initial-colour") == 0) {
        definition.setInitialColour(Colour::parse(element.GetText()));
      } else if (strcmp(element.Value(), "end-colour") == 0) {
        definition.setEndColour(Colour::parse(element.GetText()));
      }
    };

    void parseSplineDefinition(const TiXmlElement& element, SplineConfigParams& params) {
      if (strcmp(element.Value(), "spline") == 0) {
        SplineDefinition definition;

        const TiXmlElement* child = element.FirstChildElement();
        for (; child != NULL; child = child->NextSiblingElement()) {
          parseSplineDefinition(*child, definition);
        };

        params.getDefinitions().push_back(definition);
      }
    };

    // Parse an XML Spline (single) parameter representation
    void parseParam(const TiXmlElement& element, SplineConfigParams& params) {
      if (strcmp(element.Value(), "spline-count") == 0) {
        params.setCount(::atoi(element.GetText()));
      } else if (strcmp(element.Value(), "spline-definitions") == 0) {
        const TiXmlElement* child = element.FirstChildElement();
        for (; child != NULL; child = child->NextSiblingElement()) {
          parseSplineDefinition(*child, params);
        };
      }
    };
    
    // Parse an XML splines configuration
    SplineConfigParams parse(const TiXmlDocument& xml) {
      SplineConfigParams params;
    
      const TiXmlElement* root = xml.FirstChildElement();
      if (root != NULL && strcmp(root->Value(), "splines") == 0) {
        const TiXmlElement* child = root->FirstChildElement();

        for (; child != NULL; child = child->NextSiblingElement()) {
          parseParam(*child, params);
        };
      }

      return params;
    };

  public:
    explicit SplineXMLConfig(const char* path) :
      _path(path) {
    };

    SplineConfigParams parse() {
      TiXmlDocument doc;
      if (doc.LoadFile(_path)) {
        return parse(doc);
      }

      return SplineConfigParams();
    };
  };

  // --

  /**
   *  A SampledSpline is a spline representation
   *  as a series of discrete points.
   *
   *  @author Brian Gatt
   **/
  class SampledSpline {
  private:
    dynarray<vec3> _points;
    
  public:
    SampledSpline() {
    };
    
    ~SampledSpline() {
    };
    
    /**
     *  Reset the structure to its base state.
     **/
    SampledSpline& reset() {
      _points.reset();
      return *this;
    };
    
    /**
     *  Swap the internal state of splines
     **/
    void swap(SampledSpline& rhs) {
      _points.swap(rhs._points);
    };
    
    /**
     *  Accessor operator.
     *  Retrieves an indexed point from the SampledSpline.
     **/
    const vec3& operator[](size_t i) const {
      return _points[i];
    };
    
    vec3& operator[](size_t i) {
      return _points[i];
    };
    
    /**
     *  Adds a sampled point to this SampledSpline instance.
     *  NOTE: Points are added successively and should follow
     *        an implicit ordering.
     **/
    SampledSpline& addPoint(const vec3& point) {
      _points.push_back(point);
      //DEBUG_PRINT("[%.2f, %.2f, %.2f]\n", point.x(), point.y(), point.z());
      return *this;
    };
    
    SampledSpline& addPoint(const vec2& point) {
      return addPoint(vec3(point.x(), point.y(), 0.f));
    };

    /**
     *  Retrieves the current amount of sampled points recorded.
     **/
    size_t getPointCount() const {
      return _points.size();
    };

    // read and write
    dynarray<vec3>& get () {
      return _points;
    }

    // only read from it
    const dynarray<vec3>& get () const{
      return _points;
    }
  };

  // --
  
  /**
   *  Catmull Rom Spline
   *
   *  @author Artemis Tsouflidou
   *  @author Brian Gatt
   **/
  class Catmull_Rom_Spline {
    vec3 start_control_point;
    vec3 end_control_point;

		dynarray<vec3> start_end_points;

    // Sanitizes the t0 and t1 parameters to meet requirements
    void sanitize(float& t0, float& t1) const {
      // t0 should be less than t1
      if (t0 > t1) {
        std::swap(t0, t1);
      }

      // Clamp t0 to 0.0f
      t0 = std::max(t0, 0.0f);
      // Clamp t1 to the number of knots in the spline
      t1 = std::min(t1, (float) (start_end_points.size() - 1));
    };

    // Retrieves the spline parameter at the provided index
    const vec3* getPoint(int index) const {
      if (index == -1) {
        return &start_control_point;
      } else if (index == start_end_points.size()) {
        return &end_control_point;
      } else if (index > -1 && (unsigned int) index < start_end_points.size()) {
        return &start_end_points[index];
      }

      return NULL;
    };

    // Reference: http://www.codeproject.com/Articles/30838/Overhauser-Catmull-Rom-Splines-for-Camera-Animatio
		vec3 formula (float t, const vec3 &p0, const vec3 &p1, const vec3 &p2, const vec3 &p3) const {
			float t2 = square(t);
			float t3 = cube(t); 

			// polynomials
			float b0 = 0.5f*(-t3 + 2*t2 - t);
			float b1 = 0.5f*(3*t3 - 5*t2 + 2);
			float b2 = 0.5f*(-3*t3 + 4*t2 + t);
			float b3 = 0.5f*(t3 - t2);

			// desired point
			return vec3(
        b0*p0.x() + b1*p1.x() + b2*p2.x() + b3*p3.x(), // x
        b0*p0.y() + b1*p1.y() + b2*p2.y() + b3*p3.y(), // y
        b0*p0.z() + b1*p1.z() + b2*p2.z() + b3*p3.z()  // z
      );
		}
    
    vec3 formula(float t, const vec3* p0, const vec3* p1, const vec3* p2, const vec3* p3) const {
      // In case p1 is the end point, p2 will be the end control point and p3 will be NULL
      // In such cases, simply returning p1 will suffice
      if (p3 == NULL && (p0 != NULL && p1 != NULL && p2 != NULL)) {
        return *p1;
      }

      return formula(t, *p0, *p1, *p2, *p3);
    }
		
	public:
		Catmull_Rom_Spline() :
      start_control_point(0.f),
      end_control_point(0.f) {
    }
      
		void reset () {
		  start_control_point = vec3(0.f);
      end_control_point = vec3(0.f);

      start_end_points.reset();
		}

    const vec3& getStartControlPoint() const {
      return start_control_point;
    }

    void setStartControlPoint(const vec3& startControlPoint) {
      start_control_point = startControlPoint;
    }
    
    const vec3& getEndControlPoint() const {
      return end_control_point;
    }

    void setEndControlPoint(const vec3& endControlPoint) {
      end_control_point = endControlPoint;
    }

		dynarray<vec3>& getStartEndPoints () {
			return start_end_points;
		}

    const dynarray<vec3>& getStartEndPoints () const {
			return start_end_points;
		}
    
    /**
     *  Sample the whole curve from start to finish using the specified
     *  uniform interval
     *  @param sampleCount the total number of samples which will be taken
     *  @return the sampled spline
     **/
    SampledSpline sample(unsigned int sampleCount) const {
      return sample(sampleCount, 0.0f, (float) (start_end_points.size() - 1));
    };
    
    /**
     *  Sample the specified part of the curve using the specified
     *  uniform interval
     *  @param sampleCount the total number of samples which will be taken
     *  @param t0 the initial parametric point on the curve
     *  @param t1 the ending parametric point on the curve
     *  @return the sampled spline
     **/
    SampledSpline sample(unsigned int sampleCount, float t0, float t1) const {
      return sample(sampleCount, t0, t1, SampledSpline());
    };
    
    /**
     *  Sample the whole curve from start to finish using the specified
     *  uniform interval
     *  @param sampleCount the total number of samples which will be taken
     *  @param samplings the container which will host the sampled results
     *  @return the parameter samplings
     **/
    SampledSpline& sample(unsigned int sampleCount, SampledSpline& samplings) const {
      return sample(sampleCount, 0.0f, (float) (start_end_points.size() - 1), samplings);
    };
    
    /**
     *  Sample the whole curve from start to finish using the specified
     *  uniform interval
     *  @param sampleCount the total number of samples which will be taken
     *  @param t0 the initial parametric point on the curve
     *  @param t1 the ending parametric point on the curve
     *  @param samplings the container which will host the sampled results
     *  @return the parameter samplings
     **/
    SampledSpline& sample(unsigned int sampleCount, float t0, float t1, SampledSpline& samplings) const {
      // Sampling cannot occur properly unless one knot has been specified
      if (start_end_points.size() < 2) {
        return samplings;
      }

      // Sanitize the parameters for correct execution
      sanitize(t0, t1);
      
      float division = (t1 - t0) / (float) sampleCount;
      for (; t0 < t1; t0 += division) {
        // Reference: 3D Math Primer for Graphics and Game Development (Parberry, Dunn) - Chapter 13.6.1
        float segment = ::floor(t0);
        float local = t0 - segment;

        unsigned int knotIndex = (unsigned int) segment;

        samplings.addPoint( formula(local, getPoint(knotIndex - 1), getPoint(knotIndex), getPoint(knotIndex + 1), getPoint(knotIndex + 2)) );
      }

      return samplings;
    };
    
    void swap(Catmull_Rom_Spline& spline) {
      std::swap(start_control_point, spline.start_control_point);
      std::swap(end_control_point, spline.end_control_point);

      start_end_points.swap(spline.start_end_points);
    };

#ifdef _DEBUG
    void dump() const {
      
      printf("Spline [");
      printf("[%2.f, %.2f, %.2f], ", getStartControlPoint().x(), getStartControlPoint().y(), getStartControlPoint().z());
      for (size_t i = 0; i < getStartEndPoints().size(); ++i) {
        printf("[%2.f, %.2f, %.2f], ", getStartEndPoints()[i].x(), getStartEndPoints()[i].y(), getStartEndPoints()[i].z());
      }
      printf("[%2.f, %.2f, %.2f]]\n", getEndControlPoint().x(), getEndControlPoint().y(), getEndControlPoint().z());

    };
#endif
	};

  // --
  
  /**
   *  Spline Mesh and Renderer
   *
   *  @author Artemis Tsouflidou
   *  @author Brian Gatt
   **/
  class SplineRenderer {
  private:
    dynarray<GLfloat> _arrayBuffer;
    dynarray<GLuint> _indexBuffer;
    
    GLuint _glArrayBuffer;
    GLuint _glIndexBuffer;

    GLenum _drawMode;

  protected:
    void pushData(float datum) {
      _arrayBuffer.push_back(datum);
    };
    
    void pushData(const vec3& datum) {
      pushData(datum.x());
      pushData(datum.y());
      pushData(datum.z());
    };

    void pushData(const vec4& datum) {
      pushData(datum.x());
      pushData(datum.y());
      pushData(datum.z());
      pushData(datum.w());
    };

    void pushIndex(GLuint index) {
      _indexBuffer.push_back(index);
    };
    
    void pushPoint(const vec3& point, const vec4& colour) {
      // Interleaving data within the same buffer
      pushData(point);
      pushData(colour);
    };

    GLuint nextIndex() const {
      return (_arrayBuffer.is_empty() ? 0 : _indexBuffer.back() + 1);
    };

    SplineRenderer(GLenum drawMode) :
      _glArrayBuffer(0),
      _glIndexBuffer(0),
      _drawMode(drawMode) {
    };

  public:
    virtual ~SplineRenderer() {
      if (_glArrayBuffer != 0) {
        glDeleteBuffers(1, &_glArrayBuffer);
        _glArrayBuffer = 0;
      }

      if (_glIndexBuffer != 0) {
        glDeleteBuffers(1, &_glIndexBuffer);
        _glIndexBuffer = 0;
      }
    };

    void reset() {
      _arrayBuffer.reset();
      _indexBuffer.reset();
    };

    void add(const SampledSpline& spline) {
      static vec4 white(1.f);
      add(spline, white, white);
    };

    void add(const SampledSpline& spline, const vec4& initialColour, const vec4& endColour) {
      add(spline, 0, spline.getPointCount(), initialColour, endColour);
    };

    virtual void add(const SampledSpline& spline, size_t start, size_t end, const vec4& initialColour, const vec4& endColour) = 0;

    SplineRenderer& commit() {
      if (_glArrayBuffer == 0) {
        glGenBuffers(1, &_glArrayBuffer);
      }

      if (_glIndexBuffer == 0) {
        glGenBuffers(1, &_glIndexBuffer);
      }

      glBindBuffer(GL_ARRAY_BUFFER, _glArrayBuffer);
      glBufferData(GL_ARRAY_BUFFER, _arrayBuffer.size() * sizeof(GLfloat), (void*) _arrayBuffer.data(), GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _glIndexBuffer);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer.size() * sizeof(GLuint), (void*) _indexBuffer.data(), GL_STATIC_DRAW);

      return *this;
    };

    // Reference: http://openglbook.com/the-book/chapter-3-index-buffer-objects-and-primitive-types/
    void draw() {
      // Bind the ARRAY_BUFFER which contains the necessary data
      glBindBuffer(GL_ARRAY_BUFFER, _glArrayBuffer);
      // Bind the attribute_pos 'position' shader attribute using data from the currently bound ARRAY_BUFFER starting at location 0
      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, (3 + 4) * sizeof(GLfloat), (void*)0);
      // Enable the previously specified attribute array
      glEnableVertexAttribArray(attribute_pos);
      
      glVertexAttribPointer(attribute_color, 4, GL_FLOAT, GL_FALSE, (4 + 3) * sizeof(GLfloat), (void*) (0 + (3 * sizeof(GLfloat))));
      glEnableVertexAttribArray(attribute_color);
      
      // Bind the ELEMENT_ARRAY_BUFFER which contains the necessary indices
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _glIndexBuffer);
      // glDrawElements draws accordingly to the currently bound ELEMENT_ARRAY_BUFFER indices starting at location 0
      glDrawElements(_drawMode, _indexBuffer.size(), GL_UNSIGNED_INT, (void*)0);
    };

  };

  // --

  class LineSplineRenderer : public SplineRenderer {
  public:
    LineSplineRenderer() : SplineRenderer(GL_LINES) {
    };
    
    void add(const SampledSpline& spline, size_t start, size_t end, const vec4& initialColour, const vec4& endColour) {
      GLuint index = nextIndex();
      for (size_t i = 0; i < (end - start); ++i, ++index) {
        if (i > 1) {
          pushIndex(index - 1);
        }

        pushPoint(spline[start + i], smoothstep(initialColour, endColour, ((float) i / (float) (end - start))));
        pushIndex(index);
      }
    };
  };

  // --

  class QuadSplineRenderer : public SplineRenderer {
  private:
    float _width;

  public:
    QuadSplineRenderer() : SplineRenderer(GL_TRIANGLES),
      _width(0.1f) {
    };

    void setLineWidth(float width) {
      _width = width;
    };

    float getLineWidth() const {
      return _width;
    };

    void add(const SampledSpline& spline, size_t start, size_t end, const vec4& initialColour, const vec4& endColour) {
      GLuint index = nextIndex();
      for (size_t i = 0; i < (end - start); ++i) {
        vec4 colour = smootherstep(initialColour, endColour, ((float) i / (float) (end - start)));

        if (i > 1) {
          pushIndex(index - 2);
          pushIndex(index - 1);
        }

        vec3 lhs = spline[start + i];
        lhs.x() -= getLineWidth();

        pushPoint(lhs, colour);
        pushIndex(index++);

        if (i > 0) {
          pushIndex(index - 2);
          pushIndex(index - 1);
        }

        vec3 rhs = spline[start + i];
        rhs.x() += getLineWidth();

        pushPoint(rhs, colour);
        pushIndex(index++);
      }
    };
  };

  // --
  
  /** 
   *  Quaternions rotate 
   *
   *  @author Artemis Tsouflidou
   */

  // https://github.com/BSVino/MathForGameDevelopers/tree/quaternions/math
  // http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/
  // http://willperone.net/Code/quaternion.php
  class Quaternion {

		vec4 vector;
		// here are stored points after rotation with Quaternions so the destination that should be reached via slerp
		dynarray<vec3> destinationPoints;
		float t; // step in interpolation

		// rotate point v around quartnion/axis quart
		void rotate ( const vec4 &quart, const vec3 &v ) {
			
			quat Quaternion = vec4 ( quart ); 

			normalize( Quaternion );
			transfrom_Quaternion ( Quaternion );

			vec4 p = vec4(v, 0.0f);

			vector = Quaternion.rotate(p); // call function of Quaternions that implements this: q*p*(q*)
			// save the destination points
			destinationPoints.push_back( vec3(vector.x(), vector.y(), vector.z()) ); 
		}

	public:
		Quaternion (): t(0.0f) {}
    
		bool slerpRotateSmooth(dynarray<vec3> &v1) {
      return slerpRotateSmooth(v1, v1);
		}

		// v1 are the initial points, v2 is the destination array which will contain the computed points
		bool slerpRotateSmooth(const dynarray<vec3> &v1, dynarray<vec3> &v2) {
      // Ensure that the destination arrary is initialized
      // to the appropriate size
      v2.resize(v1.size());

				for ( unsigned int j = 0; j < v1.size(); j++) { // for every point
					// 1st argument is the start orientation point, 2nd argument is the end orientation point
					v2[j] = slerp( vec4( v1[j], 0.0f), vec4(destinationPoints[j], 0.0f), t);
				}		
				t += 1.0f / 10.0f;  
				if ( t > 1.0f) {
					t = 0.0f;
					return false; // rotation stops
				}
				return true;
		}

		vec3 slerp( const vec3 &v1, const vec3 &v2, float t) {		
				return slerp( vec4( v1, 0.0f), vec4(v2, 0.0f), t);
		}

		vec3 slerp (const vec4 &t1, const vec4 &t2, float t) {
			float d, angle;
			vec4 q1 = t1;
      vec4 q2 = t2;
			vec4 v;

			d = dot(q1, q2);///(l1*l2);

			if ( d < 0.0f ) {
				d = -d;
				q2 = -t2;
			}

			// angle = 0 cause problem!!!!
      if ( d > 0.9999f ) {
				//use lerp to avoid division with zero ( if angle is close to 0.0f then we divide with zero in slerp)
				v = lerp ( q1, q2, t );
      } else {
				angle = acosf(d);
				v = (q1*sinf(angle*(1-t)) + q2*sinf(angle*t))/sinf(angle);
			}

			return vec3(v.x(), v.y(), v.z());
		}

		
		// dot product between 2 vectors, don't calculate w value in the dot product
		float dot ( const vec4 &v1, const vec4 &v2 ) {
			return vec3(v1.x(), v1.y(), v1.z()).dot(vec3(v2.x(), v2.y(), v2.z()));
		}

		// first argument is the quartanion/axis of rotation
		// second argument is the vectors that we want to rotate
		void rotateWithQuaternion ( vec4 &quart, dynarray<vec3> &v ) {
			destinationPoints.reset();
			for ( unsigned int i = 0; i < v.size(); i++) { // for all points in the v array
				// rotate points with Quaternions
				rotate( quart, v[i] ); // after this we have the destination rotate points
			}
			// interpolate values between the starting rotation point and the destination rotated point
			//slerpRotateSmooth(v);
		}
		
		// length of vector part, dont take into account w value
		float length ( const vec4 &t ) {
			return sqrt( t.x()*t.x() + t.y()*t.y() + t.z()*t.z() );
		}

		// normalize the vector part so without taking into account the w
		vec4 normalize (const vec4 &t) {
      vec4 n = t;
			float len = sqrt( t.x()*t.x() + t.y()*t.y() + t.z()*t.z() );
			// if it is not already normalized then normalize it
			if ( len != 1.0f && len!=0.0f ) {
				n.x() = t.x() / len;
				n.y() = t.y() / len;
				n.z() = t.z() / len;
			}
      return n;
		}

		// Quaternion is equal to ( cos(θ/2), v*sin(θ/2) ) where v is the vector part
		void transfrom_Quaternion( vec4 &t) {
      float temp = 3.14159f/180.0f/2.0f; // degrees to radians and make angle = angle/2
			t.x() = t.x() * sin(t.w()*temp); 
			t.y() = t.y() * sin(t.w()*temp);
			t.z() = t.z() * sin(t.w()*temp);
			t.w() = cos(t.w()*temp);
		}
	};



  // --
  
  /** 
   *  Spline Ball. A sphere consisting of multiple spline instances
   *  which animate accordingly.
   *
   *  Reference: http://illogictree.com/blog/projects/
   *  Reference: http://vimeo.com/17319264
   * 
   *  @author Artemis Tsouflidou
   *  @author Brian Gatt
   */
  class SplineBall {
  public:
    enum AnimationMode { LERP, SLERP };
    enum RenderMode { LINE, QUAD };

  private:
    /**
     *  A single spline instance within the spline ball
     *
     *  @author Artemis Tsouflidou
     *  @author Brian Gatt
     **/
    class Spline {
    private:
      // The associated SplineDefinition instance reference
      const SplineDefinition* _definition;

      // The orientation of this spline i.e. the modelToWorld matrix
      mat4t _orientation;

      // Spline Key Frame References
      Catmull_Rom_Spline _start_spline;
      Catmull_Rom_Spline _end_spline;

      // The spline instance currently being animated
      Catmull_Rom_Spline _anim_spline;
      // Animation (interpolation) state
      float _t;

      vec3 interpolateControlPoint(const vec3& start, const vec3& end) {
        return lerp(start, end, std::min(_t, 1.0f));
      };
      
      vec3 interpolateKnot(const vec3& start, const vec3& end) {
        return smoothstep(start, end, std::min(_t, 1.0f));
      };

    public:
      explicit Spline(const SplineDefinition* definition = NULL) :
        _definition(definition),
        _orientation(1.f),
        _t(0.f) {
      };
        
      Spline(const SplineDefinition* definition, const mat4t& orientation) :
        _definition(definition),
        _orientation(orientation),
        _t(0.f) {
      };

      ~Spline() {
      };

      const SplineDefinition* getDefinition() const {
        return _definition;
      };

      void setDefinition(const SplineDefinition* definition) {
        _definition = definition;
      };
      
      Catmull_Rom_Spline& getStartSpline() {
        return _start_spline;
      };

      const Catmull_Rom_Spline& getStartSpline() const {
        return _start_spline;
      };

      Catmull_Rom_Spline& getEndSpline() {
        return _end_spline;
      };

      const Catmull_Rom_Spline& getEndSpline() const {
        return _end_spline;
      };
      
      Catmull_Rom_Spline& getAnimationSplineState() {
        return _anim_spline;
      };

      const Catmull_Rom_Spline& getAnimationSplineState() const {
        return _anim_spline;
      };

      mat4t& getModelToWorld() {
        return _orientation;
      };

      const mat4t& getModelToWorld() const {
        return _orientation;
      };

      float getInterpolationState() const {
        return _t;
      };
      
      const vec4& getInitialColour() const {
        return (_definition == NULL ? SplineDefinition::getDefaultColour() : _definition->getInitialColour());
      };

      const vec4& getEndColour() const {
        return (_definition == NULL ? getInitialColour() : _definition->getEndColour());
      };

      bool isAnimationComplete() const {
        return _t >= 1.0f;
      };

      void restartAnimation() {
        _t = 0;
      };

      void step(float t) {
        _t += t;
        
        _anim_spline.setStartControlPoint(
          interpolateControlPoint(
            _start_spline.getStartControlPoint(),
            _end_spline.getStartControlPoint()
          )
        );

        _anim_spline.setEndControlPoint(
          interpolateControlPoint(
            _start_spline.getEndControlPoint(),
            _end_spline.getEndControlPoint()
          )
        );

        for (size_t i = 0; i < _anim_spline.getStartEndPoints().size(); ++i) {
          _anim_spline.getStartEndPoints()[i] = interpolateKnot(
            _start_spline.getStartEndPoints()[i],
            _end_spline.getStartEndPoints()[i]
          );
        }
      };
    };

    // The configuration parameters
	  SplineConfigParams file;
    // The animating splines which make the ball
	  dynarray<Spline> splines;

    // The model orientation/location matrix (for the whole ball) in relation to the world
    mat4t modelToWorld;

    SampledSpline _samples;
    SampledSpline _slerpSamples;

    LineSplineRenderer _lineRenderer;
    QuadSplineRenderer _quadRenderer;
    SplineRenderer* _renderer;

    AnimationMode _mode;
	  Quaternion quart;
	  bool rotation;

    vec3 rotationAxis;

	  void createObjects () {
      // For the number of specified splines in the ball...
		  for (unsigned int i = 0; i < file.getCount(); i++) {
        // Select a random spline definition
        unsigned int index = rand() % file.getDefinitions().size();

				// create a CatMullRom spline according to the spline definition
        createSpline ( file.getDefinitions()[index] );
		  }
	  }

    mat4t generateSplineOrientation() const {
      mat4t orientation(1.f);

      orientation.rotateX((float) (rand() % 360));
      orientation.rotateY((float) (rand() % 360));
      orientation.rotateZ((float) (rand() % 360));

      return orientation;
    };

    // Returns a random point within the circle of the specified radius
    // Reference: http://stackoverflow.com/questions/5837572/generate-a-random-point-within-a-circle-uniformly
    vec2 randCircle(float radius) const {
      float t = 2 * pi() * randf();
      float u = randf() + randf();
      float r = (u > 1.f ? 2.f - u : u) * radius;

      return vec2(r * cos(t), r * sin(t));
    };

/*
    // Returns a random point on the circumference of a circle of the specified radius
    vec2 randCircleCircumference(float radius) const {
      float t = 2 * pi() * randf();
      return vec2(radius * cos(t), radius * sin(t));
    };
*/

    // Generates a random point along the xz-plane at y bounded by the circle defined
    // at the origin and extends with the specified bounding radius
    vec3 generateRandomPoint(float boundingRadius, float y) const {
      vec2 point = randCircle(boundingRadius);
      return vec3(point.x(), y, point.y());
    };

    vec3 generateRandomPoint(float boundingRadius, float y, const mat4t& orientation) const {
      vec3 rpoint = generateRandomPoint(boundingRadius, y);
      return orientation.rmul(vec4(rpoint.x(), rpoint.y(), rpoint.z(), 1.f)).xyz();
    };
  
/*
    // Generates a random knot along the edges of xz-plane at y bounded by the circle defined
    // at the origin and extends with the specified bounding radius
    vec3 generateRandomKnot(float boundingRadius, float y) const {
      vec2 point = randCircleCircumference(boundingRadius);
      return vec3(point.x(), y, point.y());
    };

    vec3 generateRandomKnot(float boundingRadius, float y, const mat4t& orientation) const {
      vec3 rpoint = generateRandomKnot(boundingRadius, y);
      return orientation.rmul(vec4(rpoint.x(), rpoint.y(), rpoint.z(), 1.f)).xyz();
    };
*/

	  // randomly generated points depending on length parameter of XML file
	  // all end points have length distance from (0,0)
    void generateSplinePoints(Catmull_Rom_Spline& spline, const SplineDefinition& definition, const mat4t& orientation) const {
      spline.getStartEndPoints().reset();

      // All splines will start at the origin
      spline.getStartEndPoints().push_back(vec3(0.f));

      // The length available for intermediary knots
      float knotDivision = (definition.getKnotCount() == 0 ? 0.f : definition.getLength() / (float) definition.getKnotCount());

      // Generate a random knot
      for (size_t i = 0; i < definition.getKnotCount(); ++i) {
        // Generate a random y value within the knot space
        float y = knotDivision * randf();
        // Offset the y value by knotSpace according to the current knot
        y += knotDivision * (float) (i);

        // Generate a random point on the xz-plane at the specified y position
        spline.getStartEndPoints().push_back(generateRandomPoint(definition.getRadius(), y, orientation));

/*
        // Generate a random point on the edges of the xz-plane at the specified y position
        //spline.getStartEndPoints().push_back(generateRandomKnot(definition.getRadius(), y, orientation));
*/
      }

      // The spline end point
      spline.getStartEndPoints().push_back(generateRandomPoint(definition.getRadius(), definition.getLength(), orientation));
    }

    void generateSplinePoints(Catmull_Rom_Spline& catMullSpline, const Spline& spline) const {
      generateSplinePoints(catMullSpline, *spline.getDefinition(), spline.getModelToWorld());
    };

    vec3 generateRandomPoint(float boundingRadius) const {
      vec3 point = generateRandomPoint(boundingRadius, 0.f);
      point.y() = (float) ::sqrt( square(boundingRadius) - square(point.x()) - square(point.z()) );

      return point;
    };

/*
    vec3 generateRandomControlPoint(float boundingRadius, const mat4t& orientation) const {
      vec3 rpoint = generateRandomPoint(boundingRadius);
      return orientation.rmul(vec4(rpoint.x(), rpoint.y(), rpoint.z(), 1.f)).xyz();
    };
*/

    void generateSplineControlPoints(Catmull_Rom_Spline& spline, const SplineDefinition& definition, const mat4t& orientation) const {
      vec3 start = generateRandomPoint(definition.getRadius());
      // The start control point should be reflected in the southern hemisphere of the bounding capsule
      start.y() = -start.y();

      spline.setStartControlPoint( orientation.rmul(vec4(start.x(), start.y(), start.z(), 1.f)).xyz() );

      vec3 end = generateRandomPoint(definition.getRadius());
      // The end control point should be offset/translated to the northern hemisphere of the bounding capsule
      end.y() += definition.getLength();

      spline.setEndControlPoint( orientation.rmul(vec4(end.x(), end.y(), end.z(), 1.f)).xyz() );
    };

/*
    void generateSplineControlPoints(Catmull_Rom_Spline& spline, const SplineDefinition& definition, const mat4t& orientation) const {
      spline.setStartControlPoint( vec3(0.f) );
      
      vec3 end = spline.getStartEndPoints().back();
      end.y() = 0;

      spline.setEndControlPoint(end);
    };
*/

    void generateSplineControlPoints(Catmull_Rom_Spline& catMullSpline, const Spline& spline) const {
      generateSplineControlPoints(catMullSpline, *spline.getDefinition(), spline.getModelToWorld());
    };

	  void createSpline (const SplineDefinition& definition) {
      // Push a new instance of a Spline
      splines.push_back(Spline(&definition, generateSplineOrientation()));

      // Apply modifications to the newly inserted instance
      generateSplinePoints(splines.back().getStartSpline(), splines.back());
      generateSplineControlPoints(splines.back().getStartSpline(), splines.back());
      
      // Set animation to start spline
      splines.back().getAnimationSplineState() = splines.back().getStartSpline();

      generateSplinePoints(splines.back().getEndSpline(), splines.back());
      generateSplineControlPoints(splines.back().getEndSpline(), splines.back());

/*
#ifdef _DEBUG
      splines.back().getStartSpline().dump();
      splines.back().getEndSpline().dump();
#endif
*/
	  }

    void generateNextAnimation(Spline& spline) {
      // Set the current end reference spline as the start reference spline
      spline.getEndSpline().swap(spline.getStartSpline());
      
      // Generate a new end spline reference
      generateSplinePoints(spline.getEndSpline(), spline);
      generateSplineControlPoints(spline.getEndSpline(), spline);

      spline.restartAnimation();
    };

    void sample(const Spline& spline) {      
      spline.getAnimationSplineState().sample(getSamplePointCount(), _samples);
    };
  
    void stepSlerpAnimation(float t) {
      // samples holds all the splines

      if (rotation == false) {
        _samples.swap(_slerpSamples);
        rotationAxis = vec3((float)(rand()%2), (float)(rand()%2), (float)(rand()%2));
        quart.rotateWithQuaternion( vec4(rotationAxis, 45.0f), _samples.get() );
      }

      rotation = quart.slerpRotateSmooth( _samples.get(), _slerpSamples.get() );

      for (size_t i = 0; i < splines.size(); ++i) {
        _renderer->add(_slerpSamples, i*(getSamplePointCount() + 1), (i*getSamplePointCount())+getSamplePointCount()+1, splines[i].getInitialColour(), splines[i].getEndColour());
      }
    };

    void stepLerpAnimation(float t) {
      for (size_t i = 0; i < splines.size(); ++i) {
        splines[i].step(t);
        
        _samples.reset();
        sample(splines[i]);
        _renderer->add(_samples, splines[i].getInitialColour(), splines[i].getEndColour());

        if (splines[i].isAnimationComplete()) {
          generateNextAnimation(splines[i]);
        };
      }
    };

    void resetRenderer() {
      _lineRenderer.reset();
      _quadRenderer.reset();
    };

  public:

	  SplineBall () :
      modelToWorld(1.f),
      _mode(LERP),
	    rotation (false),
      _renderer(&_quadRenderer) {
    }

	  void init ( const SplineConfigParams &_file ) {
		  file = _file;
      splines.reset();

		  createObjects ();
	  }

    const SplineConfigParams& getParameters() const {
      return file;
    };

    const mat4t& getModelToWorld() const {
      return modelToWorld;
    };

    mat4t& getModelToWorld() {
      return modelToWorld;
    };

    AnimationMode getAnimationMode() const {
      return _mode;
    };

    void setAnimationMode(AnimationMode mode) {
      _mode = mode;
      if ( mode == SLERP ) {
        _samples.reset();
        // Perform sampling once since it is unnecessary to sample at each rotation step
        for (size_t i = 0; i < splines.size(); ++i) {
          sample(splines[i]); // cut spline in pieces
        }
        rotationAxis = vec3((float)(rand()%2), (float)(rand()%2), (float)(rand()%2));
        quart.rotateWithQuaternion( vec4(rotationAxis, 45.0f), _samples.get() );
        rotation = true;
      }
    };

    RenderMode getRenderMode() const {
      return (_renderer == &_lineRenderer ? LINE : QUAD);
    };

    void setRenderMode(RenderMode mode) {
      if (mode == LINE) {
        _renderer = &_lineRenderer;
      } else {
        _renderer = &_quadRenderer;
      }
    };

    unsigned int getSamplePointCount() const {
      return 100;
    };

    /**
     *  Steps the animation by the interpolation factor provided
     **/
    void step(float t) {
      resetRenderer();

      if (_mode == LERP) {
        stepLerpAnimation(t);
      } else {
        stepSlerpAnimation(t);
      }

      _renderer->commit();
    };

	  void render (color_shader& shader, const mat4t& cameraToWorld, float nearPlane = 0.1f, float farPlane = 1000.0f) {
	    mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld, nearPlane, farPlane);
      
      static vec4 color(1, 1, 1, 1);
      shader.render(modelToProjection, color);
      
      _renderer->draw();
	  }

    void render (vertex_color_shader& shader, const mat4t& cameraToWorld, float nearPlane = 0.1f, float farPlane = 1000.0f) {
	    mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld, nearPlane, farPlane);
      
      shader.render(modelToProjection);
      _renderer->draw();
	  }

  };

  
  // -- 
  
  class splines_app : public app {
  private:
    /** 
     *  Command Line Arguments
     */
    class CmdLineArgs {
    private:
      const char* _directory;
      const char* _configurationFile;

      CmdLineArgs() :
        _directory(NULL),
        _configurationFile(NULL) {
      };

    public:
      /**
       *  @return the --directory|-d command line argument if specified or NULL
       **/
      const char* getDirectory() const {
        return _directory;
      };
      
      /**
       *  @return the --file|-f command line argument if specified or NULL
       **/
      const char* getConfigurationFile() const {
        return _configurationFile;
      };

      static CmdLineArgs parse(int argc, char* argv[]) {
        CmdLineArgs args;
        
        int i = 0;
        while (i < argc) {
          if (
              (::strcmp(argv[i], "--directory") == 0 || ::strcmp(argv[i], "-d") == 0) &&
              argc >= i + 1
            ) {
            args._directory = argv[++i];
          } else if (
              (::strcmp(argv[i], "--file") == 0 || ::strcmp(argv[i], "-f") == 0) &&
              argc >= i + 1
            ) {
            args._configurationFile = argv[++i];
          }

          ++i;
        };

        return args;
      };
    };
    
    // Configuration File Location
    const char* _configPath;

    // Camera
    Camera _cameraToWorld;
	  mat4t modelToWorld;

	  SplineBall splines_;
	  //color_shader color_shader_;
    vertex_color_shader vertex_color_shader_;

    InputManager _input;

    void parseCmdLineArgs(int argc, char* argv[]) {
      CmdLineArgs args = CmdLineArgs::parse(argc, argv);
        
      // Specify *relative* directory where the 'assets' folder may be found
      if (args.getDirectory() != NULL) {
        app_utils::prefix(args.getDirectory());
      }
        
      if (args.getConfigurationFile() != NULL) {
        _configPath = args.getConfigurationFile();
      }
    };

    // Loads the configuration file
    SplineConfigParams load(const char* relativeXMLPath) {
      DEBUG_PRINT("Loading: %s\n", relativeXMLPath);
      return SplineXMLConfig(app_utils::get_path(relativeXMLPath)).parse();
    };
    
    // Handles user input
	  void handleKeyStrokes () {
      _input.poll();
		
		  // translate camera left
		  if (is_key_down('A')) {
			  _cameraToWorld.translateLeft();
		  }
		  // translate camera right
		  if (is_key_down('D')) {
			  _cameraToWorld.translateRight();
		  }
		  // translate camera forward
		  if (is_key_down('W')) {
			  _cameraToWorld.translateForward();
		  }
		  // translate camra backwards
		  if (is_key_down('S')) {
			  _cameraToWorld.translateBack();
		  }
		  
      // pan camera upwards
      if (is_key_down(key_up)) {
			  _cameraToWorld.rotateUp();
		  }
      // pan camera downwards
		  if (is_key_down(key_down)) {
			  _cameraToWorld.rotateDown();
		  }
      // pan camera to the left
		  if (is_key_down(key_left)) {
			  _cameraToWorld.rotateLeft();
		  }
      // pan camera to the right
		  if (is_key_down(key_right)) {
			  _cameraToWorld.rotateRight();
		  }
      // reset camera
      if (is_key_down(key_space)) {
        _cameraToWorld.reset();
      }

      // Switch Animation Modes
      if (_input.processKey(key_tab)) {
        // first happens == and then ?:
        splines_.setAnimationMode(splines_.getAnimationMode() == SplineBall::LERP ? SplineBall::SLERP : SplineBall::LERP);
      }

      // Switch Rendering Modes
      if (_input.processKey(key_backspace)) {
        splines_.setRenderMode(splines_.getRenderMode() == SplineBall::LINE ? SplineBall::QUAD : SplineBall::LINE);
      }

      // Reload configuration
      if (_input.processKey(key_f5)) {
        splines_.init( load(_configPath) );
      }

	  }

    void simulate() {
      splines_.step(1.f / 30.f);
    };

  public:
    // this is called when we construct the class
    splines_app(int argc, char **argv) :
      app(argc, argv),
      _configPath("assets/splines/spline.xml"),
      modelToWorld(1.f) {
        _input.attach(this);
        parseCmdLineArgs(argc, argv);
    };

    // this is called once OpenGL is initialized
    void app_init() {

      // Initialise random number generator
      ::srand((unsigned int) time(NULL));
      
      // Load configuration file and
	    // create spline objects
	    splines_.init( load(_configPath) );
      //splines_.step(0.f);
      
	    //color_shader_.init();
      vertex_color_shader_.init();
	    //modelToWorld.loadIdentity();

    };

    // this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      handleKeyStrokes();
	    
	    simulate();

      // set a viewport - includes whole window area
      glViewport(x, y, w, h);

      // clear the background to black
      glClearColor(0, 0, 0, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // allow Z buffer depth testing (closer objects are always drawn in front of far ones)
      //glEnable(GL_DEPTH_TEST);

      // don't allow Z buffer depth testing (closer objects are always drawn in front of far ones)
				glDisable(GL_DEPTH_TEST);

      // allow alpha blend (transparency when alpha channel is 0)
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      //splines_.render(color_shader_, _cameraToWorld.getCamera());
      splines_.render(vertex_color_shader_, _cameraToWorld.getCamera());

    };
  };
};

#undef DEBUG_PRINT