#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <stdio.h>
#include <time.h>
#include <android/log.h>
#include <string.h>
#include <errno.h>

GLuint p;                
GLuint id_y, id_u, id_v; // Texture id
GLuint textureUniformY, textureUniformU,textureUniformV;

// screen size
const int screen_w = 720, screen_h = 1280;

// picture pix
const int pixel_w = 320, pixel_h = 180;
#define ATTRIB_VERTEX 3
#define ATTRIB_TEXTURE 4

#define FULL_FILL_CUT 0
#define FULL_FILL_SMALL 1

void initializeOpenGL() 
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

    InitShaders();
}
void resizeViewport(int newWidth, int newHeight) 
{
	glViewport(0, 0, newWidth, newHeight);
}
void renderFrame() 
{
    unsigned char buf[pixel_w*pixel_h*3/2];
    unsigned char *plane[3];
    
    //YUV Data
    plane[0] = buf;
    plane[1] = plane[0] + pixel_w*pixel_h;
    plane[2] = plane[1] + pixel_w*pixel_h/4;
    
    FILE *infile = NULL;

    if((infile=fopen("/storage/sdcard0/Download/test_yuv420p_320x180.yuv", "rb"))==NULL){
		__android_log_print(ANDROID_LOG_WARN, "GL-RENDER: ", "cannot open this file: %s", strerror(errno));
        return;
    }

    while (1) {
    if (fread(buf, 1, pixel_w*pixel_h*3/2, infile) != pixel_w*pixel_h*3/2){
        // Loop
        fseek(infile, 0, SEEK_SET);
        fread(buf, 1, pixel_w*pixel_h*3/2, infile);
    }
	__android_log_print(ANDROID_LOG_DEBUG, "GL-RENGER: ", "render loop");
	//Clear
	//Green background
	glClearColor(0.0,255,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	//Y
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id_y);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, pixel_w, pixel_h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, plane[0]); 
	glUniform1i(textureUniformY, 0);    
	//U
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, id_u);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, pixel_w/2, pixel_h/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, plane[1]);       
    glUniform1i(textureUniformU, 1);
	//V
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, id_v);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, pixel_w/2, pixel_h/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, plane[2]);    
    glUniform1i(textureUniformV, 2);   

    // Draw
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// Show
	eglSwapBuffers( eglGetCurrentDisplay(), eglGetCurrentSurface( EGL_DRAW ) );
	// sleep 40ms
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 40*1000*1000;
    nanosleep(&ts, NULL);
    }
}

char *getVSS() {
	return "attribute vec4 vertexIn; \
	       attribute vec2 textureIn; \
	       varying vec2 textureOut; \
	       void main(void) \
	       { \
	       	    gl_Position = vertexIn; \
	       		textureOut = textureIn; \
	       }";
}

char *getFSS() {
	return "varying vec2 textureOut; \
           uniform sampler2D tex_y; \
           uniform sampler2D tex_u; \
           uniform sampler2D tex_v; \
           void main(void) \
           { \
               vec3 yuv; \
               vec3 rgb; \
               yuv.x = texture2D(tex_y, textureOut).r; \
               yuv.y = texture2D(tex_u, textureOut).r - 0.5; \
               yuv.z = texture2D(tex_v, textureOut).r - 0.5; \
               rgb = mat3( 1,       1,         1, \
                           0,       -0.39465,  2.03211, \
                           1.13983, -0.58060,  0) * yuv; \
               gl_FragColor = vec4(rgb, 1); \
           }";
}


void InitShaders()
{
    GLint vertCompiled, fragCompiled, linked;
    
    GLint v, f;
    const char *vs,*fs;
	//Shader: step1
    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);
	//Get source code
    vs = getVSS();
    fs = getFSS();
	//Shader: step2
    glShaderSource(v, 1, &vs,NULL);
    glShaderSource(f, 1, &fs,NULL);
	//Shader: step3
    glCompileShader(v);
	//Debug
    glGetShaderiv(v, GL_COMPILE_STATUS, &vertCompiled);
    glCompileShader(f);
    glGetShaderiv(f, GL_COMPILE_STATUS, &fragCompiled);

	//Program: Step1
    p = glCreateProgram(); 
	//Program: Step2
    glAttachShader(p,v);
    glAttachShader(p,f); 

    glBindAttribLocation(p, ATTRIB_VERTEX, "vertexIn");
    glBindAttribLocation(p, ATTRIB_TEXTURE, "textureIn");
	//Program: Step3
    glLinkProgram(p);
	//Debug
    glGetProgramiv(p, GL_LINK_STATUS, &linked);  
	//Program: Step4
    glUseProgram(p);


	//Get Uniform Variables Location
	textureUniformY = glGetUniformLocation(p, "tex_y");
	textureUniformU = glGetUniformLocation(p, "tex_u");
	textureUniformV = glGetUniformLocation(p, "tex_v"); 

	static GLfloat vertexVertices[] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f,  1.0f,
		1.0f,  1.0f,
	};    

	// 图片填满屏幕、会变形（拉伸和压缩）
	static GLfloat textureVertices[] = {
		0.0f,  1.0f,
		1.0f,  1.0f,
		0.0f,  0.0f,
		1.0f,  0.0f,
	}; 
#if FULL_FILL_CUT    // 保证图片填满屏幕、不变形、裁剪掉超出屏幕的部分；所以应该修改素材
	float rw = pixel_w*1.0 / screen_w;
	float rh = pixel_h*1.0 / screen_h;
	float r = 0;
	if (rw > rh) {
		r = rh/rw;
		r = r/2;
		textureVertices[4] = (textureVertices[0] += r);
		textureVertices[6] = (textureVertices[2] -= r);
	} else {
		r = rw/rh;
		r = r/2;
		textureVertices[3] = (textureVertices[1] -= r);
		textureVertices[7] = (textureVertices[5] += r);
	}
#elif FULL_FILL_SMALL  // 保证图片不变形、宽或高填满屏幕、不裁剪（流出黑边）; 所以应该修改位置
	float rw = pixel_w*1.0 / screen_w;
	float rh = pixel_h*1.0 / screen_h;
	float r = 0;
	if (rw > rh) {
		r = rh/rw;
		vertexVertices[3] = (vertexVertices[1] = -r);
		vertexVertices[7] = (vertexVertices[5] = r);
	} else {
		r = rw/rh;
		vertexVertices[4] = (vertexVertices[0] = r);
		vertexVertices[6] = (vertexVertices[2] = -r);
	}
#endif

	//Set Arrays
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, vertexVertices);
	//Enable it
    glEnableVertexAttribArray(ATTRIB_VERTEX);    
    glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, textureVertices);
    glEnableVertexAttribArray(ATTRIB_TEXTURE);

	//Init Texture
    glGenTextures(1, &id_y); 
    glBindTexture(GL_TEXTURE_2D, id_y);    
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glGenTextures(1, &id_u);
    glBindTexture(GL_TEXTURE_2D, id_u);   
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glGenTextures(1, &id_v); 
    glBindTexture(GL_TEXTURE_2D, id_v);    
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

